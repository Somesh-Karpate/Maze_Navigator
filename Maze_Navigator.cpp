#include <iostream>
#include <vector>
#include <cstdlib>   // for rand() and srand()
#include <ctime>     // for time()
#include <limits>    // for numeric_limits
#include <iomanip>   // for setw
#include <cmath>     // for math functions

using namespace std;

const int MAX_PLAYERS = 2;   // Number of players in the game
const int MAX_LEVELS = 3;    // Number of levels in the game
const int MAZE_MIN_SIZE = 10;  // Minimum size of the maze (MAZE_MIN_SIZE x MAZE_MIN_SIZE)
const int MAZE_MAX_SIZE = 15; // Maximum size of the maze (MAZE_MAX_SIZE x MAZE_MAX_SIZE)
const int NUM_TELEPORTS = 5;  // Number of teleportation points in the maze
const int NUM_OBSTACLES = 15; // Number of obstacles in the maze
const double MONSTER_PROBABILITY = 0.2; // Probability of encountering a monster

// Maze game class
class MazeGame {
private:
    struct Player {
        pair<int, int> position;
        char symbol;
        int health;
        bool hasSword;
    };

    struct Monster {
        pair<int, int> position;
        char symbol;
        int health;
    };

    vector<vector<char>> maze;
    vector<Player> players;
    vector<Monster> monsters;
    pair<int, int> goalPosition;
    int currentLevel;

public:
    MazeGame();
    void play();
    void printMaze();
    bool movePlayer(Player& player, char direction);
    void generateMaze(int size);
    void addTeleports(int numTeleports);
    void addObstacles(int numObstacles);
    void addMonsters(int size);
    bool isLevelComplete();
    void attackMonster(Player& player);
    bool checkEncounterMonster(Player& player);
    void regenerateHealth(Player& player);
    void displayPlayerStatus(Player& player);
};

// Constructor to initialize the game
MazeGame::MazeGame() {
    currentLevel = 1;
    players = { { {0, 0}, '1', 100, false }, { {0, 0}, '2', 100, false } }; // Initialize two players at same position for simplicity
}

// Generate a new maze for the current level
void MazeGame::generateMaze(int size) {
    maze = vector<vector<char>>(size, vector<char>(size, ' '));

    // Randomly place players and goal ('G')
    srand(time(nullptr));
    for (int i = 0; i < players.size(); ++i) {
        players[i].position = {rand() % size, rand() % size};
        maze[players[i].position.first][players[i].position.second] = players[i].symbol;
    }

    do {
        goalPosition = {rand() % size, rand() % size};
    } while (maze[goalPosition.first][goalPosition.second] != ' ');

    maze[goalPosition.first][goalPosition.second] = 'G';

    // Randomly place obstacles ('X')
    addObstacles(NUM_OBSTACLES);

    // Randomly place walls ('#')
    int numWalls = size * size / 4; // Approximately one-fourth of maze cells are walls
    for (int i = 0; i < numWalls; ++i) {
        int x = rand() % size;
        int y = rand() % size;
        if (maze[x][y] == ' ') {
            maze[x][y] = '#';
        }
    }

    // Add teleportation points
    addTeleports(NUM_TELEPORTS);

    // Add monsters
    addMonsters(size);
}

// Add teleportation points to the maze
void MazeGame::addTeleports(int numTeleports) {
    int added = 0;
    while (added < numTeleports) {
        int x = rand() % maze.size();
        int y = rand() % maze.size();
        if (maze[x][y] == ' ') {
            maze[x][y] = 'T';
            added++;
        }
    }
}

// Add obstacles to the maze
void MazeGame::addObstacles(int numObstacles) {
    int added = 0;
    while (added < numObstacles) {
        int x = rand() % maze.size();
        int y = rand() % maze.size();
        if (maze[x][y] == ' ') {
            maze[x][y] = 'X';
            added++;
        }
    }
}

// Add monsters to the maze
void MazeGame::addMonsters(int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (maze[i][j] == ' ' && ((double) rand() / RAND_MAX) < MONSTER_PROBABILITY) {
                Monster monster = { {i, j}, 'M', 50 }; // Monsters start with 50 health points
                monsters.push_back(monster);
                maze[i][j] = 'M';
            }
        }
    }
}

// Print the maze grid
void MazeGame::printMaze() {
    cout << "---------------------" << endl;
    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[i].size(); ++j) {
            cout << setw(2) << maze[i][j];
        }
        cout << endl;
    }
    cout << "---------------------" << endl;
}

