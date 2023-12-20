#include "DFA.hpp"

#include <algorithm>
#include <queue>

#include "NFA.hpp"

DFA::DFA(const Expression& expression) : DFA(NFA(expression)) {}

DFA::DFA(const NFA& nfa) : Automaton(0, nfa.getAlphabet()) {
  NFA without_eps = nfa.throwEpsilon();

  size_t size = without_eps.getSize();
  std::vector<std::vector<std::pair<char, size_t>>> transitions;
  for (const auto& [from, s, to] : without_eps.getTransitions()) {
    for (size_t index = 0; index < s.length(); ++index) {
      size_t start = index == 0 ? from : size - 1;
      size_t end = index + 1 == s.length() ? to : size++;
      transitions.resize(size);
      transitions[start].emplace_back(s[index], end);
    }
  }

  std::map<std::vector<size_t>, size_t> indexes{{{0}, 0}};
  std::queue<std::vector<size_t>> queue;
  queue.emplace(1, 0);

  std::vector<std::tuple<size_t, char, size_t>> newTransitions;
  while (!queue.empty()) {
    std::map<char, std::vector<size_t>> currentTransitions;
    for (const auto& from : queue.front()) {
      for (auto [symb, to] : transitions[from]) {
        currentTransitions[symb].push_back(to);
      }
    }
    size_t v = indexes[queue.front()];
    queue.pop();
    for (auto& [symb, dest] : currentTransitions) {
      std::sort(dest.begin(), dest.end());
      dest.resize(std::unique(dest.begin(), dest.end()) - dest.begin());
      auto it = indexes.find(dest);
      if (it == indexes.end()) {
        newTransitions.emplace_back(v, symb, indexes.size());
        indexes.emplace(dest, indexes.size());
        queue.push(dest);
      } else {
        newTransitions.emplace_back(v, symb, it->second);
      }
    }
  }

  m_transitions.resize(indexes.size());
  m_final.resize(indexes.size());
  for (auto [from, symb, to] : newTransitions) {
    m_transitions[from][symb] = to;
  }

  std::vector<bool> isFinal(transitions.size());
  for (const auto& v : without_eps.getFinalStates()) {
    isFinal[v] = true;
  }
  for (const auto& [vertices, index] : indexes) {
    bool final = false;
    for (const auto& v : vertices) {
      if (isFinal[v]) {
        final = true;
        break;
      }
    }
    if (final) {
      m_final[index] = true;
    }
  }
}

std::vector<std::tuple<size_t, std::string, size_t>> DFA::getTransitions() const {
  std::vector<std::tuple<size_t, std::string, size_t>> res;
  for (size_t from = 0; from < m_transitions.size(); ++from) {
    for (const auto& [symb, to] : m_transitions[from]) {
      res.emplace_back(from, std::string(1, symb), to);
    }
  }
  return res;
}

size_t DFA::getSize() const { return m_transitions.size(); }

bool DFA::checkWord(const std::string& s) const {
  size_t st = 0;
  for (char symb : s) {
    auto it = m_transitions[st].find(symb);
    if (it == m_transitions[st].end()) {
      return false;
    }
    st = it->second;
  }
  return m_final[st];
}
