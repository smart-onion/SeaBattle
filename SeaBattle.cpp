#include <iostream>
#include <windows.h>
#include <string>
#include <cwchar>
#include <ctime>
#include <math.h>
using namespace std;

HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);

bool isRunning = true; // variable is responsible to keep game running


enum COLOR
{
	GREEN = 7 * 16 + 10,
	RED = 7 * 16 + 12,
	BLUE = 7 * 16 + 1,
	BLACK = 7 * 16,
};

enum LAYER
{
	MENU,
	GENERATE,
	GAME,
	GAMEOVER,
};


void ChangeColor(COLOR color);
void GoToXY(int x, int y);
void drawLine(int x, int y, int color, string message);
void DrawChar(int x, int y, int color, char ch);
void SetConsoleWindowColumnsAndRows(unsigned int columns, unsigned int rows);
void SetConsoleFont(int font_width, int font_height);
void CenterScreen(int font_width, int font_height, unsigned short console_window_width, unsigned short console_window_height);
void SetConsole(unsigned short console_window_width, unsigned short console_window_height);// apply console settings
int HandleMouseEvent(int x1 = -1, int x2 = -1, LAYER* layer = nullptr, int y1 = 0, int y2 = 0, char** arr = nullptr);
void DrawMenu(LAYER* layer, unsigned short console_window_width, unsigned short console_window_height);
int generateShip(int SIZE, char** arr, int shipSize, int shipCount, int count = 0);
void fillUpShips(char** arr, int shipSize, int SIZE, int shipCount);
char** generateTable(int SIZE);
void teset(char** arr, int x, int y);
void DrawTable(int startX, int startY, char** arr, bool player);
int computerTurn(char** playerTable);
bool isGameOver(char** arr, int SIZE);
void DrawGameOver(bool isPlayerOver, bool isComputerOver, LAYER* layer, unsigned short console_window_width, unsigned short console_window_height);




int main()
{

	const unsigned short console_window_width = 100; // width in characters
	const unsigned short console_window_height = 30; // height in characters

	srand(time(NULL));
	SetConsole(console_window_width, console_window_height);

	const int tableSize = 12;

	//generateTable();
	char** playerTable = nullptr;
	char** computerTable = nullptr;
	
	int action = 0;
	string message;
	int messageColumn = 43;
	int messageLine = 24;

	bool isPlayerOver = false;
	bool isComputerOver = false;

	LAYER layer = LAYER::MENU;

	unsigned int turn = rand() % 2;

	while (isRunning)
	{
			switch (layer)
			{
			case LAYER::MENU:

				DrawMenu(&layer, console_window_width, console_window_height);
				break;

			case LAYER::GENERATE:
				action = 1;


				message = "GENERATE";
				drawLine(messageColumn, messageLine, COLOR::BLUE, message);
				drawLine(messageColumn + 10, messageLine, COLOR::GREEN, "OK");


				do
				{
					if (action == 1)
					{
						playerTable = generateTable(tableSize);
						computerTable = generateTable(tableSize);
						DrawTable(20, 10, playerTable, true);
						DrawTable(55, 10, computerTable, false);
					}
					
					action = HandleMouseEvent(messageColumn, messageColumn + message.size(), &layer);

				} while (action != -1);

				message = "              ";
				drawLine(messageColumn, messageLine, COLOR::BLUE, message);
				drawLine(messageColumn + 10, messageLine, COLOR::GREEN, message);
				layer = LAYER::GAME;
				break;
			case LAYER::GAME:

				ChangeColor(COLOR::BLACK);
				DrawTable(20, 10, playerTable, true);
				DrawTable(55, 10, computerTable, false);

				isPlayerOver = isGameOver(playerTable, tableSize);
				isComputerOver = isGameOver(computerTable, tableSize);

				if (isPlayerOver)
				{
					layer = LAYER::GAMEOVER;
				}

				if (isComputerOver)
				{
					layer = LAYER::GAMEOVER;
				}



				if (turn)
				{
					message = "YOUR TURN     ";
					drawLine(10, 5, COLOR::GREEN, message);

					action = HandleMouseEvent(55, 76, &layer, 10, 21, computerTable);
					if (action == 2)
					{
						turn--;
					}
				}
				else
				{
					message = "COMPUTER TURN";
					drawLine(10, 5, COLOR::RED, message);
					Sleep(500);
					action = computerTurn(playerTable);
					if (!action)
					{
						turn++;
					}
					Sleep(500);
				}

				message = "GIVE UP!";
				drawLine(10, 24, COLOR::RED, message);
				HandleMouseEvent(10, 10 + message.size(), &layer);
				
				if (layer == LAYER::GAMEOVER)
				{
					system("cls");
				}
				break;
			case LAYER::GAMEOVER:

				DrawGameOver(isPlayerOver, isComputerOver, &layer, console_window_width,console_window_height);
				break;
			}

	}
	
}

