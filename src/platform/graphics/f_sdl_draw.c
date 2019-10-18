/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_sdl_draw.v.h"
#include <faur.v.h>

#if F_CONFIG_LIB_RENDER_SDL
#include <SDL2/SDL.h>

extern SDL_Renderer* f__sdlRenderer;

void f_platform_api__drawPixel(int X, int Y)
{
    if(SDL_RenderDrawPoint(f__sdlRenderer, X, Y) < 0) {
        f_out__error("SDL_RenderDrawPoint: %s", SDL_GetError());
    }
}

void f_platform_api__drawLine(int X1, int Y1, int X2, int Y2)
{
    if(SDL_RenderDrawLine(f__sdlRenderer, X1, Y1, X2, Y2) < 0) {
        f_out__error("SDL_RenderDrawLine: %s", SDL_GetError());
    }
}

void f_platform_api__drawHLine(int X1, int X2, int Y)
{
    f_platform_api__drawRectangleFilled(X1, Y, X2 - X1 + 1, 1);
}

void f_platform_api__drawVLine(int X, int Y1, int Y2)
{
    f_platform_api__drawRectangleFilled(X, Y1, 1, Y2 - Y1 + 1);
}

void f_platform_api__drawRectangleFilled(int X, int Y, int Width, int Height)
{
    SDL_Rect area = {X, Y, Width, Height};

    if(SDL_RenderFillRect(f__sdlRenderer, &area) < 0) {
        f_out__error("SDL_RenderFillRect: %s", SDL_GetError());
    }
}

void f_platform_api__drawRectangleOutline(int X, int Y, int Width, int Height)
{
    f_platform_api__drawRectangleFilled(X, Y, Width, 1);

    if(Height <= 1) {
        return;
    }

    f_platform_api__drawRectangleFilled(X, Y + Height - 1, Width, 1);

    if(Width <= 1 || Height <= 2) {
        return;
    }

    f_platform_api__drawRectangleFilled(X, Y + 1, 1, Height - 2);
    f_platform_api__drawRectangleFilled(X + Width - 1, Y + 1, 1, Height - 2);
}

void f_platform_api__drawCircleOutline(int X, int Y, int Radius)
{
    // Using inclusive coords
    if(--Radius <= 0) {
        if(Radius == 0) {
            f_platform_api__drawRectangleFilled(X - 1, Y - 1, 2, 2);
        }

        return;
    }

    int x = Radius;
    int y = 0;
    int error = -Radius / 2;

    int numPointPairs = 0;

    while(x > y) {
        numPointPairs++;

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;
        }
    }

    if(x == y) {
        numPointPairs++;
    }

    SDL_Point scanlines[numPointPairs * 4][2];

    x = Radius;
    y = 0;
    error = -Radius / 2;

    int scanline1 = 0;
    int x1 = X - 1 - x;
    int x2 = X + x;
    int y1 = Y - 1 - y;

    int scanline2 = numPointPairs;
    int y2 = Y + y;

    int scanline3 = numPointPairs * 2;
    int x3 = X - 1 - y;
    int x4 = X + y;
    int y3 = Y - 1 - x;

    int scanline4 = numPointPairs * 3;
    int y4 = Y + x;

    while(x > y) {
        scanlines[scanline1][0] = (SDL_Point){x1, y1};
        scanlines[scanline1][1] = (SDL_Point){x2, y1};

        scanlines[scanline2][0] = (SDL_Point){x1, y2};
        scanlines[scanline2][1] = (SDL_Point){x2, y2};

        scanlines[scanline3][0] = (SDL_Point){x3, y3};
        scanlines[scanline3][1] = (SDL_Point){x4, y3};

        scanlines[scanline4][0] = (SDL_Point){x3, y4};
        scanlines[scanline4][1] = (SDL_Point){x4, y4};

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        scanline1++;
        scanline2++;
        scanline3++;
        scanline4++;

        y1--;
        y2++;
        x3--;
        x4++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;

            x1++;
            x2--;
            y3++;
            y4--;
        }
    }

    if(x == y) {
        scanlines[scanline3][0] = (SDL_Point){x3, y3};
        scanlines[scanline3][1] = (SDL_Point){x4, y3};

        scanlines[scanline4][0] = (SDL_Point){x3, y4};
        scanlines[scanline4][1] = (SDL_Point){x4, y4};
    }

    if(SDL_RenderDrawPoints(
        f__sdlRenderer, (SDL_Point*)scanlines, numPointPairs * 4 * 2) < 0) {

        f_out__error("SDL_RenderDrawPoints: %s", SDL_GetError());
    }
}

void f_platform_api__drawCircleFilled(int X, int Y, int Radius)
{
    // Using inclusive coords
    if(--Radius <= 0) {
        if(Radius == 0) {
            f_platform_api__drawRectangleFilled(X - 1, Y - 1, 2, 2);
        }

        return;
    }

    int x = Radius;
    int y = 0;
    int error = -Radius / 2;

    const int numScanlines = (Radius + 1) * 2;
    SDL_Rect scanlines[numScanlines];

    int scanline1 = numScanlines / 2 - 1 - y;
    int x1 = X - 1 - x;
    int y1 = Y - 1 - y;
    int w1 = 2 * x + 2;

    int scanline2 = numScanlines / 2 + y;
    int y2 = Y + y;

    int scanline3 = numScanlines / 2 - 1 - x;
    int x3 = X - 1 - y;
    int y3 = Y - 1 - x;
    int w3 = 2 * y + 2;

    int scanline4 = numScanlines / 2 + x;
    int y4 = Y + x;

    while(x > y) {
        scanlines[scanline1] = (SDL_Rect){x1, y1, w1, 1};
        scanlines[scanline2] = (SDL_Rect){x1, y2, w1, 1};

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        scanline1--;
        y1--;
        scanline2++;
        y2++;
        x3--;
        w3 += 2;

        if(error > 0) { // check if x^2 + y^2 > r^2
            scanlines[scanline3] = (SDL_Rect){x3, y3, w3, 1};
            scanlines[scanline4] = (SDL_Rect){x3, y4, w3, 1};

            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;

            x1++;
            w1 -= 2;
            scanline3++;
            y3++;
            scanline4--;
            y4--;
        }
    }

    if(x == y) {
        scanlines[scanline3] = (SDL_Rect){x3, y3, w3, 1};
        scanlines[scanline4] = (SDL_Rect){x3, y4, w3, 1};
    }

    if(SDL_RenderFillRects(
        f__sdlRenderer, scanlines, (int)F_ARRAY_LEN(scanlines)) < 0) {

        f_out__error("SDL_RenderFillRects: %s", SDL_GetError());
    }
}
#endif // F_CONFIG_LIB_RENDER_SDL
