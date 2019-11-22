#include "graphics_driver.h"

 #define grass_color 0x7E0F

#include "fonts.h"

 
#include <sys/types.h>
#include <sys/wait.h>

static uint16_t graphic_memory;
static uint16_t* fbp;
static uint16_t reduced_resolution_graphics_array[24][24];
static uint16_t text_array[80][240];
static char     char_array[10][30];

static uint16_t* fbp;

bool get_bit(char c, int n){
	return c & 1 << n;	
}

void draw_pixel(int x, int y, uint16_t color){
	reduced_resolution_graphics_array[x][y] = color;
	update_screen(80+x*10,y*10,10,10);
}

void text_array_init()
{
	for (int y = 0; y < 30; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			char_array[x][y] = ' ';
		}
	}

	for (int x = 0; x < 80; x++)
	{
		for (int y = 0; y < 240; y++)
		{
			text_array[x][y] = 0x0000; 
		}
	}	
}

void print_char(int x, int y, char c)
{
	char_array[x][y] = c;	
}

void print_string(int x, int y, char* string)
{
	int i = 0;
	while(string[i]){
		print_char(x+i,y,string[i]);
		i++;
	}
	update_char_array();
	update_screen(0,0,80,240);
}

void update_char_array()
{
	int x_char = 0;
	int y_char = 0;
	char c;
	for (int y = 0; y < 240; y++)
	{
		for (int x = 0; x < 80; x++)
		{	
			x_char = x / 8;
			y_char = y / 8;
			c = char_array[x_char][y_char];



			if (get_bit(font8[c-32][x%8],y%8))
			{
				text_array[x][y] = 0xffff;
			}
			else
			{
				text_array[x][y] = 0x0000;
			}
			
		}
	}
}

void graphics_init()
{
	text_array_init();

	graphic_memory = open("/dev/fb0", O_RDWR);

    if (graphic_memory == -1)
    {  
        printf("ERROR: failed to open graphic memory\n");
    }

	int screensize = 320*240;
	int screensize_bytes = screensize / 8;

    fbp = (uint16_t*)mmap(NULL, screensize_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, graphic_memory, 0);
 	
	print_string(2,4,"Score:");

    printf("Init graphics");  


    uint16_t color = grass_color;
	for (int y = 0; y < 24; y++)
	{
		for (int x = 0; x < 24; x++)
		{
			draw_pixel(x,y, color); 
		}
	}

	print_string(1,14,"Welcome");
    print_string(1,15,"   To  ");
    print_string(1,16," Snake!");

    print_string(0,18,"Creators:");
    print_string(1,19,"Asgeir ");
    print_string(1,20,"Bendik ");
    print_string(1,21,"Marcus ");

    
}

void update_score(int score){

	char score_buffer[6]; 
    sprintf(score_buffer, "%d", score);

	for (int x = 0; x < 6; x++){
		if (score_buffer[x] >= '0' && score_buffer[x] <= '9')
		{
			print_char(x+2,6, score_buffer[x]); //*string++);
		}
		else
		{
			print_char(x+2,6, '-'); 
		}
	}
	update_screen(16,48,48,8); // Only update score pixels
}



void update_screen(int screen_x, int screen_y, int width, int height)
{
		int x_game_grid = (screen_x-80)/10;
		int y_game_grid = screen_y/10;
		int k = 320*screen_y + screen_x;

		for(int y = screen_y; y < screen_y+height; y++)
		{
			if (y % 10 == 0 && y != screen_y)
			{
				y_game_grid++;
			}			
			for (int x = screen_x; x < screen_x+width; x++)
			{
				if (x < 80)
				{ // score screen
					fbp[k++] = text_array[x][y];
				}
				else
				{ // Game screen
					if (x % 10 == 0 && x != screen_x)
					{
						x_game_grid++;
					}
					fbp[k++] = reduced_resolution_graphics_array[x_game_grid][y_game_grid];
				}
			}	
			k += 320 - width;		
			x_game_grid = (screen_x-80)/10;		
	}
	refresh_framebuffer(screen_x, screen_y, width, height);		
}





void refresh_framebuffer(int x, int y, int width, int height)
{
	struct fb_copyarea rect;
	rect.dx = x;
	rect.dy = y;
	rect.width = width; // 320
	rect.height = height; //240
    ioctl(graphic_memory, 0x4680, &rect);
}