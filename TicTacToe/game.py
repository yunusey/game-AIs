import json, os

#in JSON file, we should have keys representing board; and add them to dict
#if you get to that key in a bigger depth then update it...

file = open('./database.json', 'r')
database = dict(json.load(file))

n, m = 4, 4

starting_depth = 7

def make_str_binary(s : list) -> str:

    new_s = ''
    for i in s:
        for j in i:
            if j == 'O':
                new_s += '11'
            elif j == 'X':
                new_s += '00'
            else:
                new_s += '01'

    return new_s

def make_str_int(s : list) -> int:
    return int(make_str_binary(s), 2)

def make_list_str(s : list) -> str:
    new_str = ''
    for i in s:
        for j in i:
            if j == '':
                j = ' '
            new_str += j
    return new_str

class Game:

    def __init__(self):
        self.game = [['' for i in range(m)] for j in range(n)]
        self.left = n * m

    def checkCase(self, depth : int, maximizing : bool):

        #value_of_board = make_str_int(self.game)
        #value_str = str(value_of_board)


        isWinner = self.checkResult() 
        if isWinner != '':
            return (1 if isWinner == 'X' else (-1 if isWinner == 'O' else 0))

        if depth == 0 or self.left <= 0:
            return 0

        value_str = make_list_str(self.game)

        if value_str in database:
            depth_of_key, score = database[value_str]
            if depth_of_key >= depth:
                return score

        if maximizing:
            best_score = -2
            for x in range(n):
                for y in range(m):
                    if self.game[x][y] == '':
                        self.game[x][y] = 'X'
                        self.left -= 1
                        score = self.checkCase(depth - 1, False)
                        self.left += 1
                        best_score = max(score, best_score)
                        self.game[x][y] = ''
                        if best_score == 1:
                            database[value_str] = (depth, best_score)
                            return best_score

            database[value_str] = (depth, best_score)
            return best_score

        else:
            best_score = 2
            for x in range(n):
                for y in range(m):
                    if self.game[x][y] == '':
                        self.game[x][y] = 'O'
                        self.left -= 1
                        score = self.checkCase(depth - 1, True)
                        self.left += 1
                        best_score = min(score, best_score)
                        self.game[x][y] = ''
                        if best_score == -1:
                            database[value_str] = (depth, best_score)
                            return best_score

            database[value_str] = (depth, best_score)
            return best_score
        

    def play(self) -> tuple:

        i, j, best_result = -1, -1, -2

        for x in range(n):
            for y in range(m):
                if self.game[x][y] == '':
                    self.game[x][y] = 'X'
                    self.left -= 1

                    result = self.checkCase(starting_depth - 1, False)

                    self.left += 1
                    if result > best_result:
                        i, j = x, y
                        best_result = result

                    self.game[x][y] = ''
                    if best_result == 1:
                        break
            if best_result == 1:
                break

        self.game[i][j] = 'X'

        self.left -= 1

        return (i, j)
        

    def checkResult(self) -> str:
        for i in range(n):
            found_result = True
            for j in range(m - 1):
                if not(self.game[i][j] != '' and self.game[i][j] == self.game[i][j + 1]):
                    found_result = False
                    break
            if found_result:
                return self.game[i][0]

        for j in range(m):
            found_result = True
            for i in range(n - 1):
                if not(self.game[i][j] != '' and self.game[i + 1][j] == self.game[i][j]):
                    found_result = False
                    break
            if found_result:
                return self.game[0][j]

        found_result, i, j = True, 0, 0
        while i + 1 < n and j + 1 < m:
            if not (self.game[i][j] != '' and self.game[i + 1][j + 1] == self.game[i][j]):
                found_result = False
                break
            i, j = i + 1, j + 1

        if found_result:
            return self.game[0][0]

        found_result, i, j = True, 0, m - 1
        while i + 1 < n and j - 1 >= 0:
            if not (self.game[i][j] != '' and self.game[i + 1][j - 1] == self.game[i][j]):
                found_result = False
                break
            i, j = i + 1, j - 1
            
        if found_result:
            return self.game[0][m - 1]

        if self.left == 0:
            return 'Tie'

        return ''

game = Game()
while game.left:

    i, j = game.play()
    for i in range(n):
        for j in range(m):
            print(f'| {game.game[i][j] if game.game[i][j] != "" else " "} | ', end='')
        print()

    result = game.checkResult()
    if result != '':
        break

    if game.left == 0:
        result = '0'
        break
    
    a, b = (int(i) for i in input().split(' '))
    while game.game[a][b] != '':
        print('Give A Valid Index Pls!')
        a, b = (int(i) for i in input().split(' '))

    game.game[a][b] = 'O'
    game.left -= 1

    result = game.checkResult()
    if result != '':
        break

#Json Part in

print(result, ('Wins!' if result != 'Tie' else ''))

jsonobj = json.dumps(database, indent = 4)

with open('./database.json', 'w') as file:
    file.write(jsonobj)
