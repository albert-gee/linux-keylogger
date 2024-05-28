#ifndef PROC_BUS_INPUT_DEVICES_PARSER_H
#define PROC_BUS_INPUT_DEVICES_PARSER_H

/**
 * Struct representing input devices information from /proc/bus/input/devices
 */
typedef struct ProcBusInputDevice {
    char bus[64];               // Bus type (e.g., USB, PS/2)
    char vendor[64];            // Vendor ID of the device
    char product[64];           // Product ID of the device
    char version[64];           // Version number of the device
    char name[256];             // Human-readable name of the device
    char phys[256];             // Physical connection path of the device
    char sysfs[256];            // Path to the device's entry in the sysfs filesystem
    char uniq[256];             // Unique identifier of the device (often empty)
    char handlers[256];         // Handlers associated with the device, typically corresponding to device nodes in /dev/input/
    unsigned long prop;         // Properties of the device
    unsigned long ev;           // Event types supported by the device
    char key[256];              // String of supported keys
    unsigned long rel;          // Bitmap of supported relative axes
    unsigned long abs;          // Bitmap of supported absolute axes
    unsigned long msc;          // Bitmap of supported miscellaneous events
    unsigned long led;          // Bitmap of supported LEDs
    unsigned long snd;          // Bitmap of supported sounds
    unsigned long ff;           // Bitmap of supported force feedback effects
    unsigned long sw;           // Bitmap of supported switches
    struct ProcBusInputDevice *next; // Pointer to the next device in the linked list
} ProcBusInputDevice;

/**
 * Free the memory allocated for the linked list of input devices
 * @param head the head of the linked list
 */
void freeInputDevices(ProcBusInputDevice *head);

/**
 * Create a new input device structure
 * @return a pointer to the newly created input device
 */
ProcBusInputDevice *createDevice();

/**
 * Handle a conversion error when parsing a line from /proc/bus/input/devices
 * @param line the line that caused the error
 * @param file the file pointer to /proc/bus/input/devices
 * @param head the head of the linked list of input devices
 */
void handleConversionError(const char *line, FILE *file, ProcBusInputDevice *head);

/**
 * Parse a line from /proc/bus/input/devices and populate the device structure
 * @param device the input device structure to populate
 * @param line the line to parse
 * @param file the file pointer to /proc/bus/input/devices
 * @param head the head of the linked list of input devices
 */
void parseLine(ProcBusInputDevice *device, const char *line, FILE *file, ProcBusInputDevice *head);

/**
 * Parse the input devices from /proc/bus/input/devices
 * @return a linked list of input devices
 */
ProcBusInputDevice *parseInputDevices();

#endif //PROC_BUS_INPUT_DEVICES_PARSER_H
