#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>

#define NAME_OF_GAME "Snake Game"

// size of field
#define FIELD_LENGHT 60 
#define FIELD_HEIGHT 20

// sprites of
#define FIELD_BLOCK '#'
#define SNAKE_MODEL_HEAD '0'
#define SNAKE_MODEL_BODY 'o'
#define FRUIT_MODEL '@'

#define ESCAPE 27 // exit the game

static enum bool{ false, true } gameOver; // the variable is responsible for continuing the game
static enum move { STOP, UP = 'w', DOWN = 's', RIGHT = 'd', LEFT = 'a' } moveCharacter; // the variable is responsible for move of character

typedef struct SnakeBodyNode { // structure that holds a snake body unit
	int positionX;
	int positionY;
	struct SnakeBodyNode *next;
	struct SnakeBodyNode *previous;
} snkNode_t;

static struct Snake {
	snkNode_t *head;
	snkNode_t *tail;
}snake; // a static variable that preserves the snake head and tail

static struct Fruit { // structure of fruit that has all information about the coordinates
	int PositionX;
	int PositionY;
} fruit; // the static variable fruit

static size_t score; // the variable is responsible for the score

void draw_field();
void calculating_coordinates();
inline void control_character();
inline void new_game();
inline void setting_up();
inline void printStr_gameOver();

// POINT OF ENTRY
int main() {
	srand(time(NULL)); // random for the next new fruit

	new_game();

	return 0;
}

inline void new_game() { // start a new game
	setting_up();

	while (!gameOver) { // the continuation of the game until the gameOver is false
		draw_field();
		control_character();
		calculating_coordinates();
	}

	printStr_gameOver();
}

inline void setting_up() { // setting up a new game, initializing default variables
	gameOver = false;

	// setting up a snake
	snake.head = malloc(sizeof(snkNode_t));
	snake.tail = snake.head;
	snake.head->next = NULL;
	snake.head->previous = NULL;
	snake.head->positionX = FIELD_LENGHT / 2;
	snake.head->positionY = FIELD_HEIGHT / 2;

	// setting up the fruit
	fruit.PositionX = 1 + rand() % (FIELD_LENGHT - 1);
	fruit.PositionY = 1 + rand() % (FIELD_HEIGHT - 1);

	// setting up defaults
	moveCharacter = STOP;
	score = 0;

	// setting up the name of the game
	SetConsoleTitle(NAME_OF_GAME);
	HANDLE hStdOut;
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ccInfo;
	ccInfo.bVisible = 0;
	ccInfo.dwSize = 10;
	SetConsoleCursorInfo(hStdOut, &ccInfo);
}

void draw_field() { // the function draws a field and all sprites
	for (size_t i = 0; i <= FIELD_HEIGHT; i++) {
		for (size_t j = 0; j <= FIELD_LENGHT; j++) {
			if (i == 0 || i == FIELD_HEIGHT) { // drawing the field
				printf("%c", FIELD_BLOCK);
			}
			else if (j == 0 || j == FIELD_LENGHT) { // drawing the field
				printf("%c", FIELD_BLOCK);
			}
			else if (i == snake.head->positionY && j == snake.head->positionX) { // drawing a snake
				printf("%c", SNAKE_MODEL_HEAD);
			}
			else if (i == fruit.PositionY && j == fruit.PositionX) { // drawing a fruit
				printf("%c", FRUIT_MODEL);
			}
			else {
				enum bool ind = false;
				snkNode_t *iter = snake.head->next; // drawing a snake body unit
				while (iter != NULL && ind == false) {
					if (i == iter->positionY && j == iter->positionX) {
						printf("%c", SNAKE_MODEL_BODY);
						ind = true;
					}
					iter = iter->next;
				}
				if (!ind) { // if the tail was not drawn
					printf(" ");
				}
			}
			if (i == FIELD_HEIGHT / 2 && j == FIELD_LENGHT) { // output the score
				printf("\tScore: %d", score);
			}
		}
		printf("\n");
	}
	system("cls");
}

