#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}

	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}

	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}

		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}

	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{
	clearLine(MAXROWS + 1, 100);
	clearLine(MAXROWS + 2, 100);

	char option;
	char input;

	Point user;
	Point center(10, 40);
	DrawPoint bottom(21, 40);

	do
	{
		displayCanvas(current->item);// Displays the canvas
		char menu[20] = "<A>nimate: N";
		if (animate == true)
		{
			menu[11] = 'Y';
		}
		else
		{
			menu[11] = 'N';
		}
		cout << menu << " / <U>ndo: " << undoList.count;
		if (redoList.count > 0)
		{
			cout << " / Red<O>: " << redoList.count;
		}
		cout << " / Cl<I>p: " << clips.count;
		if (clips.count > 1)
		{
			cout << " / <P>lay";
		}
		cout << "\n";
		cout << menu;
		cout << "<F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: ";
		cin >> option;
		option = toupper(option);
		clearLine(MAXROWS + 1, 100);
		clearLine(MAXROWS + 2, 100);

		gotoxy(MAXROWS + 1, 0);

		//Animate Function 
		if (option == 'A')
		{
			if (menu[11] == 'N')
			{
				//Animate on
				menu[11] = 'Y';
				animate = true;
			}
			else
			{
				//Animate off
				menu[11] = 'N';
				animate = false;
			}
		}

		else if (option == 'U')
		{
			restore(undoList, redoList, current);

			cout << endl;
		}
		else if (redoList.count > 0 && option == 'O')
		{
			restore(redoList, undoList, current);
		}
		else if (option == 'I')
		{
			Node* clip = newCanvas(current);
			addNode(clips, clip);
		}
		else if (clips.count > 1 && option == 'P')
		{
			play(clips);
		}
		//Fill the canvas with user choice of character 
		else if (option == 'F')
		{
			addUndoState(undoList, redoList, current);

			int row;
			int col;

			cout << "Enter character to fill with from current location / <ESC> to cancel" << endl;

			input = getPoint(user);

			if (input != ESC)
			{
				row = user.row;
				col = user.col;
				char oldCh = current->item[row][col];
				char newCh = input;
				fillRecursive(current->item, row, col, oldCh, newCh, animate);
			}
		}

		//Load canvas with file
		else if (option == 'L')
		{

			Point start;
			Point end;

			cout << "Type any letter to choose the start point / <ESC> to cancel";
			input = getPoint(start);
			char change = current->item[start.row][start.col];
			current->item[start.row][start.col] = input;
			cout << current->item[start.row][start.col];

			if (input != ESC)
			{
				clearLine(MAXROWS + 1, 100);
				cout << "Type any letter to choose the end point / <ESC> to cancel";
				input = getPoint(end);

				if (input != ESC)
				{
					addUndoState(undoList, redoList, current);
					drawLine(current->item, start, end, animate);
				}
			}
			if (input == ESC)
			{
				current->item[start.row][start.col] = change;
				cout << current->item[start.row][start.col];
			}
		}

		//Allow the user to draw a box on the canvas 
		else if (option == 'B')
		{

			int height;

			cout << "Enter box height: ";
			cin >> height;
			clearLine(MAXROWS + 1, 100);

			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";
			input = getPoint(user);

			if (input == 'C' || input == 'c')
			{
				addUndoState(undoList, redoList, current);
				drawBox(current->item, center, height, animate);
			}

			else if (input != ESC)
			{
				addUndoState(undoList, redoList, current);
				drawBox(current->item, user, height, animate);
			}
		}

		// Allows the user to draw a series of boxes on the canvas 
		else if (option == 'N')
		{
			
			int height;

			//draws nested boxes
			cout << "Enter box height: ";
			cin >> height;
			clearLine(MAXROWS + 1, 100);

			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";
			input = getPoint(user);

			if (input == 'C' || input == 'c')
			{
				addUndoState(undoList, redoList, current);
				drawBoxesRecursive(current->item, center, height, animate);
			}

			else if (input != ESC)
			{
				addUndoState(undoList, redoList, current);
				drawBoxesRecursive(current->item, user, height, animate);
			}
		}

		// This will allow the user to draw a tree on the canvas 
		else if (option == 'T')
		{

			int length;
			int angle;

			//draws tree
			cout << "Enter tree height: ";
			cin >> length;
			clearLine(MAXROWS + 1, 100);

			cout << "Enter angle: ";
			cin >> angle;
			clearLine(MAXROWS + 1, 100);

			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";
			input = getPoint(user);

			if (input == 'C' || input == 'c')
			{
				addUndoState(undoList, redoList, current);
				treeRecursive(current->item, bottom, length, 270, angle, animate);
			}

			else if (input != ESC)
			{
				addUndoState(undoList, redoList, current);
				treeRecursive(current->item, user, length, 270, angle, animate);
			}
		}
	} while (option != 'M');
}


