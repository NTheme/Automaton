/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Nov 28 2023
 *  File : ParserEarley.cpp
 ******************************************/

#include "ParserEarley.hpp"

void ParserEarley::fit(const Grammar& grammar) { m_grammar = grammar; }

bool ParserEarley::predict(const std::string& word) const {
  std::vector<std::unordered_set<Situation, SituationHash>> list(word.size() + 1);

  Rule start_rule(m_grammar.alphabet().utils[0], std::string(1, m_grammar.alphabet().start));
  list[0].emplace(start_rule, 0, 0);

  for (std::size_t index = 0; index < list.size(); ++index) {
    earleyScan(index, list, word);
    std::size_t len = list[index].size() + 1;
    while (len != list[index].size()) {
      len = list[index].size();
      earleyPredict(index, list);
      earleyComplete(index, list);
    }
  }

  return std::find(list.back().begin(), list.back().end(), Situation(start_rule, 0, 1)) != list.back().end();
}

void ParserEarley::earleyScan(std::size_t index, std::vector<std::unordered_set<Situation, SituationHash>>& list,
                              const std::string& word) const {
  if (index == 0) {
    return;
  }

  for (const auto& situation : list[index - 1]) {
    if (situation.dividerCurrent() == word[index - 1]) {
      list[index].emplace(situation.rule, situation.index, situation.divider + 1);
    }
  }
}

void ParserEarley::earleyComplete(std::size_t index, std::vector<std::unordered_set<Situation, SituationHash>>& list) const {
  std::vector<Situation> add;
  for (const auto& situation : list[index]) {
    if (situation.divider != situation.rule.rhs().size()) {
      continue;
    }
    for (const auto& next_situation : list[situation.index]) {
      if (situation.rule.lhs() == next_situation.dividerCurrent()) {
        add.emplace_back(next_situation.rule, next_situation.index, next_situation.divider + 1);
      }
    }
  }

  for (const auto& situation : add) {
    list[index].insert(situation);
  }
}

void ParserEarley::earleyPredict(std::size_t index, std::vector<std::unordered_set<Situation, SituationHash>>& list) const {
  std::vector<Situation> add;
  for (const auto& situation : list[index]) {
    if (situation.divider == situation.rule.rhs().size()) {
      continue;
    }

    if (m_grammar.rules().contains(situation.dividerCurrent())) {
      for (const auto& rule : m_grammar.rules().at(situation.dividerCurrent())) {
        add.emplace_back(rule, index, 0);
      }
    }
  }

  for (const auto& situation : add) {
    list[index].insert(situation);
  }
}

ParserEarley::Situation::Situation(const Rule& n_rule, std::size_t n_index, std::size_t n_divider)
    : rule(n_rule), index(n_index), divider(n_divider) {}

bool ParserEarley::Situation::operator==(const Situation& other) const {
  return rule == other.rule && index == other.index && divider == other.divider;
}

char ParserEarley::Situation::dividerCurrent() const { return rule.rhs()[divider]; }

std::size_t ParserEarley::SituationHash::operator()(const Situation& situation) const {
  RuleHash rule;
  return std::hash<std::size_t>()(situation.index + situation.divider + rule(situation.rule));
}

/*
1 2 2
S
ab
S->ab
S->
S
2
aababb
aabbba
No
No
*/