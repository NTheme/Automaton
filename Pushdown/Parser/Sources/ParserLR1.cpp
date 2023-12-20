/******************************************
 *  Author : NThemeDEV
 *  Created : Mon Dec 9 2023
 *  File : ParserLR1.cpp
 ******************************************/

#include "ParserLR1.hpp"

void ParserLR1::fit(const Grammar& grammar) {
  m_grammar = grammar;
  buildFirst();

  Situation start(m_grammar.rules().at(m_grammar.alphabet().utils[0]).back(), 0, {m_grammar.alphabet().utils[1]});
  m_vertices.emplace(0, Vertex(buildClosure(start)));

  std::queue<std::pair<Vertex, std::size_t>> stack;
  stack.push({m_vertices[0], 0});

  while (!stack.empty()) {
    action(stack.front(), stack);
    stack.pop();
  }

  buildTable();
}

bool ParserLR1::predict(const std::string& word) const {
  std::stack<std::pair<std::size_t, char>> stack;
  stack.push({0, m_grammar.alphabet().utils[0]});
  std::string n_word = word + m_grammar.alphabet().utils[1];

  for (const auto& symb : n_word) {
    while (true) {
      std::pair<std::size_t, char> row = stack.top();
      if (!m_table.at(row.first).contains(symb)) {
        return false;
      }
      Cell dest = m_table.at(row.first).at(symb);
      if (dest == Cell(Cell::Type::REDUCE, 0) && symb == m_grammar.alphabet().utils[1]) {
        return true;
      }
      if (dest.type == Cell::Type::SHIFT) {
        stack.push({dest.index, symb});
        break;
      }
      const Rule& rule = m_grammar.getByID(dest.index);
      if (!rule.rhs().empty()) {
        if (stack.size() <= rule.rhs().size()) {
          return false;
        }
        std::string cleared;
        for (std::size_t index = 0; index < rule.rhs().size(); ++index) {
          cleared.push_back(stack.top().second);
          stack.pop();
        }
        std::reverse(cleared.begin(), cleared.end());
        if (cleared != rule.rhs()) {
          return false;
        }
      }
      row = stack.top();
      if (!m_table.at(row.first).contains(rule.lhs())) {
        return false;
      }
      stack.push({m_table.at(row.first).at(rule.lhs()).index, rule.lhs()});
    }
  }

  return false;
}

void ParserLR1::buildFirst() {
  for (const char& symb : m_grammar.alphabet().nterm) {
    std::unordered_set<Rule, RuleHash> checked;
    setFirst(symb, symb, checked);
  }
}

void ParserLR1::setFirst(char fir_symb, char cur_symb, std::unordered_set<Rule, RuleHash>& checked) {
  if (m_grammar.alphabet().yterm.contains(cur_symb)) {
    m_first[fir_symb].push_back(cur_symb);
  } else if (m_grammar.alphabet().nterm.contains(cur_symb)) {
    for (const auto& rule : m_grammar.rules().at(cur_symb)) {
      if (checked.contains(rule)) {
        return;
      }
      checked.insert(rule);
      if (rule.rhs().size() > 0) {
        setFirst(fir_symb, rule.rhs()[0], checked);
      }
    }
  }
}

std::unordered_set<ParserLR1::Situation, ParserLR1::SituationHash> ParserLR1::buildClosure(const Situation& start) {
  std::unordered_map<std::string, std::unordered_set<ParserLR1::Situation, ParserLR1::SituationHash>> situations;
  situations["proc"].insert(start);
  while (!situations["proc"].empty()) {
    Situation current = *situations["proc"].begin();
    situations["proc"].erase(situations["proc"].begin());
    if (situations["done"].contains(current)) {
      continue;
    }
    if (m_grammar.alphabet().yterm.contains(current.dividerCurrent())) {
      situations["done"].insert(current);
      continue;
    }
    if (!current.dividerIsFinished()) {
      for (const auto& rule : m_grammar.rules().at(current.dividerCurrent())) {
        if (current.dividerIsLast()) {
          situations["proc"].emplace(rule, 0, current.follow);
        } else if (m_grammar.alphabet().nterm.contains(current.dividerNext())) {
          situations["proc"].emplace(rule, 0, m_first[current.dividerNext()]);
        } else {
          situations["proc"].emplace(rule, 0, std::string(1, current.dividerNext()));
        }
      }
    }
    situations["done"].insert(current);
  }
  return situations["done"];
}

