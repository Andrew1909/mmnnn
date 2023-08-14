#include <stdio.h>
//#include <conio.h>
//#include <dos.h>
#include <string.h>
#include <Arduino.h>

#include "ex.h"
#include "DISPLAYR.h"

int _rs_pin = 2;
int _rw_pin = 255;
int _enable_pin = 3;
int _row_offsets[4];
int _data_pins[4];
int _numlines = 4;

static void EXdisplay(int value, int mode);

DISPL_PARAM	DisplParam;

/***************************************************************************\
  <ConfigDisplay> DP-4x20 VF, нет курсора
\***************************************************************************/
void ConfigDisplay(void)
{

  _data_pins[0] = 4;
  _data_pins[1] = 5;
  _data_pins[2] = 6;
  _data_pins[3] = 7; 

  //set rows
  _row_offsets[0] = 0x00;
  _row_offsets[1] = 0x40;
  _row_offsets[2] = 0x00 + 20;
  _row_offsets[3] = 0x40 + 20;

  pinMode (_rs_pin, OUTPUT);
  if (_rw_pin != 255) { 
    pinMode (_rw_pin, OUTPUT);
  }
  pinMode (_enable_pin, OUTPUT);

  for (int i=0; i< 4; ++i)
  {
    pinMode (_data_pins[i], OUTPUT);
  } 

  delayMicroseconds(50000); 

  digitalWrite (_rs_pin, LOW);
  //digitalWrite (_enable_pin, LOW);
  if (_rw_pin != 255) { 
    digitalWrite (_rw_pin, LOW);
  }

  delayMicroseconds(60000); 

  write4bits (0b00110000);//function set
  delay(40);
   
  EXdisplay(0b00101000,LOW);  //function set 
  delay(40);

  EXdisplay(0b00101000,LOW);  //function set 
  delay(40);
   
  EXdisplay(0b00001100,LOW);//display on 
  delay(40);

  ClsDisplay();//clear 
  delay(40);
  EXdisplay(0b00000110 ,LOW);//mode set 

};

/***************************************************************************\
  <ClsDisplay>
\***************************************************************************/
void ClsDisplay(void)
{
  EXdisplay(0b00000001, LOW);
  delayMicroseconds(2000);  // this command takes a long time!
};

/***************************************************************************\
  <DisplayYX>
\***************************************************************************/
void DisplayYX(int y, int x, char *str)//x,y-установить курсор
{
    int i;
 const int max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
  if ( x >= max_lines ) {
    x = max_lines - 1;    // we count rows starting w/0
  }
  if ( x >= _numlines ) {
    x = _numlines - 1;    // we count rows starting w/0
  }
EXdisplay(LCD_SETDDRAMADDR | (y + _row_offsets[x]), LOW);

if (str == NULL) return 0;  //проверка на пустой указатель
  else{
    const uint8_t *buffer=(const uint8_t *)str;
    long size=strlen(str);
    while (size--) {//берёт указатель, размер. Отправляет на печать пока размер не = 0
      if (write(*buffer++));
      else break;
    }
  }
}

long write(int value) {
  EXdisplay(value, HIGH);
  return value; // assume sucess
}

static void EXdisplay(int value, int mode)
{
		//!!!!! 29.06.07 ConfigPIO();
	digitalWrite(_rs_pin, mode);

  // if there is a RW pin indicated, set it low to Write
  if (_rw_pin != 255) { 
    digitalWrite(_rw_pin, HIGH);
  }
    write4bits(value>>4);
    write4bits(value);
}

void write4bits(int value) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(_data_pins[i], (value >> i) & 0x01);
  }
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(1);    
  digitalWrite(_enable_pin, HIGH);
  delayMicroseconds(1);    // enable pulse must be >450ns
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(100);   // commands need > 37us to settle
}
