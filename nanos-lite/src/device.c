#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  int key = _read_key();//from ioe.c
  if(key == _KEY_NONE){ // no key events return time event
    unsigned long t = _uptime();
    len = sprintf(buf, "t %d\n", t);
  } 
  else {// key events
    if(key > 0x8000) //key down events
      len = sprintf(buf, "kd %s\n",keyname[key-0x8000]);
    else //key up events
      len = sprintf(buf, "ku %s\n",keyname[key]);
  }
  return len-1;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  strncpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  
  len  >>= 2;  //bytes
  offset >>= 2;
  int y = offset / _screen.width;
  int x = offset % _screen.width;
  if(len <= _screen.width - x ) // data in one line
    _draw_rect((uint32_t *)buf, x, y, len, 1);
  else {// data in more than one line
    _draw_rect((uint32_t *)buf, x, y, _screen.width - x, 1);//draw first line
    len -= (_screen.width - x ); //sub the first line
    bool flag = false;
    int tmp_len = _screen.width - x;// buf's offset
    int tmp_y = 1;// counts of line has draw 
    if( len/_screen.width >= 1){ //fill many lines
      flag = true;
      _draw_rect(((uint32_t *)buf) + tmp_len, 0, y + tmp_y, _screen.width, len / _screen.width);
    }
    if(flag) {//has draw many lines
      tmp_len += len / _screen.width * _screen.width;
      tmp_y += len / _screen.width;
    }
    len = len % _screen.width;//draw last line
    _draw_rect(((uint32_t *)buf) + tmp_len, 0, y + tmp_y, len, 1);
  }
/*
offset /= 4;
  int y = offset / _screen.width;	
  int x = offset - _screen.width * y;
  len /= 4; 
  int lenA = len > _screen.width - x ? _screen.width - x : len;
  int lenB = len - lenA > 0 ? ( (len - lenA) / _screen.width > 1 ? (len - lenA) / _screen.width * _screen.width : 0 ) : 0;
  int lenC = len - lenA - lenB;

  //Log("fb_write x:%d y:%d len:%d lenA:%d lenB:%d lenC:%d\n", x, y, len, lenA, lenB, lenC);

  _draw_rect((uint32_t *)buf, x, y, lenA, 1);
  if (lenB)
	_draw_rect(((uint32_t *)buf) + lenA, 0, y + 1, _screen.width, lenB / _screen.width);
  if (lenC)
	_draw_rect(((uint32_t *)buf) + lenA + lenB, 0, y + lenB / _screen.width + 1, lenC, 1);*/

}

void init_device() {
  _ioe_init();
  strcpy(dispinfo ,"WIDTH:400\nHEIGHT:300");
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
