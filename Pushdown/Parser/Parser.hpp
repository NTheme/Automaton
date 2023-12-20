/******************************************
 *  Author : NThemeDEV
 *  Created : Tue Nov 28 2023
 *  File : Parser.hpp
 ******************************************/

#pragma once

#include "Grammar.hpp"

class Parser {
 public:
  virtual void fit(const Grammar& grammar) = 0;
  virtual bool predict(const std::string& word) const = 0;
  virtual ~Parser() {}

 protected:
  struct Situation;
  struct SituationHash;

  Grammar m_grammar;
};
