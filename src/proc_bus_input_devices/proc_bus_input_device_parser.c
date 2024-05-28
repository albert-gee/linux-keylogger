#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "../../include/proc_bus_input_devices/proc_bus_input_devices_parser.h"

#define MAX_LINE_LENGTH 256
#define PROC_BUS_INPUT_DEVICES "/proc/bus/input/devices"

void freeInputDevices(ProcBusInputDevice *head) {
    ProcBusInputDevice *current = head;
    while (current != NULL) {
        ProcBusInputDevice *next = current->next;
        free(current);
        current = next;
    }
}

ProcBusInputDevice *createDevice() {
    ProcBusInputDevice *device = malloc(sizeof(ProcBusInputDevice));
    if (device == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    memset(device, 0, sizeof(ProcBusInputDevice));  // Initialize all fields to zero
    return device;
}

void handleConversionError(const char *line, FILE *file, ProcBusInputDevice *head) {
    printf("Conversion error on line: %s\n", line);
    fclose(file);
    freeInputDevices(head);
    exit(EXIT_FAILURE);
}

void parseLine(ProcBusInputDevice *device, const char *line, FILE *file, ProcBusInputDevice *head) {
    bool conversionError = false;

    // This pointer is used by the strtoul() function.
    // It indicates the first character in the input string that couldn't be converted to an unsigned long integer.
    // We use it to check if the conversion was successful.
    char *endPtr = NULL;

    // Parse the line based on the first character
    switch (line[0]) {

        // I: - Identifier Line (@see https://www.linuxjournal.com/files/linuxjournal.com/linuxjournal/articles/064/6429/6429l2.html)
        // Bus: Specifies the bus type (e.g., USB, PS/2).
        // Vendor: The vendor ID of the device.
        // Product: The product ID of the device.
        // Version: The version number of the device.
        case 'I':
            sscanf(line,
                   "I: Bus=%s Vendor=%s Product=%s Version=%s",
                   device->bus, device->vendor, device->product, device->version);
            break;

            // N: - Name Line
            // Name: The human-readable name of the device.
        case 'N':
            sscanf(line, "N: Name=\"%[^\"]\"", device->name);
            break;

            // P: - Physical Path Line
            // Phys: The physical connection path of the device.
        case 'P':
            sscanf(line, "P: Phys=%s", device->phys);
            break;

        // S: - Sysfs Path Line
        // Sysfs: The path to the device's entry in the sysfs filesystem.
        case 'S':
            if (line[1] == 'N') {
                device->snd = strtoul(line + 7, &endPtr, 16);
            } else if (line[1] == 'W') {
                device->sw = strtoul(line + 6, &endPtr, 16);
            } else {
                sscanf(line, "S: Sysfs=%s", device->sysfs);
            }
            break;

        // U: - Unique Identifier Line
        // Uniq: The unique identifier of the device (often empty).
        case 'U':
            sscanf(line, "U: Uniq=%s", device->uniq);
            break;

            // H: - Handlers Line
            // Handlers: The handlers associated with the device, typically corresponding to device nodes in /dev/input/.
        case 'H':
            sscanf(line, "H: Handlers=%[^\n]", device->handlers);
            break;

            // B: - Bitmaps Line
        case 'B':
            if (strncmp(line, "B: PROP=", 8) == 0) {            // PROP: Properties of the device.
                device->prop = strtoul(line + 8, &endPtr, 16);
            } else if (strncmp(line, "B: EV=", 6) == 0) {       // EV: Event types supported by the device.
                device->ev = strtoul(line + 6, &endPtr, 16);
            } else if (strncmp(line, "B: KEY=", 7) == 0) {      // KEY: Bitmap of supported keys.
                sscanf(line, "B: KEY=%s", device->key);
            } else if (strncmp(line, "B: REL=", 7) == 0) {      // REL: Bitmap of supported relative axes.
                device->rel = strtoul(line + 7, &endPtr, 16);
            } else if (strncmp(line, "B: ABS=", 7) == 0) {      // ABS: Bitmap of supported absolute axes.
                device->abs = strtoul(line + 7, &endPtr, 16);
            } else if (strncmp(line, "B: MSC=", 7) == 0) {      // MSC: Bitmap of supported miscellaneous events.
                device->msc = strtoul(line + 7, &endPtr, 16);
            } else if (strncmp(line, "B: LED=", 7) == 0) {      // LED: Bitmap of supported LEDs.
                device->led = strtoul(line + 7, &endPtr, 16);
            } else if (strncmp(line, "B: SND=", 7) == 0) {      // SND: Bitmap of supported sounds.
                device->snd = strtoul(line + 7, &endPtr, 16);
            } else if (strncmp(line, "B: FF=", 6) == 0) {       // FF: Bitmap of supported force feedback effects.
                device->ff = strtoul(line + 6, &endPtr, 16);
            } else if (strncmp(line, "B: SW=", 6) == 0) {       // SW: Bitmap of supported switches.
                device->sw = strtoul(line + 6, &endPtr, 16);
            }
            break;

        default:
            break;
    }

    // Check if the conversion was successful if endPtr was used
    if (endPtr != NULL && (*endPtr != '\0' && *endPtr != '\n')) {
        handleConversionError(line, file, head);
    }
}

ProcBusInputDevice *parseInputDevices() {
    // Open the PROC_BUS_INPUT_DEVICES file
    FILE *file = fopen(PROC_BUS_INPUT_DEVICES, "r");
    if (file == NULL) {
        printf("Error: %s\n", strerror(errno));
        return NULL;
    }

    // The /proc/bus/input/devices file is divided into sections, each representing a single input device.
    // Initialize a linked list to store the input devices
    ProcBusInputDevice *headDevice = NULL;          // Head of the linked list
    ProcBusInputDevice *currentDevice = NULL;       // Current device in the linked list

    // Read the PROC_BUS_INPUT_DEVICES file line by line
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {

        // If the line starts with 'I' it's the identifier line
        // It is the startKeylogger of a device block
        if (line[0] == 'I') {

            // Create a new device structure
            ProcBusInputDevice *device = createDevice();
            if (device == NULL) {
                fclose(file);
                freeInputDevices(headDevice);
                return NULL;
            }

            // Parse the identifier line
            parseLine(device, line, file, headDevice);

            // Add the device to the linked list
            if (headDevice == NULL) { // First device in the list
                headDevice = device;
            } else { // Add the device as the next device of the currentDevice device
                currentDevice->next = device;
            }
            currentDevice = device; // Update the currentDevice device

        } else if (currentDevice != NULL) {

            // Parse the rest of the lines for the currentDevice device
            parseLine(currentDevice, line, file, headDevice);
        }
    }

    // Close the file and return the headDevice of the linked list
    fclose(file);
    return headDevice;
}
