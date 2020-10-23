#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <map>
using namespace std;

#define randRange(min,max)	((min) + rand() % ((max) - (min) + 1)) 

const static int LINESIZE = 1024;

class Point {
    public:
        int x;
        int y; 
};

class Maze {

    public:
    	constexpr static double REWARD_FOUND_GOAL = 100;
        constexpr static double REWARD_CAUGHT = -100;
        constexpr static double REWARD_TIME_EXPIRED = 0;
        constexpr static double REWARD_OTHER = -1;

        constexpr static int MAX_STEPS = 50;

        constexpr static int MIN_ACTION = 0;
        constexpr static int MAX_ACTION = 4;
        constexpr static int ACTION_NONE = 0;
        constexpr static int ACTION_LEFT = 1;
        constexpr static int ACTION_RIGHT = 2;
        constexpr static int ACTION_UP = 3;
        constexpr static int ACTION_DOWN = 4;
        
    private:
        int **maze_obstacles;
        constexpr static int GOAL_X = 6;
        constexpr static int GOAL_Y = 6;

        Point *player;
        Point *opponent;

        int num_steps;

        int make_player_move(int action) {
            switch (action) {
                case ACTION_NONE: break;
                case ACTION_LEFT:
                    if (maze_obstacles[player->x-1][player->y] == 0)
                        player->x = player->x - 1;
                    break;
                case ACTION_RIGHT:
                    if (maze_obstacles[player->x+1][player->y] == 0)
                        player->x = player->x + 1;
                    break;
                case ACTION_UP:
                    if (maze_obstacles[player->x][player->y - 1] == 0)
                        player->y = player->y - 1;
                    break;
                case ACTION_DOWN:                 
                    if (maze_obstacles[player->x][player->y+1] == 0)
                        player->y = player->y + 1;
                    break;   
                default:
                    cout << "Unknown action" << endl;
                    break;
            }
            if ((player->x == GOAL_X) && (player->y == GOAL_Y))
                return 1;
            if ((player->x == opponent->x) && (player->y == opponent->y))
                return -1;
            return 0;
        }

        int opponent_move() {
            int best_action = ACTION_NONE;
            double best_dist = ((player->x - opponent->x) * (player->x - opponent->x)) + ((player->y - opponent->y) * (player->y - opponent->y));
            double the_dist;
            if (maze_obstacles[opponent->x-1][opponent->y] == 0) {
                the_dist = ((player->x - (opponent->x-1)) * (player->x - (opponent->x-1))) + ((player->y - opponent->y) * (player->y - opponent->y));
                if (the_dist < best_dist) {
                    best_action = ACTION_LEFT;
                    best_dist = the_dist;
                }
            }
            if (maze_obstacles[opponent->x+1][opponent->y] == 0) {
                the_dist = ((player->x - (opponent->x+1)) * (player->x - (opponent->x+1))) + ((player->y - opponent->y) * (player->y - opponent->y));
                if (the_dist < best_dist) {
                    best_action = ACTION_RIGHT;
                    best_dist = the_dist;
                }
            }
            if (maze_obstacles[opponent->x][opponent->y-1] == 0) {
                the_dist = ((player->x - opponent->x) * (player->x - opponent->x)) + ((player->y - (opponent->y-1)) * (player->y - (opponent->y-1)));
                if (the_dist < best_dist) {
                    best_action = ACTION_UP;
                    best_dist = the_dist;
                }
            }
            if (maze_obstacles[opponent->x][opponent->y+1] == 0) {
                the_dist = ((player->x - opponent->x) * (player->x - opponent->x)) + ((player->y - (opponent->y+1)) * (player->y - (opponent->y+1)));
                if (the_dist < best_dist) {
                    best_action = ACTION_DOWN;
                    best_dist = the_dist;
                }
            }
            switch (best_action) {
                case ACTION_NONE: break;
                case ACTION_LEFT: opponent->x = opponent->x - 1; break;
                case ACTION_RIGHT: opponent->x = opponent->x + 1; break;
                case ACTION_UP: opponent->y = opponent->y - 1; break;
                case ACTION_DOWN: opponent->y = opponent->y + 1; break;
            }
            if (best_dist == 0.0)
                return 1;
            return 0;
        }
        
