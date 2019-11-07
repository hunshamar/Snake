#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <errno.h>

#define SEC 1000000000

int snake_game(int fps)
{


    struct timespec refresh_time = (struct timespec)
    {
        .tv_sec = 0,
        .tv_nsec = SEC/fps
    };
    while(1)
    {
        struct timespec start, finish;
        clock_gettime(CLOCK_REALTIME, &start);
        /*
        =====================
        Game update code here
        =====================
        */
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


void end_game()
{
    
}