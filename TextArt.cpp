/*
* This program allows display and editing of text art (also called ASCII art).
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


int main()
{
	Node* current;
	current = newCanvas();

	List undo;
	List redo;
	List clips;

	// user's choice
	char option;
	bool choice = false;

	initCanvas(current->item);

	system("cls");

	do
	{
		//clears the screen
		gotoxy(0, 0);

		//Displays the starter screen 
		displayCanvas(current->item);

		char menu[20] = "<A>nimate: N";
		if (choice == true)
		{
			menu[11] = 'Y';
		}
		else
		{
			menu[11] = 'N';
		}
		cout << menu << " / <U>ndo: " << undo.count;
		if (redo.count > 0)
		{
			cout << " / Red<O>: " << redo.count;
		}
		cout << " / Cl<I>p: " << clips.count;
		if (clips.count > 1)
		{
			cout << " / <P>lay";
		}
		cout << "\n";
		cout << "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <U>ndo / <L>oad / <S>ave / <Q>uit: ";
		cin >> option;
		option = toupper(option);
		clearLine(MAXROWS + 1, 100);
		clearLine(MAXROWS + 2, 100);

		gotoxy(MAXROWS + 1, 0);

		// Edits the test that is on the screen
		if (option == 'A')
		{
			if (menu[11] == 'N')
			{
				//Animate on
				menu[11] = 'Y';
				choice = true;
			}
			else
			{
				//Animate off
				menu[11] = 'N';
				choice = false;
			}
		}
		else if (redo.count > 0 && option == 'O')
		{
			restore(redo, undo, current);
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
		else if (option == 'E')
		{
			cout << "Press <Esc> to stop editing " << endl;
			addUndoState(undo, redo, current);
			editCanvas(current->item);
		}

		// Moves the text by changes the row and column location
		else if (option == 'M')
		{
			addUndoState(undo, redo, current);

			int num1; // row number
			int num2; // column number

			cout << "Enter row units to move: " << endl;
			cin >> num1;
			cout << "Enter column units to move: " << endl;
			cin >> num2;

			moveCanvas(current->item, num1, num2);
		}

		//Replaces the specific character with another character
		else if (option == 'R')
		{
			addUndoState(undo, redo, current);

			char charOne[2]; // character one
			char charTwo[2]; // character two

			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			cout << "Enter character to replace: " << endl;
			cin.get(charOne, 2);

			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			cout << "Enter character to replace with: " << endl;
			cin.get(charTwo, 2);

			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			//Displays the canavas with the replaced character
			replace(current->item, charOne[0], charTwo[0]);
		}

		//Replaces the current screen with the pervious screen
		else if (option == 'U')
		{
			restore(undo, redo, current);

			cout << endl;
		}

		//Load the canvas from the file and display it to the screen
		else if (option == 'L')
		{
			addUndoState(undo, redo, current);

			char name[239];
			char nameTwo[FILENAMESIZE] = "SavedFiles\\";
			char fullName[FILENAMESIZE] = "SavedFiles\\";
			int usedSize = 0;
			char options;

			cout << "<C>anvas or <A>nimation?: ";

			//ignores earlier input
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			cin >> options;

			clearLine(MAXROWS + 1, 100);

			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			// get input from user 
			cout << "Please enter the file name: ";
			cin.getline(name, FILENAMESIZE - 16);

			//if cin fails it ignores the input
			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			//concatenates the full file name together
			strncat_s(fullName, FILENAMESIZE, name, FILENAMESIZE - 16);
			strncat_s(nameTwo, FILENAMESIZE, name, FILENAMESIZE - 16);
			strncat_s(fullName, FILENAMESIZE, ".txt", 4);
			

			options = toupper(options);

			if (options == 'C')
			{
				if (loadCanvas(current->item, fullName) == false)
				{
					cout << "Error! Cannot open file.";
					cout << endl;
					system("pause");
				}
			}
			else if (options == 'A')
			{
				loadClips(clips, nameTwo);
			}
		}

		//Save the canvas in the file spot
		else if (option == 'S')
		{
			cout << "Enter the filename (do not enter .txt file):" << endl;
			saveCanvas(current->item); // save the canvas 
		}
		else if (option == 'Q')
		{
			exit(0);
		}
		else if (option == 'D')
		{
			menuTwo(current, undo, redo, clips, choice);
		}
		else if (option == 'C')
		{
			addUndoState(undo, redo, current);

			initCanvas(current->item);
		}
		// Exits the program
	} while (option != 'Q');

	delete current->next;
	delete current;

	deleteList(undo);
	deleteList(redo);
	deleteList(clips);

	return 0;
}


void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}


void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	// Replaces the old character
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			if (canvas[x][y] == oldCh)
			{
				//new character replacement
				canvas[x][y] = newCh;
			}
		}
	}
}


void editCanvas(char canvas[][MAXCOLS])
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
		return;
	}

	//Moves the cursor
	do
	{
		if (input == SPECIAL)
		{
			input = _getch();
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
			canvas[row][col] = input;
			cout << canvas[row][col];

			gotoxy(row, col);
			input = _getch();
		}

		else
		{
			gotoxy(row, col);
			input = _getch();
		}

	}

	while (input != ESC);

}


void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	//copy char from canvas to temp array
	//can use copy canvas to do that
	//copy from temp array back to canvas but move them in new pos
	//pay attention to end of the array 

	char temp[MAXROWS][MAXCOLS];
	copyCanvas(temp, canvas);
	if (rowValue > 0)
	{
		for (int i = 0; i < MAXROWS; i++)
		{
			for (int y = 0; y < MAXCOLS; y++)
			{
				if (i < rowValue)
				{
					canvas[i][y] = ' ';
				}
				else
				{
					canvas[i][y] = temp[i - rowValue][y];
				}
			}
		}
	}

	else if (rowValue < 0)
	{
		for (int i = 0; i < MAXROWS; i++)
		{
			for (int y = 0; y < MAXCOLS; y++)
			{
				int up = MAXROWS + rowValue;
				if (i < up)
				{
					canvas[i][y] = temp[i - rowValue][y];
				}
				else
				{
					canvas[i][y] = ' ';
				}
			}
		}
	}

	copyCanvas(temp, canvas);

	if (colValue > 0)
	{
		for (int i = 0; i < MAXROWS; i++)
		{
			for (int y = 0; y < MAXCOLS; y++)
			{
				if (y < colValue)
				{
					canvas[i][y] = ' ';
				}
				else
				{
					canvas[i][y] = temp[i][y - colValue];
				}
			}
		}
	}

	else if (colValue < 0)
	{
		for (int i = 0; i < MAXROWS; i++)
		{
			for (int y = 0; y < MAXCOLS; y++)
			{
				int left = MAXCOLS + colValue;
				if (y < left)
				{
					canvas[i][y] = temp[i][y - colValue];
				}
				else
				{
					canvas[i][y] = ' ';
				}
			}
		}
	}


}


void initCanvas(char canvas[][MAXCOLS])
{
	// Loops throught the canvas array so there is space on the canvas
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			canvas[x][y] = ' ';
		}
	}
}


void displayCanvas(char canvas[][MAXCOLS])
{
	// Clear the screen
	system("cls");

	//displays the screen with a border on the right edge
	for (int row = 0; row < MAXROWS; row++)
	{
		for (int col = 0; col < MAXCOLS; col++)
		{
			cout << canvas[row][col];
		}
		cout << "|";
		cout << "\n";
	}
	for (int col = 0; col < MAXCOLS; col++)
	{
		cout << "-";
	}
	cout << "\n";


}

void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	// This will make a copy of the canvas
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			to[x][y] = from[x][y];
		}
	}
}


void saveCanvas(char canvas[][MAXCOLS])
// This will save the canvas to a speific file location  
{
	ofstream outCanvas;
	char filename[FILENAMESIZE - 16];
	char fileLocation[FILENAMESIZE] = "SavedFiles\\";

	cin.ignore((numeric_limits<streamsize>::max)(), '\n');

	cout << "Please enter a file name: ";
	cin.getline(filename, FILENAMESIZE - 16);

	if (cin.fail())
	{
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	}

	strncat_s(fileLocation, FILENAMESIZE, filename, FILENAMESIZE - 16);
	strncat_s(fileLocation, FILENAMESIZE, ".txt", 4);

	outCanvas.open(fileLocation);

	// If it is not in that location an error message will occur
	if (!outCanvas)
	{
		cerr << "ERROR: File cannot be written";
		system("pause");
		return;

	}

	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			outCanvas << canvas[x][y];
		}
		outCanvas << "\n";
	}
	outCanvas.close();
	cout << "File Saved!" << endl;
	system("pause");
	return;
}


bool loadCanvas(char canvas[][MAXCOLS], char filename[])
{
	char line[MAXCOLS + 1];

	//opens file
	ifstream file;
	file.open(filename);

	//if file cannot be opened it tells the user and returns back to the main menu
	if (!file)
	{
		return false;
	}

	// This will fill the array with space characters 
	// This function will not print anything to the canvas 
	initCanvas(canvas);

	int row = 0;

	//while loop that reads the file
	while (!file.eof() && row < MAXROWS + 1)
	{
		file.getline(line, MAXCOLS + 1);
		if (file.fail())
		{
			file.clear();
			file.ignore((numeric_limits<streamsize>::max)(), '\n');
		}

		bool isEnd = false;

		for (int i = 0; i < MAXCOLS; i++)
		{
			if (line[i] == 0)
			{
				isEnd = true;
			}
			if (isEnd)
			{
				canvas[row][i] = ' ';
			}
			else
			{
				canvas[row][i] = line[i];
			}
		}
		row++;
	}
	return true;
}