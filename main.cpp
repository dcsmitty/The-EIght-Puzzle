#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <queue>

using namespace std;

//declare global variables
bool isFound;
bool isDefaultPuzzle;
int board[9];
int goal_state[9];
int algorithm;
int totalNodes;
int maxQueueSize;
int depth;
int FinalPath[100][9];



//Struct for the Tree Nodes
struct Node{
    //child nodes
    struct Node *right;
    struct Node *left;
    struct Node *up;
    struct Node *down;
    //Parent node
    struct Node *Parent;
    //saved board
    int state[9];
    //cost to get to this state
    int cost;
    //used in A* algorithm to add to cost for priority queue placement
    int dist;
    //check to make sure node doesnt go back to previous state
    int repeatCheck;
    
};

//Comparator for the priority queue used in A*
struct node_cmp
{
    bool operator()(const Node* a, const Node* b)
    {
        return (a->cost + a->dist) > (b->cost + b->dist);
    }
};

typedef struct Node node;

//Function that displays the puzzle saved in global varible board
void displayBoard()
{
    for(unsigned int i = 0; i < 9; i++)
    {
        cout << board[i] << " ";
        if(i == 2 || i == 5 || i==8)
        {
            cout << endl;
        }
    }
}

//Function used to display the path and steps to complete the 8 Puzzle
void EndDisplay()
{
    for(unsigned int i = depth+1; i > 0; i--)
    {
        cout << "Expanding the State..." << endl;
        for(unsigned int j = 0; j < 9; j++)
        {
            board[j] = FinalPath[i][j];
        }
        displayBoard();
    }
    
    cout << endl;
    cout << "Goal!!!" << endl;
    cout << "Nodes Expanded: " << totalNodes << endl;
    cout << "Max Queue Size: " << maxQueueSize << endl;
    cout << "Depth of Goal: " << depth << endl;
}

//Function used to initialize the goal state
void loadGoalState()
{
    for(unsigned int i = 0; i < 9; i++)
        {
            goal_state[i] = i+1;
        }
        
        goal_state[8] = 0;
}

//-------------------------------------------------------------------
//  Input Operations
//-------------------------------------------------------------------

//Function that decides whether to use default puzzle or custom one
void puzzleSelect()
{
    cout << "Welcome to Daniel Smith's 8-puzzle Solver" << endl;
    cout << "Type '1' to use a default puzzle, or '2' to enter your own" << endl;
    int inp;
    
    do{
        cin >> inp;
        
        if(inp == 1)
        {
            isDefaultPuzzle = true;
        }
        else if(inp == 2)
        {
            isDefaultPuzzle = false;
        }
        else
        {
            cout << "Incorrect input!!! (Enter a correct input)" << endl;
        }
    }while(inp != 1 && inp != 2);
    
}

