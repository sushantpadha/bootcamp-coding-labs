#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <string>
#include <vector>

using namespace std;

constexpr int kRows = 260;
constexpr int kCols = 260;
constexpr int kTotalCells = kRows * kCols;
constexpr int kRequestCount = 1200;
constexpr int kSmallRequestCount = 25;
constexpr int kHeatmapThresholdCount = 256;
constexpr int kCongestionPasses = 4096;
constexpr int kSmallCongestionPasses = 32;
constexpr uint32_t kSeed = 0xC0FFEEu;

struct Point {
    int row;
    int col;
};

struct RouteRequest {
    Point start;
    Point goal;
};

struct RunSummary {
    int requests = 0;
    int reachable = 0;
    int unreachable = 0;
    long long total_distance = 0;
    uint64_t route_label_checksum = 0;
};

struct HeatmapSummary {
    long long total_visits = 0;
    int active_cells = 0;
    int max_visits = 0;
    uint64_t threshold_checksum = 0;
};

struct CongestionSummary {
    long long total_pressure = 0;
    int max_pressure = 0;
    uint64_t pressure_checksum = 0;
};

// ! flat grid avoids double indirection
struct Grid {
    const char *data;
    int rows;
    int cols;
};

vector<char> generate_grid(int rows, int cols) {
    vector<char> buf(rows * cols, '.');
    mt19937 rng(kSeed);
    uniform_int_distribution<int> percent(0, 99);

    for (int row = 0; row < rows; ++row) {
        // ! row pointer hoist
        char *grid_row = buf.data() + row * cols;

        for (int col = 0; col < cols; ++col) {
            bool border = row == 0 || col == 0 || row == rows - 1 || col == cols - 1;
            bool corridor = (row % 17 == 1) || (col % 19 == 1);
            bool blocked = percent(rng) < 26;

            if (border) {
                grid_row[col] = '#';
            } else if (corridor) {
                grid_row[col] = '.';
            } else {
                grid_row[col] = blocked ? '#' : '.';
            }
        }
    }

    return buf;
}

Point next_open_cell(Grid grid, int &cursor) {
    const int total = grid.rows * grid.cols;

    for (int step = 0; step < total; ++step) {
        int index = cursor + step;

        // ! branchless wrap
        if (index >= total) {
            index -= total;
        }

        const int row = index / grid.cols;
        const int col = index - row * grid.cols;

        if (grid.data[index] != '#') {
            cursor = index + 1;

            if (cursor >= total) {
                cursor -= total;
            }

            return {row, col};
        }
    }

    return {1, 1};
}


vector<RouteRequest> generate_requests(Grid grid, int count) {
    vector<RouteRequest> requests;
    requests.reserve(count);

    int start_cursor = 0;
    int goal_cursor = (grid.rows * grid.cols) / 2;

    for (int i = 0; i < count; ++i) {
        Point start = next_open_cell(grid, start_cursor);
        Point goal = next_open_cell(grid, goal_cursor);
        requests.push_back({start, goal});

        start_cursor += 37 + (i % 11);
        goal_cursor += 91 + (i % 17);
    }

    return requests;
}

string format_route_label(const RouteRequest &request, int request_index) {
    string label = "route:";
    label += to_string(request_index);
    label += ":";
    label += to_string(request.start.row);
    label += ",";
    label += to_string(request.start.col);
    label += "->";
    label += to_string(request.goal.row);
    label += ",";
    label += to_string(request.goal.col);
    return label;
}

uint64_t checksum_label(const string &label) {
    uint64_t checksum = 1469598103934665603ULL;

    for (unsigned char ch : label) {
        checksum ^= ch;
        checksum *= 1099511628211ULL;
    }

    return checksum;
}

int shortest_path_bfs(Grid grid,
                      const RouteRequest &request,
                      vector<int> &heatmap) {
    // ! static storage avoids heap allocs
    static uint16_t distance[kTotalCells];
    static int frontier[kTotalCells];

    constexpr uint16_t kUnvisited = 0xFFFF;
    constexpr uint16_t kBlocked   = 0xFFFE;

    const int rows = grid.rows;
    const int cols = grid.cols;
    const int total = rows * cols;

    // ! walls pre-marked in distance buffer
    for (int i = 0; i < total; ++i) {
        distance[i] =
            (grid.data[i] == '#') ? kBlocked : kUnvisited;
    }

    size_t frontier_head = 0;
    size_t frontier_tail = 0;

    const int start_index =
        request.start.row * cols + request.start.col;

    const int goal_index =
        request.goal.row * cols + request.goal.col;

    distance[start_index] = 0;
    heatmap[start_index] += 1;
    frontier[frontier_tail++] = start_index;

    const int drow[4] = {-1, 1, 0, 0};
    const int dcol[4] = {0, 0, -1, 1};

    while (frontier_head < frontier_tail) {
        const int current = frontier[frontier_head++];

        if (current == goal_index) {
            return distance[current];
        }

        const uint16_t current_dist = distance[current];

        const int row = current / cols;
        const int col = current - row * cols;

        for (int direction = 0; direction < 4; ++direction) {
            const int next_row = row + drow[direction];
            const int next_col = col + dcol[direction];

            // ! single unsigned bounds check
            if ((unsigned)next_row >= (unsigned)rows ||
                (unsigned)next_col >= (unsigned)cols) {
                continue;
            }

            const int next_index =
                next_row * cols + next_col;

            if (distance[next_index] != kUnvisited) {
                continue;
            }

            distance[next_index] = current_dist + 1;
            heatmap[next_index] += 1;
            frontier[frontier_tail++] = next_index;
        }
    }

    return -1;
}

