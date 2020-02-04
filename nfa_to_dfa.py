class FiniteAutomataState:
    def __init__(self, stateName):
        self.name = stateName
        self.next_states = {}

    def getName(self):
        return "q" + str(self.name)

def take_input():
    nfa = []
    symbols = set()
    n = int(input("No. of states: "))

    for i in range(n):
        nfa.append(FiniteAutomataState(i))

    tc = int(input("Enter number of transitions: "))
    for _ in range(tc):
        s = input();
        istate = int(s.split()[0][1:])
        symbol = s.split()[1]
        nstate = int(s.split()[2][1:])

        if symbol != 'epsilon':
            symbols.add(symbol)

        if symbol in nfa[istate].next_states:
            nfa[istate].next_states[symbol].append(nstate)
        else:
            nfa[istate].next_states[symbol] = [nstate]

    return nfa, symbols

def printnfa():
    for a in nfa:
        print(a.next_states)

class Closure:
    def findClosure(self, nfa, q):
        if q in self.states:
            return
        self.states.add(q)
        if 'epsilon' in nfa[q].next_states:
            for x in nfa[q].next_states['epsilon']:
                self.findClosure(nfa, x)

    def __init__(self, nfa=None, q=None):
        self.states = set()
        if q != None:
            self.findClosure(nfa, q)

    def __eq__(self, other):
        return self.states == other.states

    def __hash__(self):
        return hash(repr(self.states))

def construct_dfa(symbols, nfa, dfa_start_state):
    dfa = {}
    processing_queue = [dfa_start_state];
    while len(processing_queue) > 0:
        closure = processing_queue.pop()
        if closure in dfa: #already processed
            continue
        for symbol in symbols:
            next_closure = Closure()
            for state in closure.states:
                if symbol in nfa[state].next_states:
                    next_closure.findClosure(nfa, state)
            if len(next_closure.states) > 0:
                if next_closure not in dfa:
                    processing_queue.append(next_closure)
                dfa[closure] = next_closure

    return dfa


nfa, symbols = take_input()

dfa_start_state = Closure(nfa, 0)
dfa = construct_dfa(symbols, nfa, dfa_start_state)

print(dfa_start_state.states)
for k in dfa:
    print(str(k.states) + " " + dfa[+ str(dfa[k].states))