//Creates the board, using the input gained from puzzleSelect()
void boardCreate()
{
    if(isDefaultPuzzle)
    {
        cout << endl;
        cout << "You are using the default board!" << endl;
        board[0] = 1;
        board[1] = 2;
        board[2] = 3;
        board[3] = 4;
        board[4] = 8;
        board[5] = 0;
        board[6] = 7;
        board[7] = 6;
        board[8] = 5;
        displayBoard();
    }
    else
    {
        bool isValidBoard;
        
        //goto calls return back to the beginning to allow user to re-input
        Loop:do{
            isValidBoard = true;
            int inp1, inp2, inp3;
            cout << endl;
            cout << "Enter your puzzle, use a zero to represent the blank" << endl;
            
            //Ask for the first row of the custom puzzle to be entered
            cout << "Enter the first row, use spae or tabs between numbers" << endl;
            cin >> inp1 >> inp2 >> inp3;
            
            //check to see to make sure numbers are between 0-8 and no duplicates
            if((inp1<0 || inp1>8) || (inp2<0 || inp2>8) || (inp3<0 || inp3>8)
            || (inp1 == inp2) || (inp1 == inp3) || (inp2 == inp3))
            {
                cout << "Input Error!!! (Re-Enter proper input)" << endl;
                isValidBoard = false;
                goto Loop;
            }
            else
            {
                board[0] = inp1;
                board[1] = inp2;
                board[2] = inp3;
            }
            
            //Ask for the second row of the custom puzzle to be entered
            cout << "Enter the second row, use space or tabs between numbers" << endl;
            cin >> inp1 >> inp2 >> inp3;
            
            //check to see to make sure numbers are between 0-8 and no duplicates
            if((inp1<0 || inp1>8) || (inp2<0 || inp2>8) || (inp3<0 || inp3>8)
            || (inp1 == inp2) || (inp1 == inp3) || (inp2 == inp3))
            {
                cout << "Input Error!!! (Re-Enter proper input)" << endl;
                isValidBoard = false;
                goto Loop;
            }
            else
            {
                //check to make sure no duplicates from previous row entries
                for(unsigned int i = 0; i < 3; i++)
                {
                    if(board[i] == inp1 || board[i] == inp2 || board[i] == inp3)
                    {
                        cout << "Input Error!!! (Re-Enter proper input)" << endl;
                        isValidBoard = false;
                        goto Loop;
                    }
                }
                board[3] = inp1;
                board[4] = inp2;
                board[5] = inp3;
            }
            
            //Ask for the third row of the custom puzzle to be entered
             cout << "Enter the third row, use space or tabs between numbers" << endl;
             cin >> inp1 >> inp2 >> inp3;
             
             //check to see to make sure numbers are between 0-8 and no duplicates
            if((inp1<0 || inp1>8) || (inp2<0 || inp2>8) || (inp3<0 || inp3>8)
            || (inp1 == inp2) || (inp1 == inp3) || (inp2 == inp3))
            {
                cout << "Input Error!!! (Re-Enter proper input)" << endl;
                isValidBoard = false;
                goto Loop;
            }
            else
            {
                //check to make sure no duplicates from previous row entries
                for(unsigned int i = 0; i < 6; i++)
                {
                    if(board[i] == inp1 || board[i] == inp2 || board[i] == inp3)
                    {
                        cout << "Input Error!!! (Re-Enter proper input)" << endl;
                        isValidBoard = false;
                        goto Loop;
                    }
                }
                board[6] = inp1;
                board[7] = inp2;
                board[8] = inp3;
            }
        }while(!isValidBoard);
        
        int inversions = 0;
        //check is puzzle is solvable
        for(unsigned i = 0; i < 9; i++)
        {
            for(unsigned j = i+1; j < 9; j++)
            {
                if(board[j] > board[i])
                {
                    inversions++;
                }
            }
        }
        
        //~ if(inversions%2 == 1)
        //~ {
            //~ cout << "The puzzle entered is unsolvable!!! (Re-Enter proper input)" << endl;
            //~ isValidBoard = false;
            //~ goto Loop;
        //~ }
        
        cout << endl;
        cout << "This is your custom board!" << endl;
        displayBoard();
    }
}

//Function that chooses which of the 3 algorithms to use
void selectAlgorithm()
{
    cout << endl;
    cout << "Enter your choice of algorithm" << endl;
    cout << "1. Uniform Cost Search" << endl;
    cout << "2. A* with the Misplaced Tile heuristic" << endl;
    cout << "3. A* with the Manhatten distance heuristic" << endl;
    int inp;
    
    do{
        //stores user inp in global varibale algorithm, makes sure there's valid entry
        cin >> inp;
        
        if(inp == 1)
        {
            algorithm = 1;
        }
        else if(inp == 2)
        {
            algorithm = 2;
        }
        else if(inp == 3)
        {
            algorithm = 3;
        }
        else
        {
            cout << "Incorrect input!!! (Enter a correct input)" << endl;
        }
    }while(inp != 1 && inp != 2 && inp != 3);
}

//-------------------------------------------------------------------
//  Node Operations
//-------------------------------------------------------------------

//clears the nodes in the tree
void ClearTree(Node * T)
{
    if(T == NULL)
        return;
    
    //recursive calls to clean up tree    
    if(T->up != NULL)
        ClearTree(T->up);
        
    if(T->right != NULL)
        ClearTree(T->right);
        
    if(T->left != NULL)
        ClearTree(T->left);
        
    if(T->down != NULL)
        ClearTree(T->down);
    
    //delete the tree node
    delete T;
    
    return;
}

/*Function used to trace the path from the goal state to the root, while
saving the path in reverse order to be display in the correct order in
the EndDisplay function*/
void BackTrace(Node* T)
{
    if(T == NULL)
    {
        return;
    }
    else
    {
        //calculate the depth and store the path from goal state to root in
        //FinalPath matrix
        depth++;
        for(unsigned int i= 0; i < 9; i++)
        {
            FinalPath[depth][i] = T->state[i];
        }
        return BackTrace(T->Parent);
    }
}