void ChangeColor(COLOR color)
{
	SetConsoleTextAttribute(h_out, color);
}

void GoToXY(int x, int y)
{
	COORD cursor;
	cursor.X = x;
	cursor.Y = y;
	SetConsoleCursorPosition(h_out, cursor);
}

void drawLine(int x, int y, int color, string message)
{
	COORD cursor;
	cursor.X = x;
	cursor.Y = y;
	SetConsoleTextAttribute(h_out, color);
	SetConsoleCursorPosition(h_out, cursor);
	cout << message;
}

void DrawChar(int x, int y, int color, char ch)
{
	COORD cursor;
	cursor.X = x;
	cursor.Y = y;
	SetConsoleTextAttribute(h_out, color);
	SetConsoleCursorPosition(h_out, cursor);
	cout << ch;
}

void SetConsoleWindowColumnsAndRows(unsigned int columns, unsigned int rows)
{
	string width = to_string(columns);
	string height = to_string(rows);
	string command = "mode con cols=" + width + " lines=" + height;
	system(command.c_str());
	COORD bufferSize;
	bufferSize.X = columns; // ширина буфера
	bufferSize.Y = rows; // высота буфера
	SetConsoleScreenBufferSize(h_out, bufferSize);
}

void SetConsoleFont(int font_width, int font_height)
{
	CONSOLE_FONT_INFOEX cfi;
	static CONSOLE_FONT_INFOEX  fontex;
	fontex.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(h_out, 0, &fontex);
	fontex.dwFontSize.X = font_width;
	fontex.dwFontSize.Y = font_height;
	SetCurrentConsoleFontEx(h_out, NULL, &fontex);
}

void CenterScreen(int font_width, int font_height, unsigned short console_window_width, unsigned short console_window_height)
{
	HWND hwnd = GetConsoleWindow();

	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);
	int left_shift = (desktop_width - console_window_width * font_width) / 2;
	int top_shift = (desktop_height - console_window_height * font_height) / 2;
	MoveWindow(hwnd, left_shift, top_shift, console_window_width * font_width, console_window_height * font_height, true);
}

void SetConsole(unsigned short console_window_width, unsigned short console_window_height) // apply console settings
{
	CONSOLE_CURSOR_INFO info;

	int font_width = 10;
	int font_height = 20;

	system("COLOR 70");

	info.bVisible = false;
	info.dwSize = 100;
	SetConsoleCursorInfo(h_out, &info);
	SetConsoleFont(font_width, font_height);
	CenterScreen(font_width, font_height, console_window_width, console_window_height);
	SetConsoleWindowColumnsAndRows(console_window_width, console_window_height);
}