    public:
        Maze() {
        }

        void setup () {
            maze_obstacles = new int*[8];
            for (int x = 1; x <= 7; x++)
                maze_obstacles[x] = new int[8];
            for (int x = 1; x <= 7; x++)
                for (int y = 1; y <= 7; y++)
                    maze_obstacles[x][y] = 1;
            for (int x = 2; x <= 6; x++)
                for (int y = 2; y <= 6; y++)
                    maze_obstacles[x][y] = 0;
            maze_obstacles[4][4] = 1;
            maze_obstacles[3][4] = 1;
            maze_obstacles[4][3] = 1;
            maze_obstacles[5][4] = 1;
            maze_obstacles[4][5] = 1;
        }
    
        void initialize() {
            player = new Point();
            opponent = new Point();
            num_steps = 1;
            do {
                player->x = randRange(2,6);
                player->y = randRange(2,6);
            } while ((maze_obstacles[player->x][player->y] == 1) || ((player->x == GOAL_X) && (player->y == GOAL_Y)));
            do {
                opponent->x = randRange(2,6);
                opponent->y = randRange(2,6);
            } while ((maze_obstacles[opponent->x][opponent->y] == 1) || ((player->x == opponent->x) && (player->y == opponent->y)));
        }
    
        void current_locations(Point *p, Point *o) {
            p->x = player->x;
            p->y = player->y;
            o->x = opponent->x;
            o->y = opponent->y;
        }
    
        double execute_action(int action) {
            int move_result = make_player_move(action);
            if (move_result == 1)
                return REWARD_FOUND_GOAL;
            else if (move_result == -1)
                return REWARD_CAUGHT;
            num_steps = num_steps + 1;
            if (opponent_move() == 1)
                return REWARD_CAUGHT;
            if (num_steps >= MAX_STEPS)
                return REWARD_TIME_EXPIRED;
            return REWARD_OTHER;
        }    

        void show_maze() {
            for (int y = 1; y <= 7; y++) {
                for (int x = 1; x <= 7; x++)
                    if (maze_obstacles[x][y] == 1)
                        cout << "X";
                    else if ((x == player->x) && (y == player->y))
                        cout << "P";
                    else if ((x == opponent->x) && (y == opponent->y))
                        cout << "O";
                    else if ((x == GOAL_X) && (y == GOAL_Y))
                        cout << "G";
                    else
                        cout << " ";
                cout << "\n";
            }
        }
 };

class Main {
    public:
        Maze the_maze;
        map<string,string> mymap;
        string combin;
        string bestreward;
        string previous;

        Main() {
        }
    
        int train_pick_action(Point *player_point, Point *opponent_point) {
            // Add code here to construct a state number from the state information
            //   (player_point and opponent_point)
            // Use your state to pick an action by looking up the value of actions
            //   for your state, for some large percentage of the time (e.g., 97%)
            //   pick the highest valued action, otherwise pick a random action
            //   (this is one way to trade off exploration exploitation)
            
            //int the_action = the_maze.MIN_ACTION - 1;
            //MY CODE
    
            combin = to_string(player_point->x) + to_string(player_point->y) + to_string(opponent_point->x)+to_string(opponent_point->y);
            int myaction;
            for(int i = 1;i<=4;i++){
                mymap[combin+ to_string(i)];
                myaction =i;
            }

            previous = combin+to_string(myaction);
            return myaction;
           
            // *** CODE TO REMOVE *** - start
            // The code below allows a user to interactively select the action, this
            //   code is provided only so you can understand the problem (and maybe
            //   play with it), you should cut this code out once you implement 
            //   your learning system.
            // cout << "The maze:" << "\n";
            // the_maze.show_maze();
            // cout << "\n";
            // do {
            //     cout << "Action (" << the_maze.MIN_ACTION << "-" << the_maze.MAX_ACTION << "): ";
            //     cin >> the_action;
            //     cout << "Action is " << the_action << "\n";
            // } while ((the_action < the_maze.MIN_ACTION) || (the_action > the_maze.MAX_ACTION));
            // *** CODE TO REMOVE *** - end
            
            //return the_action;  
              
        }
        
