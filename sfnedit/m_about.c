/*
 * sfnedit/m_about.c
 *
 * Copyright (C) 2020 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief Main window About
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"
#ifdef __WIN32__
#include <windows.h>
#include <shellapi.h>
#endif
#ifdef __MACOSX__
#define OPEN "open"
#else
#define OPEN "xdg-open"
#endif

extern ssfn_t logofnt;
char repo_url[] = "https://gitlab.com/bztsrc/scalable-font2";

/**
 * Open the repository in the default browser
 */
void about_open_repo()
{
#ifdef __WIN32__
    ShellExecuteA(0, 0, repo_url, 0, 0, SW_SHOW);
#else
    char cmd[256];
    if(!fork()) {
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        sprintf(cmd, OPEN " %s", repo_url);
        exit(system(cmd));
    }
#endif
    wins[0].field = -1;
    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
}

/**
 * The about window
 */
void view_about()
{
    int i, j, m, p;
    char *s = "Scalable Screen Font Editor", *ptr = ((char*)ssfn_src + 32);
    ui_win_t *win = &wins[0];
    uint8_t *a, *b, *c = (uint8_t*)&theme[THEME_BG];

    if(win->w > 64 && win->h > 58) {
        for(p = 40*win->p + (win->w - 64) / 2, m = 0, j = 0; j < 64 && 58 + j < win->h; j++, p += win->p, m += 64 * 4) {
            for(i = 0; i < 64; i++) {
                if(icon64[m + (i<<2)+3]) {
                    a = (uint8_t*)&win->data[p + i];
                    b = (uint8_t*)&icon64[m + (i<<2)];
                    a[2] = (b[0]*b[3] + (256 - b[3])*c[2]) >> 8;
                    a[1] = (b[1]*b[3] + (256 - b[3])*c[1]) >> 8;
                    a[0] = (b[2]*b[3] + (256 - b[3])*c[0]) >> 8;
                }
            }
        }
    }
    ssfn_dst.ptr = (uint8_t*)win->data;
    ssfn_dst.p = win->p*4;
    ssfn_dst.w = win->w;
    ssfn_dst.h = win->h;
    ssfn_dst.x = (win->w - 620) / 2;
    ssfn_dst.y = 160;
    ssfn_dst.fg = theme[THEME_FG];
    ssfn_dst.bg = theme[THEME_BG];
    while((i = ssfn_render(&logofnt, &ssfn_dst, s)) > 0) s += i;
    ssfn_dst.fg = 0xFF000000 | ((theme[THEME_BG] >> 2) & 0x3F3F3F);
    ssfn_dst.bg = 0;
    j = wins[0].field == 6 ? THEME_FG : THEME_BG;
    p = (win->w - 320) / 2;
    ui_box(win, p - 2, 180, 324, 18, theme[j], theme[wins[0].field == 6 ? THEME_LIGHT: THEME_BG], theme[j]);
    ui_text(win, p + 1, 181, repo_url);
    ssfn_dst.fg = theme[THEME_FG];
    ui_text(win, p, 180, repo_url);
    ssfn_dst.y = win->h - 18 * 16;
    if(ssfn_dst.y < 208) ssfn_dst.y = 208;
    ui_text(win, 8, ssfn_dst.y, lang[HELPSTR]);
    ui_text(win, ssfn_dst.x + 8, ssfn_dst.y, uniname_date);
    ui_text(win, ssfn_dst.x, ssfn_dst.y, ", unifont:");
    while(*ptr) ptr++;
    ptr++; while(*ptr) ptr++;
    ptr++; while(*ptr) ptr++;
    ptr++;
    ui_text(win, ssfn_dst.x + 8, ssfn_dst.y, ptr);
    ssfn_dst.y += 32;
    ui_text(win, 8, ssfn_dst.y, "Copyright (C) 2021 bzt (bztsrc@gitlab) - MIT license");
    ssfn_dst.y += 16;
    ssfn_dst.fg = theme[THEME_LIGHTER];
    for(i = CPYRGT_0; i <= CPYRGT_9 && (int)ssfn_dst.y < win->h - 18; i++) {
        ssfn_dst.y += 16;
        ui_text(win, 8, ssfn_dst.y, lang[i]);
    }
    ssfn_dst.fg = theme[THEME_FG];
}

/**
 * On mouse move handler
 */
void ctrl_about_onmove()
{
    int p = (wins[0].w - 320) / 2;
    if(event.y >= 180 && event.y < 196 && event.x >= p && event.x < p + 320)
        cursor = CURSOR_GRAB;
}

/**
 * On click (button release) handler
 */
void ctrl_about_onclick()
{
    int p = (wins[0].w - 320) / 2;
    if(event.y >= 180 && event.y < 196 && event.x >= p && event.x < p + 320)
        about_open_repo();
}

/**
 * On enter handler
 */
void ctrl_about_onenter()
{
    if(wins[0].field == 6)
        about_open_repo();
}
