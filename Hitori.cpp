//
//  main.cpp
//  _AI_Final -> Hitori Game solution by local search algorithms
//
//  Created by Thenonchetable on 12/18/19.
//  Copyright � 2019 Mohammad. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


auto constexpr __WHITE__ = true;
auto constexpr __BLACK__ = false;
auto constexpr __E__ = 2.71828;


enum evaluateTypes { __HEURISTIC_COST__, __HEURISTIC__, __COST__ };
enum AlgorithmEvaluation { __UCS__, __GREEDY__, __ASTAR__, __HILL_CLIMBING__, __S_HILL_CLIMBING__, __SA__ };
enum Direction { __TOP__, __BOTTOM__, __LEFT__, __RIGHT__ };


void _LogError(std::string e) {
   std::cout << e << std::endl;
   return;
}

struct nBoard {
   //the Board which contain all the numbers in specefic cordination
   int** _Board;
   //Size of the board
   int _Size;
   //nBoard Constructor
   nBoard(int** inp, int size) {
       this->_Size = size;
       this->_Board = new int* [this->_Size];
       for (auto i = 0; i < this->_Size; i++) {
           this->_Board[i] = new int[this->_Size];
       }
       for (auto i = 0; i < this->_Size; i++) {
           for (auto j = 0; j < this->_Size; j++) {
               this->_Board[i][j] = inp[i][j];
           }
       }
   }
   //Print all numbers in the board
   void print_board() {
       for (auto i = 0; i < this->_Size; i++) {
           for (auto j = 0; j < this->_Size; j++) {
               std::cout << this->_Board[i][j];
           }
       }
   }
};

class State {
private:
   //Black and White board
   bool** _Board;
   //Size of the board -> **note that the sizes are equal in each direction
   int _Size;

   void _evaluateState(int type) {
       //Heuristic #1
       if (type == __HEURISTIC_COST__ || type == __HEURISTIC__)
       {
           int tmp = 0;
           for (auto i = 0; i < this->_Size; i++) {
               for (auto j = 0; j < this->_Size; j++) {
                   if (this->_Board[i][j] == __BLACK__)
                       tmp++;
               }
           }
           this->_Hvlaue = (this->_Size * this->_Size) - tmp;
           this->_Cost++;
       }
       else
       {
           this->_Cost++;
       }
       //Heuristc #2 -> defined in a private method in HitoriSolver class
   }


   std::vector<int> _get_neighbors(int i, int j) {
       std::vector<int> res;
       //check for top cell
       if (j >= 1)
           res.push_back(__TOP__);
       //check for bottom cell
       if (j < this->_Size - 1)
           res.push_back(__BOTTOM__);
       //check for left cell
       if (i >= 1)
           res.push_back(__LEFT__);
       //check for right cell
       if (i < this->_Size - 1) {
           res.push_back(__RIGHT__);
       }
       return res;
   }


public:
   int _Cost;
   int _Hvlaue;

   State() {
       this->_Size = 0;
       this->_Hvlaue = 0;
       this->_Cost = 0;
   }
   //Initialize a black and white board with all white values
   State(int inp_size) {
       this->_Cost = 0;
       this->_Hvlaue = 0;
       this->_Size = inp_size;
       this->_Board = new bool* [this->_Size];
       for (auto i = 0; i < this->_Size; i++) {
           this->_Board[i] = new bool[this->_Size];
       }
       for (auto i = 0; i < this->_Size; i++) {
           for (auto j = 0; j < this->_Size; j++) {
               this->_Board[i][j] = __WHITE__;
           }
       }
   }
   //Copy Constructor
   State(const State& other) {
       this->_Size = other._Size;
       this->_Board = other._Board;
       this->_Cost = other._Cost;
   }
   //Copy Assignment
   State& operator=(const State& other) {
       if (this != &other) {
           this->_Size = other._Size;
           //this->_Board = other._Board;
           this->_Board = new bool* [this->_Size];
           for (auto i = 0; i < this->_Size; i++)
               this->_Board[i] = new bool[this->_Size];
           //Assign board with new values
           for (auto i = 0; i < this->_Size; i++) {
               for (auto j = 0; j < this->_Size; j++) {
                   this->_Board[i][j] = other._Board[i][j];
               }
           }
           this->_Cost = other._Cost;
       }
       return *this;
   }

