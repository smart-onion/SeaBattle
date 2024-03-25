#include <iostream>
#include <windows.h>
#include <string>
#include <cwchar>
#include <ctime>
#include <math.h>

using namespace std;
#pragma comment(lib, "winmm.lib")

HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);  // Global handle for console output

bool isRunning = true;  // Global variable to control the game loop

// Enumeration defining custom colors for console text attributes
enum COLOR
{
	GREEN = 7 * 16 + 10,
	RED = 7 * 16 + 12,
	BLUE = 7 * 16 + 1,
	BLACK = 7 * 16,
	BORDER = 40,
	SEA = 30,
	DAMAGE =  29,
};

// Enumeration defining different game states or layers
enum LAYER
{
	MENU,      // Menu screen
	GENERATE,  // Ship generation phase
	GAME,      // Gameplay
	GAMEOVER,  // Game over screen
};

enum SOUND {
	EXPLOSION,
	SPLASH,
	WIN,
	LOSE,
};

struct ThreadParam {
	SOUND sound;
};

// Changes the color of console text attributes.
void ChangeColor(COLOR color);

// Moves the cursor to the specified position on the console window.
void GoToXY(int x, int y);

// Draws a line of text with specified color starting from the given coordinates.
void DrawLine(int x, int y, int color, string message);

// Sets the size of the console window in terms of columns and rows.
void SetConsoleWindowColumnsAndRows(unsigned int columns, unsigned int rows);

// Sets the size of the console font.
void SetConsoleFont(int font_width, int font_height);

// Centers the console window on the screen given font dimensions.
void CenterScreen(int font_width, int font_height, unsigned short console_window_width, unsigned short console_window_height);

// Apply console settings, such as colors, font size, and window size.
void SetConsole(unsigned short console_window_width, unsigned short console_window_height);

// Handles mouse events for interaction with the game.
int HandleMouseEvent(int x1 = -1, int x2 = -1, LAYER* layer = nullptr, int y1 = 0, int y2 = 0, char** arr = nullptr);

// Draws the menu screen with options for the player.
void DrawMenu(LAYER* layer, unsigned short console_window_width, unsigned short console_window_height);

// Generates ships on the game board.
int GenerateShip(int SIZE, char** arr, int shipSize, int shipCount, int count = 0);

// Fills up the game board with ships.
void FillUpShips(char** arr, int shipSize, int SIZE, int shipCount);

// Generates a game board with ships placed randomly. 
char** GenerateTable(int SIZE);

// Marks adjacent cells of a hit ship as attacked.
void Teset(char** arr, int x, int y);

// Draws the game board with ships and hits.
void DrawTable(int startX, int startY, char** arr, bool player);

// Simulates a turn for the computer player.
int ComputerTurn(char** playerTable);

// Checks if the game is over by determining if all ships are sunk.
bool IsGameOver(char** arr, int SIZE);

// Draws the game over screen based on the game result.
void DrawGameOver(bool isPlayerOver, bool isComputerOver, LAYER* layer, unsigned short console_window_width, unsigned short console_window_height);

DWORD WINAPI PlayMusic(void*);

void SoundEffect(SOUND sound);


