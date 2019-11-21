#include <stdio.h>
#include <stdlib.h>
#include "snake.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "graphics_driver.h"

#define NUMBUTTONS 8







int main(int argc, char *argv[])
{
	graphics_init();	
	snake_game(8);
	exit(EXIT_SUCCESS);
}


