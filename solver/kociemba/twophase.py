from solver import RubiksSolver
import sys
import atexit

class TwoPhaseSolver(object):
    
    @staticmethod
    def hasPrefix(string, prefix):
        return string[:len(prefix)] == prefix

    @staticmethod
    def printUsage():
        print """Usage: python twophase.py <solver path> [--p1heuristic=file]
            [--p2heuristic file] [--identity] [--premoves=ALG]
            [--operations=ALG1,ALG2]
        """
    
    def __init__(self, argv):
        self.p1Heuristics = []
        self.p2Heuristics = []
        self.autoInput = False
        self.premoves = None
        self.operations = None
        if len(argv) < 1:
            TwoPhaseSolver.printUsage()
            sys.exit()
        self.path = argv[0]
        i = 1
        while i < len(argv):
            argument = argv[i]
            if TwoPhaseSolver.hasPrefix(argument, '--p1heuristic='):
                self.p1Heuristics = [argument[len('--p1heuristic='):]]
            elif argument == '--p2heuristic':
                if i + 1 >= len(argv):
                    print "error: invalid --p2heuristic argument"
                    sys.exit()
                self.p2Heuristics += [argv[i + 1]]
                i += 1
            elif argument == '--identity':
                self.autoInput = True
            elif TwoPhaseSolver.hasPrefix(argument, '--premoves='):
                self.premoves = argument[len('--premoves='):]
            elif TwoPhaseSolver.hasPrefix(argument, '--operations='):
                self.operations = argument[len('--operations='):]
            elif argument == '--help' or argument == '-?':
                TwoPhaseSolver.printUsage()
                sys.exit()
            else:
                print 'unknown argument: ' + argument
                sys.exit()
            i += 1
    
    def input_scramble(self):
        if self.autoInput:
            return ''.join(map((lambda x: str(x) * 9 + "\n"), range(1,7)))
        else:
            sides = ['front', 'back', 'top', 'bottom', 'right', 'left']
            string = ''
            for side in sides:
                sys.stdout.write('Enter the ' + side + ' side: ')
                string += raw_input() + '\n'
            return string
    
    def phase1_solver(self):
        solver = RubiksSolver(path = self.path, solver = 'r2f2u', premoves = self.premoves,
                              operations = self.operations, multiple = True,
                              heuristics = self.p1Heuristics)
        return solver
    
    def phase2_solver(self, p1soln, maxDepth):
        premoves = str(p1soln)
        realMax = maxDepth - p1soln.move_count()
        if self.premoves:
            premoves = self.premoves + ' ' + premoves
        solver = RubiksSolver(path = self.path, solver = 'rubiks', premoves = premoves,
                              maxdepth = realMax, heuristics = self.p2Heuristics,
                              operations = self.operations)
        return solver

p1Solver = None
p2Solver = None

def cleanup():
    if p1Solver: p1Solver.terminate()
    if p2Solver: p2Solver.terminate()

def main():
    global p1Solver, p2Solver
    solver = TwoPhaseSolver(sys.argv[1:])
    scramble = solver.input_scramble()
    
    currentLength = 100
    p1Solver = solver.phase1_solver()
    p1Solver.start(scramble)
    while True:
        p1Soln = p1Solver.next_solution()
        if p1Soln == None: break
        if p1Soln.move_count() >= currentLength: break
        p2Solver = solver.phase2_solver(p1Soln, currentLength - 1)
        p2Solver.start(scramble)
        soln = p2Solver.next_solution()
        if soln == None: continue
        p2Solver.terminate()
        p2Solver = None
        p1Soln.append(soln)
        if p1Soln.move_count() < currentLength:
            currentLength = p1Soln.move_count()
            print 'Found solution: ' + str(p1Soln) + ' [' + str(currentLength) + ' moves]'
            
    p1Solver.terminate()

atexit.register(cleanup)
main()
