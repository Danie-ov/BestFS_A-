// ----------------SUBMISSION NAMES ---------------------------
//submitted by: Shay Ezer 208445882, Daniel Niazov 207437997
//-----------------------------------------------------------------------

#include <time.h>
#include <vector>
#include <iostream>
#include "Cell.h"
#include <queue>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "freeglut_std.h"

using namespace std;

const int MSZ = 100; // maze size

const int WALL = 0;
const int SPACE = 1;
const int START = 2;
const int TARGET = 3;
const int BLACK = 4;
const int PATH = 6;
const int W = 700;
const int H = 700;

int maze[MSZ][MSZ] = { 0 };
int targetCoords[2];

vector<Cell*> grays;
bool startBestFS = false, startAstar = false, isStart = true;

vector<Cell*> cells;
vector<Cell*> visited;
vector<Cell*> notVisited;
priority_queue<Cell, vector<Cell>, CompareBestFSCells>  BestFSPq;
priority_queue<Cell, vector<Cell>, CompareAstarCells>  aStarPq;

vector <Cell*> graysStart;
vector <Cell*> graysTarget;


void InitMaze();
double manhattanDistance(int x1, int y1);
double calcDistance(int x1, int y1);

void init()
{
	glClearColor(1, 1, 1, 0);// color of window background

	// set the main axes
	glOrtho(-1, 1, -1, 1, 0, 1);

	srand(time(0));

	InitMaze();
}

void InitMaze()
{
	int i, j;

	for (i = 1; i < MSZ - 1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 0) // mostly walls
			{
				if (rand() % 100 > 35)
					maze[i][j] = WALL;
				else maze[i][j] = SPACE;
			}
			else // mostly spaces
			{
				if (rand() % 100 > 20)
					maze[i][j] = SPACE;
				else maze[i][j] = WALL;
			}
		}

	maze[MSZ / 2][MSZ / 2] = START;
	targetCoords[0] = rand() % MSZ;
	targetCoords[1] = rand() % MSZ;

	maze[targetCoords[0]][targetCoords[1]] = TARGET;

	Cell* startCell = new Cell(MSZ / 2, MSZ / 2, 0, 0, nullptr); 
	Cell* targetCell = new Cell(targetCoords[0], targetCoords[1], 0, nullptr);

	BestFSPq.push(*startCell);
	aStarPq.push(*startCell);
	notVisited.push_back(startCell);
}

void ShowMaze()
{
	int i, j;
	double sx = W / MSZ , sy = H / MSZ;
	double x, y;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0.5, 0, 0); // dark red
				break;
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case START:
				glColor3d(0.5, 0.5, 1); // light blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case BLACK:
				glColor3d(1, 1, 0); // yellow
				break;
			case PATH:
				glColor3d(0, 0.7, 0); // green
				break;
			}// switch

			x = 2.0 * (j * sx) / W - 1;
			y = 2.0 * (i * sy) / H - 1;

			glBegin(GL_POLYGON);
			glVertex2d(x, y);
			glVertex2d(x, y + (2.0 * sy) / H);
			glVertex2d(x + 2.0 * sx / W, y + (2.0 * sy) / H);
			glVertex2d(x + 2.0 * sx / W, y);
			glEnd();

		}
}

void RestorePath(Cell* ps)
{
	while (maze[ps->getXPos()][ps->getYPos()] != START)
	{
		maze[ps->getXPos()][ps->getYPos()] = PATH;
		ps = ps->getParent();
	}
}

void CheckNeighborBestFS(Cell* currentCell, int xPos, int yPos)
{
	if (maze[xPos][yPos] == SPACE)
	{
		Cell* newCell = new Cell(xPos, yPos, calcDistance(xPos, yPos), currentCell);
		BestFSPq.push(*newCell);
		maze[xPos][yPos] = BLACK;
	}
	else if (maze[xPos][yPos] == TARGET)
	{
		cout << "Solution Found!\n";
		startBestFS = false;
		RestorePath(currentCell);
	}
}

void CheckNeighborAstar(Cell* currentCell, int xPos, int yPos)
{
	vector<Cell*>::iterator visit;
	vector<Cell*>::iterator notVisit;
	if (maze[xPos][yPos] == SPACE)
	{
		Cell* c = new Cell(xPos, yPos, manhattanDistance(xPos, yPos), currentCell->getG() + 1, currentCell);
		c->setF(c->getG() + c->getH());
		visit = find(visited.begin(), visited.end(), c);
		notVisit = find(notVisited.begin(), notVisited.end(), c);
		if (visit == visited.end() && notVisit == notVisited.end())
		{
			aStarPq.push(*c);
			notVisited.push_back(c);
			maze[xPos][yPos] = BLACK;
		}
	}
	else if (maze[xPos][yPos] == TARGET)
	{
		cout << "Solution Found!\n";
		startAstar = false;
		RestorePath(currentCell);
	}
}

