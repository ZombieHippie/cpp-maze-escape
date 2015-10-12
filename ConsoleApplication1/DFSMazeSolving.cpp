#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <cstring>
#include <math.h>       /* pow */


// This line allows commonly-used functions to be used without specifying the 
// library in which that function is defined. For instance, this line allows
// the use of "cout" rather than the full specification "cout"
using namespace std;


//--------------------------------------------------------------------------
class MazeLocation
{

public:

	// Constructor. Any setup operation you wish for the class.
	MazeLocation()
	{
		x = 0; y = 0;
	} // end constructor
	MazeLocation(int a, int b, int id, bool isbarrier) : x(a), y(b), MazeLocationID(id), barrier(isbarrier)
	{} // end constructor

	int getX() const { return x; }
	int getY() const { return y; }
	int getMazeLocationID() const { return MazeLocationID; }
	bool isBarrier() const { return barrier; }
	int getVisited() const { return visited; }
	void visit() { visited++; }
	// get mark count between paths
	int getMarks(MazeLocation * neighbor) const {
		int dir = getDirection(neighbor);
		if (dir != -1)
			return marks[dir];
		else
			// indicator that wall
			return -1;
	}
	// add marks between paths
	int markPath(MazeLocation * neighbor) {
		int direction = getDirection(neighbor);
		// add mark from to
		marks[direction] ++;
		// add mark on to from
		getNeighbor(direction)->marks[(direction + 2) % 4]++;
		
		return marks[direction];
	}

	// Possibly poor use of public access for pointers to neighbors
	MazeLocation *left = NULL, *right = NULL, *up = NULL, *down = NULL;
	MazeLocation * getNeighbor(int c) const {
		switch (c % 4) {
		case 0: return up;
		case 1: return right;
		case 2: return down;
		case 3: return left;
		}
	}
	int getDirection(MazeLocation * loc) const {
		if (loc == up)
			return 0;
		else if (loc == right)
			return 1;
		else if (loc == down)
			return 2;
		else if (loc == left)
			return 3;
		else
			return -1;
	}
	MazeLocation * traceBackFrom(MazeLocation * from) const {
		int fromDir = getDirection(from);
		for (int dir = 0; dir < 4; dir++) {
			if (dir != fromDir && marks[dir] == 1)
				return getNeighbor(dir);
		}
		return NULL;
	}

private:
	int x = 0;
	int y = 0;
	int visited = 0;
	int marks[4];
	bool barrier;
	int MazeLocationID = 0;

}; // end class MazeLocation


int getMazeLocationID(int col, int row, int MAZE_WIDTH) {
	return (row*MAZE_WIDTH) + col;
};

