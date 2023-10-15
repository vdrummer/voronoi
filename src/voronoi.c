#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "hsl.h"

#define COLOR_RED   0xff0000ff
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0xff000000

typedef uint32_t Color;

typedef struct {
  int x;
  int y;
} Point;

Color image[IMG_HEIGHT][IMG_WIDTH];
Point seeds[NUM_SEEDS];
Color palette[NUM_SEEDS];

void fillImage(Color color) {
  for (int y = 0; y < IMG_HEIGHT; y++) {
    for (int x = 0; x < IMG_WIDTH; x++) {
      image[y][x] = color;
    }
  }
}

void writeImageAsPpm(const char* filePath) {
  FILE* f = fopen(filePath, "wb");
  assert(f != NULL);

  // write header
  fprintf(f, "P6\n%d %d\n255\n", IMG_WIDTH, IMG_HEIGHT);

  // write pixel data
  for (int y = 0; y < IMG_HEIGHT; y++) {
    for (int x = 0; x < IMG_WIDTH; x++) {
      // PPM doesn't have an alpha channel. Only write 3 bytes per px
      const Color color = image[y][x];
      uint8_t pixels[3] = {
        (color & 0x0000ff) >> 0 * 8,
        (color & 0x00ff00) >> 1 * 8,
        (color & 0xff0000) >> 2 * 8
      };

      fwrite(pixels, 1, 3, f);
    }
  }

  fclose(f);
}

void generateRandomSeeds() {
  for (int i = 0; i < NUM_SEEDS; i++) {
    seeds[i].x = rand() % IMG_WIDTH;
    seeds[i].y = rand() % IMG_HEIGHT;
  }
}

int dist(int x0, int y0, int x1, int y1) {
  int dx = x0 - x1;
  int dy = y0 - y1;
  return dx * dx + dy * dy;
}

void renderCircle(int cx, int cy, int radius, Color color) {
  int x0 = cx - radius;
  int y0 = cy - radius;
  int x1 = cx + radius;
  int y1 = cy + radius;

  for (int y = y0; y < y1; y++) {
    if (y >= 0 && y < IMG_HEIGHT) {
      for (int x = x0; x < x1; x++) {
        if (x >= 0 && x < IMG_WIDTH) {
          if (dist(x, y, cx, cy) < radius * radius) {
            image[y][x] = color;
          }
        }
      }
    }
  }
}

Color colorFromRgb(int r, int g, int b) {
  uint32_t color = 0;
  color += 0xff000000;
  color += r << 8 * 0;
  color += b << 8 * 1;
  color += g << 8 * 2;

  return color;
}

float randomFloat() {
  return (float) rand() / (float) RAND_MAX;
}

float randomFloatAround(float center, float dist) {
  float d = randomFloat() * dist;
  d = random() % 2 == 0 ? d : -1.0 * d;

  float result = center;
  result += d;

  if (result > 1) result = 1;
  if (result < 0) result = 0;

  return result;
}

Color randomColorFromHue(float h) {
  ColorHsl hsl = {
    .h = randomFloatAround(h, 0.05),
    .s = randomFloatAround(0.3, 0.2),
    .l = randomFloatAround(0.5, 0.2)
  };

  ColorRgb rgb = hsl_HslToRgb(hsl);
  return colorFromRgb(rgb.r, rgb.g, rgb.b);
}

void renderSeeds() {
  for (int i = 0; i < NUM_SEEDS; i++) {
    renderCircle(seeds[i].x, seeds[i].y, SEED_RADIUS, COLOR_WHITE);
  }
}

void renderVoronoi() {
  for (int y = 0; y < IMG_HEIGHT; y++) {
    for (int x = 0; x < IMG_WIDTH; x++) {
      // find the seed with the minimum distance to (x, y)
      int j = 0;
      for (int i = 1; i < NUM_SEEDS; i++) {
        int dst_j = dist(x, y, seeds[j].x, seeds[j].y);
        int dst_i = dist(x, y, seeds[i].x, seeds[i].y);
        if (dst_i < dst_j) {
          j = i;
        }
      }

      image[y][x] = palette[j];
    }
  }
}

void initPalette() {
  float hue = randomFloat();
  for (int i = 0; i < NUM_SEEDS; i++) {
    palette[i] = randomColorFromHue(hue);
  }
}

int main() {
  srand(time(NULL));

  initPalette();
  fillImage(COLOR_BLACK);
  generateRandomSeeds();

  renderVoronoi();

#ifdef DRAW_SEEDS
  renderSeeds();
#endif

  writeImageAsPpm(OUTPUT_FILE_PATH);

  return 0;
}