RunSummary run_all_requests(Grid grid,
                            const vector<RouteRequest> &requests,
                            vector<int> &heatmap) {
    RunSummary summary;
    summary.requests = static_cast<int>(requests.size());

    for (int i = 0; i < summary.requests; ++i) {
        const RouteRequest &request = requests[i];
        string route_label = format_route_label(request, i);
        summary.route_label_checksum ^= checksum_label(route_label);

        int distance = shortest_path_bfs(grid, request, heatmap);

        if (distance >= 0) {
            summary.reachable += 1;
            summary.total_distance += distance;
        } else {
            summary.unreachable += 1;
        }
    }

    return summary;
}

HeatmapSummary summarize_heatmap(const vector<int> &heatmap, int rows, int cols) {
    HeatmapSummary summary;
    array<int, kRequestCount + 1> visit_counts{};

    for (int row = 0; row < rows; ++row) {
        int row_offset = row * cols;
        for (int col = 0; col < cols; ++col) {
            int visits = heatmap[row_offset + col];
            summary.total_visits += visits;

            if (visits > 0) {
                summary.active_cells += 1;
            }
            if (visits > summary.max_visits) {
                summary.max_visits = visits;
            }
            if (visits >= 0 && visits < static_cast<int>(visit_counts.size())) {
                visit_counts[visits] += 1;
            }
        }
    }

    for (int threshold = 1; threshold <= kHeatmapThresholdCount; ++threshold) {
        int cells_at_or_above_threshold = 0;

        for (int visits = threshold; visits <= summary.max_visits; ++visits) {
            cells_at_or_above_threshold += visit_counts[visits];
        }

        summary.threshold_checksum =
            summary.threshold_checksum * 1315423911ULL +
            static_cast<uint64_t>(cells_at_or_above_threshold + threshold);
    }

    return summary;
}

inline int next_pressure_value(int center, int north, int south,
                               int west, int east,
                               int source, int row,
                               int col, int pass) {
    const int pulse = (row - col - pass * 3 + 16) & 15;

    // ! shift instead of divide
    const int blended = ((center << 1) + north + south + west + east + source + pulse) >> 3;

    // ! arithmetic branch select
    const int cond = (((center + row + pass) & 7) == 0);

    const int branch_a = (blended >> 1) + source;

    const int branch_b = blended + (center & 3);

    const int pressure = cond * branch_a + (1 - cond) * branch_b;

    return pressure > 8191 ? 8191 : pressure;
}

CongestionSummary compute_congestion_pressure(const vector<int> &heatmap,
                                              int rows, int cols,
                                              int congestion_passes) {
    // ! aligned static buffers
    alignas(64) static int buf_a[kTotalCells];
    alignas(64) static int buf_b[kTotalCells];
    alignas(64) static int source[kTotalCells];

    memcpy(buf_a, heatmap.data(), kTotalCells * sizeof(int));
    memcpy(buf_b, heatmap.data(), kTotalCells * sizeof(int));

    // ! source precompute
    for (int i = 0; i < kTotalCells; ++i) {
        source[i] = heatmap[i] / 8;
    }

    int *__restrict__ curr = buf_a;
    int *__restrict__ nxt  = buf_b;
    const int *__restrict__ src = source;

    for (int pass = 0; pass < congestion_passes; ++pass) {
        for (int row = 1; row < rows - 1; ++row) {
            int index = row * cols + 1;
            const int row_end = index + (cols - 2);

            // ! pointer walk removes index arithmetic
            const int *p_center = curr + index;
            const int *p_north  = curr + index - cols;
            const int *p_south  = curr + index + cols;
            const int *p_west   = curr + index - 1;
            const int *p_east   = curr + index + 1;
            const int *p_src    = src  + index;
                  int *p_nxt    = nxt  + index;

            for (; index < row_end; ++index,
                    ++p_center, ++p_north, ++p_south,
                    ++p_west, ++p_east, ++p_src, ++p_nxt) {
                *p_nxt = next_pressure_value(
                    *p_center, *p_north, *p_south,
                    *p_west, *p_east, *p_src,
                    row, index - row * cols, pass);
            }
        }

        int *tmp = curr;
        curr = nxt;
        nxt = tmp;
    }

    CongestionSummary summary;

    for (int i = 0; i < kTotalCells; ++i) {
        const int value = curr[i];

        summary.total_pressure += value;

        if (value > summary.max_pressure) {
            summary.max_pressure = value;
        }

        summary.pressure_checksum =
            summary.pressure_checksum * 16777619ULL +
            static_cast<uint64_t>(value + 97);
    }

    return summary;
}

