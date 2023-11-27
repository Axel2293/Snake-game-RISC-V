#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Longitud de los leds
#define W LED_MATRIX_0_WIDTH
// Alto de los leds
#define H LED_MATRIX_0_HEIGHT
// Lista circular
#define L (W*H)

// Pointer to leds
volatile unsigned int *led_base = LED_MATRIX_0_BASE;
// D-pad pointers
volatile unsigned int *d_pad_up = D_PAD_0_UP;
volatile unsigned int *d_pad_do = D_PAD_0_DOWN;
volatile unsigned int *d_pad_le = D_PAD_0_LEFT;
volatile unsigned int *d_pad_ri = D_PAD_0_RIGHT;

volatile unsigned int *head;
volatile unsigned int *tail;
volatile unsigned int *snake[L];
int headindx = 0;
int tailindx = 0;

int snake_score = 2;

// Prototipos
void snakeInit();
void snakeGame();
int moveSnake(int offset, int direction);
int eatApple(volatile unsigned int *cord);
int addApple();
void drawBorders();
void cleanLeds();

int main()
{
    cleanLeds();
    drawBorders();
    snakeGame();
}

// Functions
void snakeInit()
{
    snake[0] = led_base + 1 + W;
    snake[1] = led_base + 2 + W;

    head = led_base + 2 + W;
    tail = led_base + 1 + W;

    *snake[0] = 0x00FF00;
    *snake[1] = 0x00FF00;

    headindx++;

    addApple();
}

// Movement shit and
void snakeGame()
{
    // Starts going to the right on horizontal
    // Game indicates wether the game is over
    int offset = 1, direction = 0, game = 1;
    snakeInit();
    printf("BIENVENIDO A SNAKE\n");
    while (1)
    {
        // Make some delay
        int a = 0;
        headindx = headindx%L;
        tailindx = tailindx%L;
        for (int i = 0; i < 30000; i++)
        {
            a = i;
        }
        // UP
        if (*d_pad_up == 1 && direction != 1)
        {
            direction = 1;
            offset = -1;
            game = moveSnake(offset, direction);
        }
        // DOWN
        else if (*d_pad_do == 1 && direction != 1)
        {
            direction = 1;
            offset = 1;
            game = moveSnake(offset, direction);
        }
        // LEFT
        else if (*d_pad_le == 1 && direction != 0)
        {
            direction = 0;
            offset = -1;
            game = moveSnake(offset, direction);
        }
        // RIGHT
        else if (*d_pad_ri == 1 && direction != 0)
        {
            direction = 0;
            offset = 1;
            game = moveSnake(offset, direction);
        }
        // End the game
        else if (game == 0)
        {
            break;
        }
        else
        {
            game = moveSnake(offset, direction);
        }
    }
}

int moveSnake(int offset, int direction) {
    // Horizontal - X axis
    if (direction == 0) {
        printf("MOVE 1\n");
        // Not on a border or collision with the snake
        if (*(head + offset) != 0xFF0000 && *(head + offset) != 0x00F00F) {
            // Eat a fruit
            if (*(head + offset) == 0xF000F0) {
                eatApple(head + offset);
            } else {
                printf("MOVE 3 %X\n", head + offset);
                *(head + offset) = 0x00F00F;
                snake[(headindx + 1) % L] = head + offset;
                head = head + offset;
                *snake[tailindx] = 0x00;
                snake[tailindx] = NULL;
                tail = snake[(tailindx + 1) % L];
                headindx++;
                tailindx++;
            }
            return 1;
        } else {
            printf("PERDISTEEEEEEEEEEE");
            return 0;
        }
    }
    // Vertical - Y axis
    else {
        // Not on a border or collision with the snake
        if (*(head + (offset * W)) != 0xFF0000 && *(head + (offset * W)) != 0x00F00F) {
            // Eat a fruit
            if (*(head + (offset * W)) == 0xF000F0) {
                eatApple(head + (offset * W));
            } else {
                *(head + (offset * W)) = 0x00F00F;
                snake[(headindx + 1) % L] = head + (offset * W);
                head = head + (offset * W);
                *snake[tailindx] = 0x00;
                snake[tailindx] = NULL;
                tail = snake[(tailindx + 1) % L];
                headindx++;
                tailindx++;
            }
            return 1;
        } else {
            printf("PERDISTEEEEEEEEEEE");
            return 0;
        }
    }
}


// Adds a new part to the snake
int eatApple(volatile unsigned int *cord)
{
    // printf("Sanake score = %d \n", snake_score);

    snake[headindx + 1] = cord;
    head = cord;
    headindx++;
    *snake[headindx] = 0x00FF00;

    snake_score++; // Aumentamos score
    printf("Sanake score = %d \n", snake_score);

    return addApple();
}

// Adds an apple to the board
int addApple()
{

    // Genera coordenadas aleatorias
    int x = rand() % (W - 2) + 1; // Evita los bordes
    int y = rand() % (H - 2) + 1;

    // Verifica si la posici?n est? ocupada por la serpiente
    while (*(led_base + (W * y) + x) != 0x000000)
    {
        x = rand() % (W - 2) + 1;
        y = rand() % (H - 2) + 1;
    }

    // Coloca la manzana en el tablero
    if(*(led_base + (W * y) + x) != 0x00FF00 && *(led_base + (W * y) + x) != 0xFF0000){
        printf("NEW APPLE %X\n", *(led_base + (W * y) + x));
        *(led_base + (W * y) + x) = 0xF000F0;

    }
    return 1; // Indica que se gener? una manzana
}

void drawBorders()
{
    for (int i = 0; i < W; i++)
    {
        *(led_base + i) = 0xFF0000;
    }
    for (int i = 0; i < H; i++)
    {
        *(led_base + (W * i)) = 0xFF0000;
    }
    for (int i = 0; i < H; i++)
    {
        *((led_base + (W * i)) + (W - 1)) = 0xFF0000;
    }
    for (int i = 0; i < W; i++)
    {
        *(led_base + (W * (H - 1)) + i) = 0xFF0000;
    }
}

void cleanLeds()
{
    for (size_t i = 0; i < (W * H); i++)
    {
        *(led_base + i) = 0x00;
    }
}