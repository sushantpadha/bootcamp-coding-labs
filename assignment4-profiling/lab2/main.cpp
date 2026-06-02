#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

// This is a constant that won't change anywhere. Maybe use this to optimize something?
#define STEPS 7

struct Packet {
    int device_id;
    int lane;
    int reading;
    int quality;
    int kind;
    int stamp;
};

__attribute__((always_inline)) inline static unsigned int mix32(unsigned int x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

static std::vector<Packet> build_packets(int count, int device_count, int lane_count) {
    std::vector<Packet> packets(count);
    unsigned int seed = 123456789U;

    for (int i = 0; i < count; ++i) {
        seed = mix32(seed + i * 17U + 31U);
        Packet p;
        p.device_id = seed % device_count;
        p.lane = (seed >> 7) % lane_count;
        p.reading = (seed >> 11) & 1023;
        p.quality = (seed >> 3) & 255;
        p.kind = (seed >> 19) & 7;
        p.stamp = i;
        packets[i] = p;
    }

    return packets;
}

static std::vector<int> build_lane_weight(int lane_count) {
    std::vector<int> weight(lane_count);
    for (int i = 0; i < lane_count; ++i) {
        weight[i] = (i * 11 + 5) % 23 + 1;
    }
    return weight;
}

static std::vector<int> build_dependency_next(int n) {
    std::vector<int> next(n);
    std::vector<int> order(n);

    for (int i = 0; i < n; ++i) {
        order[i] = i;
    }

    unsigned int seed = 987654321U;
    for (int i = n - 1; i > 0; --i) {
        seed = mix32(seed + i * 29U + 11U);
        int j = seed % (i + 1);
        std::swap(order[i], order[j]);
    }

    for (int i = 0; i < n; ++i) {
        next[order[i]] = order[(i + 1) % n];
    }

    return next;
}

static std::vector<int> build_dependency_value(int n) {
    std::vector<int> value(n);
    for (int i = 0; i < n; ++i) {
        value[i] = (mix32(i * 91U + 17U) & 1023);
    }
    return value;
}

[[maybe_unused]] static std::vector<int> build_scatter_map(int n) {
    std::vector<int> remap(n);
    for (int i = 0; i < n; ++i) {
        remap[i] = mix32(i * 37U + 101U) & (n - 1);
    }
    return remap;
}

template<int history_cols>
static void refresh_history(std::vector<int>& history, const std::vector<Packet>& packets) {
    const int rows = (int)history.size() / history_cols;
    int* __restrict__ hist = history.data();
    
    for (std::size_t i = 0; i < packets.size(); ++i) {
        const Packet& p = packets[i];
        // int idx = p.device_id * history_cols + (p.stamp % history_cols);
        int idx = p.device_id * history_cols + (i & (history_cols-1));
        hist[idx] = (hist[idx] + p.reading + p.quality) & 2047;
    }

    // ! tried openmp threading -> worse at even 2 threads
    // ! tried splitting into two prefix chains -> no impact
    for (int r = 0; r < rows; ++r) {
        int* row = hist + r * history_cols;

        int carry = row[0];

        for (int c = 1; c < history_cols; ++c) {
            carry += row[c];
            carry &= 2047;
            row[c] = carry;
        }
    }
}

__attribute__((always_inline)) inline static int select_mask(int cond_mask, int a, int b) {
    return (cond_mask & a) | (~cond_mask & b);
}

static int branchy_score(const Packet& p, const std::vector<int>& lane_weight) {
    int score = p.reading + lane_weight[p.lane];

    const int x = p.reading ^ (p.quality << 2) ^ (p.kind * 97);

    const int m1 = -((x & 1) != 0);
    const int m2 = -((x & 2) != 0);
    const int m4 = -((x & 4) != 0);
    const int m8 = -((x & 8) != 0);
    const int m16 = -((x & 16) != 0);
    const int m32 = -((x & 32) != 0);

    score += select_mask(m1, 19, -7);

    score = select_mask(
        m2,
        score ^ p.quality,
        score + p.kind * 3
    );

    score += select_mask(
        m4,
        p.reading >> 2,
        -(p.quality >> 3)
    );

    score = select_mask(
        m8,
        score ^ p.device_id,
        score + p.lane * 5
    );

    score += select_mask(m16, 31, -11);

    score = select_mask(
        m32,
        score ^ (p.reading << 1),
        score + (p.quality & 15)
    );

    return score & 4095;
}

__attribute__((always_inline)) inline static int chase_dependency(
    int start,
    int /*steps*/,
    const std::vector<int>& sum7
) {
    return sum7[start & (sum7.size() - 1)];
}

static int cold_column_probe(const std::vector<int>& history, int rows, int cols, int /*seed*/) {
    int sum = 0;
    // ! every element is visited anyway - this can be vectorized netter
    for (int i = 0; i < rows * cols; ++i) {
        sum += history[i] & 31;
    }
    return sum;
}

static long long process_packets(
    const std::vector<Packet>& packets,
    const std::vector<int>& lane_weight,
    const std::vector<int>& sum7
) {
    long long total = 0;

    for (std::size_t i = 0; i < packets.size(); ++i) {
        const Packet& p = packets[i];
        int score = branchy_score(p, lane_weight);

        // ! mostly true
        const int mask = -(((score ^ p.quality) & 7) != 0);
        score += (mask & chase_dependency(score + p.device_id, STEPS, sum7)) 
                | ((~mask) & lane_weight[p.lane]);

        total += score;
    }

    return total;
}

template <int history_cols>
static long long run_epoch(
    std::vector<Packet>& packets,
    const std::vector<int>& lane_weight,
    std::vector<int>& history,
    const std::vector<int>& sum7
) {
    refresh_history<history_cols>(history, packets);

    long long total = process_packets(
        packets,
        lane_weight,
        sum7
    );

    int rows = (int)history.size() / history_cols;
    total += cold_column_probe(history, rows, history_cols, (int)(total & 1023));
    return total;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const int device_count = 4096;
    const int lane_count = 32;
    const int packet_count = 220000;
    const int dependency_count = 1 << 18;
    constexpr int history_cols = 2048;   // 2048 too
    const int epochs = 6;

    std::vector<Packet> packets = build_packets(packet_count, device_count, lane_count);
    std::vector<int> lane_weight = build_lane_weight(lane_count);
    std::vector<int> dependency_next = build_dependency_next(dependency_count);
    std::vector<int> dependency_value = build_dependency_value(dependency_count);
    std::vector<int> history(device_count * history_cols, 0);

    // ! precompute jump7
    std::vector<int> dependency_sum7(dependency_count);
    for (int i = 0; i < dependency_count; ++i) {
        int idx = i;
        int total = 0;

        idx = dependency_next[idx]; total += dependency_value[idx];
        idx = dependency_next[idx]; total += dependency_value[idx];
        idx = dependency_next[idx]; total += dependency_value[idx];
        idx = dependency_next[idx]; total += dependency_value[idx];
        idx = dependency_next[idx]; total += dependency_value[idx];
        idx = dependency_next[idx]; total += dependency_value[idx];
        idx = dependency_next[idx]; total += dependency_value[idx]; // 7

        dependency_sum7[i] = total;
    }

    long long answer = 0;
    for (int epoch = 0; epoch < epochs; ++epoch) {
        answer += run_epoch<history_cols>(
            packets,
            lane_weight,
            history, dependency_sum7
        );
    }

    std::cout << (answer & 0x7fffffffffffffffLL) << '\n';
    return 0;
}
