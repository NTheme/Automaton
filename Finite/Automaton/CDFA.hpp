/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Oct 28 2023
 *  File : CDFA.hpp
 ******************************************/

#pragma once

#include "Automaton.hpp"

class Expression;
class NFA;
class DFA;

class CDFA : public Automaton {
 public:
  explicit CDFA(const Expression& expression);
  explicit CDFA(const NFA& nfa);
  explicit CDFA(const DFA& dfa);

  std::vector<std::tuple<size_t, std::string, size_t>> getTransitions() const final;
  size_t getSize() const final;

  CDFA operator~() const;

  CDFA minimize() const;

 private:
  struct EquivalenceRelation;

  std::vector<std::vector<size_t>> m_transitions;

  CDFA(size_t size, const std::string& alphabet);

  EquivalenceRelation buildInitialRelation() const;
  EquivalenceRelation findEquivalentStates() const;
};

struct CDFA::EquivalenceRelation {
  std::vector<size_t> classIndex;
  std::vector<std::vector<size_t>> equivalenceClasses;
};
