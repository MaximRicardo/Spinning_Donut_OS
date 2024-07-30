#pragma once

#define TEXT_WIDTH 80
#define TEXT_HEIGHT 60

#define CHR_WIDTH 8     /*CHR_WIDTH (7) including the space between characters (1) equals 8 total pixels */
#define CHR_HEIGHT 8

/* Writes the cursor x and cursor y to *x and *y respectively */
void GetCursorPos(unsigned int* x, unsigned int* y);

/* Moves the text cursor to the specified coordinates */
void SetCursorPos(unsigned int x, unsigned int y);

int puts(char* str);

void printf(char* str, ...);
