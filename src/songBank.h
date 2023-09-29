#include <./piezo-music.h>
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
int susMogiusgtusRhythm[] = {
    8, 8, 8, 8,
    8, 8, 4, 8,
    16, 16, 4,
    8,
    8, 4};

int stereoLoveBPM = 40;
int stereoLoveMelody[] = {
    GS4, PAUSE, E5, PAUSE, E5, DS5, E5, FS5, E5,
    DS5, E5, CS5, DS5, B4, CS5, A4, B4, GS4};
int stereoLoveRhythm[] = {
    8, 16, 6, 16, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 2};
