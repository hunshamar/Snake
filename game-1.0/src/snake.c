#include "../inc/snake.h"
#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <../graphics_driver.h>

#define SEC 1000000000


uint32_t score = 0;
square apple;
Direction current_dir;


int snake_game(int fps)
{
    struct timespec refresh_time = (struct timespec)
    {
        .tv_sec = 0,
        .tv_nsec = SEC/fps
    };
    snake snek;
    snake_init(*snek);
    spawn_apple(*snek);
    struct timespec start_sleep = (struct timespec)
    {
        .tv_sec = 0,
        .tv_nsec = SEC/2
    };
    

    while(nanosleep(&start_sleep, &start_sleep) && errno == EINTR);
    //set display counter 2
    while(nanosleep(&start_sleep, &start_sleep) && errno == EINTR);
    //set display counter 1
    while(nanosleep(&start_sleep, &start_sleep) && errno == EINTR);
    //remove display counter
    while(1)
    {
        struct timespec start, finish;
        clock_gettime(CLOCK_REALTIME, &start);
        //Game update code below
        //=====================
        /*
        current_dir = get_direction();
        int end_game = update_game(current_dir, snek);
        if(update_game(get_direction(), snek))
        {
            end_game(score);
            return 1;
        }

        */
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


/*
Bulk of calculations by game will probably be done by this function.
Unit Y-vector is swapped compared
*/
void snake_move(snake *self, Direction dir)
{
    square* new_head = malloc(sizeof(square));
    switch(dir)
    {
    case Up:
        new_head->x = self->head->x;
        new_head->y = self->head->y - 1;
        break;
    case Down:
        new_head->x = self->head->x;
        new_head->y = self->head->y + 1;
        break;
    case Left:
        new_head->x = self->head->x - 1;
        new_head->y = self->head->y;
        break;
    case Right:
        new_head->x = self->head->x + 1;
        new_head->y = self->head->y;
        break;
    }
    pop_back(self);
    append_front(self, new_head);
}



static void spawn_apple(snake* snek)
{
    srand(time(0));
    int x = rand() % 24;
    int y = rand() % 24;
    square *checkbox = snek->head;
    int valid = 1;
    while(checkbox != NULL)
    {
        if ((x == checkbox->x) && (y == checkbox->y))
        {
            valid = 0;
            spawn_apple(snek);
        }
    }
    if(valid)
    {
        apple.x = x;
        apple.y = y;
    }
}

static void pop_back(snake *snek)
{
    snek->back = snek->back.previous;
    free(snek->back.next);
    snek->back->next = NULL;
}

static void append_back(snake *snek, Direction dir)
{
    square *appendix = malloc(sizeof(square));
    switch (dir)
    {
    case Left:
        appendix->x = snek->back->x - 1;
        appendix->y = snek->back->y;
        break;
    case Right:
        appendix->x = snek->back->x + 1;
        appendix->y = snek->back->y;
        break;
    case Up:
        appendix->x = snek->back->x;
        appendix->y = snek->back->y - 1;
        break;
    case Down:
        appendix->x = snek->back->x;
        appendix->y = snek->back->y + 1;
        break;
    }
    snek->back->next = appendix;
    appendix->previous = snek->back;
    snek->back = appendix;
}

static void append_front(snake *snek, Direction dir)
{
    square *appendix = malloc(sizeof(square));
    switch (dir)
    {
    case Left:
        appendix->x = snek->back->x - 1;
        appendix->y = snek->back->y;
        break;
    case Right:
        appendix->x = snek->back->x + 1;
        appendix->y = snek->back->y;
        break;
    case Up:
        appendix->x = snek->back->x;
        appendix->y = snek->back->y - 1;
        break;
    case Down:
        appendix->x = snek->back->x;
        appendix->y = snek->back->y + 1;
        break;
    }
    snek->front->previous = appendix;
    appendix->next = snek->back;
    snek->front = appendix;
}

static void delete_snake(snake *snek)
{
    while (snek->back != NULL)
    {
        pop_back(snek);
    }
}

int update_game(Direction dir, snake *snek)
{
    int next_x, next_y;
    switch (dir)
    {
    case Left:
        next_x = snek->head->x - 1;
        next_y = snek->head->y;
        break;
    case Right:
        next_x = snek->head->x + 1;
        next_y = snek->head->y;
        break;
    case Up:
        next_x = snek->head->x;
        next_y = snek->head->y - 1;
        break;
    case Down:
        next_x = snek->head->x;
        next_y = snek->head->y + 1;
        break;
    }
    if ((next_x == apple.x) && (next_y == apple.y))
    {
        append_front(snek, dir);
        spawn_apple(snek);
        score += 500;
    }
    else
    {
        snake_move(snek, dir);
    }
    if ((snek->head->x >= 24) || (snek->head->x < 0) || (snek->head->y >= 24) || (snek->head->y < 0))
    {
        return 1;
    }
    square *current = snek->head;
    while(current != NULL)
    {
        if ((snek->head->x == current->x) || (snek->head->y == current->y))
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}



static void snake_init(snake *snek)
{
    square *head = malloc(sizeof(square));
    head->x = 10;
    head->y = 11;
    snek->head = head;
    snek->back = head;
    append_back(snek, Left);
    append_back(snek, Left);
}

static void end_game(int score)
{
    
}
