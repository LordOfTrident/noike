#ifndef UI_H_HEADER_GUARD
#define UI_H_HEADER_GUARD

#include <assert.h> /* assert */
#include <math.h>   /* fabs */

#include <SDL2/SDL.h>

#include "sprite_sheet.h"
#include "timer.h"
#include "utils/math.h"

void uiFrame      (Rect rect, SpriteSheet *sheet, int id);
void uiCounter    (Rect rect, int value, int len, SpriteSheet *sheet, int id);
void uiProgressBar(Rect rect, float progress, float *prevProgress, float t,
                   SpriteSheet *sheet, int id, int frameFill, int frameTrack);

#endif
