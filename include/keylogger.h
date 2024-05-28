#ifndef KEYLOGGER_H
#define KEYLOGGER_H

/**
 * Key code to key name mapping
 * @see /usr/include/linux/input-event-codes.h contains list of key codes
 */
typedef struct {
    int code;
    const char *name;
} KeyCode;

/**
 * Decode the key code to a key name.
 * @see /usr/include/linux/input-event-codes.h
 * @param key The key code to decode.
 * @return The key name.
 */
const char *decodeKey(int key);

/**
 * Log the key to a file.
 * @param key The key code to log.
 */
void logKeyToFile(int key);

/**
 * Log the key from the input event to a file.
 * @param ev The input event to log.
 * @return 1 if the function executed successfully, 0 otherwise.
 */
int logKeyFromInputEventToFile(struct input_event ev);

#endif //KEYLOGGER_H
