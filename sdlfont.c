/*
MIT License

Copyright (c) Ivan Svarkovsky - 2025	<https://github.com/Svarkovsky>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 * sdlfont.c - Implementation of font management for SDLGFX.
 * Provides procedural generation of an 8x10 bitmap font and rendering functions.
 */

#include <stdio.h>
#include <string.h>
#include "sdlfont.h"
#include "sdlgfx.h" // Для доступа к sdlgfx_color и sdlgfx_point

void sdlfont_init(void) {
    // Инициализация шрифта (если потребуется, можно добавить настройки здесь)
    // Пока оставляем пустой
}

void sdlfont_generate_char_bitmap(FontBitmap bitmap, char c) {
    // Очищаем битовую карту (все пиксели выключены)
    for (int row = 0; row < FONT_HEIGHT; row++) {
        bitmap[row] = 0x00;
    }

    // Генерируем символы в зависимости от их ASCII-кода (ASCII 32–126)
    if (c < ' ' || c > '~') c = '?'; // Замена неподдерживаемых символов на '?'

    switch (c) {
        case ' ': // ASCII 32
            break;

        case '!': // ASCII 33
            bitmap[2] = 0x18; // 00011000
            bitmap[3] = 0x18;
            bitmap[4] = 0x18;
            bitmap[5] = 0x18;
            bitmap[7] = 0x18; // Точка внизу
            break;

        case '"': // ASCII 34
            bitmap[2] = 0x24; // 00100100
            bitmap[3] = 0x24;
            break;

        case '#': // ASCII 35
            bitmap[3] = 0x24; // 00100100
            bitmap[4] = 0xFF; // 11111111
            bitmap[5] = 0x24;
            bitmap[6] = 0xFF;
            bitmap[7] = 0x24;
            break;

        case '$': // ASCII 36
            bitmap[2] = 0x3E; // 00111110
            bitmap[3] = 0x40; // 01000000
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x02; // 00000010
            bitmap[6] = 0x7C; // 01111100
            bitmap[7] = 0x18; // 00011000
            break;

        case '%': // ASCII 37
            bitmap[2] = 0x62; // 01100010
            bitmap[3] = 0x64; // 01100100
            bitmap[4] = 0x08; // 00001000
            bitmap[5] = 0x10; // 00010000
            bitmap[6] = 0x26; // 00100110
            bitmap[7] = 0x46; // 01000110
            break;

        case '&': // ASCII 38
            bitmap[2] = 0x1C; // 00011100
            bitmap[3] = 0x22; // 00100010
            bitmap[4] = 0x1C; // 00011100
            bitmap[5] = 0x28; // 00101000
            bitmap[6] = 0x44; // 01000100
            bitmap[7] = 0x3A; // 00111010
            break;

        case '\'': // ASCII 39
            bitmap[2] = 0x18; // 00011000
            bitmap[3] = 0x18;
            break;

        case '(': // ASCII 40
            bitmap[1] = 0x18; // 00011000
            bitmap[2] = 0x30; // 00110000
            bitmap[3] = 0x60; // 01100000
            bitmap[4] = 0x60; // 01100000
            bitmap[5] = 0x60; // 01100000
            bitmap[6] = 0x30; // 00110000
            bitmap[7] = 0x18; // 00011000
            break;

        case ')': // ASCII 41
            bitmap[1] = 0x30; // 00110000
            bitmap[2] = 0x18; // 00011000
            bitmap[3] = 0x0C; // 00001100
            bitmap[4] = 0x0C; // 00001100
            bitmap[5] = 0x0C; // 00001100
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x30; // 00110000
            break;

        case '*': // ASCII 42
            bitmap[3] = 0x2A; // 00101010
            bitmap[4] = 0x1C; // 00011100
            bitmap[5] = 0x2A; // 00101010
            break;

        case '+': // ASCII 43
            bitmap[4] = 0x18; // 00011000
            bitmap[5] = 0x7E; // 01111110
            bitmap[6] = 0x18; // 00011000
            break;

        case ',': // ASCII 44
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x30; // 00110000
            break;

        case '-': // ASCII 45
            bitmap[5] = 0x7E; // 01111110
            break;

        case '.': // ASCII 46
            bitmap[7] = 0x18; // 00011000
            break;

        case '/': // ASCII 47
            bitmap[2] = 0x06; // 00000110
            bitmap[3] = 0x0C; // 00001100
            bitmap[4] = 0x18; // 00011000
            bitmap[5] = 0x30; // 00110000
            bitmap[6] = 0x60; // 01100000
            break;

        // Цифры
        case '0': // ASCII 48
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x46; // 01000110
            bitmap[5] = 0x4A; // 01001010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case '1': // ASCII 49
            bitmap[2] = 0x18; // 00011000
            bitmap[3] = 0x38; // 00111000
            bitmap[4] = 0x18; // 00011000
            bitmap[5] = 0x18; // 00011000
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x7E; // 01111110
            break;

        case '2': // ASCII 50
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x06; // 00000110
            bitmap[5] = 0x1C; // 00011100
            bitmap[6] = 0x30; // 00110000
            bitmap[7] = 0x7E; // 01111110
            break;

        case '3': // ASCII 51
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x1C; // 00011100
            bitmap[5] = 0x06; // 00000110
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case '4': // ASCII 52
            bitmap[2] = 0x0C; // 00001100
            bitmap[3] = 0x1C; // 00011100
            bitmap[4] = 0x2C; // 00101100
            bitmap[5] = 0x4C; // 01001100
            bitmap[6] = 0x7E; // 01111110
            bitmap[7] = 0x0C; // 00001100
            break;

        case '5': // ASCII 53
            bitmap[2] = 0x7E; // 01111110
            bitmap[3] = 0x40; // 01000000
            bitmap[4] = 0x7C; // 01111100
            bitmap[5] = 0x02; // 00000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case '6': // ASCII 54
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x40; // 01000000
            bitmap[4] = 0x7C; // 01111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case '7': // ASCII 55
            bitmap[2] = 0x7E; // 01111110
            bitmap[3] = 0x02; // 00000010
            bitmap[4] = 0x04; // 00000100
            bitmap[5] = 0x08; // 00001000
            bitmap[6] = 0x10; // 00010000
            bitmap[7] = 0x10; // 00010000
            break;

        case '8': // ASCII 56
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case '9': // ASCII 57
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x3E; // 00111110
            bitmap[6] = 0x02; // 00000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case ':': // ASCII 58
            bitmap[3] = 0x18; // 00011000
            bitmap[6] = 0x18; // 00011000
            break;

        case ';': // ASCII 59
            bitmap[3] = 0x18; // 00011000
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x30; // 00110000
            break;

        case '<': // ASCII 60
	    bitmap[0] = 0x00; // 00000000
	    bitmap[1] = 0x04; // 00000100
	    bitmap[2] = 0x0C; // 00001100
    	    bitmap[3] = 0x1C; // 00011100
	    bitmap[4] = 0x3C; // 00111100
 	    bitmap[5] = 0x3C; // 00111100
 	    bitmap[6] = 0x1C; // 00011100
            bitmap[7] = 0x0C; // 00001100
            bitmap[8] = 0x04; // 00000100
            bitmap[9] = 0x00; // 00000000
	    break;

        case '=': // ASCII 61
            bitmap[4] = 0x7E; // 01111110
            bitmap[6] = 0x7E; // 01111110
            break;

	case '>': // ASCII 62
	    bitmap[0] = 0x00; // 00000000
	    bitmap[1] = 0x20; // 00100000
	    bitmap[2] = 0x30; // 00110000
	    bitmap[3] = 0x38; // 00111000
	    bitmap[4] = 0x3C; // 00111100
	    bitmap[5] = 0x3C; // 00111100
	    bitmap[6] = 0x38; // 00111000
	    bitmap[7] = 0x30; // 00110000
	    bitmap[8] = 0x20; // 00100000
	    bitmap[9] = 0x00; // 00000000
	    break;

        case '?': // ASCII 63
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x04; // 00000100
            bitmap[5] = 0x08; // 00001000
            bitmap[7] = 0x08; // 00001000
            break;

        case '@': // ASCII 64
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x5A; // 01011010
            bitmap[5] = 0x5A; // 01011010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        // Буквы A-Z
        case 'A': // ASCII 65
            bitmap[2] = 0x18; // 00011000
            bitmap[3] = 0x24; // 00100100
            bitmap[4] = 0x24; // 00100100
            bitmap[5] = 0x7E; // 01111110
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x42; // 01000010
            break;

        case 'B': // ASCII 66
            bitmap[2] = 0x7C; // 01111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x7C; // 01111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x7C; // 01111100
            break;

        case 'C': // ASCII 67
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x40; // 01000000
            bitmap[5] = 0x40; // 01000000
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case 'D': // ASCII 68
            bitmap[2] = 0x7C; // 01111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x7C; // 01111100
            break;

        case 'E': // ASCII 69
            bitmap[2] = 0x7E; // 01111110
            bitmap[3] = 0x40; // 01000000
            bitmap[4] = 0x78; // 01111000
            bitmap[5] = 0x40; // 01000000
            bitmap[6] = 0x40; // 01000000
            bitmap[7] = 0x7E; // 01111110
            break;

        case 'F': // ASCII 70
            bitmap[2] = 0x7E; // 01111110
            bitmap[3] = 0x40; // 01000000
            bitmap[4] = 0x78; // 01111000
            bitmap[5] = 0x40; // 01000000
            bitmap[6] = 0x40; // 01000000
            bitmap[7] = 0x40; // 01000000
            break;

        case 'G': // ASCII 71
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x40; // 01000000
            bitmap[5] = 0x4E; // 01001110
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case 'H': // ASCII 72
            bitmap[2] = 0x42; // 01000010
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x7E; // 01111110
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x42; // 01000010
            break;

        case 'I': // ASCII 73
            bitmap[2] = 0x7E; // 01111110
            bitmap[3] = 0x18; // 00011000
            bitmap[4] = 0x18; // 00011000
            bitmap[5] = 0x18; // 00011000
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x7E; // 01111110
            break;

        case 'J': // ASCII 74
            bitmap[2] = 0x7E; // 01111110
            bitmap[3] = 0x06; // 00000110
            bitmap[4] = 0x06; // 00000110
            bitmap[5] = 0x06; // 00000110
            bitmap[6] = 0x66; // 01100110
            bitmap[7] = 0x3C; // 00111100
            break;

        case 'K': // ASCII 75
            bitmap[2] = 0x42; // 01000010
            bitmap[3] = 0x44; // 01000100
            bitmap[4] = 0x78; // 01111000
            bitmap[5] = 0x48; // 01001000
            bitmap[6] = 0x44; // 01000100
            bitmap[7] = 0x42; // 01000010
            break;

        case 'L': // ASCII 76
            bitmap[2] = 0x40; // 01000000
            bitmap[3] = 0x40; // 01000000
            bitmap[4] = 0x40; // 01000000
            bitmap[5] = 0x40; // 01000000
            bitmap[6] = 0x40; // 01000000
            bitmap[7] = 0x7E; // 01111110
            break;

        case 'M': // ASCII 77
            bitmap[2] = 0x42; // 01000010
            bitmap[3] = 0x7E; // 01111110
            bitmap[4] = 0x5A; // 01011010
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x42; // 01000010
            break;

        case 'N': // ASCII 78
            bitmap[2] = 0x42; // 01000010
            bitmap[3] = 0x62; // 01100010
            bitmap[4] = 0x52; // 01010010
            bitmap[5] = 0x4A; // 01001010
            bitmap[6] = 0x46; // 01000110
            bitmap[7] = 0x42; // 01000010
            break;

        case 'O': // ASCII 79
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case 'P': // ASCII 80
            bitmap[2] = 0x7C; // 01111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x7C; // 01111100
            bitmap[6] = 0x40; // 01000000
            bitmap[7] = 0x40; // 01000000
            break;

        case 'Q': // ASCII 81
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x4A; // 01001010
            bitmap[7] = 0x3C; // 00111100
            break;

        case 'R': // ASCII 82
            bitmap[2] = 0x7C; // 01111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x7C; // 01111100
            bitmap[6] = 0x48; // 01001000
            bitmap[7] = 0x46; // 01000110
            break;

        case 'S': // ASCII 83
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x02; // 00000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case 'T': // ASCII 84
            bitmap[2] = 0x7E; // 01111110
            bitmap[3] = 0x18; // 00011000
            bitmap[4] = 0x18; // 00011000
            bitmap[5] = 0x18; // 00011000
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x18; // 00011000
            break;

        case 'U': // ASCII 85
            bitmap[2] = 0x42; // 01000010
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case 'V': // ASCII 86
            bitmap[2] = 0x42; // 01000010
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x24; // 00100100
            bitmap[6] = 0x24; // 00100100
            bitmap[7] = 0x18; // 00011000
            break;

        case 'W': // ASCII 87
            bitmap[2] = 0x42; // 01000010
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x5A; // 01011010
            bitmap[5] = 0x5A; // 01011010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x42; // 01000010
            break;

        case 'X': // ASCII 88
            bitmap[2] = 0x42; // 01000010
            bitmap[3] = 0x24; // 00100100
            bitmap[4] = 0x18; // 00011000
            bitmap[5] = 0x18; // 00011000
            bitmap[6] = 0x24; // 00100100
            bitmap[7] = 0x42; // 01000010
            break;

        case 'Y': // ASCII 89
            bitmap[2] = 0x42; // 01000010
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x24; // 00100100
            bitmap[5] = 0x18; // 00011000
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x18; // 00011000
            break;

        case 'Z': // ASCII 90
            bitmap[2] = 0x7E; // 01111110
            bitmap[3] = 0x04; // 00000100
            bitmap[4] = 0x08; // 00001000
            bitmap[5] = 0x10; // 00010000
            bitmap[6] = 0x20; // 00100000
            bitmap[7] = 0x7E; // 01111110
            break;

	    case '^': // ASCII 94
	        bitmap[2] = 0x18; // 00011000
	        bitmap[3] = 0x24; // 00100100
	        bitmap[4] = 0x42; // 01000010
	        break;


        // Буквы a-z (строчные)
        case 'a': // ASCII 97
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x7E; // 01111110
            bitmap[7] = 0x42; // 01000010
            break;

        case 'b': // ASCII 98
            bitmap[2] = 0x40; // 01000000
            bitmap[3] = 0x40; // 01000000
            bitmap[4] = 0x7C; // 01111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x7C; // 01111100
            break;

        case 'c': // ASCII 99
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x40; // 01000000
            bitmap[7] = 0x3C; // 00111100
            break;

        case 'd': // ASCII 100
            bitmap[2] = 0x04; // 00000100
            bitmap[3] = 0x04; // 00000100
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x44; // 01000100
            bitmap[6] = 0x44; // 01000100
            bitmap[7] = 0x3C; // 00111100
            break;

	    case 'e': // ASCII 101
	        bitmap[4] = 0x38; // 00111000
            bitmap[5] = 0x7C; // 01111100
            bitmap[6] = 0x40; // 01000000
            bitmap[7] = 0x38; // 00111000
            break;

        case 'f': // ASCII 102
            bitmap[2] = 0x18; // 00011000
            bitmap[3] = 0x20; // 00100000
            bitmap[4] = 0x7C; // 01111100
            bitmap[5] = 0x20; // 00100000
            bitmap[6] = 0x20; // 00100000
            bitmap[7] = 0x20; // 00100000
            break;

        case 'g': // ASCII 103
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x4A; // 01001010
            bitmap[6] = 0x4A; // 01001010
            bitmap[7] = 0x34; // 00110100
            bitmap[8] = 0x04; // 00000100
            bitmap[9] = 0x38; // 00111000
            break;

        case 'h': // ASCII 104
            bitmap[2] = 0x40; // 01000000
            bitmap[3] = 0x40; // 01000000
            bitmap[4] = 0x7C; // 01111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x42; // 01000010
            break;

        case 'i': // ASCII 105
            bitmap[2] = 0x10; // 00010000
            bitmap[4] = 0x18; // 00011000
            bitmap[5] = 0x10; // 00010000
            bitmap[6] = 0x10; // 00010000
            bitmap[7] = 0x7E; // 01111110
            break;

        case 'j': // ASCII 106
            bitmap[2] = 0x08; // 00001000
            bitmap[4] = 0x0C; // 00001100
            bitmap[5] = 0x08; // 00001000
            bitmap[6] = 0x08; // 00001000
            bitmap[7] = 0x38; // 00111000
            bitmap[8] = 0x08; // 00001000
            bitmap[9] = 0x30; // 00110000
            break;

        case 'k': // ASCII 107
            bitmap[2] = 0x40; // 01000000
            bitmap[3] = 0x40; // 01000000
            bitmap[4] = 0x4C; // 01001100
            bitmap[5] = 0x50; // 01010000
            bitmap[6] = 0x60; // 01100000
            bitmap[7] = 0x4C; // 01001100
            break;

        case 'l': // ASCII 108
            bitmap[2] = 0x18; // 00011000
            bitmap[3] = 0x18; // 00011000
            bitmap[4] = 0x18; // 00011000
            bitmap[5] = 0x18; // 00011000
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x7E; // 01111110
            break;

        case 'm': // ASCII 109
            bitmap[4] = 0x5A; // 01011010
            bitmap[5] = 0x5A; // 01011010
            bitmap[6] = 0x5A; // 01011010
            bitmap[7] = 0x42; // 01000010
            break;

        case 'n': // ASCII 110
            bitmap[4] = 0x7C; // 01111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x42; // 01000010
            break;

        case 'o': // ASCII 111
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3C; // 00111100
            break;

        case 'p': // ASCII 112
            bitmap[4] = 0x7C; // 01111100
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x7C; // 01111100
            bitmap[8] = 0x40; // 01000000
            bitmap[9] = 0x40; // 01000000
            break;

        case 'q': // ASCII 113
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x44; // 01000100
            bitmap[6] = 0x44; // 01000100
            bitmap[7] = 0x3C; // 00111100
            bitmap[8] = 0x04; // 00000100
            bitmap[9] = 0x04; // 00000100
            break;

        case 'r': // ASCII 114
            bitmap[4] = 0x78; // 01111000
            bitmap[5] = 0x44; // 01000100
            bitmap[6] = 0x40; // 01000000
            bitmap[7] = 0x40; // 01000000
            break;

        case 's': // ASCII 115
            bitmap[4] = 0x3C; // 00111100
            bitmap[5] = 0x40; // 01000000
            bitmap[6] = 0x3C; // 00111100
            bitmap[7] = 0x02; // 00000010
            break;

        case 't': // ASCII 116
            bitmap[2] = 0x20; // 00100000
            bitmap[3] = 0x20; // 00100000
            bitmap[4] = 0x7C; // 01111100
            bitmap[5] = 0x20; // 00100000
            bitmap[6] = 0x20; // 00100000
            bitmap[7] = 0x1C; // 00011100
            break;

        case 'u': // ASCII 117
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x42; // 01000010
            bitmap[7] = 0x3E; // 00111110
            break;

        case 'v': // ASCII 118
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x24; // 00100100
            bitmap[7] = 0x18; // 00011000
            break;

        case 'w': // ASCII 119
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x5A; // 01011010
            bitmap[6] = 0x5A; // 01011010
            bitmap[7] = 0x24; // 00100100
            break;

        case 'x': // ASCII 120
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x24; // 00100100
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x24; // 00100100
            break;

        case 'y': // ASCII 121
            bitmap[4] = 0x42; // 01000010
            bitmap[5] = 0x42; // 01000010
            bitmap[6] = 0x24; // 00100100
            bitmap[7] = 0x18; // 00011000
            bitmap[8] = 0x08; // 00001000
            bitmap[9] = 0x30; // 00110000
            break;

        case 'z': // ASCII 122
            bitmap[4] = 0x7E; // 01111110
            bitmap[5] = 0x04; // 00000100
            bitmap[6] = 0x08; // 00001000
            bitmap[7] = 0x70; // 01110000
            break;

        case '{': // ASCII 123
            bitmap[2] = 0x18; // 00011000
            bitmap[3] = 0x30; // 00110000
            bitmap[4] = 0x30; // 00110000
            bitmap[5] = 0x30; // 00110000
            bitmap[6] = 0x30; // 00110000
            bitmap[7] = 0x18; // 00011000
            break;

        // Добавляем case для '|' (ASCII 124)
        case '|': // ASCII 124 - Вертикальная черта
            bitmap[0] = 0x18; // 00011000
            bitmap[1] = 0x18; // 00011000
            bitmap[2] = 0x18; // 00011000
            bitmap[3] = 0x18; // 00011000
            bitmap[4] = 0x18; // 00011000
            bitmap[5] = 0x18; // 00011000
            bitmap[6] = 0x18; // 00011000
            bitmap[7] = 0x18; // 00011000
            bitmap[8] = 0x18; // 00011000
            bitmap[9] = 0x18; // 00011000
            break;


        case '}': // ASCII 125
            bitmap[2] = 0x30; // 00110000
            bitmap[3] = 0x30; // 00110000
            bitmap[4] = 0x30; // 00110000
            bitmap[5] = 0x30; // 00110000
            bitmap[6] = 0x30; // 00110000
            bitmap[7] = 0x18; // 00011000
            break;

	case '~': // ASCII 126 - Тильда
    	   bitmap[0] = 0x00; // 00000000 (........)
           bitmap[1] = 0x00; // 00000000 (........)
           bitmap[2] = 0x00; // 00000000 (........)
           bitmap[3] = 0x00; // 00000000 (........)
           bitmap[4] = 0x42; // 01000010 (.X....X.)
           bitmap[5] = 0xA5; // 10100101 (X.X..X.X)
           bitmap[6] = 0x08; // 00001000 (...X....)
           bitmap[7] = 0x00; // 00000000 (........)
           bitmap[8] = 0x00; // 00000000 (........)
           bitmap[9] = 0x00; // 00000000 (........)
           break;

        default:
            // Для неподдерживаемых символов рисуем '?'
            bitmap[2] = 0x3C; // 00111100
            bitmap[3] = 0x42; // 01000010
            bitmap[4] = 0x04; // 00000100
            bitmap[5] = 0x08; // 00001000
            bitmap[7] = 0x08; // 00001000
            break;
    }
}


void sdlfont_draw_char(int x, int y, char c, SDL_Renderer *renderer) {
    FontBitmap bitmap;
    sdlfont_generate_char_bitmap(bitmap, c);

    // Устанавливаем текущий цвет рендера (предполагается, что sdlgfx_color уже настроен)
    Uint8 r, g, b;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, NULL);
    for (int row = 0; row < FONT_HEIGHT; row++) {
        for (int col = 0; col < FONT_WIDTH; col++) {
            if ((bitmap[row] >> (FONT_WIDTH - 1 - col)) & 0x01) {
                SDL_RenderDrawPoint(renderer, x + col, y + row);
            }
        }
    }
}

void sdlfont_draw_string(int x, int y, const char *text, SDL_Renderer *renderer) {
    int current_x = x;
    while (*text) {
        sdlfont_draw_char(current_x, y, *text, renderer);
        current_x += FONT_WIDTH; // Переход к следующему символу
        text++;
    }
}
