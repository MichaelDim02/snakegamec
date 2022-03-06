#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include "X11/keysym.h"


#define X 20  /* vertical */
#define Y 50 /* horizontal */

#define VOID ' '
#define CELL '#'
#define APPL 'o'
#define WALL '$'

char a[X][Y];
int score = 0;


typedef struct position {
	int x;
	int y;
} Position;


typedef struct snake {
	Position cells[100];
	int direction[100];
	int length;
} Snake;


void
print_panel()
{
	printf("\033[0;0H");

	int i;
	for (i = 0; i < Y + 2; i++) {
		printf("%c", WALL);
	}
	printf("\n");

	int j;
	for (i = 0; i < X; i++) {
		printf("%c", WALL);
		for (j = 0; j < Y; j++) {
			printf("%c",a[i][j]);
		}
		printf("%c\n", WALL);
	}

	for (i = 0; i < Y + 2; i++) {
		printf("%c", WALL);
	}
	printf("\n");
}

void
fill_array(char panel[][Y])
{
	int i, j;
	for (i = 0; i < X; i++) {
		for (j = 0; j < Y; j++) {
			panel[i][j] = VOID;
		}
	}
}

void
update_snake(Snake snake)
{
	int i;
	for (i = 0; i < snake.length; i++) {
		a[snake.cells[i].x][snake.cells[i].y] = CELL;
	}

	int x = snake.cells[snake.length-1].x;
	int y = snake.cells[snake.length-1].y;

	if (snake.direction[0] == 1)
		y = snake.cells[snake.length - 1].y - snake.length -1;
	else if (snake.direction[0] == 2)
		y = snake.cells[snake.length - 1].y + snake.length +1;
	else if (snake.direction[0] == 3)
		x = snake.cells[snake.length - 1].x - snake.length -1;
	else if (snake.direction[0] == 4)
		x = snake.cells[snake.length - 1].x + snake.length +1;

	/*if (snake.direction[snake.length-1] == 1)
		y = snake.cells[snake.length-1].y -1;
	else if (snake.direction[snake.length-1] == 2)
		y = snake.cells[snake.length-1].y  +1;
	else if (snake.direction[snake.length-1] == 3)
		x = snake.cells[snake.length-1].x - 1;
	else if (snake.direction[snake.length-1]  == 4)
		x = snake.cells[snake.length - 1].x +1;*/

	a[x][y] = VOID;
}

Position
get_apple()
{
	Position apple;

	do {
		srand(time(0));
		apple.x = rand() % (X - 0 + 1);

		srand(time(0));
		apple.y = rand() % (Y - 0 + 1);

	} while (a[apple.x][apple.y] == CELL);

	a[apple.x][apple.y] = APPL;

	return apple;
}


int
key_is_pressed(KeySym ks)
{
	Display *dpy = XOpenDisplay(":0");
	char keys_return[32];
	XQueryKeymap(dpy, keys_return);
	KeyCode kc2 = XKeysymToKeycode(dpy, ks);
	int isPressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
	XCloseDisplay(dpy);
	return isPressed;
}

Snake
get_keys(Snake snake)
{
	for (int i = 0; i < snake.length; i++)
		snake.direction[i+1] = snake.direction[i];


	if (key_is_pressed(XK_Up) || key_is_pressed(XK_W)) {
		snake.direction[0] = 4;
	}

	if (key_is_pressed(XK_Down) || key_is_pressed(XK_S)) {
		snake.direction[0] = 3;
	}

	if (key_is_pressed(XK_Left) || key_is_pressed(XK_A)) {
		snake.direction[0] = 2;
	}

	if (key_is_pressed(XK_Right) || key_is_pressed(XK_D)) {
		snake.direction[0] = 1;
	}
	return snake;
}

Snake
generation(Snake snake)
{
	for (int i = 0; i <= snake.length; i++) {
		snake.cells[i+1].y = snake.cells[i].y;
		snake.cells[i+1].x = snake.cells[i].x;
	}

	if (snake.direction[0] == 1) {
		snake.cells[0].y++;
	} else if (snake.direction[0] == 2) {
		snake.cells[0].y--;
	} else if (snake.direction[0] == 3) {
		snake.cells[0].x++;
	} else if (snake.direction[0] == 4) {
		snake.cells[0].x--;
	}

	return snake;
}

int
main()
{
	fill_array(a);

	Snake snake;

	snake.length = 1;
	snake.cells[0].y = Y / 2;
	snake.cells[0].x = X / 2;

	snake.direction[0] = 2;
	Position apple = get_apple();
	print_panel();

	do {
		if (snake.cells[0].x == apple.x &&
	    	    snake.cells[0].y == apple.y) {
			apple = get_apple();
			score += 10;

			int x = snake.cells[snake.length-1].x;
			int y = snake.cells[snake.length-1].y;

			if (snake.direction[snake.length - 1] == 1)
				y = snake.cells[snake.length - 1].y + 1;
			else if (snake.direction[snake.length - 1] == 2)
				y = snake.cells[snake.length - 1].y - 1;
			else if (snake.direction[snake.length - 1] == 3)
				x = snake.cells[snake.length - 1].x + 1;
			else if (snake.direction[snake.length - 1] == 4)
				x = snake.cells[snake.length - 1].x - 1;

			snake.length++;
			snake.cells[snake.length-1].x = x;
			snake.cells[snake.length-1].y = y;
		}

		update_snake(snake);
		print_panel();
		printf("x=%d, y=%d\n", snake.cells[0].x, snake.cells[0].y);
		printf("score: %d\nlength: %d\n", score, snake.length);

		snake = get_keys(snake);
		snake = generation(snake);

		usleep(60000);

	} while (1);

	return 0;
}
