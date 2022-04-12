// final.cpp
// Benny Small, CISP 400
// 12/16/2021
#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;

// Constants

const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 10;

const int BOARD_EMPTY = 0;
const int BOARD_WALL = 1;
const int BOARD_BATTERY = 2;
const int BOARD_ROBOT = 3;
const int NUM_BATTERIES = 40;

const int EPOCHS = 400;
const int NUM_GENES = 16;
const int GENE_SIZE = 5;
const int NUM_ROBOTS = 200;

const int MOVE_UP = 0;
const int MOVE_DOWN = 1;
const int MOVE_LEFT = 2;
const int MOVE_RIGHT = 3;
const int MOVE_RANDOM = 4;

// Function Prototypes

void ProgramGreeting();

// Classes

class Board
{
private:
    int board[BOARD_WIDTH][BOARD_HEIGHT];

public:
    int GetAtIndex(int x, int y)
    {
        if ((x < 0) || (x > BOARD_WIDTH) || (y < 0) || (y > BOARD_HEIGHT))
        {
            return 0;
        }

        return board[x][y];
    }

    bool TrySetAtIndex(int x, int y, int boardObject)
    {
        if ((x < 0) || (x > BOARD_WIDTH) || (y < 0) || (y > BOARD_HEIGHT))
        {
            return false;
        }

        board[x][y] = boardObject;
        return true;
    }

    void FillBoard()
    {
        for (int i = 0; i < 40; i++)
        {
            while (true) // Put robot in board
            {
                int selectionX = rand() % BOARD_WIDTH;
                int selectionY = rand() % BOARD_HEIGHT;
                if (board[selectionX][selectionY] == BOARD_BATTERY)
                {
                    continue;
                }
                else
                {
                    board[selectionX][selectionY] = BOARD_BATTERY;
                    break;
                }
            }
        }
    }

    void Reset()
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            for (int y = 0; y < BOARD_HEIGHT; y++)
            {
                board[x][y] = BOARD_EMPTY;
            }
        }
    }
};

class Robot
{
public:
    int genes[NUM_GENES][GENE_SIZE]{};
    int energy = 5;
    int posX = -1;
    int posY = -1;
    int totalEnergyCollected = 5;
    int turnsSurvived = 0;

    void RandomizeGenes()
    {
        for (int i = 0; i < NUM_GENES; i++)
        {
            for (int j = 0; j < GENE_SIZE - 1; j++)
            {
                genes[i][j] = rand() % 4;
            }
            genes[i][4] = rand() % 5;
        }
    }

    void TryMove(Board& b, int direction)
    {

        int moveToPosX = posX;
        int moveToPosY = posY;

        switch (direction)
        {
        case MOVE_UP:
            moveToPosY -= 1;
            break;
        case MOVE_DOWN:
            moveToPosY += 1;
            break;
        case MOVE_LEFT:
            moveToPosX -= 1;
            break;
        case MOVE_RIGHT:
            moveToPosY += 1;
            break;
        case MOVE_RANDOM:
            TryMove(b, rand() % 4);
            return;
        }

        int moveToObject = b.GetAtIndex(moveToPosX, moveToPosY);

        energy -= 1;
        turnsSurvived += 1;

        if (moveToObject == BOARD_WALL)
        {
            return;
        }
        if (moveToObject == BOARD_BATTERY)
        {
            energy += 5;
            totalEnergyCollected += 5;
        }

        b.TrySetAtIndex(posX, posY, BOARD_EMPTY);
        b.TrySetAtIndex(moveToPosX, moveToPosY, BOARD_ROBOT);
        posX = moveToPosX;
        posY = moveToPosY;

    }

    void Run(Board& b)
    {
        // North south east west
        int boardRead[4] = { b.GetAtIndex(posX, posY - 1), b.GetAtIndex(posX, posY + 1), b.GetAtIndex(posX + 1, posY), b.GetAtIndex(posX - 1, posY) };

        for (int i = 0; i < NUM_GENES - 1; i++)
        {
            bool correctGene = true;
            for (int j = 0; j < 4; j++)
            {
                if (genes[i][j] != boardRead[j])
                {
                    correctGene = false;
                    break;
                }
            }

            if (correctGene)
            {
                TryMove(b, genes[i][4]);
                return;
            }
        }

        TryMove(b, genes[NUM_GENES - 1][4]); // If no matches are found, run last gene
        return;
    }

    void Reset()
    {
        energy = 5;
        posX = -1;
        posY = -1;
        totalEnergyCollected = 5;
        turnsSurvived = 0;
    }