// Play method for MazeGame
void MazeGame::play() {
    cout << "Welcome to the Complicated Maze Game!" << endl;
    cout << "Navigate through the maze to find the goal ('G')." << endl;
    cout << "Commands: 'w' (up), 'a' (left), 's' (down), 'd' (right)" << endl;
    cout << "Commands: 'k' (kill the monster), 'r' (regenerate health), 'p' (player status)" << endl;

    while (currentLevel <= MAX_LEVELS) {
        int mazeSize = MAZE_MIN_SIZE + currentLevel - 1;
        generateMaze(mazeSize);

        cout << "Level " << currentLevel << endl;
        printMaze();

        while (!isLevelComplete()) {
            for (int i = 0; i < players.size(); ++i) {
                char move;
                cout << "Player " << players[i].symbol << ", enter your move (w/a/s/d/k/r/p): ";
                cin >> move;

                switch (move) {
                    case 'w':
                    case 'a':
                    case 's':
                    case 'd':
                        if (movePlayer(players[i], move)) {
                            printMaze();
                            if (players[i].position == goalPosition) {
                                cout << "Player " << players[i].symbol << " reached the goal ('G'). Move to the next level." << endl;
                                break;
                            }
                            // Check if player encounters a monster
                            if (checkEncounterMonster(players[i])) {
                                attackMonster(players[i]);
                            }
                            // Regenerate health for the player
                            regenerateHealth(players[i]);
                            // Display player status
                            displayPlayerStatus(players[i]);
                        } else {
                            cout << "Invalid move. Try again." << endl;
                        }
                        break;
                    case 'k':
                        if (players[i].hasSword && checkEncounterMonster(players[i])) {
                            attackMonster(players[i]);
                        } else {
                            cout << "You need a sword to attack the monster ('k')." << endl;
                        }
                        break;
                    case 'r':
                        regenerateHealth(players[i]);
                        break;
                    case 'p':
                        displayPlayerStatus(players[i]);
                        break;
                    default:
                        cout << "Invalid command. Use 'w', 'a', 's', 'd', 'k', 'r', or 'p'." << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;
                }
            }
        }

        currentLevel++;
    }

    cout << "You completed all levels. Game over!" << endl;
}

// Move player in the maze based on direction ('w', 'a', 's', 'd')
bool MazeGame::movePlayer(Player& player, char direction) {
    int newX = player.position.first;
    int newY = player.position.second;

    switch (direction) {
        case 'w':   // Move up
            newX--;
            break;
        case 'a':   // Move left
            newY--;
            break;
        case 's':   // Move down
            newX++;
            break;
        case 'd':   // Move right
            newY++;
            break;
        default:
            return false;
    }

    // Check if new position is within bounds and not a wall ('#'), obstacle ('X'), or occupied by another player
    if (newX >= 0 && newX < maze.size() && newY >= 0 && newY < maze[newX].size() &&
        maze[newX][newY] != '#' && maze[newX][newY] != 'X' &&
        maze[newX][newY] != players[0].symbol && maze[newX][newY] != players[1].symbol) {
        // Move player to new position
        maze[player.position.first][player.position.second] = ' ';
        player.position = {newX, newY};
        maze[player.position.first][player.position.second] = player.symbol;

        // Check if player lands on a teleportation point ('T')
        if (maze[newX][newY] == 'T') {
            // Teleport to a random location in the maze
            int randX, randY;
            do {
                randX = rand() % maze.size();
                randY = rand() % maze.size();
            } while (maze[randX][randY] != ' ');

            maze[newX][newY] = ' ';
            player.position = {randX, randY};
            maze[player.position.first][player.position.second] = player.symbol;
        }

        return true;
    } else {
        return false;
    }
}

// Check if the player encounters a monster at the current position
bool MazeGame::checkEncounterMonster(Player& player) {
    for (int i = 0; i < monsters.size(); ++i) {
        if (monsters[i].position == player.position) {
            return true;
        }
    }
    return false;
}

// Attack a monster
void MazeGame::attackMonster(Player& player) {
    for (int i = 0; i < monsters.size(); ++i) {
        if (monsters[i].position == player.position) {
            monsters[i].health -= 20; // Player deals 20 damage to the monster
            cout << "Player " << player.symbol << " attacked the monster ('M'). Monster health reduced to " << monsters[i].health << "." << endl;
            if (monsters[i].health <= 0) {
                maze[monsters[i].position.first][monsters[i].position.second] = ' ';
                monsters.erase(monsters.begin() + i); // Remove monster from the vector
                cout << "Player " << player.symbol << " defeated the monster ('M')." << endl;
            }
            break;
        }
    }
}

// Regenerate health for the player
void MazeGame::regenerateHealth(Player& player) {
    if (player.health < 100) {
        player.health += 40; // Regenerate 40 health points
        if (player.health > 100) {
            player.health = 100;
        }
        cout << "Player " << player.symbol << " regenerated health. Current health: " << player.health << endl;
    }
}

// Display player status (health and inventory)
void MazeGame::displayPlayerStatus(Player& player) {
    cout << "Player " << player.symbol << " status:" << endl;
    cout << " - Health: " << player.health << endl;
    cout << " - Inventory: ";
    if (player.hasSword) {
        cout << "Sword";
    } else {
        cout << "None";
    }
    cout << endl;
}

// Check if the current level is complete (any player reached the goal)
bool MazeGame::isLevelComplete() {
    for (int i = 0; i < players.size(); ++i) {
        if (players[i].position == goalPosition) {
            return true;
        }
    }
    return false;
}

int main() {
    MazeGame game;
    game.play();

    return 0;
}
