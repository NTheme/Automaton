/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Nov 28 2023
 *  File : main.cpp
 ******************************************/

#include "ParserLR1.hpp"

Rule getRule(const std::string& buf) {
  size_t index = 0;

  for (; std::isspace(buf.at(index)); ++index) {
  }
  size_t lhs_begin = index;
  for (; std::isalpha(buf.at(index)); ++index) {
  }

  for (; buf.at(index) != '>'; ++index) {
  }

  for (++index; index < buf.size() && std::isspace(buf.at(index)); ++index) {
  }
  size_t rhs_begin = index;
  for (; index < buf.size() && std::isalpha(buf.at(index)); ++index) {
  }

  return Rule(buf[lhs_begin], buf.substr(rhs_begin, index - rhs_begin));
}

std::vector<Rule> getRules(size_t nrule) {
  std::vector<Rule> rules;

  std::string buf;
  std::getline(std::cin, buf);

  while (nrule-- > 0) {
    std::getline(std::cin, buf);
    rules.emplace_back(getRule(buf));
  }

  return rules;
}

Grammar getGrammar() {
  size_t nterm, yterm, nrule;
  Alphabet alphabet;
  std::cin >> nterm >> yterm >> nrule >> alphabet.nterm >> alphabet.yterm;
  std::vector<Rule> rules = getRules(nrule);
  std::cin >> alphabet.start;

  Grammar grammar;
  grammar.fit(alphabet);
  for (const auto& rule : rules) {
    grammar.addRule(rule);
  }

  return grammar;
}

signed main() {
  ParserLR1 parser;
  parser.fit(getGrammar());

  size_t nword;
  std::cin >> nword;
  while (nword-- > 0) {
    std::string word;
    std::cin >> word;
    std::cout << (parser.predict(word) ? "Yes\n" : "No\n");
  }

  return 0;
}
