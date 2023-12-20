/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Nov 28 2023
 *  File : Grammar.cpp
 ******************************************/

#include "Grammar.hpp"

void Grammar::fit(const Alphabet& alphabet) {
  if (!isCorrectAlphabet(alphabet)) {
    throw std::invalid_argument("This alphabet is not allowed!");
  }

  m_alphabet = alphabet;
  m_alphabet.nterm.push_back(m_alphabet.utils[0]);
  m_rules.clear();
  m_rules[m_alphabet.utils[0]].emplace_back(m_alphabet.utils[0], std::string(1, m_alphabet.start), m_cnt++);
}

void Grammar::addRule(const Rule& rule) {
  if (!isCorrectRule(rule)) {
    throw std::invalid_argument("This rule is not allowed!");
  }

  m_rules[rule.lhs()].emplace_back(rule.lhs(), rule.rhs(), m_cnt++);
}

const Alphabet& Grammar::alphabet() const { return m_alphabet; }

const std::unordered_map<char, std::vector<Rule>>& Grammar::rules() const { return m_rules; }

const Rule& Grammar::getByID(std::size_t id) const {
  for (const auto& nterm_block : m_rules) {
    for (const auto& rule : nterm_block.second) {
      if (rule.id() == id) {
        return rule;
      }
    }
  }
  throw std::out_of_range("Rule not found!");
}

bool Grammar::isCorrectAlphabet(const Alphabet& alphabet) const {
  if (!alphabet.nterm.contains(alphabet.start)) {
    return false;
  }
  for (const auto& symb : alphabet.yterm) {
    if (alphabet.nterm.contains(symb)) {
      return false;
    }
  }
  for (const auto& symb : alphabet.utils) {
    if (alphabet.nterm.contains(symb) || alphabet.yterm.contains(symb)) {
      return false;
    }
  }
  return true;
}

bool Grammar::isCorrectRule(const Rule& rule) const {
  if (!m_alphabet.nterm.contains(rule.lhs())) {
    return false;
  }
  for (const auto& symb : rule.rhs()) {
    if (!m_alphabet.nterm.contains(symb) && !m_alphabet.yterm.contains(symb)) {
      return false;
    }
  }
  return true;
}