   bool operator> (const State& other) {
       if (&other == this) return false;
       return (this->_Hvlaue + this->_Cost) > (other._Hvlaue + other._Cost);
   }

   //what's the value of a specific cell in the board
   bool what_is(int coordinate_i, int coordinate_j) {
       if (coordinate_i < 0 || coordinate_i >= this->_Size || coordinate_j < 0
           || coordinate_j >= this->_Size) {
           _LogError(std::to_string(coordinate_i) + " | " + std::to_string(coordinate_j));
           _LogError("invalid input in <what_is> method ... the size is:" + std::to_string(this->_Size));
           return NULL;
       }
       return this->_Board[coordinate_i][coordinate_j];
   }
   //set a value to a specific cell by it's value
   void set_cell(int coordinate_i, int coordinate_j, bool value) {
       if (coordinate_i < 0 || coordinate_i >= this->_Size || coordinate_j < 0
           || coordinate_j >= this->_Size) {
           _LogError("invalid input in <set_cell> method");
           return;
       }
       this->_Board[coordinate_i][coordinate_j] = value;
   }

   void printState() {
       for (auto i = 0; i < this->_Size; i++) {
           for (auto j = 0; j < this->_Size; j++) {
               if (this->_Board[i][j] == __BLACK__)
                   std::cout << " 0 ";
               else
                   std::cout << " 1 ";
           }
           std::cout << std::endl;
       }
       std::cout << std::endl;
   }

   //check the cordination which passed to argument is valid to making black or not
   bool _is_valid_colorize(int i, int j) {
       if (this->_Board[i][j] == __BLACK__)
           return false;
       std::vector<int> tmp = this->_get_neighbors(i, j);
       for (auto a : tmp) {
           //
           //
           //Top cell is available
           //
           //
           if (a == __TOP__) {
               //Condition 1 , 2
               if (this->_Board[i][j - 1] == __BLACK__)
                   return false;
               //Condition 3 -> loop
               auto I = this->_get_neighbors(i, j - 1);
               bool flag = false;
               for (int neighbor : I) {
                   if (neighbor == __TOP__)
                       if (this->_Board[i][j - 2] == __WHITE__)
                           flag = true;
                   if (neighbor == __LEFT__)
                       if (this->_Board[i - 1][j - 1] == __WHITE__)
                           flag = true;
                   if (neighbor == __RIGHT__)
                       if (this->_Board[i + 1][j - 1] == __WHITE__)
                           flag = true;
               }
               if (!flag)
               {
                   return false;
               }
           }
           //
           //
           //Bottom cell is available
           //
           //
           if (a == __BOTTOM__) {
               //Condition 1 , 2
               if (this->_Board[i][j + 1] == __BLACK__)
                   return false;
               //Condition 3 -> loop
               auto I = this->_get_neighbors(i, j + 1);
               bool flag = false;
               for (auto neighbor : I) {
                   if (neighbor == __BOTTOM__) {
                       if (this->_Board[i][j + 2] == __WHITE__)
                           flag = true;
                   }
                   if (neighbor == __LEFT__) {
                       if (this->_Board[i - 1][j + 1] == __WHITE__)
                           flag =true;
                   }
                   if (neighbor == __RIGHT__) {
                       if (this->_Board[i + 1][j + 1] == __WHITE__)
                           flag = true;
                   }
               }
               if (!flag) {
                   return false;
               }
           }
           //
           //
           //Left cell is available
           //
           //
           if (a == __LEFT__) {
               //Condition 1 , 2
               if (this->_Board[i - 1][j] == __BLACK__) {
                   return false;
               }
               //Condition 3 -> loop
               auto I = this->_get_neighbors(i - 1, j);
               bool flag = false;
               for (auto neighbor : I) {
                   if (neighbor == __TOP__)
                       if (this->_Board[i - 1][j - 1] == __WHITE__)
                           flag = true;
                   if (neighbor == __LEFT__)
                       if (this->_Board[i - 2][j] == __WHITE__)
                           flag = true;
                   if (neighbor == __BOTTOM__)
                       if (this->_Board[i - 1][j + 1] == __WHITE__)
                           flag = true;
               }
               if (!flag) {
                   return false;
              }
           }
           //
           //
           //Bottom cell is available
           //
           //
           if (a == __RIGHT__) {
               //Condition 1 , 2
               if (this->_Board[i + 1][j] == __BLACK__)
                   return false;
               //Condition 3 -> loop
               auto I = this->_get_neighbors(i + 1, j);
               bool flag = false;
               for (auto neighbor : I) {
                   if (neighbor == __TOP__)
                       if (this->_Board[i + 1][j - 1] == __WHITE__)
                           flag = true;
                   if (neighbor == __RIGHT__)
                       if (this->_Board[i + 2][j] == __WHITE__)
                           flag = true;
                   if (neighbor == __BOTTOM__)
                       if (this->_Board[i + 1][j + 1] == __WHITE__)
                           flag = true;
               }
               if (!flag) {
                   return false;
               }
           }
       }
       return true;
   }

