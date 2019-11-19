#include "graphics_driver.h"

 
#include "fonts.h"

 
#include <sys/types.h>
#include <sys/wait.h>

static uint16_t graphic_memory;
static uint16_t* fbp;
static uint16_t reduced_resolution_graphics_array[24][24];
static uint16_t score_array[80][240];


bool get_bit(char c, int n){
	return c & 1 << n;	
}

void graphics_init(){

	printf("Printing the letter %c \n", 24 + 32);
	for (int x = 0; x < 8; x++){
		for (int y = 0; y < 8; y++){
			printf("%d",get_bit(font8[24][y],x));
		}
		printf("\n");
	}
	printf("Printing the letter %c \n", 23 + 32);
	for (int x = 0; x < 8; x++){
		for (int y = 0; y < 8; y++){
			printf("%d",get_bit(font8[23][y],x));
		}
		printf("\n");
	}
	printf("Printing the letter %c \n", 22 + 32);
	for (int x = 0; x < 8; x++){
		for (int y = 0; y < 8; y++){
			printf("%d",get_bit(font8[22][y],x));
		}
		printf("\n");
	}

	graphic_memory = open("/dev/fb0", O_RDWR);
	/* legg til noe feil shit her */

    if (graphic_memory == -1)  //oko
        printf("Error: unable to open framebuffer device.\n");


	uint16_t* fbp;

	int screensize = 320*240;

	int screensize_bytes = screensize / 8;

    fbp = (uint16_t*)mmap(NULL, screensize_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, graphic_memory, 0);
 	if ((int)fbp == MAP_FAILED) {
        printf("Error: failed to memorymap framebuffer.\n");
        
    }


    printf("Init graphics");

    /*int k = 0;
	uint16_t color = 0x0011;
	for(int i = 0; i < 240; i++){
		//printf("bf sleep \n")	;
		//sleep(1);
		//printf("af sleep \n");
		color*=2;
		refresh_framebuffer();
		for (int j = 0; j < 320; j++){
			fbp[k] = color;
			k++;
		}
	}*/

	uint16_t color = 0xffff;
	for (int y = 0; y < 24; y++){
		for (int x = 0; x < 24; x++){
			reduced_resolution_graphics_array[x][y] = color;
			//printf("Writing to x,y = %d, %d the color %x \n", x,y,color);
			color = ~color;
		}
		color = ~color;
	}


	

	//map_reduced_to_full();
	int x = 0;
	int y = 0;
	int k = 0;
	for(int i = 0; i < 240; i++){
		if (i % 10 == 0 && i){
			y++;
			//printf("Y is %d\n", x)
		}
		//printf("I to 240, now: %d \n", i);
		for (int j1 = 0; j1 < 80; j1++){
			fbp[k] = 0x0011;
			k++;
		}
		for (int j2 = 0; j2 < 240; j2++){
			if (j2 % 10 == 0 && j2){
				x++;
				//printf("X is %d\n",x);
			}
			//printf("writing to screen\n");
			fbp[k] = reduced_resolution_graphics_array[x][y];
			//printf("Writin x,y = %d, %d", x,y);
			k++;
			
			
		}
		x = 0;
		
	}
	refresh_framebuffer();
}

void map_reduced_to_full(){
	int x = 0;
	int y = 0;
	int k = 0;
	for(int i = 0; i < 240; i++){
		refresh_framebuffer();
		for (int j = 0; j < 320; j++){
			printf("writing to screen\n");
			fbp[k] = 0x0011;//reduced_resolution_graphics_array[x][y];
			k++;
			
			if (j % 10 == 0){
				x++;
			}
		}
		if (i % 10 == 0){
			y++;
		}
	}
}

void refresh_framebuffer(){
	struct fb_copyarea rect;
	rect.dx = 0;
	rect.dy = 0;
	rect.width = 320;
	rect.height = 240;
    ioctl(graphic_memory, 0x4680, &rect);
}