//Function that changes the state of the puzzle through up, right, left and
//down shifts.  Also checks bounds and sets invalid states to NULL
Node* Shifter(Node * T, int type, int pos)
{
    //call for when dealing with the UP child node of the Node
    if(type == 1)
    {
        totalNodes++;
        //check to make sure the 0 isnt in top row, if it is up child is NULL
        if(pos == 0 || pos == 1 || pos == 2)
        {
            totalNodes--;
            T->up = NULL;
            return T;
        }
        //if check passes then make the move and store new state in child node UP
        else
        {
            for(unsigned int i = 0; i < 9; i++)
            {
                T->up->state[i] = T->state[i];
            }
        
            T->up->state[pos] = T->up->state[pos-3];
            T->up->state[pos-3] = 0;
        }
    }
    //call for when dealing with the RIGHT child node of the Node
    else if(type == 2)
    {
        totalNodes++;
        //check to make sure the 0 isnt in right-most column, if it is up child is NULL
        if(pos == 2 || pos == 5 || pos == 8)
        {
            totalNodes--;
            T->right = NULL;
            return T;
        }
        //if check passes then make the move and store new state in child node RIGHT
        else
        {
            for(unsigned int i = 0; i < 9; i++)
            {
                T->right->state[i] = T->state[i];
            }
            
            T->right->state[pos] = T->right->state[pos+1];
            T->right->state[pos+1] = 0;
        }
    }
    //call for when dealing with the LEFT child node of the Node
    else if(type == 3)
    {
        totalNodes++;
        //check to make sure the 0 isnt in left-most column, if it is up child is NULL
        if(pos == 0 || pos == 3 || pos == 6)
        {
            totalNodes--;
            T->left = NULL;
            return T;
        }
        //if check passes then make the move and store new state in child node LEFT
        else
        {
            for(unsigned int i = 0; i < 9; i++)
            {
                T->left->state[i] = T->state[i];
            }
            
            T->left->state[pos] = T->left->state[pos-1];
            T->left->state[pos-1] = 0;
        }
    }
     //call for when dealing with the Down child node of the Node
    else if(type == 4)
    {
        totalNodes++;
        //check to make sure the 0 isnt in bottom row, if it is up child is NULL
        if(pos == 6 || pos == 7 || pos == 8)
        {
            totalNodes--;
            T->down = NULL;
            return T;
        }
        //if check passes then make the move and store new state in child node Down
        else
        {
            for(unsigned int i = 0; i < 9; i++)
            {
                T->down->state[i] = T->state[i];
            }
            
            T->down->state[pos] = T->down->state[pos+3];
            T->down->state[pos+3] = 0;
        }
    }
    else
    {
        cout << "Error!!!" << endl;
    }
    return T;
}

/*Function that inserts Nodes into the tree and calls all needed helper Node
functions as well as find when the state has reached the goal state*/
Node* Insert(Node * T)
{
    //check to see if the state is the goal state
    bool test = true;
    for(unsigned int i = 0; i < 9; i++)
    {
        if(T->state[i] != goal_state[i])
        {
            test = false;
        }
    }
    //if it is the state set flags and get the path from root to goal
    if(test == true)
    {
        BackTrace(T);
        depth--;
        isFound = true;
        return T;
    }
    
    //if not the goal state find the pos of 0 in the current state
    int type;
    int pos;
    for(unsigned int i = 0; i < 9; i++)
    {
        if(T->state[i] == 0)
            pos = i;
    }
    
    //create a new node for child node UP and intialize/create it
    if(T->repeatCheck != 4)
    {
        T->up = new Node();
        T->up->cost = T->cost + 1;
        T->up->Parent = T;
        T->up->repeatCheck = 1;
        type = 1;
        T = Shifter(T, type, pos);
    }
    else
    {
        T->up = NULL;
    }
    
    //create a new node for child node RIGHT and intialize/create it
    if(T->repeatCheck != 3)
    {
        T->right = new Node();
        T->right->cost = T->cost + 1;
        T->right->Parent = T;
        T->right->repeatCheck = 2;
        type = 2;
        T = Shifter(T, type, pos);
    }
    else
    {
        T->right = NULL;
    }
    
    //create a new node for child node LEFT and intialize/create it
    if(T->repeatCheck != 2)
    {
        T->left = new Node();
        T->left->cost = T->cost + 1;
        T->left->Parent = T;
        T->left->repeatCheck = 3;
        type = 3;
        T = Shifter(T, type, pos);
    }
    else
    {
        T->left = NULL;
    }
    
    //create a new node for child node DOWN and intialize/create it
    if(T->repeatCheck != 1)
    {
        T->down = new Node();
        T->down->cost = T->cost + 1;
        T->down->Parent = T;
        T->down->repeatCheck = 4;
        type = 4;
        T = Shifter(T, type, pos);
    }
    else
    {
        T->down = NULL;
    }
    
    return T;
}

