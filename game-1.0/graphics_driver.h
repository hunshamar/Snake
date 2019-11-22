#ifndef GRAPHICS_H_
#define GRAPHICS_H

#include <stdint.h>
#include <sys/mman.h>
#include "fonts.h"
#include <sys/types.h>
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
void refresh_framebuffer(int x, int y, int width, int height);

void update_screen(int screen_x, int screen_y, int width, int height);
void print_string(int x, int y, char* string);

void refresh_reduced_grid(int x, int y, int width, int height);
void refresh_text_display();

void draw_pixel(int x, int y, uint16_t color);
void update_text_array();


void update_score(int score);


#endif