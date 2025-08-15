#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <linux/time.h>

#define MAX_ITERATIONS 1000
#define NUM_CHANNELS 3

typedef struct {
    double real;
    double imag;
} Complex;

typedef struct {
    uint8_t r, g, b;
} Color;

Complex complex_multiply(Complex a, Complex b) {
    Complex result;
    result.real = a.real * b.real - a.imag * b.imag;
    result.imag = a.real * b.imag + a.imag * b.real;
    return result;
}

Complex complex_add(Complex a, Complex b) {
    Complex result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

double complex_magnitude(Complex c) {
    return sqrt(c.real * c.real + c.imag * c.imag);
}

int getMandelbrotIterations(Complex c) {
    Complex z = {0.0, 0.0};
    int n = 0;
    
    while (complex_magnitude(z) <= 2.0 && n < MAX_ITERATIONS) {
        z = complex_add(complex_multiply(z, z), c);
        n++;
    }
    return n;
}

Color mapIterationToColor(int n) {
    Color color;
    
    if (n == MAX_ITERATIONS) {
        color.r = 0;
        color.g = 0;
        color.b = 0;
        return color;
    }
    
    color.r = (uint8_t)(n % 256);
    color.g = (uint8_t)((n * 2) % 256);
    color.b = (uint8_t)((n * 5) % 256);
    
    return color;
}

double get_time() {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return (double)clock() / CLOCKS_PER_SEC;
    }
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main() {
    int height, width;
    printf("Masukkan lebar gambar:" );
    
    if (scanf("%d", &width) != 1) {
        fprintf(stderr, "Input tidak valid!\n");
        return 1;
    }

    printf("Masukan Tinggi gambar:");
    if (scanf("%d", &height) != 1) {
        fprintf(stderr, "Input tidak valid!\n");
        return 1;
    }

    
    if (width <= 0 || height <= 0 || width > 10000 || height > 10000) {
        fprintf(stderr, "Resolusi tidak valid! Gunakan nilai antara 1-10000.\n");
        return 1;
    }
    
    printf("Menggunakan resolusi: %dx%d\n", width, height);
    
    size_t pixel_count = (size_t)width * height * NUM_CHANNELS;
    printf("Ukuran memori yang dibutuhkan: %.2f MB\n", pixel_count / (1024.0 * 1024.0));

    uint8_t *pixels = (uint8_t*)calloc(pixel_count, sizeof(uint8_t));
    if (pixels == NULL) {
        fprintf(stderr, "Gagal mengalokasikan memori untuk piksel gambar!\n");
        fprintf(stderr, "Coba gunakan resolusi yang lebih kecil.\n");
        return 1;
    }

    memset(pixels, 0, pixel_count);

    printf("\nRendering PARALEL...\n");
    
    #ifdef _OPENMP
    printf("OpenMP tersedia. Menggunakan %d threads.\n", omp_get_max_threads());
    #else
    printf("OpenMP tidak tersedia. Menggunakan implementasi serial kedua.\n");
    #endif
    
    double start_parallel = get_time();

    #ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic, 10)
    #endif
    for (int y = 0; y < height; ++y) {
        
        for (int x = 0; x < width; ++x) {
            double cx = -2.0 + (double)x / width * 3.0;
            double cy = -1.5 + (double)y / height * 3.0;
            Complex c = {cx, cy};

            int iterations = getMandelbrotIterations(c);
            Color color = mapIterationToColor(iterations);

            size_t index = ((size_t)y * width + x) * NUM_CHANNELS;
            pixels[index] = color.r;
            pixels[index + 1] = color.g;
            pixels[index + 2] = color.b;
        }
    }

    double end_parallel = get_time();
    double duration_parallel = end_parallel - start_parallel;
    printf("Rendering PARALEL selesai dalam %.2f detik.\n", duration_parallel);

    int result2 = stbi_write_png("mandelbrot_parallel.png", width, height, NUM_CHANNELS, pixels, width * NUM_CHANNELS);
    if (result2) {
        printf("Gambar 'mandelbrot_parallel.png' telah disimpan.\n");
    } else {
        printf("GAGAL menyimpan gambar 'mandelbrot_parallel.png'!\n");
    }

    printf("\n--- Hasil Benchmarking ---\n");
    printf("Resolusi Gambar: %dx%d\n", width, height);
    
    #ifdef _OPENMP
    printf("Jumlah Thread Maksimal: %d\n", omp_get_max_threads());
    #else
    printf("OpenMP: Tidak tersedia\n");
    #endif

    printf("Waktu Eksekusi Paralel: %.6f detik\n", duration_parallel);


    free(pixels);
    return 0;
}