//-------------------------------------------------------------------
//  Different Algorithms Base
//-------------------------------------------------------------------

//The main loop chosen in order to use uniform cost search on the 8 Puzzle
void UniformSearch()
{
    priority_queue<Node*, std::vector<Node*>, node_cmp> q;
    
    Node*newNode;
    
    //Create and Initalize the root node
    newNode = new Node();
    newNode->cost = 0;
    for(unsigned int i = 0; i < 9; i++)
    {
        newNode->state[i] = board[i];
        
    }
    newNode->up = NULL;
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->down = NULL;
    newNode->Parent = NULL;
    newNode->repeatCheck = 0;
    totalNodes++;
    
    
    //Push the root node onto stack to start the building of the tree
    if(newNode){
        q.push(newNode);
    }
    
    while(!q.empty() && !isFound)
    {
        //check to find the max size of the queue during its run
        if(q.size() > maxQueueSize)
        {
            maxQueueSize = q.size();
        }
        //call to insert up,right,left and down nodes into the specified node
        Node * temp_node = Insert(q.top());
        q.pop();
        
        //Pushes the child nodes onto the priority queue, which only uses cost
        //to determine placement in the queue
        if(temp_node->up){
            q.push(temp_node->up);
        }
        
        if(temp_node->right){
            q.push(temp_node->right);
        }
        
        if(temp_node->left){
            q.push(temp_node->left);
        }
                
        if(temp_node->down){
            q.push(temp_node->down);
        }
        
    }
    
    EndDisplay();
    
    ClearTree(newNode);
}

//The main loop chosen in order to use A* with Misplaced Tiles on the 8 Puzzle
void MisplacedTile()
{
    priority_queue<Node*, std::vector<Node*>, node_cmp> q;
    
    Node*newNode;
    
    //build up starting Node
    newNode = new Node();
    newNode->cost = 0;
    for(unsigned int i = 0; i < 9; i++)
    {
        newNode->state[i] = board[i];
    }
    newNode->up = NULL;
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->down = NULL;
    newNode->Parent = NULL;
    newNode->repeatCheck = 0;
    totalNodes++;
    
    //check that pushes root Node onto the tree
    if(newNode){
        q.push(newNode);
    }
    
    //Loop to continue creating tree until solution is found
    while(!q.empty() && !isFound)
    {
        //if statement to get the max queue size
        if(q.size() > maxQueueSize)
        {
            maxQueueSize = q.size();
        }
        //call to insert up,right,left and down nodes into the specified node
        Node * temp_node = Insert(q.top());
        q.pop();
        
        /*Find the number of misplaced tiles between the goal state and each child
        state and set the dist of the child nodes to their corresponding misplaced
        number*/
        int upcount = 0;
        int rightcount = 0;
        int leftcount = 0;
        int downcount = 0;
        for(unsigned int i = 0; i < 9; i++)
        {
            if(temp_node->up)
            {
                if(goal_state[i] != temp_node->up->state[i])
                {
                    upcount++;
                }
            }
            if(temp_node->right)
            {
                if(goal_state[i] != temp_node->right->state[i])
                {
                    rightcount++;
                }
            }
            if(temp_node->left)
            {
                if(goal_state[i] != temp_node->left->state[i])
                {
                    leftcount++;
                }
            }
            if(temp_node->down)
            {
                if(goal_state[i] != temp_node->down->state[i])
                {
                    downcount++;
                }
            }
        }
        
        if(temp_node->up)
        {
            temp_node->up->dist = upcount;
        }
        if(temp_node->right)
        {
            temp_node->right->dist = rightcount;
        }
        if(temp_node->left)
        {
            temp_node->left->dist = leftcount;
        }
        if(temp_node->down)
        {
            temp_node->down->dist = downcount;
        }
        
        //Pushes the child nodes onto the priority queue, which uses cost + dist
        //to determine placement in the queue
        if(temp_node->up){
            q.push(temp_node->up);
        }
        
        if(temp_node->right){
            q.push(temp_node->right);
        }
        
        if(temp_node->left){
            q.push(temp_node->left);
        }
                
        if(temp_node->down){
            q.push(temp_node->down);
        }
    }
    
    //After goal state is found display and clean up tree
    EndDisplay();
    
    ClearTree(newNode);
}