int main() {
	const unsigned short console_window_width = 100;
	const unsigned short console_window_height = 30;
	srand(time(NULL));
	SetConsole(console_window_width, console_window_height);

	const int tableSize = 12;
	char** playerTable = nullptr;
	char** computerTable = nullptr;
	int action = 0;
	string message;
	int messageColumn = 43;
	int messageLine = 24;
	bool isPlayerOver = false;
	bool isComputerOver = false;
	LAYER layer = LAYER::MENU;
	bool game = false;

	unsigned int turn = rand() % 2;

	// Main game loop
	while (isRunning) {
		
		switch (layer) {
		case LAYER::MENU:
			// Display the main menu and handle user input
			DrawMenu(&layer, console_window_width, console_window_height);
			break;

		case LAYER::GENERATE:

			// Generate game tables and switch to game layer
			action = 1;
			message = "GENERATE";
			DrawLine(messageColumn, messageLine, COLOR::BLUE, message);
			DrawLine(messageColumn + 10, messageLine, COLOR::GREEN, "OK");
			do {
				if (action == 1) {
					playerTable = GenerateTable(tableSize);
					computerTable = GenerateTable(tableSize);
					DrawTable(20, 10, playerTable, true);
					DrawTable(55, 10, computerTable, false);
				}
				action = HandleMouseEvent(messageColumn, messageColumn + message.size(), &layer);
			} while (action != -1);
			message = "              ";
			DrawLine(messageColumn, messageLine, COLOR::BLUE, message);
			DrawLine(messageColumn + 10, messageLine, COLOR::GREEN, message);
			layer = LAYER::GAME;
			break;

		case LAYER::GAME:

			// Play the game
			ChangeColor(COLOR::BLACK);
			DrawTable(20, 10, playerTable, true);
			DrawTable(55, 10, computerTable, false);
			isPlayerOver = IsGameOver(playerTable, tableSize);
			isComputerOver = IsGameOver(computerTable, tableSize);
			// Check if game is over
			if (isPlayerOver || isComputerOver) {
				layer = LAYER::GAMEOVER;
				game = false;
			}
			// Determine whose turn it is
			if (turn) {
				// Player's turn
				message = "YOUR TURN     ";
				DrawLine(55, 7, COLOR::GREEN, message);
				action = HandleMouseEvent(55, 76, &layer, 10, 21, computerTable);
				if (action == 2) {
					turn--;
					DrawLine(55, 7, COLOR::GREEN, "             ");
				}
			}
			else {
				// Computer's turn
				message = "COMPUTER TURN";
				DrawLine(20, 7, COLOR::RED, message);
				Sleep(3000);
				action = ComputerTurn(playerTable);
				if (!action) {
					turn++;
					DrawLine(20, 7, COLOR::GREEN, "             ");
				}
				
			}
			message = "GIVE UP!";
			DrawLine(46, 23, COLOR::RED, message);
			HandleMouseEvent(46, 46 + message.size(), &layer);
			if (layer == LAYER::GAMEOVER) {
				game = false;
				system("cls");
			}
			break;

		case LAYER::GAMEOVER:
			// Display game over screen
			DrawGameOver(isPlayerOver, isComputerOver, &layer, console_window_width, console_window_height);

			if (!game)
			{
				if (isPlayerOver || !isPlayerOver && !isComputerOver)
				{
					SoundEffect(SOUND::LOSE);
				}
				else if (isComputerOver)
				{
					SoundEffect(SOUND::WIN);

				}
			}
			game = true;
			break;
		}
	}
	return 0;
}


/// <summary>
/// Changes the text color in the console window.
/// </summary>
/// <param name="color">The desired color to set the text to.</param>
void ChangeColor(COLOR color)
{
	// SetConsoleTextAttribute is a Windows API function that changes the text color.
	SetConsoleTextAttribute(h_out, color);
}

/// <summary>
/// Moves the console cursor to the specified position.
/// </summary>
/// <param name="x">The X-coordinate of the new cursor position.</param>
/// <param name="y">The Y-coordinate of the new cursor position.</param>
void GoToXY(int x, int y)
{
	// COORD is a structure in the Windows API representing a pair of coordinates.
	COORD cursor;

	// Set the X and Y coordinates of the cursor.
	cursor.X = x;
	cursor.Y = y;

	// SetConsoleCursorPosition is a Windows API function that moves the console cursor to the specified position.
	SetConsoleCursorPosition(h_out, cursor);
}


/// <summary>
/// Draws a line of text with specified color at the specified position.
/// </summary>
/// <param name="x">The X-coordinate of the starting position of the line.</param>
/// <param name="y">The Y-coordinate of the starting position of the line.</param>
/// <param name="color">The color of the text to be drawn.</param>
/// <param name="message">The text message to be drawn.</param>
void DrawLine(int x, int y, int color, string message)
{
	// COORD is a structure in the Windows API representing a pair of coordinates.
	COORD cursor;

	// Set the X and Y coordinates of the cursor.
	cursor.X = x;
	cursor.Y = y;

	// SetConsoleTextAttribute is a Windows API function that changes the text color.
	SetConsoleTextAttribute(h_out, color);

	// SetConsoleCursorPosition is a Windows API function that moves the console cursor to the specified position.
	SetConsoleCursorPosition(h_out, cursor);

	// Output the message at the specified position.
	cout << message;
}


