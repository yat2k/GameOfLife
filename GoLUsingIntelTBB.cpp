
#include "GridWindow.h"
#include <iostream>
#include <tbb\tbb.h>
#include <random>
#include <chrono>

using namespace std;
using namespace tbb;
using namespace chrono;


void initialise(Grid& gridArray);
void update(Grid& inputArray, Grid& outputArray);
int calculateNeighbours(Grid& inputArray, int rowIndex, int colIndex);
void updateInputArray(Grid &outputArray, Grid &inputArray);
void serial_populate(Grid& gridArray);
void parallel_populate(Grid& gridArray);
void serial_nextGeneration(Grid& inputArray, Grid& outputArray);
void parallel_nextGeneration(Grid& inputArray, Grid& outputArray);

int numberOfGenerations = 0;
long long totalTime = 0;

// Main start point for the programme.  This sets up a 2D array (Grid) and creates the window to display the grid.
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	// *** Create a 2D array (Grid) and call the initialise function to set it's elements
	// Example array size (n x n)
	const int n = 10;
	

	Grid myArray = Grid(n);
	initialise(myArray);


	// Create and show a window that draws the 2D array.  This example creates a window (400 x 400) pixels in size.
	GridWindow mainWindow = GridWindow(400, 400, myArray, hInstance, iCmdShow);
	
	

	// Run the main event loop.  This calls the update function defined below to modify the 2D array elements
	mainWindow.mainLoop(update);


	return 0;
}


// Function called to initialise the 2D grid array.  This uses TBB's parallel_for to setup an example checker-board pattern.
// *** ADD YOUR OWN INITIALISATION CODE TO SET THE APPROPRIATE SQUARES FOR YOUR "GAME OF LIFE" IMPLEMENTATION ***
void initialise(Grid& gridArray)
{
	
	//serial_populate(gridArray); //serial version
	parallel_populate(gridArray); //parallel version
}


// Update function - this is called automatically from the window's event loop.  This takes an input array (inputArray) and returns the result of any processing in 'outputArray'.  Here you can set the output array values.

void update(Grid& inputArray, Grid& outputArray)
{
	
	auto start = high_resolution_clock::now();
	
	if (numberOfGenerations == 1000)
	{
		
		char time_buffer[100];
		sprintf_s(time_buffer, "%I64d microseconds \n", totalTime); //convert message into an outputtable string.

		OutputDebugStringA(time_buffer);
		numberOfGenerations = 0;
		totalTime = 0;
	}
	
	//serial_nextGeneration(inputArray, outputArray); //serial version
	parallel_nextGeneration(inputArray, outputArray); //parallel version

	auto end = high_resolution_clock::now(); //end the timer
	auto duration = duration_cast<microseconds>(end - start);
	totalTime = totalTime + duration.count();
}

int calculateNeighbours(Grid& inputArray, int rowIndex, int colIndex)
{
	int neighbours = 0;
	int n = inputArray.getNumElements();

	for (int i = -1; i <= 1; i++) //count neighbours in each row
	{
		for (int j = -1; j <= 1; j++) //count neighbours in each columns
		{
			neighbours += inputArray[rowIndex + i][colIndex + j];
		}
	}


	return neighbours;
}

void updateInputArray(Grid &outputArray, Grid &inputArray)
{
	for (int i = 0; i < inputArray.getNumElements(); i++)
	{
		for (int j = 0; j < inputArray.getNumElements(); j++)
		{
			inputArray[i][j] = outputArray[i][j];
		}
	}
}

void parallel_nextGeneration(Grid &inputArray, Grid &outputArray)
{
	//Parallel Version - implements the Game of Life rules

	int n = inputArray.getNumElements(); //get number of values in inputArray
	parallel_for(

	blocked_range2d<int, int>(1, n-1, 1, n-1),

	[&](blocked_range2d<int, int>& range) 
	{

		int yStart = range.rows().begin();
		int yEnd = range.rows().end();


		for (int i = yStart; i < yEnd; i++) //rows
		{

			int xStart = range.cols().begin();
			int xEnd = range.cols().end();

			for (int j = xStart; j < xEnd; j++) //cols
			{


				//Kernel
				int neighbours = calculateNeighbours(inputArray, i, j);

				if ((inputArray[i][j] == 1) && (neighbours < 2))
				{
					outputArray[i][j] = 0;
				}
				else if ((inputArray[i][j] == 1) && (neighbours > 3))
				{
					outputArray[i][j] = 0;
				}
				else if ((inputArray[i][j] == 0) && (neighbours == 3))
				{
					outputArray[i][j] = 1;
				}
				else
				{
					outputArray[i][j] = inputArray[i][j];
				}


			}
		}
	}
	);

	inputArray = outputArray;
	numberOfGenerations++;
	
	char message[50];
	sprintf_s(message, "Generation number %d \n", numberOfGenerations); //convert message into an outputtable string.

	OutputDebugStringA(message);
	
}

