#include <am.h>
#include <x86.h>
#define RTC_PORT 0x48   // Note that this is not standard

#define I8042_DATA_PORT 0x60   
#define I8042_STATUS_PORT 0x64      
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  unsigned long time =inl(RTC_PORT);

  return time-boot_time ;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  /*int i;
  for (i = y*_screen.width+x; i < (y+h)* _screen.width +x+w; i++) {
   int index_y=i/_screen.width;
   int index_x=i%_screen.width;
    
  // if((index_x>=x&&index_y>=y&&index_x<=(w+x)&&index_y<=(y+h))) {
       int line =index_x-x;
       int row = index_y-y;
       
       memcpy(fb+i,pixels+row*w+line,4); 
	//fb[i] = pixels[i-x*y];
    }
  //  else  fb[i] = i;*/
 /* int i,j;
  for(i=0;i<h;i++ )
    for(j=0;j<w;j++){
     fb[(y+i)*_screen.width+j+x] = pixels[i*w+j];
    }	*/
  for(int i=0;i<h;i++)
  	  memcpy(fb+(y+i)*_screen.width+x,pixels+i*w,w*4);
}

void _draw_sync() {
}

int _read_key() {
  int status =inb(I8042_STATUS_PORT);
  int key =inl(I8042_DATA_PORT);
  if(status==0x1) return key;
  return _KEY_NONE;
}
