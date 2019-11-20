#include "graphics_driver.h"

 
#include "fonts.h"

 
#include <sys/types.h>
#include <sys/wait.h>

static uint16_t graphic_memory;
static uint16_t* fbp;
static uint16_t reduced_resolution_graphics_array[24][24];
static uint16_t score_array[80][240];
static char     char_array[10][30];

static uint16_t* fbp;


bool get_bit(char c, int n){
	return c & 1 << n;	
}

char* int_to_string(int score){ //Combines char with int for easy oled-printing

	printf("Converting the score: %d to a string \n", score);

    char string[5]; 
    sprintf(string, "%d", score);

    printf("The string is [%s]\n", string);

    return string;

}

void draw_pixel(int x, int y, uint16_t color){
	reduced_resolution_graphics_array[x][y] = color;
	update_screen();
	//refresh_framebuffer(90,10,100,100);
	refresh_reduced_grid(x,y,1,1);
}

void score_array_init(){

	int c = 'a';
	for (int y = 0; y < 30; y++){
		for (int x = 0; x < 10; x++){
			char_array[x][y] = ' ';
			if (c == 'z'){
				c = 'a';
			}

		}
	}

	for (int x = 0; x < 80; x++){
		for (int y = 0; y < 240; y++){
			score_array[x][y] = 0;
		}
	}	
}

void print_char(int x, int y, char c){
	char_array[x][y] = c;
	
}

void print_string(int x, int y, char* string, int strlength){
	printf("String printed: [%s] \n", string);

	for (int i = 0; i < strlength; i++){
		print_char(x+i,y,string[i]);

		//char_array[x+i][y] = string[i];
	}
	update_char_array();
	update_screen();
	refresh_text_display();
}

void update_char_array(){

	int x_char = 0;
	int y_char = 0;
	char c;
	for (int y = 0; y < 240; y++){
		for (int x = 0; x < 80; x++){
			
			x_char = x / 8;
			y_char = y / 8;
			c = char_array[x_char][y_char];



			if (get_bit(font8[c-32][x%8],y%8)){
				score_array[x][y] = 0xffff;
			}
			else{
				score_array[x][y] = 0x0000;
			}
			
		}
	}
}

void graphics_init(){
	score_array_init();

	

	graphic_memory = open("/dev/fb0", O_RDWR);
	/* legg til noe feil shit her */

    if (graphic_memory == -1)  //oko
        printf("Error: unable to open framebuffer device.\n");



	int screensize = 320*240;

	int screensize_bytes = screensize / 8;

    fbp = (uint16_t*)mmap(NULL, screensize_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, graphic_memory, 0);
 	if ((int)fbp == MAP_FAILED) {
        printf("Error: failed to memorymap framebuffer.\n");
        
    }

    

	print_string(2,2,"Hello.", 6);

	char* string = "Score:";
	for (int x = 0; x < 6; x++){
		print_char(x+2,4, string[x]); //*string++);
	}	



    printf("Init graphics");

    uint16_t color = 0xaaaa;
	for (int y = 0; y < 24; y++){
		for (int x = 0; x < 24; x++){
			reduced_resolution_graphics_array[x][y] = color;
			//printf("Writing to x,y = %d, %d the color %x \n", x,y,color);
			color = ~color;
		}
		color = ~color;
	}


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
    update_char_array();
	update_screen();
	refresh_framebuffer(0,0,320,240);
}

void update_score(int score){

	char score_buffer[5]; 
    sprintf(score_buffer, "%d", score);

	for (int x = 0; x < 5; x++){
		if (score_buffer[x] >= '0' && score_buffer[x] <= '9'){
			print_char(x+2,6, score_buffer[x]); //*string++);
		}
		else{
			print_char(x+2,6, '-'); //*string++);
		}
	}
	update_char_array();
	update_screen();
	refresh_score_display();
}

void update_screen(){




	

	




		

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
				fbp[k++] = score_array[j1][i];
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
	
}

void refresh_score_display(){
	refresh_framebuffer(16,48,40,8);
}

void refresh_reduced_grid(int x, int y, int width, int height){

	printf("Refreshing from x = %d to y = %d with w: %d h : %d \n", 80+x*10, y*10, width*10, height*10);
	refresh_framebuffer(80+x*10,y*10,width*10,height*10);	
}	

void refresh_text_display(){
	refresh_framebuffer(0,0,80,240);
}

void map_reduced_to_full(){
	int x = 0;
	int y = 0;
	int k = 0;
	for(int i = 0; i < 240; i++){
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



void refresh_framebuffer(int x, int y, int width, int height){
	struct fb_copyarea rect;
	rect.dx = x;
	rect.dy = y;
	rect.width = width; // 320
	rect.height = height; //240
    ioctl(graphic_memory, 0x4680, &rect);
}