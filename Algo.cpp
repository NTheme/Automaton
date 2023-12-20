#include "Finite/Automaton/CDFA.hpp"
#include "Finite/Automaton/DFA.hpp"
#include "Finite/Automaton/NFA.hpp"
#include "Finite/Expression/Expression.hpp"
#include "Pushdown/Parser/ParserEarley.hpp"
#include "Pushdown/Parser/ParserLR1.hpp"

void notify(const std::string& ask) {
  std::cout << ask;
  std::cout.flush();
}

template <typename Type>
void communicate(const std::string& ask, Type& ret) {
  notify(ask);
  std::cin >> ret;
}

void notifyUsage() {
  std::string usage =
      "-----Usage-----\n"
      "Opts: -a, -t\n\n"

      "> -a: Select an automaton\n"
      "----> frla: Finite automaton\n"
      "----> pcfa: Pushdown Context-Free automaton\n\n"

      "> -t: Depends on the previous option value\n"
      "--> Finite automaton was selected\n"
      "----> rton: Transform a regular expression to a NFA\n"
      "----> rtod: Transform a regular expression to a DFA\n"
      "----> rtoc: Transform a regular expression to a minimal CDFA\n"
      "----> ntod: Transform an automaton to a DFA\n"
      "----> ntoc: Transform an automaton to a CDFA\n"
      "----> ator: Transform an automaton to a regular expression\n"
      "--> Pushdown context-free automaton was selected\n"
      "----> erly: Use Earley's algorithm to check if a word can be recognized\n"
      "----> alr1: Use LR-1 algorithm to check if a word can be recognized\n";
  notify(usage);
  exit(0);
}

Expression readExpression() {
  std::string expression;
  notify("Enter regular expression in single line: ");
  std::getline(std::cin, expression);
  return Expression(expression);
}

NFA readNFA() {
  size_t nstates;
  communicate("Enter number of states in NFA: ", nstates);

  std::string alphabet;
  communicate("Enter all alphabet symbols without spaces: ", alphabet);

  NFA nfa(nstates, alphabet);

  size_t nfinal;
  communicate("Enter number of final states in FSM: ", nfinal);

  notify("Enter indices of final states (0-indexed): ");
  for (size_t i = 0; i < nfinal; ++i) {
    size_t v;
    std::cin >> v;
    nfa.addFinalState(v);
  }

  size_t ntransitions;
  communicate("Enter number of transitions: ", ntransitions);

  notify("Enter transitions in format \"from word to\" (replace word with \"-\" if it is empty):\n");
  for (size_t i = 0; i < ntransitions; ++i) {
    size_t from, to;
    std::string s;
    std::cin >> from >> s >> to;
    nfa.addTransition({from, s == "-" ? "" : s, to});
  }

  return nfa;
}

Rule parseRule(const std::string& buf) {
  size_t index = 0;

  for (; std::isspace(buf.at(index)); ++index) {
  }
  size_t lhs_begin = index;
  for (; index < buf.size() && std::isalpha(buf.at(index)); ++index) {
  }

  size_t lhs_end = index;
  if (lhs_end - lhs_begin != 1) {
    throw std::invalid_argument("Not a CF grammar!");
  }

  for (; index < buf.size() && std::isspace(buf.at(index)); ++index) {
  }
  size_t rhs_begin = index;
  for (; index < buf.size() && std::isalpha(buf.at(index)); ++index) {
  }

  return Rule(buf[lhs_begin], buf.substr(rhs_begin, index - rhs_begin));
}

Grammar readGrammar() {
  Alphabet alphabet;
  communicate("Enter all non-terminal symbols without spaces: ", alphabet.nterm);
  communicate("Enter all terminal symbols without spaces: ", alphabet.yterm);
  communicate("Enter a start non-terminal symbol: ", alphabet.start);

  Grammar grammar;
  grammar.fit(alphabet);

  size_t nrule;
  communicate("Enter a number of rules: ", nrule);

  std::string buf;
  std::getline(std::cin, buf);
  notify("Enter rules in format \"left right\" (type nothing if right is empty):\n");
  while (nrule-- > 0) {
    std::getline(std::cin, buf);
    if (buf.empty()) {
      notify("Rule cannot be empty!\n");
      ++nrule;
    } else {
      grammar.addRule(parseRule(buf));
    }
  }

  return grammar;
}

void finiteRTON() {
  notify("-----Transforming a regular expression to a NFA-----\n\n");
  std::cout << NFA(readExpression());
}

void finiteRTOD() {
  notify("-----Transforming a regular expression to a DFA-----\n\n");
  std::cout << DFA(readExpression());
}

void finiteRTOC() {
  notify("-----Transforming a regular expression to a CDFA-----\n\n");
  std::cout << CDFA(readExpression());
}

void finiteNTOD() {
  notify("-----Transforming an automaton to a DFA-----\n\n");
  std::cout << DFA(readNFA());
}

void finiteNTOC() {
  notify("-----Transforming an automaton to a minimal CDFA-----\n\n");
  std::cout << CDFA(readNFA());
}

void finiteATOR() {
  notify("-----Transforming an automaton to a regular expression-----\n\n");
  std::cout << Expression(readNFA()) << '\n';
}

void pushdownParser(Parser* parser) {
  parser->fit(readGrammar());

  size_t nword;
  communicate("Enter a number of words: ", nword);
  while (nword-- > 0) {
    std::string word;
    communicate("Enter a word: ", word);
    notify(parser->predict(word) ? "Result: Yes\n" : "Result: No\n");
  }
}

void pushdownEarley() {
  notify("Using an Earley's algorithm to check if a word can be recognized\n\n");
  Parser* parser = new ParserEarley;
  pushdownParser(parser);
  delete parser;
}

void pushdownLR1() {
  notify("Using a LR-1 algorithm to check if a word can be recognized\n\n");
  Parser* parser = new ParserLR1;
  pushdownParser(parser);
  delete parser;
}

void processFinite(const std::string& task) {
  if (task == "rton") {
    finiteRTON();
  } else if (task == "rtod") {
    finiteRTOD();
  } else if (task == "rtoc") {
    finiteRTOC();
  } else if (task == "ntod") {
    finiteNTOD();
  } else if (task == "ntoc") {
    finiteNTOC();
  } else if (task == "ator") {
    finiteATOR();
  } else {
    notifyUsage();
    throw std::invalid_argument("Invalid option!");
  }
}

void processPushdown(const std::string& task) {
  if (task == "erly") {
    pushdownEarley();
  } else if (task == "alr1") {
    pushdownLR1();
  } else {
    notifyUsage();
    throw std::invalid_argument("Invalid option!");
  }
}

void process(const std::pair<std::string, std::string>& opts) {
  if (opts.first == "help") {
    notifyUsage();
  } else if (opts.first == "frla") {
    processFinite(opts.second);
  } else if (opts.first == "pcfa") {
    processPushdown(opts.second);
  } else {
    notifyUsage();
    throw std::invalid_argument("Invalid option!");
  }
}

std::pair<std::string, std::string> readOpts(int argc, char* const* argv) {
  if (argc == 1) {
    notifyUsage();
  }
  std::pair<std::string, std::string> ret;

  std::string opts = "h:a:t:";

  char opt;
  while ((opt = getopt(argc, argv, opts.c_str())) != -1) {
    switch (opt) {
      case 'h':
        ret.first = "help";
        break;
      case 'a':
        ret.first = optarg;
        break;
      case 't':
        ret.second = optarg;
        break;
    }
  }

  return ret;
}

int main(int argc, char* const* argv) {
  process(readOpts(argc, argv));
  notify("\nGoodbye!");
  return 0;
}
