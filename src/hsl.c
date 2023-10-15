#include "hsl.h"
#include <math.h>

float hueToRgb(float p, float q, float t) {
  if (t < 0) t += 1;
  if (t > 1) t -= 1;
  if (t < 1/6.0) return p + (q - p) * 6 * t;
  if (t < 1/2.0) return q;
  if (t < 2/3.0) return p + (q - p) * (2/3.0 - t) * 6;
  return p;
}

ColorRgb hsl_HslToRgb(ColorHsl hsl) {
  float r, g, b;

  if (hsl.s == 0.0) {
    r = g = b = hsl.l;
  } else {
    float q = hsl.l < 0.5
      ? hsl.l * (1 + hsl.s)
      : hsl.l + hsl.s - hsl.l * hsl.s;
    float p = 2 * hsl.l - q;

    r = hueToRgb(p, q, hsl.h + 1/3.0);
    g = hueToRgb(p, q, hsl.h);
    b = hueToRgb(p, q, hsl.h - 1/3.0);
  }

  ColorRgb rgb;
  rgb.r = nearbyintf(r * 255);
  rgb.g = nearbyintf(g * 255);
  rgb.b = nearbyintf(b * 255);

  return rgb;
}
