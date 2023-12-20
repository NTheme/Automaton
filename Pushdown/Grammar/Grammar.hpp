/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Nov 28 2023
 *  File : Grammar.hpp
 ******************************************/

#pragma once

#include <unordered_map>
#include <vector>

#include "Alphabet.hpp"
#include "Rule.hpp"

class Grammar {
 public:
  void fit(const Alphabet& alphabet);
  void addRule(const Rule& rule);

  const Alphabet& alphabet() const;
  const std::unordered_map<char, std::vector<Rule>>& rules() const;
  const Rule& getByID(std::size_t id) const;

 private:
  Alphabet m_alphabet;
  std::size_t m_cnt;
  std::unordered_map<char, std::vector<Rule>> m_rules;

  bool isCorrectAlphabet(const Alphabet& alphabet) const;
  bool isCorrectRule(const Rule& rule) const;
};