/// <summary>
/// Sets the size of the console window and buffer.
/// </summary>
/// <param name="columns">The number of columns in the console window.</param>
/// <param name="rows">The number of rows in the console window.</param>
void SetConsoleWindowColumnsAndRows(unsigned int columns, unsigned int rows)
{
	// Convert the column and row values to strings.
	string width = to_string(columns);
	string height = to_string(rows);

	// Formulate the command to change the console mode.
	string command = "mode con cols=" + width + " lines=" + height;

	// Execute the command using system().
	system(command.c_str());

	// COORD is a structure in the Windows API representing a pair of coordinates.
	COORD bufferSize;

	// Set the width and height of the console buffer.
	bufferSize.X = columns;
	bufferSize.Y = rows;

	// SetConsoleScreenBufferSize is a Windows API function that sets the size of the console screen buffer.
	SetConsoleScreenBufferSize(h_out, bufferSize);
}


/// <summary>
/// Sets the font size of the console.
/// </summary>
/// <param name="font_width">The width of the console font.</param>
/// <param name="font_height">The height of the console font.</param>
void SetConsoleFont(int font_width, int font_height)
{
	// CONSOLE_FONT_INFOEX is a structure in the Windows API representing console font information.
	CONSOLE_FONT_INFOEX cfi;

	// Initialize a CONSOLE_FONT_INFOEX structure to retrieve current font information.
	static CONSOLE_FONT_INFOEX fontex;
	fontex.cbSize = sizeof(CONSOLE_FONT_INFOEX);

	// GetCurrentConsoleFontEx is a Windows API function that retrieves information about the current console font.
	GetCurrentConsoleFontEx(h_out, 0, &fontex);

	// Update the width and height of the font.
	fontex.dwFontSize.X = font_width;
	fontex.dwFontSize.Y = font_height;

	// SetCurrentConsoleFontEx is a Windows API function that sets the current console font.
	SetCurrentConsoleFontEx(h_out, NULL, &fontex);
}


/// <summary>
/// Centers the console window on the screen.
/// </summary>
/// <param name="font_width">The width of the console font.</param>
/// <param name="font_height">The height of the console font.</param>
/// <param name="console_window_width">The width of the console window in characters.</param>
/// <param name="console_window_height">The height of the console window in characters.</param>
void CenterScreen(int font_width, int font_height, unsigned short console_window_width, unsigned short console_window_height)
{
	// Get the handle to the console window.
	HWND hwnd = GetConsoleWindow();

	// Get the width and height of the desktop screen.
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	// Calculate the left and top shifts to center the console window.
	int left_shift = (desktop_width - console_window_width * font_width) / 2;
	int top_shift = (desktop_height - console_window_height * font_height) / 2;

	// MoveWindow is a Windows API function that moves and resizes a window.
	MoveWindow(hwnd, left_shift, top_shift, console_window_width * font_width, console_window_height * font_height, true);
}


/// <summary>
/// Applies settings to the console window.
/// </summary>
/// <param name="console_window_width">The width of the console window in characters.</param>
/// <param name="console_window_height">The height of the console window in characters.</param>
void SetConsole(unsigned short console_window_width, unsigned short console_window_height)
{
	CONSOLE_CURSOR_INFO info;

	// Default font width and height.
	int font_width = 10;
	int font_height = 20;

	// Change console text color to light green on black background.
	system("COLOR 70");

	// Configure cursor settings.
	info.bVisible = false;
	info.dwSize = 100;
	SetConsoleCursorInfo(h_out, &info);

	SetConsoleFont(font_width, font_height);

	CenterScreen(font_width, font_height, console_window_width, console_window_height);

	SetConsoleWindowColumnsAndRows(console_window_width, console_window_height);
}


