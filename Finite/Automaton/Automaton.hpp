/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Oct 28 2023
 *  File : Automaton.hpp
 ******************************************/

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

class Automaton {
 public:
  virtual std::vector<std::tuple<size_t, std::string, size_t>> getTransitions() const = 0;
  virtual size_t getSize() const = 0;
  
  std::vector<size_t> getFinalStates() const;
  std::string getAlphabet() const;

 protected:
  std::vector<bool> m_final;
  std::string m_alphabet;

  Automaton(size_t size, const std::string& alphabet);
};

std::ostream& operator<<(std::ostream& out, const Automaton& automaton);
