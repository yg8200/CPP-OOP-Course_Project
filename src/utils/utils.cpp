#include "utils.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <thread>
#include <csignal>
#include <random>
#include <streambuf>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

bool COLORS_ENABLED = true; // default: colors ON

namespace {
// Output sink that discards everything (used for silent playback).
class NullBuffer : public std::streambuf {
protected:
    int overflow(int c) override { return c; }
    std::streamsize xsputn(const char*, std::streamsize n) override { return n; }
};

NullBuffer nullBuffer;
std::streambuf* realCoutBuffer = nullptr;
bool silentMode = false;
}

namespace {
std::mt19937 rng{1};
}

void seedRandom(unsigned int seed) {
    rng.seed(seed);
}

size_t randomIndex(size_t bound) {
    return static_cast<size_t>(rng()) % bound; // mt19937 output is identical on every platform
}

void setSilentMode(bool silent) {
    if (silent == silentMode)
        return;
    if (silent) {
        std::cout.flush();
        realCoutBuffer = std::cout.rdbuf(&nullBuffer);
    } else {
        std::cout.rdbuf(realCoutBuffer);
    }
    silentMode = silent;
}

#ifndef _WIN32
namespace {
termios savedTermios{};
volatile sig_atomic_t terminalIsRaw = 0;

// Async-signal-safe: only tcsetattr()/write() are used.
void restoreTerminal() {
    if (terminalIsRaw) {
        tcsetattr(STDIN_FILENO, TCSANOW, &savedTermios);
        terminalIsRaw = 0;
        static const char showCursorSeq[] = "\033[?25h";
        ssize_t ignored = write(STDOUT_FILENO, showCursorSeq, sizeof(showCursorSeq) - 1);
        (void)ignored;
    }
}

extern "C" void onTerminationSignal(int sig) {
    restoreTerminal();
    std::signal(sig, SIG_DFL);
    std::raise(sig);
}

class TerminalMode {
public:
    TerminalMode() : active(isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &original) == 0) {
        if (active) {
            savedTermios = original;
            terminalIsRaw = 1;
            // Ctrl+C / kill must not leave the terminal without echo and with a hidden cursor.
            std::signal(SIGINT, onTerminationSignal);
            std::signal(SIGTERM, onTerminationSignal);
            std::signal(SIGHUP, onTerminationSignal);
            termios raw = original;
            raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
            raw.c_cc[VMIN] = 1;
            raw.c_cc[VTIME] = 0;
            tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        }
    }

    ~TerminalMode() {
        if (active)
            restoreTerminal();
    }

private:
    termios original{};
    bool active;
};

TerminalMode& terminalMode() {
    static TerminalMode mode;
    return mode;
}

int pendingKey = -1;

bool inputReady(long timeoutMicroseconds) {
    fd_set input;
    FD_ZERO(&input);
    FD_SET(STDIN_FILENO, &input);

    timeval timeout{};
    timeout.tv_usec = timeoutMicroseconds;
    return select(STDIN_FILENO + 1, &input, nullptr, nullptr, &timeout) > 0;
}
}
#endif

void gotoxy(int x, int y) {
    std::cout.flush();
#ifdef _WIN32
    COORD coord;
    coord.X = static_cast<SHORT>(x);
    coord.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    std::cout << "\033[" << y + 1 << ';' << x + 1 << 'H';
#endif
}

void hideCursor() {
#ifdef _WIN32
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
#else
    std::cout << "\033[?25l";
#endif
}

void showCursor() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
#else
    std::cout << "\033[?25h";
#endif
}

void flushKeyboard() {
#ifdef _WIN32
    while (_kbhit())
        _getch();
#else
    while (keyAvailable())
        readKey();
#endif
}

int readKey() {
    std::cout.flush();
#ifdef _WIN32
    return _getch();
#else
    (void)terminalMode();
    if (pendingKey != -1) {
        const int key = pendingKey;
        pendingKey = -1;
        return key;
    }

    char input = 0;
    if (read(STDIN_FILENO, &input, 1) != 1)
        return -1;

    if (input == '\033' && inputReady(30000)) {
        char sequence = 0;
        if (read(STDIN_FILENO, &sequence, 1) == 1 && sequence == '[' &&
            inputReady(30000)) {
            char direction = 0;
            if (read(STDIN_FILENO, &direction, 1) == 1) {
                switch (direction) {
                case 'A': pendingKey = 72; break;
                case 'B': pendingKey = 80; break;
                case 'C': pendingKey = 77; break;
                case 'D': pendingKey = 75; break;
                default: return 27;
                }
                return 0;
            }
        }
        return 27;
    }

    return static_cast<unsigned char>(input);
#endif
}

int readGameKey() {
    int key = readKey();
    // Arrow/function keys arrive as a 0 (or 224) prefix followed by a scan code. That code
    // can equal a letter ('K', 'M', 'H', 'P' - 'k' and 'm' are player-2 keys), so swallow it.
    if (key == 0 || key == 224) {
        readKey();
        return 0;
    }
    return key;
}

bool keyAvailable() {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    (void)terminalMode();
    return pendingKey != -1 || inputReady(0);
#endif
}

void sleepMs(unsigned int milliseconds) {
    std::cout.flush(); // make sure the frame that was just drawn is actually visible
    if (silentMode)
        return;
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void cls() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

void printColored(char c, Color color) {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!COLORS_ENABLED) {
        SetConsoleTextAttribute(h, 7);
        std::cout << c;
        return;
    }

    WORD wColor = 7;
    switch (color) {
    case Color::RED:     wColor = 12; break;
    case Color::GREEN:   wColor = 10; break;
    case Color::YELLOW:  wColor = 14; break;
    case Color::BLUE:    wColor = 9;  break;
    case Color::MAGENTA: wColor = 13; break;
    case Color::CYAN:    wColor = 11; break;
    case Color::WHITE:   wColor = 15; break;
    case Color::GRAY:    wColor = 8;  break;
    case Color::DIJON:   wColor = 6;  break;
    default:             wColor = 7;  break;
    }

    SetConsoleTextAttribute(h, wColor);
    std::cout << c;
    SetConsoleTextAttribute(h, 7);
#else
    if (!COLORS_ENABLED) {
        std::cout << c;
        return;
    }

    int ansiColor = 37;
    switch (color) {
    case Color::RED:     ansiColor = 31; break;
    case Color::GREEN:   ansiColor = 32; break;
    case Color::YELLOW:  ansiColor = 33; break;
    case Color::BLUE:    ansiColor = 34; break;
    case Color::MAGENTA: ansiColor = 35; break;
    case Color::CYAN:    ansiColor = 36; break;
    case Color::WHITE:   ansiColor = 97; break;
    case Color::GRAY:    ansiColor = 90; break;
    case Color::DIJON:   ansiColor = 33; break;
    default:             ansiColor = 37; break;
    }

    std::cout << "\033[" << ansiColor << 'm' << c << "\033[0m";
#endif
}

void printColor(const std::string& text, int color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    std::cout << text;
    SetConsoleTextAttribute(hConsole, 7);
#else
    std::cout << "\033[" << color << 'm' << text << "\033[0m";
#endif
}

void clear_map_box() {
    for (int y = 0; y < 19; y++) {
        gotoxy(0, y);
        std::cout << std::string(80, ' ');
    }
}

void print_room_by_txtName(const std::string& name) {
    cls();

    std::ifstream file(name);
    if (!file.is_open()) {
        gotoxy(0, 0);
        std::cout << "Error opening screen file: " << name;
        readKey();
        return;
    }

    std::string line;
    int y = 0;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        gotoxy(0, y++);
        std::cout << line;
    }
}
