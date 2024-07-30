#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "Print.h"
#include "LowLevelIO.h"
#include "Chars.h"
#include "Screen.h"

unsigned int cursorX = 0, cursorY = 0;

/* Shifts every character on the screen up 1 row, characters on the bottom row get initialized to 0 */
static void ShiftScreenUp() {

    size_t y;
    for (y = 0; y < VGA_HEIGHT; y++) {

        size_t x;
        for (x = 0; x < VGA_WIDTH/8; x++) {
            char* currentAddress = (char*)((VGA_WIDTH/8) * y + x + VGA_ADDRESS);    /* Address of the character at (x, y) */
            if (y < VGA_HEIGHT-1) {
                char* rowBelowAddress = (char*)((size_t)currentAddress + (VGA_WIDTH/8)*CHR_HEIGHT); /* Address of the character at (x, y+1) */
                *currentAddress = *rowBelowAddress;
            }
            else {
                /* If this is the bottom row, just write a 0 to both bytes */
                *currentAddress = 0;
            }
        }

    }

}

/* Writes the cursor x and cursor y to *x and *y respectively */
void GetCursorPos(unsigned int* x, unsigned int* y) {

    *x = cursorX;
    *y = cursorY;

}

/* Moves the text cursor to the specified coordinates */
void SetCursorPos(unsigned int x, unsigned int y) {

    cursorX = x;
    cursorY = y;   

}


static void DrawCharacterBitmap(char c) {

    uint8_t* startAddress = (uint8_t*)(VGA_ADDRESS+(VGA_WIDTH/8)*(cursorY*CHR_HEIGHT)+cursorX);
    size_t i;

    /* Only print if it is actually a character */
    if (c < 32 || c > 126) return;

    for (i = 0; i < CHR_HEIGHT; i++) {
        *(uint8_t*)(startAddress+i*(640/8)) = chrs[c - ' '][i];     /* Load the current byte of the bitmap to the frame buffer */
    }

}

/* Prints a character to the screen

c   - character to print

*/
static void PrintCharacter(char c) {

    if (c != '\n') {
        /* If the character is not a new line, just print the character as usual */

        DrawCharacterBitmap(c);

        /* Move the cursor position accordingly */
        ++cursorX;
        if (cursorX >= TEXT_WIDTH) { /* If cursorX goes past the width of the screen, then start from the beginning of a new line */
            cursorX = 0;
            ++cursorY;
        }
        if (cursorY >= TEXT_HEIGHT) {    /* If cursorY goes past the height of the screen, then shift the screen up 1 row and keep cursorY at the bottom of the screen */
            cursorY = TEXT_HEIGHT-1;
            ShiftScreenUp();
        }
    }
    else {
        /* If the character is a new line, don't print anything and instead just change the cursor position to the beginning of the next line */
        cursorX = 0;
        ++cursorY;
        if (cursorY >= TEXT_HEIGHT) {    /* If cursorY goes past the height of the screen, then shift the screen up 1 row and keep cursorY at the bottom of the screen */
            cursorY = TEXT_HEIGHT-1;
            ShiftScreenUp();
        }
    }

}

/*

str - string to be printed
newLine - if true, print a new line after the string, else do not

*/
static int putsWithNewLineOption(char* str, bool newLine) {

    if (str == NULL) return 0;

    size_t i;   /* Index in the string */
    
    i = 0;
    while (str[i] != '\0') {    /* Loop until a null terminator is reached */
        PrintCharacter(str[i]);

        /* Move to the next character */
        ++i;
    }

    if (newLine) PrintCharacter('\n');

    return 0;

}

int puts(char* str) {

    return putsWithNewLineOption(str, true);

}

static void PrintDecimalUnsignedInt(unsigned int val) {

    /* Maximum number of digits an int can have is 10 */
    char str[11];
    char flippedStr[11];
    size_t i = 0;
    size_t j = 0;

    /* Program breaks if I don't do this */
    if (val < 10) {
        /* If val less than 10, just print val and return */
        PrintCharacter('0'+val);
        return;
    }

    while (val >= 10) {
        str[i] = (val % 10) + '0';
        
        val = val / 10;
        ++i;
    }

    /* Need to do it one extra time, to print the most significant digit */
    str[i] = (val % 10) + '0';

    /* Flip around the string, so it is the right way around */
    flippedStr[i+1] = '\0'; /* Null terminate the string before the index of the end of the number is lost */
    for (; j <= i; j++) {
        flippedStr[i] = str[j];
        flippedStr[j] = str[i];        

        --i;
    }

    putsWithNewLineOption(flippedStr, false);
    
}

static void PrintDecimalInt(int val) {

    if (val < 0) {
        PrintCharacter('-');    /* Print a negative symbol */
        val *= -1;  /* Set val to be positive before printing it */
    }

    /* Now val can be printed like an unsigned int */
    PrintDecimalUnsignedInt(val);

}

