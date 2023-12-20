#include "Expression.hpp"

#include <algorithm>
#include <unordered_set>

#include "NFA.hpp"

Expression::Expression() : m_root(std::make_shared<Node>()) {}

Expression::Expression(std::string str) {
  size_t writeIdx = 0;
  for (char symb : str) {
    if (isspace(symb)) {
      continue;
    }
    str[writeIdx++] = symb;
  }
  str.resize(writeIdx);

  size_t ind = 0;
  m_root = parseExpression(str, ind);
  if (ind < str.size()) {
    throw std::invalid_argument("Incorrect input string");
  }
}

Expression::Expression(const Automaton& automaton) {
  std::vector<std::vector<Expression>> matrix(automaton.getSize() + 2,
                                              std::vector<Expression>(automaton.getSize() + 2, Expression(NodePtr())));
  matrix[automaton.getSize()][0] = Expression();

  for (const auto& v : automaton.getFinalStates()) {
    matrix[v][automaton.getSize() + 1] = Expression();
  }
  for (const auto& [from, str, to] : automaton.getTransitions()) {
    Expression exp;
    for (const auto& symb : str) {
      exp.m_root = multiply(exp.m_root, std::make_shared<Node>(symb));
    }
    matrix[from][to] += exp;
  }

  for (size_t v = 0; v < automaton.getSize(); ++v) {
    Expression loop = *matrix[v][v];
    for (size_t from = 0; from < matrix.size(); ++from) {
      if (from == v) {
        continue;
      }
      for (size_t to = 0; to < matrix[from].size(); ++to) {
        if (to == v) {
          continue;
        }
        matrix[from][to] += matrix[from][v] * loop * matrix[v][to];
      }
    }
  }

  *this = matrix[automaton.getSize()][automaton.getSize() + 1];
}

Expression& Expression::operator+=(const Expression& other) { return *this = *this + other; }

Expression& Expression::operator*=(const Expression& other) { return *this = *this * other; }

Expression Expression::operator+(const Expression& other) const { return {add(m_root, other.m_root)}; }

Expression Expression::operator*(const Expression& other) const { return {multiply(m_root, other.m_root)}; }

Expression Expression::operator*() const { return {star(m_root)}; }

std::string Expression::toString() const { return buildString(m_root); }

Expression::Expression(NodePtr&& ptr) : m_root(std::move(ptr)) {}

std::string Expression::prepareExpression(NodeType type, const NodePtr& node) {
  std::string res = buildString(node);
  if (node->type > type) {
    res = "(" + res + ")";
  }
  return res;
}

std::string Expression::buildString(const NodePtr& node) {
  if (!node) {
    return "0";
  }
  switch (node->type) {
    case NodeType::Unity:
      return "1";
    case NodeType::Symbol:
      return {node->sym};
    case NodeType::Star:
      return prepareExpression(node->type, node->left) + '*';
    case NodeType::Sum:
      return prepareExpression(node->type, node->left) + "+" + prepareExpression(node->type, node->right);
    case NodeType::Product:
      return prepareExpression(node->type, node->left) + prepareExpression(node->type, node->right);
      break;
    default:
      throw std::runtime_error("Unknown NodeType");
  }
}

NFA Expression::toNFA() const {
  std::unordered_set<char> alphas;
  for (const auto& symb : toString()) {
    if (std::isalpha(symb)) {
      alphas.insert(symb);
    }
  }

  std::string alphabet;
  for (const auto& symb : alphas) {
    alphabet.push_back(symb);
  }
  std::sort(alphabet.begin(), alphabet.end());

  NFA empty = buildNFA(m_root);

  NFA ret(empty.getSize(), alphabet);
  for (const auto& state : empty.getFinalStates()) {
    ret.addFinalState(state);
  }
  for (const auto& transition : empty.getTransitions()) {
    ret.addTransition(transition);
  }

  return ret;
}

