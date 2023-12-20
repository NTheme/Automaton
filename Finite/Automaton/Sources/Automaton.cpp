#include "Automaton.hpp"

Automaton::Automaton(size_t size, const std::string& alphabet) : m_final(size), m_alphabet(alphabet) {}

std::vector<size_t> Automaton::getFinalStates() const {
  std::vector<size_t> res;
  for (size_t vertex = 0; vertex < m_final.size(); ++vertex) {
    if (m_final[vertex]) {
      res.push_back(vertex);
    }
  }
  return res;
}

std::string Automaton::getAlphabet() const { return m_alphabet; }

std::ostream& operator<<(std::ostream& out, const Automaton& automaton) {
  if (automaton.getSize() == 0) {
    return out;
  }

  out << "\nStart state: 0\n";
  out << "Final states: ";
  for (const auto& v : automaton.getFinalStates()) {
    out << v << ' ';
  }

  out << "\n\nTransitions (format: \"from letter to\"):\n";
  for (const auto& [from, s, to] : automaton.getTransitions()) {
    out << from << ' ';
    out << (s.empty() ? "-" : s.c_str()) << ' ';
    out << to << '\n';
  }

  return out;
}