int HandleMouseEvent(int x1, int x2, LAYER* layer, int y1, int y2, char** arr)
{
	bool RIGHT_CLICK;
	bool LEFT_CLICK;
	bool DBL_CLICK;

	HANDLE h_in = GetStdHandle(STD_INPUT_HANDLE);

	COORD mouseCoords; // variable responsible for console coordinates
	const int eventCount = 256;
	INPUT_RECORD allEvents[eventCount]; // event array happened in console
	DWORD readEvent; // variable that contain count of passed events

	const int SIZE = 12;

	SetConsoleMode(h_in, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS); // enable mouse mode

	ReadConsoleInput(h_in, allEvents, eventCount, &readEvent); // get all events in console

	for (int i = 0; i < readEvent; i++)
	{
		RIGHT_CLICK = allEvents[i].Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED;
		LEFT_CLICK = allEvents[i].Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED;
		DBL_CLICK = allEvents[i].Event.MouseEvent.dwEventFlags == DOUBLE_CLICK;

		mouseCoords.X = allEvents[i].Event.MouseEvent.dwMousePosition.X; // set X mouse coordinate
		mouseCoords.Y = allEvents[i].Event.MouseEvent.dwMousePosition.Y; // set Y mouse coordinate



		switch (layer ? *layer : -1) {
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
		case LAYER::GENERATE:
			if (mouseCoords.X >= x1 && mouseCoords.X < x2 && mouseCoords.Y == 24 && LEFT_CLICK) // game over if pressed EXIT in console
			{
				return 1;
			}
			if (mouseCoords.X >= x1 + 10 && mouseCoords.X < x2 + 10 && mouseCoords.Y == 24 && LEFT_CLICK) // game over if pressed EXIT in console
			{
				return -1;
			}
			break;
		case LAYER::GAME:
			if (mouseCoords.X > x1 && mouseCoords.X < x2 && mouseCoords.Y > y1 && mouseCoords.Y < y2 && LEFT_CLICK) //
			{
				int x = (mouseCoords.X - x1) / 2;
				int y = mouseCoords.Y - y1;

				if (arr[y][x] == '~' || arr[y][x] == '.')
				{
					arr[y][x] = '*';
					return 2;
				}
				else if (arr[y][x] == 'S')
				{
					arr[y][x] = 'X';

					teset(arr, x, y);
					return 1;
				}
			}
			if (mouseCoords.X >= x1 && mouseCoords.X < x2 && mouseCoords.Y == 24 && LEFT_CLICK) // start new game if pressed NEW GAME in console
			{
				*layer = LAYER::GAMEOVER;

			}
			break;
		case LAYER::GAMEOVER:
			if (mouseCoords.X >= x1 && mouseCoords.X < x2 && mouseCoords.Y == 20 && LEFT_CLICK) // start new game if pressed NEW GAME in console
			{
				system("cls");
				*layer = LAYER::MENU;

			}
			break;
		}

	}
	return 0;

}

void DrawMenu(LAYER* layer, unsigned short console_window_width, unsigned short console_window_height)
{

	string welcome = "W E L C O M E  T O  S E A   B A T T L E ! ! !";
	string newGame = "NEW GAME";
	string exit = "EXIT";

	short welcomeLine = 2;
	short newGameLine = 10;
	short exitLine = 12;

	drawLine((console_window_width - welcome.size()) / 2, welcomeLine, COLOR::GREEN, welcome);
	drawLine((console_window_width - newGame.size()) / 2, newGameLine, COLOR::GREEN, newGame);
	drawLine((console_window_width - exit.size()) / 2, exitLine, COLOR::RED, exit);
	HandleMouseEvent((console_window_width - exit.size()) / 2, (console_window_width - exit.size()) / 2 + exit.size(), layer);
	HandleMouseEvent((console_window_width - newGame.size()) / 2, (console_window_width - newGame.size()) / 2 + newGame.size(), layer);

}

