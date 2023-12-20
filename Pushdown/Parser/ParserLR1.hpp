/******************************************
 *  Author : NThemeDEV
 *  Created : Mon Dec 10 2023
 *  File : ParserLR1.hpp
 ******************************************/

#pragma once

#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "Parser.hpp"

class ParserLR1 : public Parser {
 public:
  void fit(const Grammar& grammar) final;
  bool predict(const std::string& word) const final;

 private:
  struct Situation;
  struct SituationHash;
  struct Vertex;
  struct Cell;

  std::unordered_map<std::size_t, std::unordered_map<char, Cell>> m_table;
  std::unordered_map<char, std::string> m_first;
  std::unordered_map<std::size_t, Vertex> m_vertices;

  void buildFirst();
  void setFirst(char fir_symb, char cur_symb, std::unordered_set<Rule, RuleHash>& checked);
  std::unordered_set<ParserLR1::Situation, ParserLR1::SituationHash> buildClosure(const Situation& situation);
  void action(const std::pair<Vertex, std::size_t> ver, std::queue<std::pair<Vertex, std::size_t>>& stack);
  void actionInner(const std::pair<Vertex, std::size_t> ver, std::queue<std::pair<Vertex, std::size_t>>& stack,
                   char symb);
  void buildTable();

  template <typename Key, typename Value>
  bool findUMapKey(const std::unordered_map<Key, Value>& map, const Value& value);

  template <typename Key, typename Value>
  Key getUMapKey(const std::unordered_map<Key, Value>& map, const Value& value);
};

struct ParserLR1::Situation {
  Situation(const Rule& n_rule, std::size_t n_divider, const std::string& n_follow);
  bool operator==(const Situation& other) const;

  bool dividerIsFinished() const;
  bool dividerIsLast() const;
  char dividerCurrent() const;
  char dividerNext() const;

  Situation nextSituation() const;

  const Rule& rule;
  std::size_t divider;
  std::string follow;
};

struct ParserLR1::SituationHash {
  std::size_t operator()(const Situation& situation) const;
};

struct ParserLR1::Vertex {
  Vertex() = default;
  Vertex(const std::unordered_set<ParserLR1::Situation, ParserLR1::SituationHash>& n_situations);

  bool operator==(const Vertex& other) const;

  std::unordered_set<ParserLR1::Situation, ParserLR1::SituationHash> situations;
  std::unordered_map<char, std::size_t> routines;
  std::unordered_set<ParserLR1::Situation, ParserLR1::SituationHash> empties;
};

struct ParserLR1::Cell {
  enum class Type;
  Cell(Type n_type, std::size_t n_index);

  bool operator==(const Cell& other) const = default;

  Type type;
  std::size_t index;
};

enum class ParserLR1::Cell::Type { REDUCE, SHIFT };
