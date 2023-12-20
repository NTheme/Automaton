/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Oct 28 2023
 *  File : CDFA.hpp
 ******************************************/

#pragma once

#include "Automaton.hpp"
  
class Expression;
class NFA;

class DFA : public Automaton {
 public:
  explicit DFA(const Expression& expression);
  explicit DFA(const NFA& nfa);

  std::vector<std::tuple<size_t, std::string, size_t>> getTransitions() const final;
  size_t getSize() const final;

  bool checkWord(const std::string& s) const;

 private:
  std::vector<std::map<char, size_t>> m_transitions;
};