// gets the pointer to the current Cell and the coordinates of its neighbor
void checkNeighborsOfBestFS(Cell* cell)
{
	CheckNeighborBestFS(cell, cell->getXPos() + 1, cell->getYPos()); // Down
	if (startBestFS)
		CheckNeighborBestFS(cell, cell->getXPos() - 1, cell->getYPos()); // Up
	if (startBestFS)
		CheckNeighborBestFS(cell, cell->getXPos(), cell->getYPos() + 1); // Right
	if (startBestFS)
		CheckNeighborBestFS(cell, cell->getXPos(), cell->getYPos() - 1); // Left
}

void checkNeighborsOfAStar(Cell* cell)
{
	CheckNeighborAstar(cell, cell->getXPos() + 1, cell->getYPos()); // Down
	if (startAstar)
		CheckNeighborAstar(cell, cell->getXPos() - 1, cell->getYPos()); // Up
	if (startAstar)
		CheckNeighborAstar(cell, cell->getXPos(), cell->getYPos() + 1); // Right
	if (startAstar)
		CheckNeighborAstar(cell, cell->getXPos(), cell->getYPos() - 1); // Left
}

void BestFSIteration()
{
	Cell* pCurrent = new Cell(); // Current cell.
	if (BestFSPq.empty())
	{
		cout << "There is no solution!\n";
		startBestFS = false;
		return;
	}
	else
	{
		*pCurrent = BestFSPq.top();
		BestFSPq.pop();
		if (isStart)
		{
			pCurrent->setH(calcDistance(pCurrent->getXPos(), pCurrent->getYPos()));
			isStart = false;
		}
		if (maze[pCurrent->getXPos()][pCurrent->getYPos()] == TARGET)
		{
			startBestFS = false;
			cout << "The solution has been found\n";
			RestorePath(pCurrent);
			return;
		}
		else
		{
			if (maze[pCurrent->getXPos()][pCurrent->getYPos()] != START)
				maze[pCurrent->getXPos()][pCurrent->getYPos()] = BLACK;
			checkNeighborsOfBestFS(pCurrent);
		}
	}
}

void AStarIteration()
{
	vector<Cell*>::iterator notVisit;
	Cell* pCurrent = new Cell(); // Current cell.

	if (aStarPq.empty())
	{
		cout << "There is no solution!\n";
		startAstar = false;
		return;
	}
	else
	{
		*pCurrent = aStarPq.top();
		aStarPq.pop();
		if (isStart)
		{
			pCurrent->setH(manhattanDistance(pCurrent->getXPos(), pCurrent->getYPos()));
			pCurrent->setF(0);
			isStart = false;
		}
		if (maze[pCurrent->getXPos()][pCurrent->getYPos()] == TARGET)
		{
			startAstar = false;
			cout << "The solution has been found\n";
			RestorePath(pCurrent);
			return;
		}
		else
		{
			visited.push_back(pCurrent);
			notVisit = find(notVisited.begin(), notVisited.end(), pCurrent);
			if (notVisit != notVisited.end())
			{
				notVisited.erase(notVisit);
			}
			if (maze[pCurrent->getXPos()][pCurrent->getYPos()] != START)
				maze[pCurrent->getXPos()][pCurrent->getYPos()] = BLACK;
			checkNeighborsOfAStar(pCurrent);
		}
	}
}

double calcDistance(int x1, int y1)
{
	return sqrt((x1 - targetCoords[0]) * (x1 - targetCoords[0]) + (y1 - targetCoords[1]) * (y1 - targetCoords[1]));
}

double manhattanDistance(int x1, int y1)
{
	return (abs(x1 - targetCoords[0]) + abs(y1 - targetCoords[1]));
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	ShowMaze();

	glutSwapBuffers();
}

void idle()
{

	if (startBestFS)
		BestFSIteration();
	else if (startAstar)
		AStarIteration();

	glutPostRedisplay();
}

void menu(int choice)
{
	switch (choice)
	{
	case 1: // BestFS
		startBestFS = true;
		break;
	case 2: // A*
		startAstar = true;
		break;
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 100);
	glutCreateWindow(" HW2 ");

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Run BestFS", 1);
	glutAddMenuEntry("Run A star", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();
	glutMainLoop();
	return EXIT_SUCCESS;
}