int generateShip(int SIZE, char** arr, int shipSize, int shipCount, int count)
{

	bool isBusy;
	bool vertical;
	bool place = true;

	for (int y = 1; y < SIZE - 1; y++)
	{
		for (int x = 1; x < SIZE - 1; x++)
		{
			isBusy = rand() % 20;
			vertical = rand() % 2 - 1;
			place = true;

			if (!isBusy)
			{
				if (vertical)
				{
					if (y + shipSize >= SIZE)
					{
						place = false;
					}
					else
					{
						for (int i = 0; i < shipSize; i++)
						{
							if (arr[y + i][x] == 'S' || arr[y + i][x] == '.')
							{
								place = false;
							}
						}
					}
				}
				else
				{
					if (x + shipSize >= SIZE)
					{
						place = false;
					}
					else
					{
						for (int i = 0; i < shipSize; i++)
						{
							if (arr[y][x + i] == 'S' || arr[y][x + i] == '.')
							{
								place = false;
							}
						}
					}
				}

				if (place)
				{
					count++;
					if (vertical)
					{
						for (int i = 0; i < shipSize; i++)
						{
							arr[y + i][x] = 'S';
						}

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
						for (int i = 0; i < shipSize; i++)
						{
							arr[y][x + i] = 'S';
						}

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
			if (count == shipCount)
			{
				return count;;
			}
			place = false;

		}
	}
	return count;
}

void fillUpShips(char** arr, int shipSize, int SIZE, int shipCount)
{
	int count = generateShip(SIZE, arr, shipSize, shipCount);
	if (count == shipCount)
	{
		return;
	}
	fillUpShips(arr, shipSize, SIZE, shipCount - count);
}

char** generateTable(int SIZE)
{
	char** arr = new char* [SIZE];

	int shipSize = 1;
	int shipCount = 4;

	for (int i = 0; i < SIZE; i++)
	{
		arr[i] = new char[SIZE];
	}

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



	for (int i = 4; i != 0; i--)
	{
		fillUpShips(arr, shipSize, SIZE, shipCount);

		shipSize++;
		shipCount--;

	}

	return arr;
}

void teset(char** arr, int x, int y)
{
	if (arr[y][x] == '*' || arr[y][x] == '.' || arr[y][x] == '|' || arr[y][x] == '-' || arr[y][x] == 'x' || arr[y][x] == '~')
	{
		return;
	}
	else if (arr[y + 1][x] == 'S' || arr[y - 1][x] == 'S' || arr[y][x + 1] == 'S' || arr[y][x - 1] == 'S')
	{
		return;
	}
	else {
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


	teset(arr, x, y + 1);
	teset(arr, x, y - 1);
	teset(arr, x + 1, y);
	teset(arr, x - 1, y);


}

void DrawTable(int startX, int startY, char** arr, bool player)
{
	const int SIZE = 12;

	//int vertical = -1;
	//int damageShip = 0;
	//int directionFwd;
	//int directionAft;
	//int temp[4][2]{};
	//int index = 0;


	for (int y = 0; y < SIZE; y++)
	{
		GoToXY(startX, startY + y);

		for (int x = 0; x < SIZE; x++)
		{
			if (y == 0 && x != SIZE - 1 || y == SIZE - 1 && x != SIZE - 1)
			{

				cout << arr[y][x] << '-';

			}
			else if (arr[y][x] == '.' && player)
			{
				cout << '~' << ' ';
			}
			else if (arr[y][x] == '.' || arr[y][x] == 'S' && !player)
			{
				cout << '~' << ' ';
			}
			else if (arr[y][x] == '*')
			{
				cout << '*' << ' ';
			}
			else
			{

				cout << arr[y][x] << ' ';
			}

		}
		cout << '\n';
	}


	/*if (player)
	{
		color = COLOR::GREEN;
		ChangeColor(color);

		for (int y = 0; y < SIZE; y++)
		{
			GoToXY(startX, startY + y);

			for (int x = 0; x < SIZE; x++)
			{
				if (y == 0 && x != SIZE - 1 || y == SIZE - 1 && x != SIZE - 1)
				{
					cout << arr[y][x] << '-';

				}
				else if (arr[y][x] == '.')
				{
					cout << '~' << ' ';
				}
				else if (arr[y][x] == '*')
				{
					cout << '*' << ' ';
				}
				else
				{
					cout << arr[y][x] << ' ';
				}

			}
			cout << '\n';
		}
	}
	else
	{
		color = COLOR::RED;
		ChangeColor(color);

		for (int y = 0; y < SIZE; y++)
		{
			GoToXY(startX, startY + y);

			for (int x = 0; x < SIZE; x++)
			{

				// define if damaged ship is verticale
				/*if (arr[y][x] == 'X')
				{
					//if (arr[y - 1][x] == 'X' || arr[y + 1][x] == 'X' || arr[y][x - 1] == 'X' || arr[y][x + 1] == 'X')
					//{
					//	arr[y][x] = '@';
					//	damageShip++;
					//}
					if (arr[y - 1][x] == 'S' || arr[y + 1][x] == 'S')
					{
						vertical = 1;
					}
					else if (arr[y][x - 1] == 'S' || arr[y][x + 1] == 'S')
					{
						vertical = 0;
					}

					if (vertical == 1)
					{
						int directionFwd = y;
						int directionAft = y;

						while (arr[directionFwd][x] == 'X' && arr[directionAft][x] == 'X')
						{
							temp[index][0] = x;
							temp[index][1] = y;
							directionFwd += 1;
							directionAft -= 1;


						}
					}


				}*/


				/*if (y == 0 && x != SIZE - 1 || y == SIZE - 1 && x != SIZE - 1)
				{
					cout << arr[y][x] << "-";

				}
				else if (arr[y][x] == '.' || arr[y][x] == 'S')
				{
					cout << '~' << ' ';
				}
				else
				{
					cout << arr[y][x] << " ";
				}

			}
			cout << '\n';
		}
	}*/

	for (int i = int('A'), j = startX + 2; i < int('K'); i++, j += 2)
	{
		GoToXY(j, startY - 1);
		cout << (char)i << " ";
	}

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

int computerTurn(char** playerTable)
{
	const int SIZE = 12;
	bool hasPlace = true;

	while (hasPlace)
	{
		hasPlace = false;

		static int attackFieldX = 11;
		static int attackFieldY = 11;

		int fireX = rand() % attackFieldX + 1;
		int fireY = rand() % attackFieldY + 1;

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


		if (playerTable[fireY][fireX] == 'S')
		{
			//bool up = playerTable[fireY - 1][fireX] == 'S';
			//bool down = playerTable[fireY + 1][fireX] == 'S';
			//bool left = playerTable[fireY][fireX - 1] == 'S';
			//bool right = playerTable[fireY][fireX + 1] == 'S';

			//if ( up || down || left || right)
			//{
			//	playerTable[fireY][fireX] = 'X';
			//	return;
			//}
			//else
			//{
			//	playerTable[fireY][fireX] = 'X';
			//}

			playerTable[fireY][fireX] = 'X';
			teset(playerTable, fireX, fireY);
			return 1;
		}
		else if (playerTable[fireY][fireX] == '.' || playerTable[fireY][fireX] == '~')
		{
			playerTable[fireY][fireX] = '*';
			return 0;
		}

	}
	return 0;

}

bool isGameOver(char** arr, int SIZE)
{
	for (int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			if (arr[y][x] == 'S')
			{
				return false;
			}
		}
	}
	return true;
}

void DrawGameOver(bool isPlayerOver, bool isComputerOver, LAYER* layer, unsigned short console_window_width, unsigned short console_window_height)
{
	string message;
	int y = 10;


	if (isPlayerOver)
	{
		message = "You lose";
		drawLine((console_window_width - message.size()) / 2, y, COLOR::RED, message);
	}

	if (isComputerOver)
	{
		message = "You win";
		drawLine((console_window_width - message.size()) / 2, y, COLOR::GREEN, message);

	}

	if (!isPlayerOver && !isComputerOver)
	{
		message = "You lose";
		drawLine((console_window_width - message.size()) / 2, y, COLOR::RED, message);
	}

	message = "Return to main menu";
	y = 20;
	drawLine((console_window_width - message.size()) / 2, y, COLOR::RED, message);
	HandleMouseEvent((console_window_width - message.size()) / 2, (console_window_width - message.size()) / 2 + message.size(), layer);
}