/// <summary>
/// Handles mouse input events for different layers of a console-based game.
/// </summary>
/// <param name="x1">The left boundary of the game area.</param>
/// <param name="x2">The right boundary of the game area.</param>
/// <param name="layer">Pointer to the current layer of the game.</param>
/// <param name="y1">The top boundary of the game area.</param>
/// <param name="y2">The bottom boundary of the game area.</param>
/// <param name="arr">A 2D array representing the game board.</param>
/// <returns>An integer representing the result of mouse event handling.</returns>
int HandleMouseEvent(int x1, int x2, LAYER* layer, int y1, int y2, char** arr)
{
	// Define variables to store mouse click states
	bool RIGHT_CLICK;
	bool LEFT_CLICK;
	bool DBL_CLICK;

	// Get handle to standard input
	HANDLE h_in = GetStdHandle(STD_INPUT_HANDLE);

	COORD mouseCoords; // variable responsible for console coordinates
	const int eventCount = 256;
	INPUT_RECORD allEvents[eventCount]; // event array happened in console
	DWORD readEvent; // variable that contain count of passed events

	const int SIZE = 12;

	SetConsoleMode(h_in, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS); // enable mouse mode

	ReadConsoleInput(h_in, allEvents, eventCount, &readEvent); // get all events in console
	
	// Loop through all events
	for (int i = 0; i < readEvent; i++)
	{
		// Check mouse button states
		RIGHT_CLICK = allEvents[i].Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED;
		LEFT_CLICK = allEvents[i].Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED;
		DBL_CLICK = allEvents[i].Event.MouseEvent.dwEventFlags == DOUBLE_CLICK;

		mouseCoords.X = allEvents[i].Event.MouseEvent.dwMousePosition.X; // set X mouse coordinate
		mouseCoords.Y = allEvents[i].Event.MouseEvent.dwMousePosition.Y; // set Y mouse coordinate



		// Handle mouse events based on the current game layer
		switch (layer ? *layer : -1) {
		// Handle mouse events for the menu layer
		case LAYER::MENU:

			if (mouseCoords.X >= x1 && mouseCoords.X < x2 && mouseCoords.Y == 12 && LEFT_CLICK) // game over if pressed EXIT in console
			{
				isRunning = false;
			}
			if (mouseCoords.X >= x1 && mouseCoords.X < x2 && mouseCoords.Y == 10 && LEFT_CLICK) // start new game if pressed NEW GAME in console
			{
				*layer = LAYER::GENERATE;
				system("cls");
			}

			return 1;
			break;
		// Handle mouse events for the generate layer
		case LAYER::GENERATE:
			if (mouseCoords.X >= x1 && mouseCoords.X < x2 && mouseCoords.Y == 24 && LEFT_CLICK) 
			{
				return 1;
			}
			if (mouseCoords.X >= x1 + 10 && mouseCoords.X < x2 + 10 && mouseCoords.Y == 24 && LEFT_CLICK)
			{
				return -1;
			}
			break;
		// Handle mouse events for the game layer
		case LAYER::GAME:
			if (mouseCoords.X > x1 && mouseCoords.X < x2 && mouseCoords.Y > y1 && mouseCoords.Y < y2 && LEFT_CLICK) //
			{
				int x = (mouseCoords.X - x1) / 2;
				int y = mouseCoords.Y - y1;

				if (arr[y][x] == '~' || arr[y][x] == '.')
				{
					arr[y][x] = '*';
					SoundEffect(SOUND::SPLASH);
					return 2;
				}
				else if (arr[y][x] == 'S')
				{
					arr[y][x] = 'X';
					SoundEffect(SOUND::EXPLOSION);
					Teset(arr, x, y);
					return 1;
				}
			}
			if (mouseCoords.X >= x1 && mouseCoords.X < x2 && mouseCoords.Y == 23 && LEFT_CLICK) 
			{
				*layer = LAYER::GAMEOVER;

			}
			break;
		// Handle mouse events for the gameover layer
		case LAYER::GAMEOVER:
			if (mouseCoords.X >= x1 && mouseCoords.X < x2 && mouseCoords.Y == 20 && LEFT_CLICK)
			{
				system("cls");
				*layer = LAYER::MENU;

			}
			break;
		}

	}
	return 0;

}