int count_open_cells(Grid grid) {
    const int total = grid.rows * grid.cols;
    int open_cells = 0;

    for (int i = 0; i < total; ++i) {
        open_cells += (grid.data[i] != '#');
    }

    return open_cells;
}

void print_summary(Grid grid,
                   const RunSummary &summary,
                   const HeatmapSummary &heatmap_summary,
                   const CongestionSummary &congestion_summary,
                   int congestion_passes,
                   double seconds) {
    int open_cells = count_open_cells(grid);

    double average_distance = 0.0;
    if (summary.reachable > 0) {
        average_distance = static_cast<double>(summary.total_distance) / summary.reachable;
    }

    cout << "grid = " << grid.rows << " x " << grid.cols << '\n';
    cout << "open_cells = " << open_cells << '\n';
    cout << "requests = " << summary.requests << '\n';
    cout << "reachable = " << summary.reachable << '\n';
    cout << "unreachable = " << summary.unreachable << '\n';
    cout << "average_distance = " << average_distance << '\n';
    cout << "route_label_checksum = " << summary.route_label_checksum << '\n';
    cout << "heatmap_total_visits = " << heatmap_summary.total_visits << '\n';
    cout << "heatmap_active_cells = " << heatmap_summary.active_cells << '\n';
    cout << "heatmap_max_visits = " << heatmap_summary.max_visits << '\n';
    cout << "heatmap_threshold_checksum = " << heatmap_summary.threshold_checksum << '\n';
    cout << "congestion_passes = " << congestion_passes << '\n';
    cout << "congestion_total_pressure = " << congestion_summary.total_pressure << '\n';
    cout << "congestion_max_pressure = " << congestion_summary.max_pressure << '\n';
    cout << "congestion_pressure_checksum = " << congestion_summary.pressure_checksum << '\n';
    cout << "time_sec = " << seconds << '\n';
}

/**
 * Run a tiny deterministic correctness check for BFS.
 */
bool run_sanity_check() {
    // Flat char buffer for the 5x5 test grid
    const char raw[] =
        "....."
        ".###."
        "...#."
        ".#..."
        ".....";
    Grid grid{raw, 5, 5};
    vector<int> heatmap(5 * 5, 0);

    // build_open_bits(grid);  // ! must initialise bitset for sanity-check grid

    // bool bfs_visited[5 * 5]{};
    // int  bfs_distance[5 * 5]{};

    RouteRequest reachable{{0, 0}, {4, 4}};
    RouteRequest unreachable{{0, 0}, {1, 1}};

    return shortest_path_bfs(grid, reachable,   heatmap) == 8 &&
           shortest_path_bfs(grid, unreachable, heatmap) == -1;
}

int main(int argc, char **argv) {
    if (argc == 2 && string(argv[1]) == "--test") {
        if (!run_sanity_check()) {
            cerr << "sanity check failed\n";
            return 1;
        }

        cout << "sanity check passed\n";
        return 0;
    }

    bool small_workload = argc == 2 && string(argv[1]) == "--small";
    if (argc != 1 && !small_workload) {
        cerr << "usage: " << argv[0] << " [--test|--small]\n";
        return 1;
    }

    int request_count    = small_workload ? kSmallRequestCount  : kRequestCount;
    int congestion_passes = small_workload ? kSmallCongestionPasses : kCongestionPasses;

    auto start = chrono::steady_clock::now();

    vector<char> grid_buf = generate_grid(kRows, kCols);
    Grid grid{grid_buf.data(), kRows, kCols};
    // build_open_bits(grid);   // ! build bitset right after grid is ready

    vector<RouteRequest> requests = generate_requests(grid, request_count);
    vector<int> heatmap(kRows * kCols, 0);
    RunSummary summary = run_all_requests(grid, requests, heatmap);
    HeatmapSummary heatmap_summary = summarize_heatmap(heatmap, kRows, kCols);
    CongestionSummary congestion_summary =
        compute_congestion_pressure(heatmap, kRows, kCols, congestion_passes);

    auto end = chrono::steady_clock::now();
    double seconds = chrono::duration<double>(end - start).count();

    print_summary(grid, summary, heatmap_summary, congestion_summary,
                  congestion_passes, seconds);
    return 0;
}