// Pass arguments or parameters from command-line execution. argc is the count of
// those parameters, including the executable filename. argv[] is an array of the 
// parameters.
int main(int argc, char *argv[])
{
	string token;
	int xValue, yValue;
	ifstream fin;
	int coordPairs;  // number of coordinate pairs in the file
	int ptX, ptY;
	int MAZE_WIDTH = 6;
	int MAZE_HEIGHT = 6;
	set<int> barrierIDs;
	MazeLocation *mazeStartPosition = NULL; // overall start and goal
	MazeLocation *mazeGoalPosition = NULL;
	MazeLocation *previousLocation = NULL;  // The previous maze location, for use in pointer setup
	MazeLocation *currentLocation = mazeStartPosition;



	// Check the number of arguments. Expected: filename of a file
	if (argc != 2)  // This check is often hardcoded
	{   // If failure in parameters, offer advice for correction
		cout << "\nThis program uses command-line argument.\n";
		cout << "Usage: a.exe <filename>\n";
		exit(0);
	}



	try  // All lines within this block are part of the same exception handler
	{
		fin.open(argv[1]);
	}
	catch (exception& ex)
	{
		cout << ex.what();  // display standard explanation of the exception
		exit(0);  // exit the program
	}

	fin >> MAZE_WIDTH;
	fin >> MAZE_HEIGHT;
	
	fin >> coordPairs;
	while (fin >> ptX)
	{
		// Do something with the element read from the file
		fin >> ptY;
		barrierIDs.insert(getMazeLocationID(ptX, ptY, MAZE_WIDTH));
		//Point dummyPoint(ptX, ptY, pointCounter++);
		//myPointvector.push_back(dummyPoint);  // vector will expand as needed

		//cout << "Now myPointvector has size " << myPointvector.size() << endl;

	} // end while

	fin.close();


	// Create a linked set of MazeLocation objects that define the maze and barriers.
	// First, create the column at the left. This column is different from other columns
	// because it does not have a column to its left which should be double-linked.
	for (int row = 0; row < MAZE_HEIGHT; row++)
	{
		// The newest MazeLocation to be added to the maze.
		// newMazeLoc is a pointer, and "new" is used to allocate an object to be pointed to.
		int mazeLocId = getMazeLocationID(0, row, MAZE_WIDTH);
		bool locIsBarrier = barrierIDs.find(mazeLocId) != barrierIDs.end();
		MazeLocation *newMazeLoc = new MazeLocation(0, row, mazeLocId, locIsBarrier);

		//cout << "Created object, type A.   ID " << newMazeLoc->getMazeLocationID() << endl;

		if (row == 0)  // Generating the location of the hardcoded start state
		{
			// mazeStartPosition and newMazeLoc are both pointers, so assign one pointer to another
			mazeStartPosition = newMazeLoc;   // pointer to maze start state
			previousLocation = newMazeLoc; // pointer to the previous maze location, for pointer setup
		}

		// Create vertical linkage of the first column of all rows
		if (row > 0) // This is at least the second row
		{
			previousLocation->down = newMazeLoc;  // double-linkage of the previous and new maze location
			newMazeLoc->up = previousLocation;
			previousLocation = newMazeLoc; // update pointer to the previous maze location, for next iteration
		}
	}

	previousLocation = mazeStartPosition; // reset to the first location in the present row


	// Next, create the remainder of each row from its left column to its right column. 
	// Each row itself is horizontally double-linked. For the second row to the end row, each
	// location in a row will be vertically double-linked to the corresponding location in the row above.
	for (int row = 0; row < MAZE_HEIGHT; row++)
	{
		for (int col = 1; col < MAZE_WIDTH; col++) // Begin creation at the second column (the first col is already created)
		{
			// The newest MazeLocation to be added to the maze.
			// newMazeLoc is a pointer, and "new" is used to allocate an object to be pointed to.
			int mazeLocId = getMazeLocationID(col, row, MAZE_WIDTH);
			bool locIsBarrier = barrierIDs.find(mazeLocId) != barrierIDs.end();
			MazeLocation *newMazeLoc = new MazeLocation(col, row, mazeLocId, locIsBarrier);

			//cout << "Created object, type B.   ID " << newMazeLoc->getMazeLocationID() << endl;

			if (row == MAZE_HEIGHT - 1 && col == MAZE_WIDTH - 1)  // Generating the location of the hardcoded goal state
			{
				// mazeGoalPosition and newMazeLoc are both pointers, so assign one pointer to another
				mazeGoalPosition = newMazeLoc;   // pointer to maze goal state
			}

			// Create horizontal linkage of maze locations for all columns
			previousLocation->right = newMazeLoc;  // double-linkage of the previous and new maze location
			newMazeLoc->left = previousLocation;

			// For the second and subsequent rows, create vertical double links to row above
			if (row > 0) // This is at least the second row
			{
				// The vertical pointer location is several links away from the newMazeLoc.
				// The tempLocation is "above" this new location
				MazeLocation *tempLocation = (previousLocation->up)->right;
				//cout << "               tempLocation ID is " << tempLocation->getMazeLocationID() << endl;
				tempLocation->down = newMazeLoc;  // double-linkage of the previous and new maze location
				newMazeLoc->up = tempLocation;
			}



			// After each new creation, update the "previous" pointer
			previousLocation = newMazeLoc;
		}

		// After the end location in this row, reset the pointer to the first location in the next row
		while (previousLocation->left != NULL)
		{
			previousLocation = previousLocation->left; // back up to initial column of the present row...
		}
		previousLocation = previousLocation->down; // ...and down to the initial column of the next row.
	}
	

	// debug
	// draw top wall
	for (int coltw = -2; coltw < MAZE_WIDTH; coltw++) { cout << '#'; }
	cout << endl;

	// draw cells
	for (int rowi = 0; rowi < MAZE_HEIGHT; rowi++) {
		MazeLocation * mzloc = mazeStartPosition;
		// move down to row
		for (int rowto = 0; rowto < rowi; rowto++) {
			mzloc = mzloc->down;
		}
		cout << '#';
		for (int coli = 0; coli < MAZE_WIDTH; coli++) {
			cout << (mzloc->isBarrier() ? 'X' : ' ');
			mzloc = mzloc->right;
		}
		cout << '#';
		cout << endl;
	}
	// draw bottom wall
	for (int coltw = -2; coltw < MAZE_WIDTH; coltw++) { cout << '#'; }
	cout << endl;


	// Insert solution to algorithm here
	// Using Trémaux's algorithm
	MazeLocation * mzloc = mazeStartPosition;
	MazeLocation * prevLoc = NULL;

	//mzloc->visit();
	bool solved = true;
	while (mzloc != mazeGoalPosition) {
		MazeLocation * potentialLoc = prevLoc;
		int potentialLocMarks = (prevLoc == NULL ? 2 : mzloc->getMarks(prevLoc));
		// choose direction
		for (int dir = 0; dir < 4; dir++) {
			MazeLocation * testLoc = mzloc->getNeighbor(dir);

			// trying to go out of bounds
			if (testLoc == NULL)
				continue;

			// trying to go into barrier
			if (testLoc->isBarrier())
				continue;

			// acknowledge the amount of marks from to
			if (mzloc->getMarks(testLoc) <= potentialLocMarks) {
				potentialLoc = testLoc;
				potentialLocMarks = mzloc->getMarks(testLoc);
			}
		}
		if (potentialLoc == NULL) {
			cout << "No solution." << endl;
			solved = false;
			break;
		}
		else {
			// When arriving at a marked junction and if your current path is marked only once then turn around and walk back
			if (mzloc->getMarks(potentialLoc) > 0 && mzloc->getMarks(prevLoc) == 1) {
				potentialLoc = prevLoc;
			}
			prevLoc = mzloc;
			mzloc->markPath(potentialLoc);
			mzloc = potentialLoc;
		}
	}


	// debug
	// draw top wall
	for (int coltw = -2; coltw < MAZE_WIDTH; coltw++) { cout << "|X-X-X-X"; }
	cout << '|' << endl;

	// draw cells
	for (int rowi = 0; rowi < MAZE_HEIGHT; rowi++) {
		MazeLocation * mzloc = mazeStartPosition;
		// move down to row
		for (int rowto = 0; rowto < rowi; rowto++) {
			mzloc = mzloc->down;
		}
		cout << "|X-X-X-X";
		for (int coli = 0; coli < MAZE_WIDTH; coli++) {
			if (mzloc->isBarrier()) {
				cout << "|X-X-X-X";
			} else {
				cout << '|';
				MazeLocation * neighbor = NULL;
				for (int dir = 0; dir < 4; dir++) {
					if (dir > 0)
						cout << '-';
					neighbor = mzloc->getNeighbor(dir);
					if (neighbor != NULL && !neighbor->isBarrier())
						cout << mzloc->getMarks(neighbor);
					else
						cout << ' ';
				}
			}
			mzloc = mzloc->right;
		}
		cout << "|X-X-X-X";
		cout << '|' << endl;
	}
	// draw bottom wall
	for (int coltw = -2; coltw < MAZE_WIDTH; coltw++) { cout << "|X-X-X-X"; }
	cout << '|' << endl;

	// trace steps
	if (solved) {
		int steps = -1;
		MazeLocation * prev = NULL;
		MazeLocation * tracer = mazeStartPosition;
		while (tracer != NULL) {
			steps++;
			cout << '(' << tracer->getX() << ',' << tracer->getY() << ')';
			MazeLocation * newT = tracer->traceBackFrom(prev);
			prev = tracer;
			tracer = newT;
			if (tracer != NULL)
				cout << ", ";
		}
		cout << " with length " << steps << '.' << endl;
	}
	

	int pause;
	cin >> pause;
	return pause;


} // end main
