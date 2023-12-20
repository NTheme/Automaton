#include "NFA.hpp"

#include <algorithm>
#include <set>

#include "Expression.hpp"

NFA::NFA(size_t size, const std::string& alphabet) : Automaton(size, alphabet), m_transitions(size) {}

NFA::NFA(const Expression& expression) : NFA(expression.toNFA()) {}

void NFA::addTransition(const std::tuple<size_t, std::string, size_t>& transition) {
  m_transitions[std::get<0>(transition)][std::get<1>(transition)].push_back(std::get<2>(transition));
}

void NFA::addFinalState(size_t state) { m_final[state] = true; }

std::vector<std::tuple<size_t, std::string, size_t>> NFA::getTransitions() const {
  std::vector<std::tuple<size_t, std::string, size_t>> res;

  for (size_t from = 0; from < m_transitions.size(); ++from) {
    for (const auto& [s, destinations] : m_transitions[from]) {
      for (auto to : destinations) {
        res.emplace_back(from, s, to);
      }
    }
  }

  return res;
}

size_t NFA::getSize() const { return m_transitions.size(); }

NFA NFA::throwEpsilon() const {
  std::vector<std::vector<size_t>> epsilonGraph(m_transitions.size());
  std::vector<std::vector<std::pair<std::string, size_t>>> newTransitions(m_transitions.size());
  for (const auto& [from, s, to] : getTransitions()) {
    if (s.empty()) {
      epsilonGraph[from].push_back(to);
    } else {
      newTransitions[from].emplace_back(s, to);
    }
  }

  auto [num_components, components] = condensate(epsilonGraph);
  std::vector<bool> hasFinal(num_components);
  for (size_t vertex : getFinalStates()) {
    hasFinal[components[vertex]] = true;
  }

  std::vector<std::vector<size_t>> componentVertices(m_transitions.size());
  for (size_t vertex = 0; vertex < m_transitions.size(); ++vertex) {
    componentVertices[components[vertex]].push_back(vertex);
  }

  std::vector<std::set<std::pair<std::string, size_t>>> componentTransitions(num_components);
  for (size_t index = 0; index < num_components; ++index) {
    for (size_t vertex : componentVertices[index]) {
      componentTransitions[index].insert(newTransitions[vertex].begin(), newTransitions[vertex].end());
      for (const auto& to : epsilonGraph[vertex]) {
        if (components[to] == index) {
          continue;
        }

        componentTransitions[index].insert(componentTransitions[components[to]].begin(),
                                           componentTransitions[components[to]].end());
        if (hasFinal[components[to]]) hasFinal[index] = true;
      }
    }
  }

  NFA res(m_transitions.size(), m_alphabet);
  for (size_t vertex = 0; vertex < m_transitions.size(); ++vertex) {
    for (const auto& [s, to] : componentTransitions[components[vertex]]) {
      res.addTransition({vertex, s, to});
    }
  }
  for (size_t index = 0; index < num_components; ++index) {
    if (!hasFinal[index]) {
      continue;
    }

    for (const auto& vertex : componentVertices[index]) {
      res.addFinalState(vertex);
    }
  }
  return res;
}

NFA& NFA::operator+=(const NFA& other) { return *this = *this + other; }

NFA& NFA::operator*=(const NFA& other) { return *this = *this * other; }

NFA NFA::operator+(const NFA& other) const {
  std::string alphabet = m_alphabet + other.m_alphabet;
  alphabet.resize(std::unique(alphabet.begin(), alphabet.end()) - alphabet.begin());
  NFA res(m_transitions.size() + other.m_transitions.size() + 1, alphabet);

  for (const auto& [from, s, to] : getTransitions()) {
    res.addTransition({from + 1, s, to + 1});
  }
  for (const auto& [from, s, to] : other.getTransitions()) {
    res.addTransition({from + m_transitions.size() + 1, s, to + m_transitions.size() + 1});
  }
  res.addTransition({0, "", 1});
  res.addTransition({0, "", m_transitions.size() + 1});

  for (size_t vertex : getFinalStates()) {
    res.addFinalState(vertex + 1);
  }
  for (size_t vertex : other.getFinalStates()) {
    res.addFinalState(vertex + m_transitions.size() + 1);
  }

  return res;
}

NFA NFA::operator*(const NFA& other) const {
  std::string alphabet = m_alphabet + other.m_alphabet;
  alphabet.resize(std::unique(alphabet.begin(), alphabet.end()) - alphabet.begin());
  NFA res(m_transitions.size() + other.m_transitions.size(), alphabet);

  for (const auto& [from, s, to] : getTransitions()) {
    res.addTransition({from, s, to});
  }
  for (const auto& [from, s, to] : other.getTransitions()) {
    res.addTransition({from + m_transitions.size(), s, to + m_transitions.size()});
  }
  for (const auto& vertex : getFinalStates()) {
    res.addTransition({vertex, "", m_transitions.size()});
  }
  for (const auto& vertex : other.getFinalStates()) {
    res.addFinalState(vertex + m_transitions.size());
  }

  return res;
}

NFA NFA::operator*() const {
  NFA res(m_transitions.size() + 1, m_alphabet);

  for (const auto& [from, s, to] : getTransitions()) {
    res.addTransition({from + 1, s, to + 1});
  }
  for (const auto& vertex : getFinalStates()) {
    res.addTransition({vertex + 1, "", 0});
  }
  res.addTransition({0, "", 1});
  res.addFinalState(0);

  return res;
}

NFA::DFSData::DFSData(size_t size)
    : vertexColors(size, VertexColor::White),
      timeIn(size),
      upTime(size),
      vertexComponents(size),
      timer(0),
      componentCnt(0),
      vertexStack() {}

void NFA::DFS(const std::vector<std::vector<size_t>>& graph, size_t vertex, DFSData& data) {
  data.vertexColors[vertex] = VertexColor::Grey;
  data.upTime[vertex] = data.timeIn[vertex] = data.timer++;
  data.vertexStack.push(vertex);

  for (size_t to : graph[vertex]) {
    if (data.vertexColors[to] == VertexColor::White) {
      DFS(graph, to, data);
      data.upTime[vertex] = std::min(data.upTime[vertex], data.upTime[to]);
    } else if (data.vertexColors[to] != VertexColor::Purple) {
      data.upTime[vertex] = std::min(data.upTime[vertex], data.timeIn[to]);
    }
  }

  data.vertexColors[vertex] = VertexColor::Black;
  if (data.timeIn[vertex] == data.upTime[vertex]) {
    while (data.vertexStack.top() != vertex) {
      size_t u = data.vertexStack.top();
      data.vertexStack.pop();
      data.vertexComponents[u] = data.componentCnt;
      data.vertexColors[u] = VertexColor::Purple;
    }
    data.vertexStack.pop();
    data.vertexComponents[vertex] = data.componentCnt++;
    data.vertexColors[vertex] = VertexColor::Purple;
  }
}

NFA::Condensation NFA::condensate(const std::vector<std::vector<size_t>>& graph) {
  size_t size = graph.size();
  DFSData data(size);
  for (size_t vertex = 0; vertex < size; ++vertex) {
    if (data.vertexColors[vertex] == VertexColor::White) {
      DFS(graph, vertex, data);
    }
  }
  return {data.componentCnt, data.vertexComponents};
}
