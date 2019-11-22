#include "graphics_driver.h"

#define grass_color 0x7E0F
#define black_color 0x0000
#define white_color 0xffff



static uint16_t graphic_memory;
static uint16_t* fbp;
static uint16_t reduced_resolution_graphics_array[24][24]; // Reduced grid for playing snake. 
static uint16_t text_array[80][240]; // Part of screen to print text and game score
static char     char_array[10][30]; // Holds the characters to print

static uint16_t* fbp;



void draw_pixel(int x, int y, uint16_t color){
	reduced_resolution_graphics_array[x][y] = color;
	update_screen(80+x*10,y*10,10,10);
}

/* 
 Initializes the char_array to only hold blank values by default.
 The text_array is sett to black on all pixels by default. 
*/
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
			text_array[x][y] = black_color; 
		}
	}	
}


/*
	Prints a character to the screen 
*/
void print_string(int x, int y, char* string)
{
	int i = 0;
	while(string[i]){
		char_array[x+i][y] = string[i];
		i++;
	}
	update_text_array();
	update_screen(x*8,y*8,i*8,8);
}



bool get_bit(char c, int n)
{
	return c & 1 << n;	
}

/* 
	Updates the text array with correct pixels to print the characters from the character array
	Uses the font.h file to get the 8x8 pixel grid for the characters from the char_array and writes the character
	in the correct format to the text_array

 */
void update_text_array()
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
				text_array[x][y] = white_color;
			}
			else
			{
				text_array[x][y] = black_color;
			}
			
		}
	}
}

void initialize_screen_to_black() 
{
    int k = 0;
    for (int y = 0; y < 240; y++)
    {
    	for (int x = 0; x < 320; x++)
    	{
    		fbp[k++] = black_color;
    	}
    }
    refresh_framebuffer(0,0,320,240);
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

    initialize_screen_to_black();

  

 	
	print_string(2,4,"Score:");


	/* Print the grass for the snake game on the 24x24 grid */
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


    print_string(2,6,score_buffer);
}

/*
 	Dynamically updates the screen to only draw the newly updated pixels.
	screen_x (from 0 to 319) and screen_y (from 0 to 239) is the pixel to write from
	Width and height creates a rectangle which decides which parts of the screen to update. 
	THe function reads from either the text_array or the 24x24 reduced_resolution_graphics array
	depending on the rectangle to update. 
*/

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

/*
	Wwrites from the graphics memory to the screen
*/
void refresh_framebuffer(int x, int y, int width, int height)
{
	struct fb_copyarea rect;
	rect.dx = x;
	rect.dy = y;
	rect.width = width; // 320
	rect.height = height; //240
    ioctl(graphic_memory, 0x4680, &rect);
}