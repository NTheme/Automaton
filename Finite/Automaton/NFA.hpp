#pragma once

#include <stack>

#include "Automaton.hpp"

class Expression;

class NFA : public Automaton {
 public:
  NFA(size_t n, const std::string& alphabet);
  explicit NFA(const Expression& expression);

  void addTransition(const std::tuple<size_t, std::string, size_t>& transition);
  void addFinalState(size_t state);

  std::vector<std::tuple<size_t, std::string, size_t>> getTransitions() const final;
  size_t getSize() const final;

  NFA throwEpsilon() const;

  NFA& operator+=(const NFA& other);
  NFA& operator*=(const NFA& other);

  NFA operator+(const NFA& other) const;
  NFA operator*(const NFA& other) const;
  NFA operator*() const;

 private:
  std::vector<std::map<std::string, std::vector<size_t>>> m_transitions;

  enum class VertexColor;

  struct DFSData;
  struct Condensation;

  static void DFS(const std::vector<std::vector<size_t>>& graph, size_t v, DFSData& data);
  static Condensation condensate(const std::vector<std::vector<size_t>>& graph);
};

enum class NFA::VertexColor { White, Grey, Black, Purple };

struct NFA::DFSData {
  std::vector<VertexColor> vertexColors;
  std::vector<size_t> timeIn;
  std::vector<size_t> upTime;
  std::vector<size_t> vertexComponents;
  size_t timer;
  size_t componentCnt;
  std::stack<size_t> vertexStack;

  explicit DFSData(size_t n);
};

struct NFA::Condensation {
  size_t componentCnt;
  std::vector<size_t> vertexComponents;
};