void ParserLR1::action(const std::pair<Vertex, std::size_t> ver, std::queue<std::pair<Vertex, std::size_t>>& stack) {
  for (const auto& symb : m_grammar.alphabet().yterm) {
    actionInner(ver, stack, symb);
  }
  for (const auto& symb : m_grammar.alphabet().nterm) {
    actionInner(ver, stack, symb);
  }

  for (const auto& situation : ver.first.situations) {
    if (situation.dividerIsFinished()) {
      m_vertices[ver.second].empties.insert(situation);
    }
  }
}

void ParserLR1::actionInner(const std::pair<Vertex, std::size_t> ver, std::queue<std::pair<Vertex, std::size_t>>& stack,
                            char symb) {
  std::unordered_set<Situation, ParserLR1::SituationHash> new_situations;
  for (const auto& situation : ver.first.situations) {
    if (situation.dividerCurrent() == symb) {
      auto closure = buildClosure(situation.nextSituation());
      new_situations.insert(closure.begin(), closure.end());
    }
  }
  if (!new_situations.empty()) {
    if (findUMapKey(m_vertices, Vertex(new_situations))) {
      m_vertices[ver.second].routines[symb] = getUMapKey(m_vertices, Vertex(new_situations));
    } else {
      m_vertices[ver.second].routines[symb] = m_vertices.size();
      m_vertices[m_vertices.size()] = Vertex(new_situations);
      stack.push({m_vertices[m_vertices.size() - 1], m_vertices.size() - 1});
    }
  }
}

void ParserLR1::buildTable() {
  for (const auto& [key, val] : m_vertices) {
    for (const auto& [route, dest] : val.routines) {
      m_table[key].emplace(route, Cell(Cell::Type::SHIFT, dest));
    }
  }
  for (const auto& [key, val] : m_vertices) {
    for (const auto& empty : val.empties) {
      for (const auto& next : empty.follow) {
        if (m_table[key].contains(next) && m_table[key].at(next) != Cell(Cell::Type::REDUCE, empty.rule.id())) {
          throw std::logic_error("Not LR(1) grammar!");
        }
        m_table[key].emplace(next, Cell(Cell::Type::REDUCE, empty.rule.id()));
      }
    }
  }
}

template <typename Key, typename Value>
bool ParserLR1::findUMapKey(const std::unordered_map<Key, Value>& map, const Value& value) {
  for (const auto& [key, val] : map) {
    if (val == value) {
      return true;
    }
  }
  return false;
}

template <typename Key, typename Value>
Key ParserLR1::getUMapKey(const std::unordered_map<Key, Value>& map, const Value& value) {
  for (const auto& [key, val] : map) {
    if (val == value) {
      return key;
    }
  }
  throw std::out_of_range("Not found!");
}

ParserLR1::Situation::Situation(const Rule& n_rule, std::size_t n_divider, const std::string& n_follow)
    : rule(n_rule), divider(n_divider), follow(n_follow) {}

bool ParserLR1::Situation::operator==(const Situation& other) const {
  return rule == other.rule && divider == other.divider && follow == other.follow;
}

bool ParserLR1::Situation::dividerIsFinished() const { return divider == rule.rhs().size(); }

bool ParserLR1::Situation::dividerIsLast() const { return divider >= std::max(rule.rhs().size(), 1UL) - 1; }

char ParserLR1::Situation::dividerCurrent() const { return rule.rhs()[divider]; }

char ParserLR1::Situation::dividerNext() const { return rule.rhs()[divider + 1]; }

ParserLR1::Situation ParserLR1::Situation::nextSituation() const {
  return Situation(rule, std::min(divider + 1, rule.rhs().size()), follow);
}

std::size_t ParserLR1::SituationHash::operator()(const Situation& situation) const {
  RuleHash rule;
  return std::hash<std::size_t>()(situation.divider + std::hash<std::string>()(situation.follow) +
                                  rule(situation.rule));
}

ParserLR1::Vertex::Vertex(const std::unordered_set<ParserLR1::Situation, ParserLR1::SituationHash>& n_situations)
    : situations(n_situations) {}

bool ParserLR1::Vertex::operator==(const Vertex& other) const { return situations == other.situations; }

ParserLR1::Cell::Cell(Type n_type, std::size_t n_index) : type(n_type), index(n_index) {}
