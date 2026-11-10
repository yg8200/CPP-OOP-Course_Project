#pragma once

#include "colors.h"
#include <string>

// Global variable indicating if the user wants a colored game or not
extern bool COLORS_ENABLED;

// Move console cursor to (x, y)
void gotoxy(int x, int y);

// Hide the console cursor
void hideCursor();

// Show the console cursor
void showCursor();

// Portable, seedable random numbers (std::rand differs between MSVC / libc++ / glibc, which
// made recordings made on one platform unplayable on another).
void seedRandom(unsigned int seed);
size_t randomIndex(size_t bound); // uniform-ish value in [0, bound); bound must be > 0

// Clear keyboard buffer
void flushKeyboard();

// Read one key without requiring Enter and check whether a key is available.
int readKey();
bool keyAvailable();

// Like readKey(), but swallows arrow/function-key sequences (returns 0 for them).
int readGameKey();

// While silent, nothing is written to std::cout and sleepMs() returns immediately.
void setSilentMode(bool silent);

// Pause execution for a number of milliseconds.
void sleepMs(unsigned int milliseconds);

// Clear the screen
void cls();

// Print a specific character colored
void printColored(char c, Color color);

// Print a string colored
void printColor(const std::string& text, int color);

// Clear the 80x19 map area
void clear_map_box();

// Print a full screen text file by name
void print_room_by_txtName(const std::string& name);
