#include "spellcheck.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>

template <std::input_iterator Iterator, std::indirect_unary_predicate<Iterator> UnaryPred>
std::vector<Iterator> find_all(Iterator begin, Iterator end, UnaryPred pred);

Corpus tokenize(std::string& source) {
  auto spaces = find_all(source.begin(), source.end(), ::isspace);
  Corpus tokens{};

  // ! spaces.size() >= 2 guaranteed
  auto first = spaces.begin();
  auto last = spaces.end();
  auto second = (first + 1);

  auto inserter = std::inserter(tokens, tokens.end());

  std::transform(second, last, first, inserter,
    [&source](const auto& it1, const auto& it2) { return Token(source, it2, it1); });

  // std::cerr << "A: " << tokens.size() << "\n";

  std::erase_if(tokens, [](const Token& t) { return t.content.empty(); });

  // std::cerr << "B: " << tokens.size() << "\n";

  return tokens;
}

std::set<Misspelling> spellcheck(const Corpus& source, const Dictionary& dictionary) {
  namespace rv = std::ranges::views;
  auto view = source
    | rv::filter([&](const Token& t) { return !dictionary.contains(t.content); })
    | rv::transform([&](const Token& t) {
      auto suggestions = dictionary | rv::filter([&](const std::string& s) { return levenshtein(t.content, s) == 1; });
      return Misspelling { t, std::set(suggestions.begin(), suggestions.end()) };
    })
    | rv::filter([&](const Misspelling& m) { return !m.suggestions.empty(); });

  return std::set<Misspelling>(view.begin(), view.end());
};

/* Helper methods */

#include "utils.cpp"