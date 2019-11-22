#ifndef SNAKE_H
#define SNAKE_H

typedef enum
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3
}Direction;

typedef struct square
{
    int x;
    int y;
<<<<<<< HEAD
    square* next;
    square* previous;
};
    
=======
    struct square* next;
    struct square* previous;
}square;

>>>>>>> 3ff8a875e263efef721aaeba5d8b1fa550796a35


typedef struct snake
{
    square* head; //pointer to head square of snake body
    square* back; //pointer to back square
} snake;


//starts the snake game
int snake_game(int fps);


#endif