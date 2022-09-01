#include <bits/stdc++.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <fstream>
#include <random>

std::vector<std::vector<char>> createBoard(int n){
    return std::vector<std::vector<char>> (n, std::vector<char>(n, ' '));
}

class Result{
public:
    int depth, value;
    Result(){
        this->depth = 0, this->value = 0;
    }
    Result(int depth_, int value_){
        this->depth = depth_, this->value = value_;
    }
    int operator [](size_t i){
        if(i == 0){
            return this->depth;
        }
        else if(i == 1){
            return this->value;
        }
    }
};

std::map<std::string, Result> jsonToMap(std::string filename){

    std::ifstream file; file.open(filename);
    if(!file.is_open()){
        std::cout << "Unsuccess :(" << std::endl;
        return std::map<std::string, Result>();
    }

    Json::Reader reader; Json::Value root;
    if(!reader.parse(file, root, false)){
        std::cout << "Couldn't Parse" << std::endl;
        return std::map<std::string, Result>();
    }

    std::map<std::string, Result> results;
    for(auto const& key : root.getMemberNames()){
        Result value(root[key][0].asInt(), root[key][1].asInt());
        results[key] = value;
    }

    return results;
}

bool writeToJson(std::map<std::string, Result>& results, std::string filename){
    std::ofstream file; file.open(filename);
    if(!file.is_open()){
        std::cout << "Could not open file to write!" << std::endl;
        return false;
    }
    Json::Value root;

    for(auto& i : results){
        root[i.first][0] = i.second.depth, root[i.first][1] = i.second.value;
    }

    Json::StyledWriter writer;
    file << writer.write(root);

    file.close();
    return true;
}

int n, max_depth = 6;


void printBoard(std::vector<std::vector<char>>& board){
    for(auto i : board){
        for(auto j : i){
            std::cout << "| " << j << " |" << ' ';
        }
        std::cout << std::endl;
    }
}

class Game{

public:

    std::map<std::string, Result> results;
    char ai = 'X'; int n;
    int winner = 0, left = 0; // meaning tie;
    int best_result_for_ai = (ai == 'X' ? 1 : -1);
    bool aiMaximizing = (ai == 'X' ? true : false);

    std::vector<std::vector<char>> board;
    std::string board_as_str;

    Game(std::vector<std::vector<char>> board){
        this->n = board.size();
        for(int i = 0; i < (int)board.size(); i++) for(int j = 0; j < (int)board.size(); j++) this->left += (board[i][j] == ' ');
        this->winner = this->checkBoard(board);
        if(this->winner == -1 || this->winner == 1 || this->winner == 0){
            return;
        }
        else{
            this->board = board; 
            for(int i = 0; i < this->n; i++){
                for(int j = 0; j < this->n; j++){
                    this->board_as_str += this->board[i][j];
                }
            }
            this->n = board.size();
        }
        this->results = std::map<std::string, Result>();
    }

    int minimax(int depth, bool is_maximizing, int best_result_can_be_made){

        if(this->results.find(this->board_as_str) != this->results.end()){
            auto value = this->results[this->board_as_str];
            if(value.depth >= depth){
                return value.value;
            }
        }

        int winner = this->checkBoard(this->board);
        if(winner == -1 || winner == 0 || winner == 1){
            this->results[this->board_as_str] = Result(max_depth, winner);
            return winner;
        }

        if(this->left <= 0 || !depth){
            return 0;
        }

        int best_result = (is_maximizing ? -2 : 2);

        for(int i = 0; i < this->n && best_result != best_result_can_be_made; i++){
            for(int j = 0; j < this->n && best_result != best_result_can_be_made; j++){

                if(this->board[i][j] != ' ') continue;

                this->board[i][j] = (is_maximizing ? 'X' : 'O');
                this->board_as_str[this->n * i + j] = (is_maximizing ? 'X' : 'O');
                this->left--;
                
                int result = this->minimax(depth - 1, !is_maximizing, -best_result_can_be_made);
                best_result = (is_maximizing ? std::max(best_result, result) : std::min(best_result, result));

                this->board[i][j] = ' ';
                this->board_as_str[this->n * i + j] = ' ';
                this->left++;
            }
        }
        Result value(depth, best_result);
        this->results[this->board_as_str] = value;
        return best_result;
    }

    std::pair<int, int> play(){

        int best_i = -1, best_j = -1, best_result = -2;

        for(int i = 0; i < this->n && best_result != this->best_result_for_ai; i++){
            for(int j = 0; j < this->n && best_result != this->best_result_for_ai; j++){
                
                if(this->board[i][j] != ' ') continue;
                
                this->board[i][j] = this->ai;
                this->board_as_str[this->n * i + j] = this->ai; 
                this->left--;

                int result = this->minimax(max_depth, !this->aiMaximizing, -1);

                if(result > best_result){
                    best_result = result;
                    best_i = i, best_j = j;
                }
                
                this->board[i][j] = ' ';
                this->board_as_str[this->n * i + j] = ' ';
                this->left++;
            }
        }

        this->board[best_i][best_j] = this->ai;
        this->board_as_str[this->n * best_i + best_j] = this->ai;
        this->left--;

        return std::make_pair(best_i, best_j);
    }

    int checkBoard(std::vector<std::vector<char>>& a){

        int n = a.size();
        for(int i = 0; i < n; i++){
            if(a[i][0] == ' ') continue;
            bool found = true;
            for(int j = 1; j < n; j++){
                if(a[i][j] != a[i][0]){
                    found = false;
                    break;
                }
            }
            if(found) return (a[i][0] == 'X' ? 1 : -1);
        }

        for(int j = 0; j < n; j++){
            if(a[0][j] == ' ') continue;
            bool found = true;
            for(int i = 1; i < n; i++){
                if(a[i][j] != a[0][j]){
                    found = false;
                    break;
                }
            }
            if(found) return (a[0][j] == 'X' ? 1 : -1);
        }

        bool found = (a[0][0] != ' ');
        for(int i = 0; i < n && found; found = (a[i][i] == a[0][0]), i++);
        if(found) return (a[0][0] == 'X' ? 1 : -1); 

        found = (a[0][n - 1] != ' ');
        for(int i = 0, j = n - 1; i < n && found; found = (a[i][j] == a[0][n - 1]), i++, j--);
        if(found) return (a[0][n - 1] == 'X' ? 1 : -1);

        return (this->left ? 2 : 0);
    }

};

int main() {

    std::vector<std::vector<char>> board = createBoard(4);
    Game game(board);
    game.results = jsonToMap("database.json");

    int winner = 2;

    while(game.left){

        auto pair = game.play();
        std::cout << pair.first << ' ' << pair.second << std::endl;
        printBoard(game.board);

        winner = game.checkBoard(game.board);        
        if(winner != 2) break;

        int x, y; std::cin >> x >> y;
        game.board[x][y] = 'O';
        game.board_as_str[game.n * x + y] = 'O';
        game.left -= 1;

        printBoard(game.board);
        winner = game.checkBoard(game.board);        
        if(winner != 2) break;

    }

    if(!game.left || winner == 0){
        std::cout << "Tie Game" << std::endl;
    }

    else if(winner == 1){
        std::cout << "X(Computer) Wins!" << std::endl;
    }

    else{
        std::cout << "O(You) Win!" << std::endl;
    }

    bool success = writeToJson(game.results, "database.json");

    return 0;    
}