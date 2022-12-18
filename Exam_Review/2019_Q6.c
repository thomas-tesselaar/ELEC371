#include "nios2_control.h"

#define TIMER_STATUS (volatile unsigned int *) 0x5000
#define TIMER_CONTROL (volatile unsigned int *) 0x5004
#define TIMER_START_LO (volatile unsigned int *) 0x5008
#define TIMER_START_HI (volatile unsigned int *) 0x500C
#define LEDS (volatile unsigned int *) 0x6000
#define INPORT_DATA (volatile unsigned int *) 0x6800
#define INPORT_STATUS (volatile unsigned int *) 0x6804
#define INPORT_CONTROL (volatile unsigned int *) 0x6808
#define JTAG_UART_DATA (volatile unsigned int *) 0x6A00
#define JTAG_UART_STATUS (volatile unsigned int *) 0x6A04

unsigned int timer_flag;
unsigned int inport_flag;
int inport_data;
int sum;

void Init() {
  *TIMER_STATUS = 1;
  *TIMER_CONTROL = 7;
  *TIMER_START_LO = 6250000 & 0xFFFF;
  *TIMER_START_HI = 6250000 >> 16;
  
  *LEDS = 1;
  sum = 0;
  timer_flag = 0;
  inport_flag = 0;
  
  NIOS2_WRITE_IENABLE(3);
  NIOS2_WRITE_STATUS(1);
}

void PrintChar(unsigned int ch) {
  unsigned int st;
  do {
  st = *JTAG_UART_STATUS;
  st = st & 0xFFFF0000;
  } while(st == 0);
  *JTAG_UART_DATA = ch;
}

void PrintHexDigit(unsigned int hex) {
  if (hex >= 10) {
    PrintChar(hex - 10 + 'A');
  } else { 
    PrintChar(hex + '0');}
}

void HandleTimer() {
  *TIMER_STATUS = 0
  if (*LEDS = 0x80) {
    timer_flag = 1;
    *LEDS = 1;
  } else {
    *LEDS = *LEDS << 1;}
}

void HandleInport() {
  *INPORT_STATUS = *INPORT_STATUS & 0xFFFFFFDF; // bit 5 0
  inport_data = *INPORT_DATA;
  inport_flag = 1;
}

void InterruptHandler() {
  unsigned int ipending = NIOS2_READ_IPENDING();
  
  if (ipending & 1) {
    HandleTimer();
  }
  if (ipending & 2) {
    HandleInport();
  }
}

int main() {
  int recent_notifs = 0;
  int total_notifs = 0;
  int average;
  
  Init();
  
  while(1) {
    if (timer_flag) {
      timer_flag = 0;
      PrintChar('\b');
      PrintChar('\b');
      PrintHexDigit(recent_notifs >> 4);
      PrintHexDigit(recent_notifs & 0xF);
      recent_notifs = 0;
    }
    if (inport_flag) {
      inport_flag = 0;
      recent_notifs++;
      total_notifs++;
      sum += inport_data;
      average = sum / total_notifs;
    }
  }
}

