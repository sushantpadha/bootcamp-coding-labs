/*
 * Assignment 1: Marriage Pact
 * Adapted by Tinkercademy from Stanford CS106L
 * (originally by Haven Whitney, with modifications by Fabio Ibanez
 * & Jacob Roberts-Baca).
 *
 * Complete each STUDENT TODO below. Read the README carefully — the
 * requirements there (ranges, projections, sample, reserve, no raw
 * for-loops in find_matches, iterator-safe erase in run_mixer) are
 * part of the assignment, not optional polish.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/**
 * Reads `filename` line by line and returns the applicants.
 *
 * Requirements:
 *   - Take `filename` as `const std::string&`.
 *   - Call `reserve()` before populating, with a sensible capacity.
 *     Justify your choice in short_answer.txt.
 */
std::vector<std::string> get_applicants(const std::string &filename)
{
    // STUDENT TODO: Implement this function.
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open " + filename);
    }
    
    std::vector<std::string> applicants;
    applicants.reserve(1000); // estimate from the students.txt file
    std::string line;
    while (std::getline(file, line))
    {
        applicants.push_back(line);
    }

    return applicants;
}

/**
 * Returns the initials of `name`, uppercased.
 *   e.g. initials("Marceline McMillan") == "MM"
 *
 * Requirements:
 *   - Parameter must be `std::string_view` (no allocation).
 */
std::string initials(std::string_view name)
{
    // STUDENT TODO: Implement this function.
    auto space = name.find(' ');
    if (space == std::string::npos || space == 0 || space == name.size() - 1)
    {
        throw std::runtime_error("Invalid name: " + std::string(name));
    }
    char first = std::toupper(static_cast<unsigned char>(name[0]));
    char second = std::toupper(static_cast<unsigned char>(name[space + 1]));

    return std::string{first, second};
}

/**
 * Returns every applicant in `students` who shares initials with `name`.
 *
 * Requirements:
 *   - No raw `for` loops. Use std::ranges::copy_if (or views::filter
 *     piped into a vector). Use a projection where it makes the call
 *     clearer.
 *   - Take `students` as `const std::vector<std::string>&`.
 */
std::vector<std::string> find_matches(std::string_view name, const std::vector<std::string> &students)
{
    // STUDENT TODO: Implement this function.
    auto target = initials(name);
    std::vector<std::string> matches;
    matches.reserve(students.size()); // worst-case capcity

    std::ranges::copy_if(
        students, std::back_inserter(matches), [&](const std::string &proj) { return proj == target; },
        [](const std::string &s) { return initials(s); });

    return matches;
}

/**
 * Returns one randomly-chosen match, or "NO MATCHES FOUND." if empty.
 *
 * Requirements:
 *   - Use std::sample with a seeded std::mt19937.
 *   - Do NOT use pop_back() or rand() % size.
 */
std::string get_match(const std::vector<std::string> &matches)
{
    // STUDENT TODO: Implement this function.
    if (matches.empty())
    {
        return "NO MATCHES FOUND.";
    }
    std::mt19937 gen{std::random_device{}()};
    std::vector<std::string> sample;
    sample.reserve(1);
    std::sample(matches.begin(), matches.end(), std::back_inserter(sample), 1, gen);
    return sample[0];
}

/**
 * Runs a multi-round mixer. In each round, scan the remaining
 * applicants left-to-right; for each applicant, look for another
 * applicant with the same initials still in the pool. If found,
 * pair them, remove both from `applicants`, and record the pair.
 * Continue rounds until a full pass yields no new pairs.
 *
 * `applicants` is mutated: paired names are removed. Whatever is
 * left over at the end is unpaired.
 *
 * Requirements:
 *   - The naive "iterate and erase as you go" approach WILL invalidate
 *     your iterator. You must handle this — see the README for the
 *     three acceptable strategies — and document your choice in
 *     short_answer.txt.
 */
std::vector<std::pair<std::string, std::string>> run_mixer(std::vector<std::string> &applicants)
{
  std::vector<std::pair<std::string, std::string>> pairs;
  pairs.reserve(applicants.size() / 2);

  bool found_pair = true;
  while (found_pair) { // this is technically redundnant
    found_pair = false;
    for (size_t i = 0; i < applicants.size(); ++i) {
      std::string self_initials = initials(applicants[i]);
      for (size_t j = i + 1; j < applicants.size(); ++j) {
        if (initials(applicants[j]) == self_initials) {
          pairs.emplace_back(applicants[i], applicants[j]);
          applicants.erase(applicants.begin() + j);
          applicants.erase(applicants.begin() + i);
          found_pair = true;
          i--;  // recheck this index, a new element shifted into i
          break;
        }
      }
    }
  }
  return pairs;
}

/* #### Please don't remove this line! #### */
#include "tests/utils.hpp"