NFA Expression::buildNFA(const NodePtr& node) {
  if (!node) {
    return NFA(0, "");
  }
  switch (node->type) {
    case NodeType::Unity: {
      NFA unity(1, "");
      unity.addFinalState(0);
      return unity;
    }
    case NodeType::Symbol: {
      NFA symbol(2, "");
      symbol.addTransition({0, std::string(1, node->sym), 1});
      symbol.addFinalState(1);
      return symbol;
    }
    case NodeType::Star:
      return *buildNFA(node->left);
    case NodeType::Sum:
      return buildNFA(node->left) + buildNFA(node->right);
    case NodeType::Product:
      return buildNFA(node->left) * buildNFA(node->right);
      break;
    default:
      throw std::runtime_error("Unknown NodeType");
  }
}

Expression::NodePtr Expression::add(const NodePtr& left, const NodePtr& right) {
  if (!left) {
    return right;
  }
  if (!right) {
    return left;
  }
  if (left->type == NodeType::Unity && right->type == NodeType::Unity) {
    return left;
  }
  return std::make_shared<const Node>(NodeType::Sum, left, right);
}

Expression::NodePtr Expression::multiply(const NodePtr& left, const NodePtr& right) {
  if (!left || !right) {
    return nullptr;
  }
  if (left->type == NodeType::Unity) {
    return right;
  }
  if (right->type == NodeType::Unity) {
    return left;
  }
  return std::make_shared<const Node>(NodeType::Product, left, right);
}

Expression::NodePtr Expression::star(const NodePtr& node) {
  if (!node) {
    return std::make_shared<const Node>();
  }
  if (node->type == NodeType::Unity || node->type == NodeType::Star) {
    return node;
  }
  return std::make_shared<const Node>(node);
}

Expression::NodePtr Expression::parseExpression(const std::string& str, size_t& ind) {
  NodePtr res = parseConcatenation(str, ind);
  if (ind < str.size() && str[ind] == '+') {
    res = add(res, parseExpression(str, ++ind));
  }
  return res;
}

Expression::NodePtr Expression::parseConcatenation(const std::string& str, size_t& ind) {
  NodePtr res = parseElement(str, ind);
  if (ind < str.size() && str[ind] == '.') {
    res = multiply(res, parseConcatenation(str, ++ind));
  }
  return res;
}

Expression::NodePtr Expression::parseElement(const std::string& str, size_t& ind) {
  NodePtr res = parsePrimitive(str, ind);
  if (ind < str.size() && str[ind] == '*') {
    res = star(res);
    ++ind;
  }
  return res;
}

Expression::NodePtr Expression::parsePrimitive(const std::string& str, size_t& ind) {
  if (ind == str.size()) throw std::invalid_argument("Incorrect input string");
  NodePtr res;
  if (str[ind] == '(') {
    res = parseExpression(str, ++ind);
    if (ind == str.size() || str[ind] != ')') {
      throw std::invalid_argument("Incorrect input string");
    }
  } else if (str[ind] == '1') {
    res = std::make_shared<Node>();
  } else if (('a' <= str[ind] && str[ind] <= 'z') || ('A' <= str[ind] && str[ind] <= 'Z')) {
    res = std::make_shared<Node>(str[ind]);
  } else if (str[ind] != '0') {
    throw std::invalid_argument("Incorrect input string");
  }
  ++ind;

  return res;
}

Expression::Node::Node() : type(NodeType::Unity), sym() {}

Expression::Node::Node(char symb) : type(NodeType::Symbol), sym(symb) {}

Expression::Node::Node(NodeType type, const NodePtr& left, const NodePtr& right)
    : type(type), sym(), left(left), right(right) {}

Expression::Node::Node(const NodePtr& node) : type(NodeType::Star), sym(), left(node), right() {}

std::ostream& operator<<(std::ostream& out, const Expression& expression) {
  out << expression.toString();
  return out;
}