/// <summary>
/// Draws the menu screen and handles mouse events for menu interactions.
/// </summary>
/// <param name="layer">Pointer to the current layer of the game.</param>
/// <param name="console_window_width">The width of the console window in characters.</param>
/// <param name="console_window_height">The height of the console window in characters.</param>
void DrawMenu(LAYER* layer, unsigned short console_window_width, unsigned short console_window_height)
{
	// Define strings for menu options
	string welcome[] = {
		" ____ ____ ____ ____ ____ ____ ____ ____ ____",
		"||S |||e |||a |||B |||a |||t |||t |||l |||e ||",
		"||__|||__|||__|||__|||__|||__|||__|||__|||__||",
		"|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|/__\\|" 
	};

	ChangeColor(COLOR::BLUE);
	for (int x = (console_window_width - welcome[0].size()) / 2, y = 2, i = 0; y < 6 ; y++,i++)
	{
		GoToXY(x, y);
		cout << welcome[i];
	}

	string newGame = "NEW GAME";
	string exit = "EXIT";

	// Define line positions for menu options
	short welcomeLine = 1;
	short newGameLine = 10;
	short exitLine = 12;

	// Draw menu options with appropriate colors
	DrawLine((console_window_width - newGame.size()) / 2, newGameLine, COLOR::GREEN, newGame);
	DrawLine((console_window_width - exit.size()) / 2, exitLine, COLOR::RED, exit);

	// Handle mouse events for menu interactions
	HandleMouseEvent((console_window_width - exit.size()) / 2, (console_window_width - exit.size()) / 2 + exit.size(), layer, welcomeLine, exitLine, nullptr);
	HandleMouseEvent((console_window_width - newGame.size()) / 2, (console_window_width - newGame.size()) / 2 + newGame.size(), layer, welcomeLine, newGameLine, nullptr);
}


/// <summary>
/// Randomly generates ships on the game board.
/// </summary>
/// <param name="SIZE">The size of the game board.</param>
/// <param name="arr">A 2D array representing the game board.</param>
/// <param name="shipSize">The size of the ships to generate.</param>
/// <param name="shipCount">The total number of ships to generate.</param>
/// <param name="count">The count of ships already generated.</param>
/// <returns>The updated count of ships generated.</returns>
int GenerateShip(int SIZE, char** arr, int shipSize, int shipCount, int count)
{
	bool isBusy;
	bool vertical;
	bool place = true;

	// Iterate over the game board
	for (int y = 1; y < SIZE - 1; y++)
	{
		for (int x = 1; x < SIZE - 1; x++)
		{
			// Randomly determine if the cell is busy
			isBusy = rand() % 20;
			// Randomly determine if the ship should be vertical or horizontal
			vertical = rand() % 2 - 1;
			place = true;

			// Check if the cell is not busy
			if (!isBusy)
			{
				// If ship should be vertical
				if (vertical)
				{
					// Check if there's enough space to place the ship vertically
					if (y + shipSize >= SIZE)
					{
						place = false;
					}
					else
					{
						// Check if the cells where the ship will be placed are empty
						for (int i = 0; i < shipSize; i++)
						{
							if (arr[y + i][x] == 'S' || arr[y + i][x] == '.')
							{
								place = false;
							}
						}
					}
				}
				// If ship should be horizontal
				else
				{
					// Check if there's enough space to place the ship horizontally
					if (x + shipSize >= SIZE)
					{
						place = false;
					}
					else
					{
						// Check if the cells where the ship will be placed are empty
						for (int i = 0; i < shipSize; i++)
						{
							if (arr[y][x + i] == 'S' || arr[y][x + i] == '.')
							{
								place = false;
							}
						}
					}
				}

				// If there's enough space and the cells are empty, place the ship
				if (place)
				{
					count++;
					if (vertical)
					{
						// Place the ship vertically
						for (int i = 0; i < shipSize; i++)
						{
							arr[y + i][x] = 'S';
						}

						// Mark surrounding cells as empty
						for (int i = -1; i < shipSize + 1; i++)
						{
							for (int j = -1; j < 2; j++)
							{
								if (arr[y + i][x + j] != 'S' && arr[y + i][x + j] != '-' && arr[y + i][x + j] != '|')
								{
									arr[y + i][x + j] = '.';
								}
							}
						}
					}
					else
					{
						// Place the ship horizontally
						for (int i = 0; i < shipSize; i++)
						{
							arr[y][x + i] = 'S';
						}

						// Mark surrounding cells as empty
						for (int i = -1; i < 2; i++)
						{
							for (int j = -1; j < shipSize + 1; j++)
							{
								if (arr[y + i][x + j] != 'S' && arr[y + i][x + j] != '-' && arr[y + i][x + j] != '|')
								{
									arr[y + i][x + j] = '.';
								}
							}
						}
					}
				}
			}
			// If the desired number of ships is generated, return
			if (count == shipCount)
			{
				return count;;
			}
			place = false;
		}
	}
	return count;
}


