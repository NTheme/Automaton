/******************************************
 *  Author : NThemeDEV
 *  Created : Fri Oct 20 2023
 *  File : test.cpp
 ******************************************/

#include <gtest/gtest.h>

#include "ParserEarley.hpp"
#include "ParserLR1.hpp"

enum class ParserSelect { EARLEY, LR1 };

void test0() {
  Alphabet alphabet;
  alphabet.nterm = "S";
  alphabet.yterm = "ab";
  Grammar grammar;
  grammar.fit(alphabet);
  try {
    grammar.addRule(Rule('S', "ac"));
  } catch (const std::invalid_argument&) {
  }
}

void test1() {
  Alphabet alphabet;
  alphabet.nterm = "S$&";
  Grammar grammar;
  try {
    grammar.fit(alphabet);
  } catch (const std::invalid_argument&) {
  }
}

void test2() {
  Alphabet alphabet;
  alphabet.nterm = "S";
  alphabet.yterm = "S";
  Grammar grammar;
  try {
    grammar.fit(alphabet);
  } catch (const std::invalid_argument&) {
  }
}

void test3() {
  Alphabet alphabet;
  Grammar grammar;
  try {
    grammar.fit(alphabet);
  } catch (const std::invalid_argument&) {
  }
}

void test4() {
  Grammar grammar;
  try {
    grammar.getByID(12);
  } catch (const std::out_of_range&) {
  }
}

void testFPrepare(Parser& parser) {
  Alphabet alphabet;
  alphabet.nterm = "S";
  alphabet.yterm = "ab";
  alphabet.start = 'S';
  Grammar grammar;
  grammar.fit(alphabet);
  grammar.addRule(Rule('S', "aSbS"));
  grammar.addRule(Rule('S', ""));
  parser.fit(grammar);
}

bool testF(ParserSelect parser, const std::string& word) {
  if (parser == ParserSelect::EARLEY) {
    ParserEarley parser;
    testFPrepare(parser);
    return parser.predict(word);
  }
  if (parser == ParserSelect::LR1) {
    ParserLR1 parser;
    testFPrepare(parser);
    return parser.predict(word);
  }
  return false;
}

TEST(EarleyTest, RuleException) { test0(); }

TEST(EarleyTest, UtilException) { test1(); }

TEST(EarleyTest, TermException) { test2(); }

TEST(EarleyTest, StarException) { test3(); }

TEST(EarleyTest, FindException) { test4(); }

TEST(EarleyTest, StatementsY) { ASSERT_EQ(testF(ParserSelect::EARLEY, "aababb"), true); }

TEST(EarleyTest, StatementsN) { ASSERT_EQ(testF(ParserSelect::EARLEY, "aabbba"), false); }

TEST(LR1Test, RuleException) { test0(); }

TEST(LR1Test, UtilException) { test1(); }

TEST(LR1Test, TermException) { test2(); }

TEST(LR1Test, StarException) { test3(); }

TEST(LR1Test, FindException) { test4(); }

TEST(LR1Test, StatementsY) { ASSERT_EQ(testF(ParserSelect::LR1, "aababb"), true); }

TEST(LR1Test, StatementsN) { ASSERT_EQ(testF(ParserSelect::LR1, "aabbba"), false); }
