#include <stdio.h>
#include <stdlib.h>
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


/* 
 * Converts the active button input 8 bit register number
 * into a boolean array with SW1 mapped as b[0]
 */
void buttons_pressed(uint8_t input, int* buttons){
	input = ~input;
	for (int i = 0; i < NUMBUTTONS; i++){
		buttons[i] = (input >> i) & 0x01;
	}
}


int main(int argc, char *argv[])
{

	graphics_init();

	printf("Starting game \n");
    int descr = open("/dev/gamepad", O_RDONLY);


    uint8_t input = 0;
    int buttons[NUMBUTTONS];

	while(1){

	    
	    read( descr, &input, 1);
	    buttons_pressed(input, buttons);
	    
	    /*printf("Button array: {");
	    for(int i = 0; i < NUMBUTTONS; i++){
	    	printf("%d, ", buttons[i]);
	    }
	    printf("}\n");
		*/
	    


	   

	    
	}
	


	exit(EXIT_SUCCESS);
}