/// <summary>
/// Fills up the game board with ships until the specified number of ships is reached.
/// </summary>
/// <param name="arr">A 2D array representing the game board.</param>
/// <param name="shipSize">The size of the ships to generate.</param>
/// <param name="SIZE">The size of the game board.</param>
/// <param name="shipCount">The total number of ships to generate.</param>
void FillUpShips(char** arr, int shipSize, int SIZE, int shipCount)
{
	// Generate ships and count the number of ships generated
	int count = GenerateShip(SIZE, arr, shipSize, shipCount);

	// If the desired number of ships is reached, return
	if (count == shipCount)
	{
		return;
	}

	// Recursively fill up ships until the specified number is reached
	FillUpShips(arr, shipSize, SIZE, shipCount - count);
}


/// <summary>
/// Generates a game board with ships.
/// </summary>
/// <param name="SIZE">The size of the game board.</param>
/// <returns>A 2D array representing the game board.</returns>
char** GenerateTable(int SIZE)
{
	// Allocate memory for the game board
	char** arr = new char* [SIZE];

	// Define ship size and count
	int shipSize = 1;
	int shipCount = 4;

	// Allocate memory for each row of the game board
	for (int i = 0; i < SIZE; i++)
	{
		arr[i] = new char[SIZE];
	}

	// Initialize the game board with water '~', borders '-', and '|'
	for (int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			if (y == 0 || y == SIZE - 1)
			{
				arr[y][x] = '-';
			}
			else if (x == 0 || x == SIZE - 1)
			{
				arr[y][x] = '|';
			}
			else
			{
				arr[y][x] = '~';
			}
		}
	}

	// Generate ships of decreasing size
	for (int i = 4; i != 0; i--)
	{
		// Fill up ships on the game board
		FillUpShips(arr, shipSize, SIZE, shipCount);

		// Update ship size and count for the next iteration
		shipSize++;
		shipCount--;
	}

	return arr;
}


/// <summary>
/// Recursively reveals adjacent cells on the game board when a ship is hit.
/// </summary>
/// <param name="arr">A 2D array representing the game board.</param>
/// <param name="x">The x-coordinate of the cell to reveal.</param>
/// <param name="y">The y-coordinate of the cell to reveal.</param>
void Teset(char** arr, int x, int y)
{
	// Base case: If the cell is already revealed or not part of the game board, return
	if (arr[y][x] == '*' || arr[y][x] == '.' || arr[y][x] == '|' || arr[y][x] == '-' || arr[y][x] == 'x' || arr[y][x] == '~')
	{
		return;
	}
	// Check if the cell contains a ship and has adjacent ship cells
	else if (arr[y + 1][x] == 'S' || arr[y - 1][x] == 'S' || arr[y][x + 1] == 'S' || arr[y][x - 1] == 'S')
	{
		return;
	}
	else
	{
		// Mark adjacent cells as hit and mark the current cell as hit ('x')
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (arr[y + i][x + j] != 'X' && arr[y + i][x + j] != '|' && arr[y + i][x + j] != '-' && arr[y + i][x + j] != 'x')
				{
					arr[y + i][x + j] = '*';
				}
				arr[y][x] = 'x';
			}
		}
	}

	// Recursively reveal adjacent cells
	Teset(arr, x, y + 1);
	Teset(arr, x, y - 1);
	Teset(arr, x + 1, y);
	Teset(arr, x - 1, y);
}


