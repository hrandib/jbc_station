/* ***********************************************************************
**
**  Copyright (C) 2005  Christian Kranz
**
**  Siemens S65 Display Control
**
*********************************************************************** */
// disp.h
#ifndef _DISP_H
#define _DISP_H

#include "avr/io.h"

#define DISP_W 132
#define DISP_H 176
#define CHAR_H 14
#define CHAR_W 8
#define TEXT_COL 16
#define TEXT_ROW 12


extern uint16_t textcolor;  // color of border around text
extern uint16_t backcolor;    // color of background



/////////////////////////////// function prototypes ////////////////////////

void port_init(void);
void lcd_write(uint8_t);

void lcd_cspulse(void);
void lcd_comtype(uint8_t dat1);
void lcd_comdat(uint8_t dat1, uint8_t dat2);

void lcd_wrdat16(uint16_t);
void lcd_init_c(void);
void fill_screen(uint16_t color);
void put_char(uint8_t x, uint8_t y, char c, uint8_t rot);

#endif
// end of file disp.h
