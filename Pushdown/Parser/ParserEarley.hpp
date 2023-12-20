/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Nov 28 2023
 *  File : ParserEarley.hpp
 ******************************************/

#pragma once

#include <unordered_set>
#include <iostream>
#include <vector>

#include "Parser.hpp"

class ParserEarley : public Parser {
 public:
  void fit(const Grammar& grammar) final;
  bool predict(const std::string& word) const final;

 private:
  struct Situation;
  struct SituationHash;

  void earleyScan(std::size_t index, std::vector<std::unordered_set<Situation, SituationHash>>& list,
                  const std::string& word) const;
  void earleyComplete(std::size_t index, std::vector<std::unordered_set<Situation, SituationHash>>& list) const;
  void earleyPredict(std::size_t index, std::vector<std::unordered_set<Situation, SituationHash>>& list) const;
};

struct ParserEarley::Situation {
  Situation(const Rule& rule, std::size_t index, std::size_t divider);

  bool operator==(const Situation& other) const;

  char dividerCurrent() const;

  const Rule& rule;
  std::size_t index;
  std::size_t divider;
};

struct ParserEarley::SituationHash {
  std::size_t operator()(const Situation& situation) const;
};
