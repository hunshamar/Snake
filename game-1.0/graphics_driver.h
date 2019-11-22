#include <stdint.h>
#include <sys/mman.h>





#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#include <string.h>

void graphics_init();
void refresh_framebuffer();

void update_screen(int screen_x, int screen_y, int width, int height);
void map_reduced_to_full();
void print_string(int x, int y, char* string);



void refresh_score_display();
void refresh_reduced_grid(int x, int y, int width, int height);
void refresh_text_display();

void draw_pixel(int x, int y, uint16_t color);
void update_char_array();


void update_score(int score);
