#pragma once

#include <memory>

class Automaton;
class NFA;

class Expression {
 public:
  explicit Expression(std::string str);
  explicit Expression(const Automaton& automaton);

  Expression& operator+=(const Expression& other);
  Expression& operator*=(const Expression& other);

  Expression operator+(const Expression& other) const;
  Expression operator*(const Expression& other) const;
  Expression operator*() const;

  NFA toNFA() const;
  std::string toString() const;

 private:
  enum class NodeType;
  struct Node;
  using NodePtr = std::shared_ptr<const Node>;

  NodePtr m_root;

  Expression();
  Expression(NodePtr&& ptr);

  static std::string prepareExpression(NodeType type, const NodePtr& node);
  static std::string buildString(const NodePtr& node);
  
  static NFA buildNFA(const NodePtr& node);

  static NodePtr add(const NodePtr& left,
                                         const NodePtr& right);
  static NodePtr multiply(const NodePtr& left,
                                              const NodePtr& right);
  static NodePtr star(const NodePtr& node);

  static NodePtr parseExpression(const std::string& str, size_t& ind);
  static NodePtr parseConcatenation(const std::string& str, size_t& ind);
  static NodePtr parseElement(const std::string& str, size_t& ind);
  static NodePtr parsePrimitive(const std::string& str, size_t& ind);
};

enum class Expression::NodeType { Unity, Symbol, Star, Product, Sum };

struct Expression::Node {
  NodeType type;
  char sym;
  NodePtr left;
  NodePtr right;

  Node();
  explicit Node(char symb);
  explicit Node(const NodePtr& node);
  Node(NodeType type, const NodePtr& left, const NodePtr& right);
};

std::ostream& operator<<(std::ostream& out, const Expression& expression);
