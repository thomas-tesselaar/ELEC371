/* for standalone testing of this file by itself using the simulator,
   keep the following line, but for in-lab activity with the Monitor Program
   to have a multi-file project, comment out the following line */

//#define TEST_CHARIO


/* no #include statements should be required, as the character I/O functions
   do not rely on any other code or definitions (the .h file for these
   functions would be included in _other_ .c files) */


/* because all character-I/O code is in this file, the #define statements
   for the JTAG UART pointers can be placed here; they should not be needed
   in any other file */

#define JTAG_UART_DATA (volatile unsigned int *) 0x10001000
#define JTAG_UART_STATUS (volatile unsigned int *) 0x10001004


/* place the full function definitions for the character-I/O routines here */

void PrintChar(unsigned int ch) {
    unsigned int st;
    
    do
    {
        st = *JTAG_UART_STATUS;
        st = st & 0xFFFF0000;
    } while(st==0);
    
    *JTAG_UART_DATA = ch;
}

void PrintString(char *s) {
    char ch;
    while(1) {
        ch = *s;
        if(ch=='\0') {
            break;
        } else {
            PrintChar((unsigned int) ch);
            s++;
        }
    }
}

void PrintHexDigit(unsigned int hex) {
    if (hex >= 10) {
        hex = hex - 10 + 'A';
    } else {
        hex = hex + '0';
    }
    PrintChar(hex);
}

unsigned int GetChar(){
    unsigned int info, st;
    do{
        info = *JTAG_UART_DATA;
        st = info & 0x8000;
    } while(st==0);
    return info & 0xFF;
}

unsigned int CheckChar(){
    unsigned int info;
    info = *JTAG_UART_DATA;
    if(info&0x8000)
        return info & 0xFF;
    else
        return 0;
}



#ifdef TEST_CHARIO

/* this portion is conditionally compiled based on whether or not
   the symbol exists; it is only for standalone testing of the routines
   using the simulator; there is a main() routine in lab4.c, so
   for the in-lab activity, the following code would conflict with it */

int main (void)
{

  /* place calls here to the various character-I/O routines
     to test their behavior, e.g., PrintString("hello\n");  */

  return 0;
} 

#endif /* TEST_CHARIO */