// Get a single point from screen, with character entered at that point	//Shweta
char getPoint(Point& pt)
{
	char input;
	int row = 0, col = 0;

	// Move cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);
	input = _getch();

	// Exits the editing canvas
	if (input == ESC)
	{
		return input;
	}

	//Moves the cursor
	do
	{
		if (input == SPECIAL)
		{
			input = _getch();

			//Moves the cursor to the left
			if (input == LEFTARROW)
			{
				if (col > 0)
				{
					gotoxy(row, col - 1);
					col--;
				}

				else
				{
					gotoxy(row, col);
				}
			}

			// Moves the cursor to the right 
			else if (input == RIGHTARROW)
			{
				if (col < MAXCOLS - 1)
				{
					gotoxy(row, col + 1);
					col++;
				}

				else
				{
					gotoxy(row, col);
				}
			}
			
			//Moves the cursor down
			else if (input == DOWNARROW)
			{
				if (row < MAXROWS - 1)
				{
					gotoxy(row + 1, col);
					row++;
				}

				else
				{
					gotoxy(row, col);
				}
			}

			// Moves the cursor up
			else if (input == UPARROW)
			{
				if (row > 0)
				{
					gotoxy(row - 1, col);
					row--;
				}

				else
				{
					gotoxy(row, col);
				}
			}
			input = _getch();
		}

		else if (input == '\0')
		{
			input = _getch();
			input = _getch();
		}

		else if (input >= 32 && input <= 126)
		{

			gotoxy(row, col);
			pt.row = row;
			pt.col = col;
			return input;
			input = _getch();
		}

		else
		{
			gotoxy(row, col);
			input = _getch();
		}

	}

	while (input != ESC || (input >= 32 && input <= 126));
}


// Recursively fill a section of the screen	
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	Point fill(row, col);

	//Base Case
	if (canvas[row][col] == oldCh)
	{
		drawHelper(canvas, fill, newCh, animate);
	}

	//Recursive Case
	if (row < MAXROWS - 1 && canvas[row + 1][col] == oldCh)
	{
		fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
	}

	if (row > 1 && canvas[row - 1][col] == oldCh)
	{
		fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
	}

	if (col < MAXCOLS - 1 && canvas[row][col + 1] == oldCh)
	{
		fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
	}

	if (col > 1 && canvas[row][col - 1] == oldCh)
	{
		fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
	}
}


// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	DrawPoint endPoint = findEndPoint(start, height / 3, startAngle);
	if (endPoint.row >= 0 && endPoint.row <= MAXROWS - 1 && endPoint.col >= 0 && endPoint.col <= MAXCOLS - 1)
	{
		drawLine(canvas, start, endPoint, animate);
	}

	//Base Case
	if (endPoint.row <= 0 || endPoint.row >= MAXROWS - 1 || endPoint.col <= 0 || endPoint.col >= MAXCOLS - 1
		|| start.row <= 0 || start.row >= MAXROWS || start.col <= 0 || start.col >= MAXCOLS || height <= 3)
	{
		return;
	}

	//Recursive Case
	treeRecursive(canvas, endPoint, height - 2, startAngle + branchAngle, branchAngle, animate);
	treeRecursive(canvas, endPoint, height - 2, startAngle - branchAngle, branchAngle, animate);
}


// Recursively draw nested boxes 
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	//Base Case
	if (height == 2 || height == 3)
	{
		drawBox(canvas, center, height, animate);
	}

	//Recursive Case
	else if (height > 2)
	{
		drawBox(canvas, center, height, animate);
		drawBoxesRecursive(canvas, center, height - 2, animate);
	}
}