/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Nov 28 2023
 *  File : Rule.cpp
 ******************************************/

#include "Rule.hpp"

Rule::Rule(char lhs, const std::string& rhs, std::size_t id) : m_lhs(lhs), m_rhs(rhs), m_id(id) {}

const char& Rule::lhs() const { return m_lhs; }

const std::string& Rule::rhs() const { return m_rhs; }

const std::size_t& Rule::id() const { return m_id; }

std::size_t RuleHash::operator()(const Rule& rule) const { return std::hash<std::string>()(rule.lhs() + rule.rhs()); }
