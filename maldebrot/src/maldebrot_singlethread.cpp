// #include <iostream>

// int main() {
//     std::cout << "Hello world!" << std::endl;
// }

#include "pngwriter.h"

int main() {
  const int SIZE = 8192;
  pngwriter image(SIZE, SIZE, 1.0, "result.png");

  for (int pixel_x = 0; pixel_x < SIZE; pixel_x++) {
    for (int pixel_y = 0; pixel_y < SIZE; pixel_y++) {
      double cx = ((double)pixel_x - ((double)SIZE) / 2) / SIZE * 3.0;
      double cyi = ((double)pixel_y - ((double)SIZE) / 2) / SIZE * 3.0;

      bool draw = true;
      double zx = 0;
      double zyi = 0;
      for (int i = 0; i < 2000; i++) {
        double zx_prev = zx;
        double zyi_prev = zyi;
        zx = zx_prev * zx_prev - zyi_prev * zyi_prev + cx;
        zyi = 2 * zx_prev * zyi_prev + cyi;
        if (zx * zx + zyi * zyi > 4) {
          draw = false;
          break;
        }
      }

      if (draw) {
        image.plot(pixel_x, pixel_y, 0.0, 0.0, 0.0);
      }
    }
  }

  image.close();
  return 0;
}