#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t blur_pixel(uint32_t *pixels, int index, int width, int height) {
    int kernel[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
    int coeff = 1/16;
    int offsets[9] = {
        -width - 1, -width, -width + 1,
        -1,          0,      1,
         width - 1,  width,  width + 1
    };

    int sum_r = 0;
    int sum_g = 0;
    int sum_b = 0;
    int sum_a = 0;

    int weight;
    for (int i = 0; i < 9; i++) {
        if (index + offsets[i] < 0 || index + offsets[i] >= width * height) {
            continue;
        }
        uint32_t pixel = pixels[index + offsets[i]];
        int weight = kernel[i];
        uint8_t a = (pixel >> 24) & 0xff;
        uint8_t r = (pixel >> 16) & 0xff;
        uint8_t g = (pixel >> 8)  & 0xff;
        uint8_t b = pixel & 0xff;
        sum_r += r * weight;
        sum_g += g * weight;
        sum_b += b * weight;
        sum_a += a * weight;
    }

    sum_r *= coeff;
    sum_g *= coeff;
    sum_b *= coeff;
    sum_a *= coeff;

    return ((uint32_t)sum_a << 24) |
           ((uint32_t)sum_r << 16) |
           ((uint32_t)sum_g << 8)  |
           sum_b;
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

uint32_t *blur_image(uint32_t *pixels, int width, int height) {
    uint32_t *blurred = malloc(width * height * sizeof(uint32_t));

    if (!blurred) {
        return NULL;
    }

    for (int i = 0; i < width * height; i++) {
        blurred[i] = blur_pixel(pixels, i, width, height);   
    }

    return blurred;
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

    uint32_t *blurred = blur_image(pixels, width, height);

    if (!blurred) {
        printf("Failed to blur image\n");
        free(pixels);
        return 1;
    }

    if (!save_png("output/blurred.jpg", blurred, width, height)) {
        printf("Failed to save image\n");
        free(pixels);
        free(blurred);
        return 1;
    }

    printf("Blurred image saved successfully\n");

    free(pixels);
    free(blurred);

    return 0;
}