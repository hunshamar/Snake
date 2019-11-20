
typedef enum Direction Direction;
enum Direction
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3
};

typedef struct snake_square square;
struct snake_square
{
    int x;
    int y;
    square* next;
    square* previous;
};


typedef snake snake;
struct snake
{
    square* head; //pointer to head square of snake body
    square* back; //pointer to back square
    int size;
};


//starts the snake game
int snake_game(int fps);

//ends the snake game
void end_game(int score);