//The main loop chosen in order to use A* with Manhatten Dist on the 8 Puzzle
void ManhattanDist()
{
     priority_queue<Node*, std::vector<Node*>, node_cmp> q;
    
    //build up starting Node
    Node*newNode;
    
    newNode = new Node();
    newNode->cost = 0;
    for(unsigned int i = 0; i < 9; i++)
    {
        newNode->state[i] = board[i];
    }
    newNode->up = NULL;
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->down = NULL;
    newNode->Parent = NULL;
    newNode->repeatCheck = 0;
    totalNodes++;
    
    //check that pushes root Node onto the tree
    if(newNode){
        q.push(newNode);
    }
    
    while(!q.empty() && !isFound)
    {
        //if statement to get the max queue size
        if(q.size() > maxQueueSize)
        {
            maxQueueSize = q.size();
        }
        //call to insert up,right,left and down nodes into the specified node
        Node * temp_node = Insert(q.top());
        q.pop();
        
        /*Find the distance between the values in the node and their goal state
         * while setting dist to equal the total Manhatten distance away the
         * tiles are*/
        int upcount = 0;
        int rightcount = 0;
        int leftcount = 0;
        int downcount = 0;
        
        int targetx;
        int targety;
        int upStateX;
        int upStateY;
        int rightStateX;
        int rightStateY;
        int leftStateX;
        int leftStateY;
        int downStateX;
        int downStateY;
        
        for(unsigned int i = 0; i < 9; i++)
        {
            //Checks for the Goal State
            
            //check for the goal state x value
            if(goal_state[i] == 0 || goal_state[i] == 3 || goal_state[i] == 6){
                targetx = 1;
            }
            else if(goal_state[i] == 1 || goal_state[i] == 4 || goal_state[i] == 7){
                targetx = 2;
            }
            else{
                targetx = 3;
            }
            //check for the goal state y value
            if(goal_state[i] == 1 || goal_state[i] == 2 || goal_state[i] == 3){
                targety = 1;
            }
            else if(goal_state[i] == 4 || goal_state[i] == 5 || goal_state[i] == 6){
                targety = 2;
            }
            else{
                targety = 3;
            }
            
            //Checks for the Up Node
            if(temp_node->up)
            {
                //Checks for the Up Node x value
                if(temp_node->up->state[i] == 0 || temp_node->up->state[i] == 3 || 
                temp_node->up->state[i] == 6){
                    upStateX = 1;
                }
                else if(temp_node->up->state[i] == 1 || temp_node->up->state[i] == 4 
                || temp_node->up->state[i] == 7){
                    upStateX = 2;
                }
                else{
                    upStateX = 3;
                }
                //Checks for the Up Node y value
                if(temp_node->up->state[i] == 1 || temp_node->up->state[i] == 2 || 
                temp_node->up->state[i] == 3){
                    upStateY = 1;
                }
                else if(temp_node->up->state[i] == 4 || temp_node->up->state[i] == 5 
                || temp_node->up->state[i] == 6){
                    upStateY = 2;
                }
                else{
                    upStateY = 3;
                }
            }
            
            //Checks for the Right Node
            if(temp_node->right)
            {
                //Checks for the Right Node x value
                if(temp_node->right->state[i] == 0 || temp_node->right->state[i] == 3 || 
                temp_node->right->state[i] == 6){
                    rightStateX = 1;
                }
                else if(temp_node->right->state[i] == 1 || temp_node->right->state[i] == 4 
                || temp_node->right->state[i] == 7){
                    rightStateX = 2;
                }
                else{
                    rightStateX = 3;
                }
                //Checks for the Right Node y value
                if(temp_node->right->state[i] == 1 || temp_node->right->state[i] == 2 || 
                temp_node->right->state[i] == 3){
                    rightStateY= 1;
                }
                else if(temp_node->right->state[i] == 4 || temp_node->right->state[i] == 5 
                || temp_node->right->state[i] == 6){
                    rightStateY = 2;
                }
                else{
                    rightStateY = 3;
                }
            }
            
            //Checks for the Left Node
            if(temp_node->left)
            {
                //Checks for the Left Node x value
                if(temp_node->left->state[i] == 0 || temp_node->left->state[i] == 3 || 
                temp_node->left->state[i] == 6){
                    leftStateX = 1;
                }
                else if(temp_node->left->state[i] == 1 || temp_node->left->state[i] == 4 
                || temp_node->left->state[i] == 7){
                    leftStateX = 2;
                }
                else{
                    leftStateX = 3;
                }
                //Checks for the Left Node y value
                if(temp_node->left->state[i] == 1 || temp_node->left->state[i] == 2 || 
                temp_node->left->state[i] == 3){
                    leftStateY = 1;
                }
                else if(temp_node->left->state[i] == 4 || temp_node->left->state[i] == 5 
                || temp_node->left->state[i] == 6){
                    leftStateY = 2;
                }
                else{
                    leftStateY = 3;
                }
            }
            
            //Checks for the Down Node
            if(temp_node->down)
            {
                //Checks for the Down Node x value
                if(temp_node->down->state[i] == 0 || temp_node->down->state[i] == 3 || 
                temp_node->down->state[i] == 6){
                    downStateX = 1;
                }
                else if(temp_node->down->state[i] == 1 || temp_node->down->state[i] == 4 
                || temp_node->down->state[i] == 7){
                    downStateX = 2;
                }
                else{
                    downStateX = 3;
                }
                //Checks for the Down Node y value
                if(temp_node->down->state[i] == 1 || temp_node->down->state[i] == 2 || 
                temp_node->down->state[i] == 3){
                    downStateY = 1;
                }
                else if(temp_node->down->state[i] == 4 || temp_node->down->state[i] == 5 
                || temp_node->down->state[i] == 6){
                    downStateY = 2;
                }
                else{
                    downStateY = 3;
                }
            }
            
            //checks for each of the child nodes to make sure they are not NULL,
            //as well as set the count variables to the total distance away
            if(temp_node->up)
            {
                int uptemp = abs(upStateX-targetx) + abs(upStateY - targety);
                upcount += uptemp;
            }
            if(temp_node->right)
            {
                int righttemp = abs(rightStateX-targetx) + abs(rightStateY - targety);
                rightcount += righttemp;
            }
            if(temp_node->left)
            {
                int lefttemp = abs(leftStateX-targetx) + abs(leftStateY - targety);
                leftcount += lefttemp;
            }
            if(temp_node->down)
            {
                int downtemp = abs(downStateX-targetx) + abs(downStateY - targety);
                downcount += downtemp;
            }
        }
        
        //sets dist variable in each child node to the distance away
        if(temp_node->up)
        {
            temp_node->up->dist = upcount;
        }
        if(temp_node->right)
        {
            temp_node->right->dist = rightcount;
        }
        if(temp_node->left)
        {
            temp_node->left->dist = leftcount;
        }
        if(temp_node->down)
        {
            temp_node->down->dist = downcount;
        }
        
        //Pushes the child nodes onto the priority queue, which uses cost + dist
        //to determine placement in the queue
        if(temp_node->up){
            q.push(temp_node->up);
        }
        
        if(temp_node->right){
            q.push(temp_node->right);
        }
        
        if(temp_node->left){
            q.push(temp_node->left);
        }
                
        if(temp_node->down){
            q.push(temp_node->down);
        }
            
    }
    
    //Pushes the child nodes onto the priority queue, which uses cost + dist
    //to determine placement in the queue
        
    EndDisplay();
    
    ClearTree(newNode);
}

int main()
{
    //intialize variables
    isDefaultPuzzle = true;
    isFound = false;
    totalNodes = 0;
    maxQueueSize = 0;
    depth = 0;
    
    
    //load goal state and ask for user input functions
    loadGoalState();
    puzzleSelect();
    boardCreate();
    selectAlgorithm();
    
    //check to see which algorithm will be used
    if(algorithm == 1)
    {
        UniformSearch();
    }
    else if(algorithm == 2)
    {
        MisplacedTile();
    }
    else
    {
        ManhattanDist();
    }
    
    return 0;
}
