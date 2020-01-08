/* invariants:
 *   - 32 states are provided from q0 to q31
 *   - 26 symbols are provided from a-z
 * 
 * providing inputs outside this range will not be taken care by the program
 */
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

struct Transition {
  unsigned int input_state;
  unsigned int output_states;
  int symbol;
};

struct FA {
  // states and alphabets already defined by invarients
  unsigned int starting_state;
  unsigned int final_states;
  vector<struct Transition> transition_table;
};

struct FA DFA;
struct FA NFA;

unsigned int numToSet(int input) {
  return (0x1 << input);
}

int setToNum(unsigned int set) {
  int num = -1;
  while (set != 0x0) {
    num += 1;
    set = (set >> 1);
  }
  return num;
}

char numToSymbol(int num) {
  return (num + 'a');
}

int symbolToNum(char symbol) {
  return (symbol - 'a');
}

string setToString(unsigned int set) {
  int num = 0;

  string s = "{";
  while (set != 0) {
    if (set & 0x1) {
      s = s + "q" + string(1, (num + '0'));
      if (set != 0x1) {
        s = s + ",";
      }
    }
    set >>= 1;
    num += 1;
  }
  s = s + "}";
  return s;
}

void addTransition(struct FA* machine, struct Transition t) {
  //check if transition with same input state and symbol exists
  vector<struct Transition>::iterator i;
  for (i = (*machine).transition_table.begin(); i != (*machine).transition_table.end(); i++) {
    if ((*i).input_state == t.input_state && (*i).symbol == t.symbol) {
      (*i).output_states |= t.output_states;
      return;
    }
  }
  (*machine).transition_table.push_back(t);
}

void inputTransition() {
  cout << "Add a new transition for NFA\n";
  int qi, qo;
  char s;
  cout << "Enter input state: q";
  cin >> qi;
  cout << "Enter output state: q";
  cin >> qo;
  cout << "Enter symbol: ";
  cin >> s;

  struct Transition t;
  t.input_state = numToSet(qi);
  t.output_states = numToSet(qo);
  t.symbol = symbolToNum(s);

  addTransition(&NFA, t);
}

void showTable(struct FA machine) {
  cout << "State\t\tSymbol\tOutput State\n";

  vector<struct Transition>::iterator i;
  for (i = machine.transition_table.begin(); i != machine.transition_table.end(); i++) {
    cout << setToString((*i).input_state) << "\t\t"
         << numToSymbol((*i).symbol) << "\t"
         << setToString((*i).output_states) << endl;
  }
}

void initNFA() {
  int ss, fs, n;
  cout << "Enter the starting state: q";
  cin >> ss;
  NFA.starting_state = numToSet(ss);
  cout << "Enter number of final states: ";
  cin >> n;
  for (int i = 0; i < n; i++) {
    cout << "Enter final state #" << i+1 << " : q";
    cin >> fs;
    NFA.final_states |= fs;
  }
  cout << "Enter number of transitions: ";
  cin >> n;
  for (int i = 0; i < n; i++) {
    inputTransition();
  }
}

unsigned int multipleTransition(unsigned int states, int symbol) {
  unsigned int output_states = 0x0;

  int num = 0;
  while (states != 0x0) {
    if (states & 0x1) {
      unsigned int qi = num;

      vector<struct Transition>::iterator i;
      for (i = NFA.transition_table.begin(); i != NFA.transition_table.end(); i++) {
        if ((*i).symbol == symbol && (*i).input_state == numToSet(qi)) {
          output_states |= (*i).output_states;
          break;
        }
      }
    }
    states >>= 1;
    num += 1;
  }
  return output_states;
}

bool isPresentInDFA(unsigned int state) {
  if (state == 0x0) return true; //this state meant that no transition

  vector<struct Transition>::iterator i;
  for (i = DFA.transition_table.begin(); i != DFA.transition_table.end(); i++) {
    if (state == (*i).input_state)
      return true;
  }
  return false;
}

void addStateToDFA(unsigned int qi) {
  for (int s = 'a'; s <= 'z'; s++) {
    int qo = multipleTransition(qi, symbolToNum(s));
    if (qo != 0x0) {
      struct Transition t;
      t.input_state = qi;
      t.output_states = qo;
      t.symbol = symbolToNum(s);

      addTransition(&DFA, t);
    }
  }
}

void initDFA() {
  DFA.starting_state = NFA.starting_state;
  DFA.final_states = 0;
  //add the transitions for NFA's starting state to DFA
  vector<struct Transition>::iterator i;
  for (i = NFA.transition_table.begin(); i != NFA.transition_table.end(); i++) {
    if ((*i).input_state == DFA.starting_state) {
      addTransition(&DFA, *i);
    }
  }
}


void bootstrapDFA() {
  //adding rest of the states
  queue<unsigned int> newStatesForDFA;
  do {
    while (!newStatesForDFA.empty()) {
      addStateToDFA(newStatesForDFA.front());
      newStatesForDFA.pop();
    }

    vector<struct Transition>::iterator i;
    for (i = DFA.transition_table.begin(); i != DFA.transition_table.end(); i++) {
      if (!isPresentInDFA((*i).output_states)) {
        newStatesForDFA.push((*i).output_states);
      }
    }
  } while (!newStatesForDFA.empty());
}

void createDFA() {
  initDFA();
  bootstrapDFA();
}

int main() {
  initNFA();
  cout << "**** NFA *****\n";
  showTable(NFA);
  createDFA();
  cout << "**** DFA *****\n";
  showTable(DFA);

  return 0;
}