   int get_size() {
       return this->_Size;
   }

   int get_cost() {
       return this->_Cost;
   }

   void increment_cost() {
       this->_Cost++;
   }

   void evaluate(int type) {
       this->_evaluateState(type);
   }
};

bool operator< (const State& lhs, const State& rhs) {
   return (lhs._Hvlaue + lhs._Cost) < (rhs._Hvlaue + rhs._Cost);
}

class HitoriSolver {
private:
   //Constant numbers board
   nBoard* _NumbersBoard;
   //Black and White board for the start state
   State* _StartState;
   //Size
   int _Size;
   //Priority Queue
   std::priority_queue<State> _PQueue;
   //SA and Hill Climbing successor temp
   std::vector<State> _SA_Successors;
   std::vector<State> _Hill_Climbing_Successors;

   //Goal State
   State _Found;


   //Successor function -> create all possible next states
   State _successor(State state, int type_of_value) {
       this->_SA_Successors.erase(this->_SA_Successors.begin(), this->_SA_Successors.end());
       this->_Hill_Climbing_Successors.erase(this->_Hill_Climbing_Successors.begin(), this->_Hill_Climbing_Successors.end());
       State best;
       for (auto i = 0; i < this->_Size; i++) {
           for (auto j = 0; j < this->_Size; j++) {
               if (state._is_valid_colorize(i, j)) {
                   //if the algorithm is UCS -> just evaluate cost + just best successor needed
                   if (type_of_value == __UCS__) {
                       State tmp;
                       tmp = state;
                       tmp.set_cell(i, j, __BLACK__);
                       tmp.evaluate(__COST__);
                       if (tmp >  best)
                           best = tmp;
                   }
                   //if the algorithm is Greedy -> just evaluate huristic + just best successor needed
                   if (type_of_value == __GREEDY__) {
                       State tmp;
                       tmp = state;
                       tmp.set_cell(i, j, __BLACK__);
                       tmp.evaluate(__HEURISTIC__);
                       if (tmp > best)
                           best = tmp;
                   }
                   //if the algorithm is A* -> evaluate both of cost and huristic + all successors needed
                   if (type_of_value == __ASTAR__) {
                       State tmp;
                       tmp = state;
                       tmp.set_cell(i, j, __BLACK__);
                       tmp.evaluate(__HEURISTIC_COST__);
                       //evaluate_state(*tmp, __HEURISTIC_COST__);
                       tmp.printState();
                       //_LogError("####################");
                       this->_PQueue.push(tmp);
                   }
                   //if the algorithm is Simple Hill Climbing -> evaluate just huristic + just best successor needed
                   if (type_of_value == __HILL_CLIMBING__) {
                       State tmp;
                       tmp = state;
                       tmp.set_cell(i, j, __BLACK__);
                       tmp.evaluate(__HEURISTIC__);
                       if (tmp > best)
                           best = tmp;
                   }
                   //if the algorithm is Stochastic Hill Climbing -> evaluate just huristic + all successors needed
                   if (type_of_value == __S_HILL_CLIMBING__) {
                       State tmp;
                       tmp = state;
                       tmp.set_cell(i, j, __BLACK__);
                       tmp.evaluate(__HEURISTIC__);
                       this->_Hill_Climbing_Successors.push_back(tmp);
                   }
                   //if the algorithm is Simulated Anneling -> evaluate just huristic + all successors needed
                   if (type_of_value == __SA__) {
                       State tmp;
                       tmp = state;
                       tmp.set_cell(i, j, __BLACK__);
                       tmp.evaluate(__HEURISTIC__);
                       this->_SA_Successors.push_back(tmp);
                   }
               }
           }
       }

       return best;
   }

