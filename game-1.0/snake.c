#include "snake.h"
#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include "graphics_driver.h"

#define SEC 1000000000
#define NUMBUTTONS 8
#define GRASS_COLOR 0x7e0f
#define SNAKE_COLOR 0x0000
#define APPLE_COLOR 0xf800

static uint32_t score = 0;
static square apple;
static Direction current_dir = Right;
static snake snek;
uint8_t input = 0;
int button_file = -1;

int buttons_pressed()
{
    if(button_file == -1)
    {
        button_file = open("/dev/gamepad", O_RDONLY);
    }
    read(button_file, &input, 1);
	input = ~input;
	for (int i = 0; i < NUMBUTTONS; i++){
		if ((input >> i) & 0x01){
			return i;
		}
	}
	return -1;
}

static void update_direction()
{
    if (buttons_pressed() != -1)
    {
        int button = buttons_pressed();
        switch(button)
        {
        case 0:
            if (current_dir != Right)
            {
                current_dir = Left;
            }
            break;
        case 1:
            if (current_dir != Down)
            {
                current_dir = Up;
            }
            break;
        case 2:
            if (current_dir != Left)
            {
                current_dir = Right;
            }
            break;
        case 3:
            if (current_dir != Up)
            {
                current_dir = Down;
            }
            break;
        case 4:
            if (current_dir != Right)
            {
                current_dir = Left;
            }
            break;
        case 5:
            if (current_dir != Down)
            {
                current_dir = Up;
            }
            break;
        case 6:
            if (current_dir != Left)
            {
                current_dir = Right;
            }
            break;
        case 7:
            if (current_dir != Up)
            {
                current_dir = Down;
            }
            break;
        default:
            break;
        }
    }
}




static void append_back(Direction dir)
{
    square *appendix = malloc(sizeof(square));
    appendix->x = snek.back->x;
    appendix->y = snek.back->y;
    switch (dir)
    {
    case Left:
        appendix->x--;
        break;
    case Right:
        appendix->x++;
        break;
    case Up:
        appendix->y--;
        break;
    case Down:
        appendix->y++;
        break;
    default:
        break;
    }
    snek.back->next = appendix;
    appendix->previous = snek.back;
    snek.back = appendix;
    draw_pixel(snek.back->x, snek.back->y, SNAKE_COLOR);
}

static void append_front(Direction dir)
{
    square *appendix = malloc(sizeof(square));
    appendix->x = snek.head->x;
    appendix->y = snek.head->y;
    switch (dir)
    {
    case Left:
        appendix->x--;
        break;
    case Right:
        appendix->x++;
        break;
    case Up:
        appendix->y--;
        break;
    case Down:
        appendix->y++;
        break;
    }
    snek.head->previous = appendix;
    appendix->next = snek.head;
    snek.head = appendix;
    draw_pixel(snek.head->x, snek.head->y, SNAKE_COLOR);
}


static void spawn_apple()
{
    srand(time(0));
    int x = rand() % 24;
    int y = rand() % 24;
    square *checkbox = snek.head;
    int valid = 1;
    while(checkbox != NULL)
    {
        if ((x == checkbox->x) && (y == checkbox->y))
        {
            valid = 0;
            spawn_apple();
        }
        checkbox = checkbox->next;
    }
    if(valid)
    {
        apple.x = x;
        apple.y = y;
        draw_pixel(x, y, APPLE_COLOR);
    }
}

static void pop_back()
{
    if((snek.back->y < 24) && (snek.back->y > -1) && (snek.back->x <24) && (snek.back->x > -1))
    {
        draw_pixel(snek.back->x, snek.back->y, GRASS_COLOR);
    }
    if(snek.back->previous==NULL)
    {
        free(snek.back);
        snek.back = NULL;
    }
    else
    {
        snek.back = snek.back->previous;
        free(snek.back->next);
        snek.back->next = NULL;
    }
}

void snake_move(Direction dir)
{
    pop_back();
    append_front(dir);
}




static void delete_snake()
{
    while (snek.back != NULL)
    {
        pop_back(snek);
    }
}

int update_game(Direction dir)
{
    int next_x, next_y;
    next_x = snek.head->x;
    next_y = snek.head->y;
    switch (dir)
    {
    case Left:
        next_x--;
        break;
    case Right:
        next_x++;
        break;
    case Up:
        next_y--;
        break;
    case Down:
        next_y++;
        break;
    }
    if ((next_x == apple.x) && (next_y == apple.y))
    {
        append_front(dir);
        spawn_apple();
        score += 500;
        update_score(score);
    }
    else
    {
        snake_move(dir);
    }
    if ((snek.head->x >= 24) || (snek.head->x < 0) || (snek.head->y >= 24) || (snek.head->y < 0))
    {
        return 1;
    }
    square *current = snek.head->next;
    while(current != NULL)
    {
        if ((snek.head->x == current->x) && (snek.head->y == current->y))
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}



static void snake_init()
{
    square *head = malloc(sizeof(square));
    head->x = 10;
    head->y = 11;
    snek.head = head;
    snek.back = head;
    draw_pixel(snek.back->x, snek.back->y, SNAKE_COLOR);
    append_back(Left);
    append_back(Left);
}

void end_game(int score)
{
    delete_snake();
}

int snake_game(int fps)
{
    struct timespec refresh_time = (struct timespec)
    {
        .tv_sec = 0,
        .tv_nsec = SEC/fps
    };
    snake_init();
    spawn_apple();
    struct timespec start_sleep = (struct timespec)
    {
        .tv_sec = 0,
        .tv_nsec = SEC/2
    };
    
    print_string(0, 0, "Starting:", 9);
    print_string(0, 1, "3", 1);
    while(nanosleep(&start_sleep, &start_sleep) && errno == EINTR);
    print_string(0, 1, "2", 1);
    while(nanosleep(&start_sleep, &start_sleep) && errno == EINTR);
    print_string(0, 1, "1", 1);
    while(nanosleep(&start_sleep, &start_sleep) && errno == EINTR);
    print_string(0, 1, " ", 1);
    print_string(0, 0, "         ", 9);
    //remove display counter
    while(1)
    {
        struct timespec start, finish;
        clock_gettime(CLOCK_REALTIME, &start);
        //Game update code below
        //=====================
        update_direction();
        if(update_game(current_dir))
        {
            end_game(score);
            return 1;
        }

        //=====================
        //Game update code above
        clock_gettime(CLOCK_REALTIME, &finish);
        long elapsed = finish.tv_nsec - start.tv_nsec;
        if(elapsed < refresh_time.tv_nsec)
        {
            struct timespec sleep_time = (struct timespec)
            {
                .tv_sec = 0,
                .tv_nsec = refresh_time.tv_nsec - elapsed
            };
            while(nanosleep(&sleep_time, &sleep_time) && errno == EINTR);
        }
    }
    return 0;
}