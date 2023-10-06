#include <./piezo-music.h>

// ------------------ SONG CONSTANTS --------------------
#define MOGUSTGUTUS 0
#define STEREO_LOVE 1
#define BETTER_OFF_ALONE 2

// ------------------ SONG BANK -------------------------
int susMogiusgtusBPM = 30;
int susMogiusgtusMelody[] = {
    C4, DS4, F4, FS4,
    F4, DS4, C4, PAUSE,
    AS3, D4, C4,
    PAUSE,
    G1, C2};
int rereSusMogiusgtusMelody[] = {
    C4, DS4, F4, FS4,
    E4, DS4, C4, PAUSE,
    AS3, B4, C4,
    PAUSE,
    G1, C2};
float susMogiusgtusRhythm[] = {
    8, 8, 8, 8,
    8, 8, 4, 8,
    16, 16, 4,
    8,
    8, 4};


int stereoLoveBPM = 43;
int stereoLoveMelody[] = {
    GS4, PAUSE, E5, PAUSE, E5, DS5, E5, FS5, E5,
    DS5, E5, CS5, DS5, B4, CS5, A4, B4, GS4};
float stereoLoveRhythm[] = {
    8, 16, 6, 16, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 2};


int betterOffAloneBPM = 48;
int betterOffAloneMelody[] = {
    B5, B5, GS5, B5, B5, AS5,
    FS5, FS6, FS6, DS6,
    B5, B5, GS5, B5, B5, AS5,
    FS5, E6, E6, DS6,
    B5,
};
float betterOffAloneRhythm[] = {
    4, 8, 4, 4, 4, 4,
    8, 5.33, 5.33, 8,
    4, 8, 4, 4, 4, 4,
    8, 5.33, 5.33, 8,
    4,
};
