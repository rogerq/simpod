/*
 * simpod.ino
 *
 * SBUS to USB-HID adapter pod.
 *
 * Read values from SBUS receiver and pushes to USB HID (Joystic) interface.
 *
 * Roger Quadros <rogerquads@gmail.com>
 * 4th March, 2018
 * Licence: GPLv2
 */

#include "SBUS.h"

/* receiver is on Serial port 1 */
SBUS sbus_rx(Serial1);

#define NCH 16
uint16_t ch[NCH];
uint8_t fail_safe;
uint16_t lost_frames = 0;

uint16_t chc[NCH]; /* channels cache */
uint8_t fsc;  /* fail safe cache */
uint16_t lfc;  /* lost frames cache */

void myprintf(const char *format, ...) {
  #define BUFSZ 100
  char buf[BUFSZ];
  va_list arg_ptr;

  va_start(arg_ptr, format);
  vsnprintf(buf, BUFSZ, format, arg_ptr);
  va_end(arg_ptr);

  for(char *p = &buf[0]; *p; p++)
  {
    // emulate cooked mode for newlines
    if(*p == '\n')
      Serial.write('\r');
    Serial.write(*p);
  }
}

void setup() {
  /* UART for debug messages */
  Serial.begin(115200);
  myprintf("sbus test\n");

  sbus_rx.begin();
  myprintf("sbus initialized\n");
}

void loop() {
  int i;

  /* get a SBUS packet from receiver */
  if(sbus_rx.read(&ch[0], &fail_safe, &lost_frames)) {
    /* check if anything changed and dump to UART */
    if (memcmp(ch, chc, NCH * sizeof(ch[0])) || fail_safe != fsc || lost_frames != lfc) {
      myprintf("F:%d L:%-5d ", fail_safe, lost_frames);
      for (i = 0; i < 8; i++) {
        myprintf("CH%d:%-5d ", i,ch[i]);
      }
      myprintf("\n");
    }

    /* update channels cache */
    memcpy(chc, ch, NCH * sizeof(ch[0]));
    fsc = fail_safe;
    lfc = lost_frames;
  }
}

