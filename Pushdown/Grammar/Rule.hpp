/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Nov 28 2023
 *  File : Rule.hpp
 ******************************************/

#pragma once

#include <iostream>

class Rule {
 public:
  Rule(char lhs, const std::string& rhs, std::size_t id = 0);
  bool operator==(const Rule& other) const = default;

  const char& lhs() const;
  const std::string& rhs() const;
  const std::size_t& id() const;

 private:
  char m_lhs;
  std::string m_rhs;
  std::size_t m_id;
};

struct RuleHash {
  std::size_t operator()(const Rule& rule) const;
};
