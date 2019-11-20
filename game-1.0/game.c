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

    int score = 0;

    int x = 1;
   	int y = 1;


	while(1){

	    
	    read( descr, &input, 1);
	    buttons_pressed(input, buttons);
	    

	    update_score(score);

	    score+=10;

	    if (score == 100){
	    	print_string(1,12,"The Game", 8);
	    	print_string(1,13,"Hei", 3);
	    	print_string(1,14,"Korleis", 7);
	    	print_string(1,15,"gjenge", 6);
	    	print_string(1,16,"det?", 4);
	    	

	    	//refresh_framebuffer(0,0,320,240);	
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


