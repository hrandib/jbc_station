/* ***********************************************************************
**
**  Copyright (C) 2005  Christian Kranz
**
**  Siemens S65 Display Control for LPH display
**
*********************************************************************** */
/*********************************************
* Chip type           : ATMEGA128
* Clock frequency     : clock 16 MHz
*********************************************/
#include <avr/io.h>
#include <inttypes.h>

#include "disp.h"


int main(void)
{
  uint8_t i;
  
  char txt[]="gruenes Display";

  port_init();

  lcd_init_c();
  backcolor=0x0780;
  textcolor=0x0000;
  fill_screen(0x0780);

  i=0;
  while (txt[i]!=0)
  {
    put_char(30+i*CHAR_W,60,txt[i],0);   //  0 deg. rotated
    put_char(5+i*CHAR_W,10,txt[i],1);   // 90 deg. rotated
    i++;
  }

  while (1) 
  {

  }
  return(0);	  
}

