from subprocess import Popen, PIPE

class RubiksSolver(object):

    def __init__(self, path='../solver', solver='rubiks', mindepth=0, maxdepth=20, multiple=False, premoves=None, heuristics=[], operations=None):
        self.path = path
        self.solver = solver
        self.mindepth = mindepth
        self.maxdepth = maxdepth
        self.multiple = multiple
        self.premoves = premoves
        self.heuristics = heuristics
        self.operations = operations

    def start(self, scramble):
        arguments = [self.path, self.solver,
                     '--mindepth=' + str(self.mindepth),
                     '--maxdepth=' + str(self.maxdepth)]
        if self.premoves: arguments.append('--premoves=' + self.premoves)
        if self.multiple: arguments.append('--multiple')
        if self.operations: arguments.append('--operations=' + self.operations)
        if self.solver == 'rubiks':
            for heuristic in self.heuristics:
                arguments.append('--heuristic')
                arguments.append(heuristic)
        elif len(self.heuristics) > 0:
            arguments.append('--heuristic=' + self.heuristics[0])
        self.proc = Popen(arguments, stdout=PIPE, stdin=PIPE, close_fds=True)
        self.proc.stdin.write(scramble + "\n")
        self.stream = self.proc.stdout

    def next_solution(self):
        solnOut = 'Found solution: '
        while 1:
            _line = self.stream.readline()
            if _line == '' or not _line: break
            line = _line.rstrip()
            if line[:len(solnOut)] == solnOut:
                desc = line[len(solnOut):]
                return RubiksAlgorithm.parse_algorithm(desc)
        return None

    def terminate(self):
        self.proc.stdin.close()
        self.proc.stdout.close()
        self.proc.terminate()

class RubiksAlgorithm(object):

    @staticmethod
    def parse_algorithm(algo):
        moves = algo.split(' ')
        return RubiksAlgorithm(moves)

    def __init__(self, moves):
        self.moves = moves

    def __str__(self):
        string = ''
        for i in range(0, len(self.moves)):
            if i != 0: string += ' '
            string += self.moves[i]
        return string

    def move_count(self):
        return len(self.moves)
        
    def append(self, algo):
        self.moves += algo.moves