void serial_nextGeneration(Grid &inputArray, Grid &outputArray)
{
	//Serial version - implements the Game of Life rules to each element - this works
	
	int n = inputArray.getNumElements(); //get number of values in inputArray (
	//Outer Loop
	for (int i = 1; i < n - 1; i++) //Rows
	{

		for (int j = 1; j < n - 1; j++) //Columns
		{

			//Kernel

			int neighbours = calculateNeighbours(inputArray, i, j);

			if ((inputArray[i][j] == 1) && (neighbours < 2))
			{
				outputArray[i][j] = 0;
			}
			else if ((inputArray[i][j] == 1) && (neighbours > 3))
			{
				outputArray[i][j] = 0;
			}
			else if ((inputArray[i][j] == 0) && (neighbours == 3))
			{
				outputArray[i][j] = 1;
			}
			else
			{
				outputArray[i][j] = inputArray[i][j];
			}


		}


	}
	inputArray = outputArray;
	
	numberOfGenerations++;
	char message[50];
	sprintf_s(message, "Generation number %d \n", numberOfGenerations); //convert message into an outputtable string.

	OutputDebugStringA(message);
	
}

void serial_populate(Grid& gridArray)
{
	const int n = gridArray.getNumElements();

	random_device rd;
	mt19937 mt(rd());
	uniform_int_distribution<int> iDist(0, 1);

	

	//Serial version - generates a random integer between 0 and 1 if the integer is 1 then it sets gridArray element to 1, if its not 1 (therefore 0) then move onto the next iteration - this works

	auto start = high_resolution_clock::now();//start the timer


	for (int i = 0; i < n; ++i) //rows
	{
		for (int j = 0; j < n; ++j) //cols
		{
			int randomNumber = iDist(mt);

			if (randomNumber == 1)
			{
				gridArray[i][j] = randomNumber;
			}
		}
	}

	auto end = high_resolution_clock::now(); //end the timer
	auto duration = duration_cast<microseconds>(end-start);
	char time_buffer[50];
	sprintf_s(time_buffer, "Serial initialisation took %I64d\n microseconds", duration.count()); //convert message into an outputtable string.

	OutputDebugStringA(time_buffer);
}

void parallel_populate(Grid& gridArray)
{
	const int n = gridArray.getNumElements();

	random_device rd;
	mt19937 mt(rd());
	uniform_int_distribution<int> iDist(0, 1);

	//Parallel version which implements the same rules as the serial version however using threads to split up the array and distribute the workload to different threads. - this works

	auto start = high_resolution_clock::now(); //start the timer

	parallel_for(

		blocked_range2d<int, int>(0, n, 0, n),

		[&](blocked_range2d<int, int>& range)
	{

		int yStart = range.rows().begin();
		int yEnd = range.rows().end();

		for (int i = yStart; i < yEnd; i++)
		{

			int xStart = range.cols().begin();
			int xEnd = range.cols().end();

			for (int j = xStart; j < xEnd; j++)
			{

				int randomNumber = iDist(mt);

				if (randomNumber == 1) //investigate if this impacts the speed of initialise
				{
					// Set array element to 1 (on) or 0 (off) to create a checker-board pattern.
					gridArray[i][j] = randomNumber;
				}
			}
		}
	}
	);




	auto end = high_resolution_clock::now(); //end the timer
	auto duration = duration_cast<microseconds>(end - start);
	char time_buffer[100];
	sprintf_s(time_buffer, "Parallel initialisation took %I64d microseconds \n", duration.count()); //convert message into an outputtable string.

	OutputDebugStringA(time_buffer);
}
