#include "graphics_driver.h"

void graphics_init(){
	uint16_t graphic_memory = open("/dev/fb0", O_RDWR);
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

    int k = 0;
	uint16_t color = 0xffff;
	for(int i = 0; i < 320; i++){
		for (int j = 0; j < 240; j++){
			fbp[k] = color;
			k++;
		}
	}

	struct fb_copyarea rect;
	rect.dx = 0;
	rect.dy = 0;
	rect.width = 320;
	rect.height = 240;


    ioctl(graphic_memory, 0x4680, &rect);

}

