# Useful algorithms for Finite and Pushdown Context-Free automatons

## Build & Run

#### 1. Clone the repo and go to project folder
  ```sh
  git clone https://github.com/NTheme/Finite-Automaton.git
  cd Automaton
  ```


### Compile & Install

#### 2. Now you can start to build and install. num_cores is a number of your processor's cores (usually 16)
  ```sh
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=. -G"Unix Makefiles"
  cmake --build build --config Release -- -j <num_cores + 1>
  sudo cmake --install build
  ```

#### 3. Launch
  ```
  Automaton <flags>
  ```
  
  
### Testing algorithms
  Add flags ```-DTEST_EARLEY=1``` or ```-DTEST_LR1=1``` to check aldorithms on automatic tests.
  Compiled files will be stored in ```CMAKE_INSTALL_PREFIX/bin```

### Testing coverage
#### 2. Compile a coverage
  ```shell
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DTEST_COVERAGE=1 -G"Unix Makefiles"
  cd build
  make test_coverage -j <num_cores + 1>
  ```
  Coverage report will appear in ```build/coverage``` (of course if you're not me and lucky enough to deal with lcov...)


## Usage

### Opts: -a, -t

* -a: Select an automaton
  * frla: Finite automaton
  * pcfa: Pushdown context-free automaton

* -t: Depends on the previous option value
  * Finite automaton was selected
    * rton: Transform a regular expression to a NFA
    * rtod: Transform a regular expression to a DFA
    * rtoc: Transform a regular expression to a minimal CDFA
    * ntod: Transform an automaton to a DFA
    * ntoc: Transform an automaton to a CDFA
    * ator: Transform an automaton to a regular expression
  * Pushdown context-free automaton was selected
    * erly: Use Earley's algorithm to check if a word can be recognized
    * alr1: Use LR-1 algorithm to check if a word can be recognized

### Then follow the instructions from the program