        int test_pick_action(Point *player_point, Point *opponent_point) {
            // Add code here to construct a state number from the state information
            //   (player_point and opponent_point)
            // Use your state to pick an action by looking up the value of actions
            //   for your state, and picking the highest valued action (if more than
            //   one action has the highest value pick amongst those actions
            //   randomly)
            
            // int the_action = the_maze.MIN_ACTION - 1;
            //MY CODE
            combin = to_string(player_point->x) + to_string(player_point->y) + to_string(opponent_point->x)+to_string(opponent_point->y);

            int myaction;
            bestreward=mymap.at(combin+"1");
            for(int i=1;i<=4;i++){
                if (mymap.at(combin+to_string(i))>= bestreward){
                    bestreward=mymap.at(combin+to_string(i));
                    myaction = i;
                }
            }
            



            return myaction;
            // *** CODE TO REMOVE *** - start
            // The code below allows a user to interactively select the action, this
            //   code is provided only so you can understand the problem (and maybe
            //   play with it), you should cut this code out once you implement 
            //   your learning system.
            // cout << "The maze:" << "\n";
            // the_maze.show_maze();
            // cout << "\n";
            // do {
            //     cout << "Action (" << the_maze.MIN_ACTION << "-" << the_maze.MAX_ACTION << "): ";
            //     cin >> the_action;
            //     cout << "Action is " << the_action << "\n";
            // } while ((the_action < the_maze.MIN_ACTION) || (the_action > the_maze.MAX_ACTION));
            // *** CODE TO REMOVE *** - end
            
            //return the_action;        
        }
        
        void train_learner(Point *player_point, Point *opponent_point, int action, double reward) {
            // Update your learner model with the state (player_point,opponent_point), 
            //   action, and reward action provided, note that you will likely have
            //   to keep track of the previous state and action to make this work
        

            mymap[previous]=reward;


        }
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: a.out <numtraingames> <numtestgames>" << endl;
        return -1;
    }
    int num_train_games = atoi(argv[1]);
    int num_test_games = atoi(argv[2]);
    int action;
    double reward;
    Point *player_point = new Point();
    Point *opponent_point = new Point();
    Main main_item;
    main_item.the_maze.setup();
    
    for (int i = 1; i <= num_train_games; i++) {
        cout << "Training game " << i << "\n";
        main_item.the_maze.initialize();  
        double game_reward = 0;
        do {
            main_item.the_maze.current_locations(player_point,opponent_point);
            action = main_item.train_pick_action(player_point,opponent_point);
            reward = main_item.the_maze.execute_action(action);
            game_reward = game_reward + reward;
            main_item.train_learner(player_point,opponent_point,action,reward);
        } while (reward == main_item.the_maze.REWARD_OTHER);
        
        cout << "Reward for training game " << i << " is " << game_reward << "\n";
    }
    
    double total_reward = 0;
    for (int i = 1; i <= num_test_games; i++) {
        cout << "Test game " << i << "\n";
        main_item.the_maze.initialize();  
        double game_reward = 0;
        do {
            main_item.the_maze.current_locations(player_point,opponent_point);
            action = main_item.test_pick_action(player_point,opponent_point);
            reward = main_item.the_maze.execute_action(action);
            game_reward = game_reward + reward;
        } while (reward == main_item.the_maze.REWARD_OTHER);
        cout << "Reward for test game " << i << " is " << game_reward << "\n";
        total_reward = total_reward + game_reward;
    }
    cout << "Average test reward = " << (total_reward / num_test_games) << "\n";
    return 1;
}