inline void control_character() { // the function is responsible for controlling the character
	if (_kbhit()) { // if the keyboard key was pressed
		switch (_getch()) { // the keyboard key code reading
		case (int)LEFT:
			moveCharacter = LEFT;
			break;
		case (int)UP:
			moveCharacter = UP;
			break;
		case (int)RIGHT:
			moveCharacter = RIGHT;
			break;
		case (int)DOWN:
			moveCharacter = DOWN;
			break;
		case ESCAPE: // exit the game
			exit(1);
			break;
		default:
			break;
		}
	}
}

void calculating_coordinates() {

	snkNode_t * iter = snake.tail;
	while (iter->previous != NULL) { // machining the snake body
		iter->positionX = iter->previous->positionX;
		iter->positionY = iter->previous->positionY;
		iter = iter->previous;
	}

	switch (moveCharacter) // movement of the snake in the direction
	{
	case LEFT:
		snake.head->positionX--;
		break;
	case UP:
		snake.head->positionY--;
		break;
	case RIGHT:
		snake.head->positionX++;
		break;
	case DOWN:
		snake.head->positionY++;
		break;
	default:
		break;
	}

	iter = snake.head->next;
	while (iter != NULL) { // testing the snake to collide with itself
		if (snake.head->positionX == iter->positionX && snake.head->positionY == iter->positionY) {
			gameOver = true;
		}
		iter = iter->next;
	}

	// if the snake faces the wall, it comes out the other side
	if (snake.head->positionX < 1) { //	faces the wall	|<-
		snake.head->positionX = FIELD_LENGHT - 1;
	}
	else if (snake.head->positionX > FIELD_LENGHT - 1) { //	faces the wall	->|
		snake.head->positionX = 1;
	}
	else if (snake.head->positionY < 1) { //	faces the wall	/\ 
		snake.head->positionY = FIELD_HEIGHT - 1;
	}
	else if (snake.head->positionY > FIELD_HEIGHT - 1) { //	faces the wall	\/ 
		snake.head->positionY = 1;
	}

	if (snake.head->positionX == fruit.PositionX && snake.head->positionY == fruit.PositionY) { // eating fruit
		// a new random po³ition for the next fruit
		fruit.PositionX = 1 + rand() % (FIELD_LENGHT - 1);
		fruit.PositionY = 1 + rand() % (FIELD_HEIGHT - 1);

		// adding a new snake body unit
		snake.tail->next = malloc(sizeof(snkNode_t));
		snake.tail->next->next = NULL;
		snake.tail->next->previous = snake.tail;
		snake.tail = snake.tail->next;

		score++; // increase in score
	}
}

inline void printStr_gameOver() { // print Game Over
	char mas[6][57] = {
		{' ',' ',' ','_','_','_','_','_','_',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','_','_','_','_',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\n','\0'},
		{' ',' ','/',' ',' ','_','_','_','_','|',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','/',' ','_','_',' ','\\',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\n','\0'},
		{' ',' ','|',' ','|',' ',' ','_','_',' ',' ','_','_',' ','_',' ','_',' ','_','_',' ','_','_','_',' ',' ',' ','_','_','_',' ',' ','|',' ','|',' ',' ','|',' ','|','_',' ',' ',' ','_','_','_','_','_',' ','_',' ','_','_',' ','\n','\0'},
		{' ',' ','|',' ','|',' ','|','_',' ','|','/',' ','_','`',' ','|',' ','\'','_',' ','`',' ','_',' ','\\',' ','/',' ','_',' ','\\',' ','|',' ','|',' ',' ','|',' ','\\',' ','\\',' ','/',' ','/',' ','_',' ','\\',' ','\'','_','_','|','\n','\0'},
		{' ',' ','|',' ','|','_','_','|',' ','|',' ','(','_','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ','|',' ',' ','_','_','/',' ','|',' ','|','_','_','|',' ','|','\\',' ','v',' ','/',' ',' ','_','_','/',' ','|',' ',' ',' ','\n','\0'},
		{' ',' ','\\','_','_','_','_','_','_','|','\\','_','_',',','_','|','_','|',' ','|','_','|',' ','|','_','|','\\','_','_','_','|',' ',' ','\\','_','_','_','_','/',' ',' ','\\','_','/',' ','\\','_','_','_','|','_','|',' ',' ',' ','\n','\0'}
	}; // the array containing strings

	system("cls");
	printf("\n\n");
	for (int i = 0; i < 6; i++) { // print array
		printf("%s", mas[i]);
	}
	printf("\n\n\n\t      ");
	system("pause");
}