/*  Print an integer in hex format

val - Value to print
uppercase   - If true, print the number as uppercase hex, else print it as lowercase hex

*/
static void PrintHexInt(unsigned int val, bool upperCase) {

    /* Maximum number of digits an int can have is 10 */
    char str[9];
    char flippedStr[9];
    size_t i = 0;
    size_t j = 0;

    /* Program breaks if I don't do this */
    if (val < 10) {
        /* If val is below 10, just print the number and return */
        PrintCharacter('0'+val);
        return;
    }
    else if (val < 16) {
        /* If val is below 16, do the same thing but with hex digits */
        PrintCharacter('0'-10+val);
        return;
    }

    while (val >= 16) {
        uint8_t c = (val % 16);
        if (c <= 9) str[i] = c + '0';
        else {
            if (upperCase) str[i] = c + 'A' - 10;
            else str[i] = c + 'a' - 10;
        }
        
        val = val / 16;
        ++i;
    }

    /* Need to do it one extra time, to print the most significant digit */
    str[i] = (val % 16);
    if (str[i] <= 9) str[i] = str[i] + '0';
    else {
        if (upperCase) str[i] = str[i] + 'A' - 10;
        else str[i] = str[i] + 'a' - 10;
    }

    /* Flip around the string, so it is the right way around */
    flippedStr[i+1] = '\0'; /* Null terminate the string before the index of the end of the number is lost */
    for (; j <= i; j++) {
        flippedStr[i] = str[j];
        flippedStr[j] = str[i];        

        --i;
    }

    putsWithNewLineOption(flippedStr, false);

}

void printf(char* str, ...) {

    size_t i;

    va_list argPtr;
    va_start(argPtr, str);

    if (str == NULL) {
        va_end(argPtr);
        return;
    }

    /* Read the string */
    for (i = 0; str[i] != '\0'; i++) {

        if (str[i] == '%') {
            ++i;    /* The '%' symbol can safely be skipped */

            if (str[i] == 's') {
                /* Print the string using puts without a new line */
                putsWithNewLineOption(va_arg(argPtr, char*), false);
            }
            else if (str[i] == 'c') {
                PrintCharacter(va_arg(argPtr, int));    /* char gets promoted to int */
            }
            else if (str[i] == 'u') {
                /* Print an unsigned integer converted to a string */
                PrintDecimalUnsignedInt(va_arg(argPtr, unsigned int));
            }
            else if (str[i] == 'd') {
                /* Print an integer converted to a string */
                PrintDecimalInt(va_arg(argPtr, int));
            }
            else if (str[i] == 'x') {
                /* Print an integer converted to a lowercase hex string */
                PrintHexInt(va_arg(argPtr, unsigned int), false);
            }
            else if (str[i] == 'X') {
                /* Print an integer converted to an uppercase hex string */
                PrintHexInt(va_arg(argPtr, unsigned int), true);
            }
            else if (str[i] == 'z' && str[i+1] == 'u') {
                /* Assume size_t to be the same width as int. If so, then size_t can be printed like a regular unsigned int */
                PrintDecimalUnsignedInt(va_arg(argPtr, size_t));
                /* Skip past the 'z' character, so the 'u' character can be skipped aswell when the loop continues */
                ++i;
            }
            else if (str[i] == 'z' && str[i+1] == 'd') {
                /* Assume size_t to be the same width as int. If so, then size_t can be printed like a regular int */
                PrintDecimalInt((int)va_arg(argPtr, size_t));
                /* Skip past the 'z' character, so the 'u' character can be skipped aswell when the loop continues */
                ++i;
            }
            else if (str[i] == 'z' && str[i+1] == 'x') {
                /* Assume size_t to be the same width as int. If so, then size_t can be printed like a regular int */
                PrintHexInt(va_arg(argPtr, size_t), false);
                /* Skip past the 'z' character, so the 'u' character can be skipped aswell when the loop continues */
                ++i;
            }
            else if (str[i] == 'z' && str[i+1] == 'X') {
                /* Assume size_t to be the same width as int. If so, then size_t can be printed like a regular int */
                PrintHexInt(va_arg(argPtr, size_t), true);
                /* Skip past the 'z' character, so the 'u' character can be skipped aswell when the loop continues */
                ++i;
            }
            else if (str[i] == 'p') {
                /* Due to the flat memory model of the OS, a pointer is always 32 bits. So, assuming int is 32 bits, a pointer can be printed like a regular unsigned int */
                PrintHexInt((unsigned int)va_arg(argPtr, void*), true);
            }
            else if (str[i] == '%') {
                /* Print a percent symbol */
                PrintCharacter('%');
            }
        }
        else {
            PrintCharacter(str[i]);
        }

    }
    
    va_end(argPtr);

}




