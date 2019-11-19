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

void graphics_init();
void refresh_framebuffer();


void map_reduced_to_full();