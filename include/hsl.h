#ifndef VORONOI_HSL_H
#define VORONOI_HSL_H

#include <stdint.h>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} ColorRgb;

typedef struct {
  float h;
  float s;
  float l;
} ColorHsl;

ColorRgb hsl_HslToRgb(ColorHsl hsl);

#endif //VORONOI_HSL_H