/// <summary>
/// Draws the game board with ships and hits.
/// </summary>
/// <param name="startX">The x-coordinate of the starting position to draw the table.</param>
/// <param name="startY">The y-coordinate of the starting position to draw the table.</param>
/// <param name="arr">A 2D array representing the game board.</param>
/// <param name="player">A boolean indicating whether the board is for the player (true) or opponent (false).</param>
void DrawTable(int startX, int startY, char** arr, bool player)
{
	const int SIZE = 12;

	// Loop through the game board
	for (int y = 0; y < SIZE; y++)
	{
		GoToXY(startX, startY + y);

		for (int x = 0; x < SIZE; x++)
		{
			// Draw board edges
			if (arr[y][x] == '-')
			{
				//GoToXY(startX + x, startY + y);
				ChangeColor(COLOR::BORDER);
				cout << "  ";

			}
			else if (arr[y][x] == '|')
			{
				ChangeColor(COLOR::BORDER);
				cout << "  ";
			}
			// Draw cells based on player's view
			else if (arr[y][x] == '.' && player)
			{
				ChangeColor(COLOR::SEA);

				cout << '~' << ' ';
			}
			else if (arr[y][x] == '.' || arr[y][x] == 'S' && !player)
			{
				ChangeColor(COLOR::SEA);

				cout << '~' << ' ';
			}
			else if (arr[y][x] == '*')
			{
				ChangeColor(COLOR::SEA);

				cout << '*' << ' ';
			}
			else if (arr[y][x] == 'X' || arr[y][x] == 'x')
			{
				ChangeColor(COLOR::DAMAGE);
				cout << 'X' << ' ';
			}
			else
			{
				ChangeColor(COLOR::SEA);

				cout << arr[y][x] << ' ';
			}
		}
		cout << '\n';
	}
	ChangeColor(COLOR::BLACK);
	// Draw column labels (A-K)
	for (int i = int('A'), j = startX + 2; i < int('K'); i++, j += 2)
	{
		GoToXY(j, startY - 1);
		cout << (char)i << " ";
	}

	// Draw row numbers (1-10)
	for (int i = 1; i < 11; i++)
	{
		if (i == 10)
		{
			GoToXY(startX - 2, startY + i);
			cout << i;
		}
		else
		{
			GoToXY(startX - 1, startY + i);
			cout << i;
		}
	}
}


/// <summary>
/// Performs the computer's turn by randomly selecting a target on the player's table.
/// </summary>
/// <param name="playerTable">A 2D array representing the player's game board.</param>
/// <returns>1 if the computer hits a ship, 0 otherwise.</returns>
int ComputerTurn(char** playerTable)
{
	const int SIZE = 12;
	bool hasPlace = true;

	// Check if there are still available places to attack on the player's table
	while (hasPlace)
	{
		hasPlace = false;

		static int attackFieldX = 11;
		static int attackFieldY = 11;

		// Generate random coordinates within the attack field
		int fireX = rand() % attackFieldX + 1;
		int fireY = rand() % attackFieldY + 1;

		// Check if there are available places to attack
		for (int i = 0; i < SIZE; i++)
		{
			for (int j = 0; j < SIZE; j++)
			{
				if (playerTable[i][j] == '.' || playerTable[i][j] == '~')
				{
					hasPlace = true;
				}
			}
		}

		// If the target is a ship, mark it as hit ('X') and reveal adjacent cells
		if (playerTable[fireY][fireX] == 'S')
		{
			playerTable[fireY][fireX] = 'X';
			Teset(playerTable, fireX, fireY);
			SoundEffect(SOUND::EXPLOSION);
			return 1;
		}
		// If the target is empty, mark it as missed ('*')
		else if (playerTable[fireY][fireX] == '.' || playerTable[fireY][fireX] == '~')
		{
			playerTable[fireY][fireX] = '*';
			SoundEffect(SOUND::SPLASH);
			return 0;
		}
	}
	return 0;
}