   bool is_goal(State s) {
       bool* colVal = new bool[this->_Size];
       bool* rowVal = new bool[this->_Size];
       for (auto i = 0; i < this->_Size; i++) {
           //clear two value check lists
           for (auto k = 0; k < this->_Size; k++)
               colVal[i] = rowVal[i] = true;
           for (auto j = 0; j < this->_Size; j++) {
               //check 1
               //if you have seen this number before with white color -> Row
               if (rowVal[this->_NumbersBoard->_Board[i][j] - 1] == false && s.what_is(i, j) == __WHITE__)
                   return false;
               //else if the color of this number is white, set the value in the check list to false
               else if (s.what_is(i, j) == __WHITE__)
                   rowVal[this->_NumbersBoard->_Board[i][j] - 1] = false;
               //if you have seen this number before with white color -> Col
               if (colVal[this->_NumbersBoard->_Board[j][i] - 1] == false && s.what_is(j, i) == __WHITE__)
                   return false;
               //else if the color of this number is white, set the value in the check list to false
               else if (s.what_is(j, i) == __WHITE__)
                   colVal[this->_NumbersBoard->_Board[j][i] - 1] = false;
           }
       }
       //it's Goal :)
       return true;
   }

   int choose_random_hill() {
       int s = 0;
       int index = 0;
       sort(this->_Hill_Climbing_Successors.begin(), this->_Hill_Climbing_Successors.end());
       for (auto i = 0; i < this->_Hill_Climbing_Successors.size(); i++)
           s = s + this->_Hill_Climbing_Successors[i]._Hvlaue;
       srand(time(NULL));
       int random = rand() % s + 1;
       for (auto i = 0; i < this->_Hill_Climbing_Successors.size(); i++) {
           if (random <= this->_Hill_Climbing_Successors[i]._Hvlaue) {
               index = i;
               break;
           }
       }
       return index;
   }

   int choose_random_SA() {
       srand(time(NULL));
       int random = rand() % this->_SA_Successors.size();
       return random;
   }

