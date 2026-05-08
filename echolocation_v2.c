#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846

#pragma pack(push, 1)
typedef struct {
    char riff[4];
    unsigned int fileSize;
    char wave[4];
    char fmt[4];
    unsigned int fmtSize;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned int sampleRate;
    unsigned int byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char data[4];
    unsigned int dataSize;
} WavHeader;
#pragma pack(pop)

typedef struct {
    double real;
    double imag;
} Complex;

/* ===================== PROTOTYPE ===================== */
int write_wav(const char *filename, int sampleRate, int numSamples, double *signal);

/* ===================== WAV WRITE ===================== */
int write_wav(const char *filename, int sampleRate, int numSamples, double *signal) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open output file\n");
        return 1;
    }

    int dataSize = numSamples * 2;

    WavHeader header;
    memcpy(header.riff, "RIFF", 4);
    header.fileSize = 36 + dataSize;
    memcpy(header.wave, "WAVE", 4);
    memcpy(header.fmt, "fmt ", 4);
    header.fmtSize = 16;
    header.audioFormat = 1;
    header.numChannels = 1;
    header.sampleRate = sampleRate;
    header.byteRate = sampleRate * 2;
    header.blockAlign = 2;
    header.bitsPerSample = 16;
    memcpy(header.data, "data", 4);
    header.dataSize = dataSize;

    fwrite(&header, sizeof(WavHeader), 1, file);

    for (int i = 0; i < numSamples; i++) {
        double s = signal[i];

        if (s > 32767.0) s = 32767.0;
        if (s < -32768.0) s = -32768.0;

        short sample = (short)s;
        unsigned short us = (unsigned short)sample;

        fputc(us & 0xff, file);
        fputc((us >> 8) & 0xff, file);
    }

    fclose(file);
    return 0;
}

/* ===================== PEAK DETECTION ===================== */
void find_peaks(double *signal, int signalLen, int pulseSamples, double threshold,
                 int **peaks, int *numPeaks) {

    *numPeaks = 0;
    *peaks = malloc(signalLen * sizeof(int));

    for (int i = pulseSamples + 1; i < signalLen - 1; i++) {
        if (signal[i] > signal[i-1] &&
            signal[i] > signal[i+1] &&
            fabs(signal[i]) > threshold) {

            (*peaks)[(*numPeaks)++] = i;
        }
    }

    /* sort by amplitude */
    for (int a = 0; a < *numPeaks; a++) {
        for (int b = a + 1; b < *numPeaks; b++) {
            if (fabs(signal[(*peaks)[a]]) < fabs(signal[(*peaks)[b]])) {
                int tmp = (*peaks)[a];
                (*peaks)[a] = (*peaks)[b];
                (*peaks)[b] = tmp;
            }
        }
    }

    /* keep top 3 */
    if (*numPeaks > 3) {
        *numPeaks = 3;
    }
}

/* ===================== MAIN ===================== */
int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.wav output.wav\n", argv[0]);
        return 1;
    }

    int sr = 44100;
    int pLen = 44100;
    double *clean = malloc(pLen * sizeof(double));

    /* fake signal placeholder (replace with your real pipeline if needed) */
    for (int i = 0; i < pLen; i++) {
        clean[i] = sin(2 * PI * 1000 * i / sr);
    }

    int *peaks;
    int numPeaks;

    find_peaks(clean, pLen, 0, 0.2, &peaks, &numPeaks);

    printf("Scan Report:\n");
    printf("Number of peaks found: %d\n", numPeaks);

    for (int i = 0; i < numPeaks; i++) {
        printf("Peak %d at sample %d\n", i + 1, peaks[i]);
    }

    write_wav(argv[2], sr, pLen, clean);

    free(clean);
    free(peaks);

    return 0;
}
