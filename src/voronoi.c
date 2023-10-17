#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "config.h"

#define COLOR_RED   0xff0000ff
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0xff000000

#define GOLDEN_RATIO_CONJUGATE 0.618033988749895

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

Color randomColor() {
  uint8_t r = rand() % 255;
  uint8_t g = rand() % 255;
  uint8_t b = rand() % 255;

  uint32_t color = 0;
  color += 0xff000000;
  color += r << 8 * 0;
  color += b << 8 * 1;
  color += g << 8 * 2;

  return color;
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

Color HSVtoRGB(float H, float S,float V){
    float s = S/100;
    float v = V/100;
    float C = s*v;
    float X = C*(1-fabs(fmod(H/60.0, 2)-1));
    float m = v-C;
    float r,g,b;
    if(H >= 0 && H < 60){
        r = C,g = X,b = 0;
    }
    else if(H >= 60 && H < 120){
        r = X,g = C,b = 0;
    }
    else if(H >= 120 && H < 180){
        r = 0,g = C,b = X;
    }
    else if(H >= 180 && H < 240){
        r = 0,g = X,b = C;
    }
    else if(H >= 240 && H < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }
    int R = (r+m)*255;
    int G = (g+m)*255;
    int B = (b+m)*255;

    uint32_t color = 0;
    color += 0xff000000;
    color += R << 8 * 0;
    color += B << 8 * 1;
    color += G << 8 * 2;

  return color;
}

void initPalette() {
  int h = rand() % 360;
  const double s = 50;
  const double v = 95;

  for (int i = 0; i < NUM_SEEDS; i++) {
    palette[i] = HSVtoRGB(h, s, v);
    h = rand() % 360;
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
