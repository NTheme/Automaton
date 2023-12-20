#include "CDFA.hpp"

#include "DFA.hpp"

CDFA::CDFA(const Expression& expression) : CDFA(DFA(expression)) {}

CDFA::CDFA(const NFA& nfa) : CDFA(DFA(nfa)) {}

CDFA::CDFA(const DFA& dfa) : CDFA(dfa.getSize() + 1, dfa.getAlphabet()) {
  for (size_t vertex = 0; vertex < dfa.getSize(); ++vertex) {
    for (size_t index = 0; index < m_alphabet.size(); ++index) {
      m_transitions[vertex][index] = dfa.getSize();
      for (const auto& [from, symb, to] : dfa.getTransitions()) {
        if (from == vertex && symb[0] == m_alphabet[index]) {
          m_transitions[vertex][index] = to;
          break;
        }
      }
    }
  }

  for (size_t vertex : dfa.getFinalStates()) {
    m_final[vertex] = true;
  }
  for (size_t index = 0; index < m_alphabet.size(); ++index) {
    m_transitions[dfa.getSize()][index] = dfa.getSize();
  }

  *this = minimize();
}

std::vector<std::tuple<size_t, std::string, size_t>> CDFA::getTransitions() const {
  std::vector<std::tuple<size_t, std::string, size_t>> res;
  for (size_t vertex = 0; vertex < m_transitions.size(); ++vertex) {
    for (size_t index = 0; index < m_alphabet.size(); ++index) {
      res.emplace_back(vertex, std::string(1, m_alphabet[index]), m_transitions[vertex][index]);
    }
  }
  return res;
}

size_t CDFA::getSize() const { return m_transitions.size(); }

CDFA CDFA::operator~() const {
  CDFA res = *this;
  for (size_t index = 0; index < res.m_final.size(); ++index) {
    res.m_final[index] = !res.m_final[index];
  }
  return res;
}

CDFA CDFA::minimize() const {
  EquivalenceRelation relation = findEquivalentStates();
  CDFA res(relation.equivalenceClasses.size(), m_alphabet);

  for (size_t index = 0; index < relation.equivalenceClasses.size(); ++index) {
    size_t vertex = relation.equivalenceClasses[index][0];
    for (size_t j = 0; j < m_alphabet.size(); ++j) {
      res.m_transitions[index][j] = relation.classIndex[m_transitions[vertex][j]];
    }
    res.m_final[index] = m_final[vertex];
  }

  return res;
}

CDFA::CDFA(size_t size, const std::string& alphabet)
    : Automaton(size, alphabet), m_transitions(size, std::vector<size_t>(alphabet.size())) {}

CDFA::EquivalenceRelation CDFA::buildInitialRelation() const {
  EquivalenceRelation relation;
  relation.classIndex.resize(getSize());
  relation.equivalenceClasses.resize(2);

  for (size_t vertex = 0; vertex < getSize(); ++vertex) {
    relation.classIndex[vertex] = m_final[vertex];
    relation.equivalenceClasses[relation.classIndex[vertex]].push_back(vertex);
  }

  return relation;
}

CDFA::EquivalenceRelation CDFA::findEquivalentStates() const {
  EquivalenceRelation relation = buildInitialRelation();
  std::vector<bool> isDistinguishable(getSize());

  for (bool changed = true; changed;) {
    changed = false;
    for (auto& states : relation.equivalenceClasses) {
      for (size_t index = 0; index < m_alphabet.size(); ++index) {
        size_t destClass = relation.classIndex[m_transitions[states[0]][index]];
        bool split = false;
        for (size_t index = 1; index < states.size(); ++index) {
          size_t curClass = relation.classIndex[m_transitions[states[index]][index]];
          isDistinguishable[states[index]] = curClass != destClass;
          if (isDistinguishable[states[index]]) {
            split = true;
          }
        }
        if (!split) {
          continue;
        }

        changed = true;
        size_t writeIdx = 0;
        std::vector<size_t> newClass;
        for (size_t vertex : states) {
          if (isDistinguishable[vertex]) {
            newClass.push_back(vertex);
          } else {
            states[writeIdx++] = vertex;
          }
          isDistinguishable[vertex] = false;
        }

        states.resize(writeIdx);
        for (size_t vertex : newClass) {
          relation.classIndex[vertex] = relation.equivalenceClasses.size();
        }
        relation.equivalenceClasses.push_back(std::move(newClass));
        break;
      }
      if (changed) {
        break;
      }
    }
  }

  return relation;
}