   State evaluate_state(State s, int type) {
       //Heuristic #2
       int val = 0;
       int* rows = new int[this->_Size];
       for (auto i = 0; i < this->_Size; i++)
           rows[i] = 0;
       int* cols = new int[this->_Size];
       for (auto i = 0; i < this->_Size; i++)
           cols[i] = 0;
       if (type == __HEURISTIC_COST__ || type == __HEURISTIC__) {
           for (auto i = 0; i < this->_Size; i++) {
               for (auto j = 0; j < this->_Size; j++) {
                   if (s.what_is(i, j) == __WHITE__) {
                       rows[this->_NumbersBoard->_Board[i][j] - 1]++;
                   }
               }
           }
           for (auto i = 0; i < this->_Size; i++) {
               for (auto j = 0; j < this->_Size; j++) {
                   if (s.what_is(i, j) == __WHITE__) {
                       rows[this->_NumbersBoard->_Board[j][i] - 1]++;
                   }
               }
           }
           for (auto i = 0; i < this->_Size; i++)
               val++;
           s._Hvlaue = val;
           return s;
       }
       return s;
   }

public:
   //Constructor function with input file address argument
   HitoriSolver(std::string inp) {
       if (inp == "") {
           _LogError("invalid input file address!");
           return;
       }
       std::ifstream File;
       File.open(inp);
       if (!File.is_open()) {
           _LogError("the input file is damaged or terminated!");
           return;
       }
       File >> this->_Size;
       this->_StartState = new State(this->_Size);
       int** temp = new int* [this->_Size];
       for (auto i = 0; i < _Size; i++)
           temp[i] = new int[_Size];
       for (auto i = 0; i < this->_Size; i++) {
           for (auto j = 0; j < this->_Size; j++) {
               File >> temp[i][j];
           }
       }
       this->_NumbersBoard = new nBoard(temp, this->_Size);
       this->_StartState = new State(this->_Size);
   }
   //Solve the game with Uniformed Cost Search Algorithm
   bool solve_by_ucs() {
       State init = *this->_StartState;
       while (true) {
           init = this->_successor(init, __UCS__);
           if (init.get_size() == 0)
               break;
           if (this->is_goal(init))
           {
               this->_Found = init;
               return true;
           }
       }
       return false;
   }
   //Solve the game with Greedy Algorithm which uses only best heuristic
   bool solve_by_Greedy() {
       State init = *this->_StartState;
       while (true) {
           std::cout << "yes" << std::endl;
           init = this->_successor(init, __GREEDY__);
           if (init.get_size() == 0)
               break;
           if (this->is_goal(init))
           {
               this->_Found = init;
               return true;
           }
       }
       return false;
   }
   //Solve the game with A* Algorithm
   bool solve_by_Astar() {
       State init = *this->_StartState;
       this->_PQueue.push(init);
       while (!this->_PQueue.empty()) {
           State tmp = this->_PQueue.top();
           this->_PQueue.pop();
           //tmp.printState();
           if (this->is_goal(tmp)) {
               this->_Found = tmp;
               return true;
           }
           this->_successor(tmp, __ASTAR__);
       }
       return false;
   }

   //Solve the game with Simple Hill-Climbing Algorithm
   bool solve_by_hillclimbing() {
       State init = *this->_StartState;
       while (true) {
           State tmp = _successor(init, __HILL_CLIMBING__);
           if (tmp.get_size() == 0)
               break;
           if (tmp > init) {
               this->_Found = init;
               return true;
           }
           init = tmp;
       }
       return false;
   }
   //Solve the game with Stochastic Hill-Climbing Algorithm
   bool solve_by_stochastic_hillclimbing() {
       State init = *this->_StartState;
       while (true) {
           _successor(init, __S_HILL_CLIMBING__);
           if (this->_Hill_Climbing_Successors.size() == 0)
               break;
           State tmp = this->_Hill_Climbing_Successors[this->choose_random_hill()];
           if (tmp > init) {
               this->_Found = init;
               return true;
           }
           init = tmp;
       }
       return false;
   }

   State solve_by_SA(int* temp, int Tsize) {
       State init = *this->_StartState;
       int T;
       for (auto i = 0; i < Tsize; i++) {
           T = temp[i];
           if (T == 0)
               return init;
           State tmp = this->_SA_Successors[this->choose_random_SA()];
           if (this->is_goal(tmp)) {
               this->_Found = tmp;
               return true;
           }
           if (tmp > init)
               init = tmp;
           else {
               srand(time(NULL));
               int tmp_index = pow(__E__, tmp._Hvlaue - init._Hvlaue);
               int rand_index = rand() % 100 + 1;
               if (rand_index <= tmp_index * 100)
                   init = tmp;
           }
       }
       return false;
   }

   void print_goal() {
       for (auto i = 0; i < this->_Size; i++) {
           for (auto j = 0; j < this->_Size; j++) {
               if (this->_Found.what_is(i, j) == __BLACK__)
                   std::cout << "  1  ";
               else
                   std::cout << "  0  ";
           }
           std::cout << std::endl;
       }
   }
};

int main(int argc, const char* argv[]) {

   HitoriSolver* AI_Final = new HitoriSolver("input.txt");
   bool res = AI_Final->solve_by_Astar();
   if (res == false)
       std::cout << "No" << std::endl;
   else
       AI_Final->print_goal();;
}