    static void BreedRobots(Robot parent1, Robot parent2, Robot& child1, Robot& child2)
    {
        bool whosGenesFirst = (rand() % 2) == 0; // true = parent1, false = parent2

        // whosFirst == true && (i < splitGenomeSize) == true --> parent1
        // whosFirst == false && (i < splitGenomeSize) == true --> parent2
        // whosFirst == true && (i < splitGenomeSize) == false --> parent2
        // whosFirst == false && (i < splitGenomeSize) == false --> parent1

        // first child genome = parent1 if whosFirst == (i < splitGenomeSize), else parent2
        // second child genome is inverse of this

        for (int i = 0; i < NUM_GENES; i++)
        {
            bool usingParent1sGenes = whosGenesFirst == (i < (NUM_GENES / 2));

            for (int j = 0; j < GENE_SIZE; j++)
            {
                if (i < (NUM_GENES / 2))
                {
                    child1.genes[i][j] = usingParent1sGenes ? parent1.genes[i][j] : parent2.genes[i][j];
                    child2.genes[i][j] = usingParent1sGenes ? parent2.genes[i][j] : parent1.genes[i][j];
                }
                else
                {
                    child1.genes[i][j] = usingParent1sGenes ? parent2.genes[i][j] : parent1.genes[i][j];
                    child2.genes[i][j] = usingParent1sGenes ? parent1.genes[i][j] : parent2.genes[i][j];
                }
            }

            if (rand() % 20 == 0) // 5% chance
            {
                Robot& chosenChild = (rand() % 2 == 0) ? child1 : child2;

                int mutationSelection = rand() % 5;
                if (mutationSelection == 4) // Mutating action
                {
                    chosenChild.genes[i][4] = rand() % 5;
                }
                else
                {
                    chosenChild.genes[i][rand() % 4] = rand() % 4;
                }
            }
        }
    }
};




int main()
{
    srand(time(NULL));

    ProgramGreeting();

    cout << "Input any key and press enter to begin the simulation." << endl;
    string s;
    cin >> s;

    Robot* robots = new Robot[NUM_ROBOTS];

    for (int i = 0; i < NUM_ROBOTS; i++)
    {
        robots[i].RandomizeGenes();
    }

    Board b = Board();

    for (int g = 0; g < EPOCHS; g++)
    {
        cout << "-------------------------------------------" << endl;
        for (int j = 0; j < NUM_ROBOTS; j++)
        {
            b.Reset();

            Robot& robot = robots[j];
            robot.Reset();

            b.FillBoard();

            while (true) // Put robot in board
            {
                int selectionX = rand() % BOARD_WIDTH;
                int selectionY = rand() % BOARD_HEIGHT;
                if (b.GetAtIndex(selectionX, selectionY) == BOARD_BATTERY)
                {
                    continue;
                }
                else
                {
                    b.TrySetAtIndex(selectionX, selectionY, BOARD_ROBOT);
                    robot.posX = selectionX;
                    robot.posY = selectionY;
                    break;
                }
            }

            while (robot.energy > 0)
            {
                robot.Run(b);
                //PrintBoard(b);
            }
        }

        sort(robots, robots + NUM_ROBOTS, [](Robot const& lhs, Robot const& rhs) { return lhs.totalEnergyCollected > rhs.totalEnergyCollected; });

        cout << "Total energy collected in generation " << g + 1 << ": " << flush;
        int sum = 0;
        for (int i = 0; i < NUM_ROBOTS; i++)
        {
            sum += robots[i].totalEnergyCollected;
        }
        cout << sum << endl;

        cout << "Average fitness of generation " << g + 1 << ": " << (sum / NUM_ROBOTS) << endl;

        for (int i = 0; i < NUM_ROBOTS / 2; i++)
        {
            Robot parent1 = robots[i];
            Robot parent2 = robots[i + 1];
            Robot child1 = Robot();
            Robot child2 = Robot();

            Robot::BreedRobots(parent1, parent2, child1, child2);

            robots[i + (NUM_ROBOTS / 2)] = child1;
            robots[i + (NUM_ROBOTS / 2) + 1] = child2;
        }
    }

    cout << "=================================================" << endl;
    cout << "As you can see, the robots' average fitness starts out at about 10-15, and increases over time until it plateaus. It usually plateaus at around 20, but if you're lucky, it may get up to even 40! Evolution in action!" << endl;
    cout << "=================================================" << endl;
    cout << "Press any key and enter to exit." << endl;
    cin >> s;

    return 0;
}

// Program Greeting
void ProgramGreeting()
{
    tm bt{};
    time_t currentTimeRaw = time(0);
    localtime_s(&bt, &currentTimeRaw);

    cout << "=======================================================================" << endl
        << "This is a program to simulate the evolution of a collection of robots." << endl
        << "Program created by Benjamin Small" << endl
        << "Due date: 12/16/2021" << endl
        << "Current date: " << put_time(&bt, "%x") << endl
        << "=========================================================" << endl;
}