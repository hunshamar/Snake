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
int buttons_pressed(uint8_t input){
	input = ~input;
	for (int i = 0; i < NUMBUTTONS; i++){
		if ((input >> i) & 0x01){
			return i;
		}
	}
	return -1;
}




int main(int argc, char *argv[])
{

	graphics_init();

	printf("Starting game \n");
    int descr = open("/dev/gamepad", O_RDONLY);


    uint8_t input = 0;
    //uint8_t last_input = 255;

    int buttons[NUMBUTTONS];

    int score = 0;
    int direction = -1;	
    int last_direction = -1;

    int x = 1;
   	int y = 1;


	while(1){

	    
	    read( descr, &input, 1);
	    buttons_pressed(input, buttons);

	    //printf("Input = %d", input);
	    

	    update_score(score);

	    score+=10;


	    if (input != 255){
	    	direction = buttons_pressed(input, buttons);
	    	if (direction != last_direction){
	    		last_direction = direction;
		    	switch(direction){
		    	case 0:
			    	print_string(1,12,"Button: L", 9);
			    	break;
			    case 1:
			    	print_string(1,12,"Button: U", 9);
			    	break;
		    	case 2:
		    		print_string(1,12,"Button: R", 9);
		    		break;
		    	case 3:
		    		print_string(1,12,"Button: D", 9);
		    		break;
		    	}
	    	}
	    }








	    
	    if (score % 100 == 0 && score < 2100){

	    	draw_pixel(x,y,0x0000);
	    	
	    	
			if (score <= 1000){
	    		x++;
	    	}
	    	else{
	    		y++;
	    	}
	    }





	    /*printf("Button array: {");
	    for(int i = 0; i < NUMBUTTONS; i++){
	    	printf("%d, ", buttons[i]);
	    }
	    printf("}\n");
		*/
	    


	   

	    
	}
	


	exit(EXIT_SUCCESS);
}


