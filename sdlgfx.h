// sdlgfx.h
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
 * sdlgfx.h - Header file for the simple SDL2 graphics library.
 * Original gfx library by Ivan Svarkovsky (c) 2019.
 * SDL2 port and modifications by Ivan Svarkovsky.
 */


#ifndef SDLGFX_H
#define SDLGFX_H

#include <SDL2/SDL.h>
#include <math.h>
#include "sdlfont.h" // Include sdlfont.h here

#ifdef __cplusplus
extern "C" {
#endif

extern SDL_Window *sdlgfx_window;
extern SDL_Renderer *sdlgfx_renderer;
extern SDL_Texture *sdlgfx_texture; // Новая глобальная текстура для фона

void sdlgfx_open(int width, int height, const char *title, int initial_x, int initial_y);
void sdlgfx_close(void);
void sdlgfx_color(int r, int g, int b);
void sdlgfx_clear_color(int r, int g, int b);
void sdlgfx_clear(void);
void sdlgfx_point(int x, int y);
void sdlgfx_line(int x1, int y1, int x2, int y2);
void sdlgfx_rectangle(int x1, int y1, int x2, int y2);
void sdlgfx_fill_rectangle(int x1, int y1, int x2, int y2);
void sdlgfx_circle(int x, int y, int radius);
void sdlgfx_fill_circle(int x, int y, int radius);
void sdlgfx_string(int x, int y, const char *cc);
void sdlgfx_flush(void);
void sdlgfx_wait(void);
int sdlgfx_event_waiting(void);
int sdlgfx_xsize(void);
int sdlgfx_ysize(void);
void sdlgfx_move_win_abs(int x, int y);
void sdlgfx_move_win_rel(int dx, int dy);
int GetPix(int x, int y);
void sdlgfx_pixel(int x, int y);
void sdlgfx_update_texture(void (*draw_func)(int, int, float, int), int width, int height, float time, int technique, int update);
// 03 19 2025
void sdlgfx_pixel_gradient(int x, int y, int r, int g, int b);
void sdlgfx_gradient_vertical_ex(int r1, int g1, int b1, int r2, int g2, int b2, int r3, int g3, int b3, int r4, int g4, int b4, int num_stops, int width, int height, int scanlines_enabled, int noise_enabled, int pixel_noise_enabled);

SDL_bool sdlgfx_is_char_pixel(const char* text, int text_cols, int char_index_x, int pixel_x_in_char, int char_index_y, int pixel_y_in_char);


#ifdef __cplusplus
}
#endif

#endif // SDLGFX_H
