#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t *blur_image(const char *filename, int *width, int *height, int *channels) {
    
    uint32_t *pixels = read_image(filename, width, height, channels);
    // perform gaussian blurring

}

uint32_t *read_image(
    const char *filename,
    int *width,
    int *height,
    int *channels
) {
    unsigned char *data = stbi_load(filename, width, height, channels, 4);

    if (!data) {
        printf("Failed to load image: %s\n", filename);
        return NULL;
    }

    uint32_t *pixels = malloc((*width) * (*height) * sizeof(uint32_t));

    if (!pixels) {
        stbi_image_free(data);
        return NULL;
    }

    for (int y = 0; y < *height; y++) {
        for (int x = 0; x < *width; x++) {
            int i = (y * (*width) + x) * 4;

            uint8_t r = data[i + 0];
            uint8_t g = data[i + 1];
            uint8_t b = data[i + 2];
            uint8_t a = data[i + 3];

            pixels[y * (*width) + x] =
                ((uint32_t)a << 24) |
                ((uint32_t)r << 16) |
                ((uint32_t)g << 8)  |
                b;
        }
    }

    stbi_image_free(data);
    return pixels;
}

int save_png(
    const char *filename,
    const uint32_t *pixels,
    int width,
    int height
) {
    unsigned char *rgba = malloc(width * height * 4);

    if (!rgba) {
        return 0;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint32_t p = pixels[y * width + x];

            int i = (y * width + x) * 4;

            rgba[i + 0] = (p >> 16) & 0xff;
            rgba[i + 1] = (p >> 8)  & 0xff;
            rgba[i + 2] = p & 0xff;
            rgba[i + 3] = (p >> 24) & 0xff;
        }
    }

    int ok = stbi_write_png(filename, width, height, 4, rgba, width * 4);

    free(rgba);
    return ok;
}


int main(void) {
    int width, height, channels;

    uint32_t *pixels = read_image(
        "download.jpg",
        &width,
        &height,
        &channels
    );

    if (!pixels) {
        return 1;
    }

    printf("Width: %d\n", width);
    printf("Height: %d\n", height);
    printf("Original channels: %d\n", channels);
    printf("Number of pixels: %d\n", width * height);

    if (!save_png("output/image.png", pixels, width, height)) {
        printf("Failed to save image\n");
        free(pixels);
        return 1;
    }

    printf("Image saved successfully\n");

    free(pixels);
    return 0;
}