/// <summary>
/// Checks if the game is over by determining if all ships on the game board are sunk.
/// </summary>
/// <param name="arr">A 2D array representing the game board.</param>
/// <param name="SIZE">The size of the game board.</param>
/// <returns>True if the game is over (all ships are sunk), false otherwise.</returns>
bool IsGameOver(char** arr, int SIZE)
{
	// Iterate through the game board
	for (int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			// If there's a ship cell remaining, the game is not over
			if (arr[y][x] == 'S')
			{
				return false;
			}
		}
	}
	// If no ship cells are found, the game is over
	return true;
}


/// <summary>
/// Draws the game over screen based on the game result.
/// </summary>
/// <param name="isPlayerOver">Indicates if the player's game is over.</param>
/// <param name="isComputerOver">Indicates if the computer's game is over.</param>
/// <param name="layer">Pointer to the current layer of the game.</param>
/// <param name="console_window_width">Width of the console window.</param>
/// <param name="console_window_height">Height of the console window.</param>
void DrawGameOver(bool isPlayerOver, bool isComputerOver, LAYER* layer, unsigned short console_window_width, unsigned short console_window_height)
{
	string message;
	string win[]{
	" ____ ____ ____ _________ ____ ____ ____",
	"||Y |||o |||u |||       |||W |||i |||n ||",
	"||__|||__|||__|||_______|||__|||__|||__||",
	"|/__\\|/__\\|/__\\|/_______\\|/__\\|/__\\|/__\ |"
	};

	string lose[]{
	" ____ ____ ____ _________ ____ ____ ____ ____",
	"||Y |||o |||u |||       |||L |||o |||s |||e ||",
	"||__|||__|||__|||_______|||__|||__|||__|||__||",
	"|/__\\|/__\\|/__\\|/_______\\|/__\\|/__\\|/__\\|/__\ |"
	};
	int y = 10;

	static bool play = true;

	// Draw appropriate message based on game result
	if (isPlayerOver || !isPlayerOver && !isComputerOver)
	{
		for (int i = 0, y = 5; i < 4; i++, y++ )
		{
			GoToXY(28, y);
			cout << lose[i];
		}
		//DrawLine((console_window_width - message.size()) / 2, y, COLOR::RED, message);
	}

	if (isComputerOver)
	{
		for (int i = 0, y = 5; i < 4; i++, y++)
		{
			GoToXY(30, y);
			cout << win[i];
		}
	}

	Sleep(500);
	// Draw option to return to main menu
	message = "Return to main menu";
	y = 20;
	DrawLine((console_window_width - message.size()) / 2, y, COLOR::RED, message);
	HandleMouseEvent((console_window_width - message.size()) / 2, (console_window_width - message.size()) / 2 + message.size(), layer);
}


DWORD WINAPI PlayMusic(LPVOID  param)
{
	int volume = 10;
	// Convert the volume level to a DWORD between 0 and 100
	DWORD dwVolume = ((DWORD)volume * 0xFFFF) / 100;

	// Set the volume level for all sounds
	waveOutSetVolume(NULL, MAKELONG(dwVolume, dwVolume));


	PlaySoundA("./music/theme.wav", NULL, SND_NODEFAULT | SND_ASYNC);
	return 0;
}

void SoundEffect(SOUND sound)
{
	switch (sound)
	{
	case EXPLOSION:
		PlaySoundA("./music/explosion.wav", 0, SND_NODEFAULT | SND_ASYNC);
		break;
	case SPLASH:
		PlaySoundA("./music/splash.wav", 0, SND_NODEFAULT | SND_ASYNC);
		break;
	case WIN:
		PlaySoundA("./music/gameWin.wav", 0, SND_NODEFAULT | SND_ASYNC);
		break;
	case LOSE:
		PlaySoundA("./music/gameOver.wav", 0, SND_NODEFAULT | SND_SYNC);
		break;
	default:
		break;
	}
}