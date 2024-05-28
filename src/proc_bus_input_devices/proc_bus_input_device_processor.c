#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "../../include/proc_bus_input_devices/proc_bus_input_devices_parser.h"

char *getInputDeviceEventHandlers(const int eventTypes[], int numEventTypes) {

    // Parse the input devices from /proc/bus/input/devices
    ProcBusInputDevice *devices = parseInputDevices();
    if (devices == NULL) {
        printf("Failed to parse input devices\n");
        return NULL;
    }

    // Allocate memory for the array of event handlers
    size_t bufferSize = 256;
    char *eventHandlers = malloc(bufferSize);
    if (eventHandlers == NULL) {
        printf("Failed to allocate memory for event handlers\n");
        free(devices);
        free(eventHandlers);
        return NULL;
    }

    // Initialize to an empty string
    eventHandlers[0] = '\0';

    // Iterate over the linked list of devices
    ProcBusInputDevice *current = devices;
    while (current != NULL) {

        // Check if all elements of eventTypes[] are present in current->ev
        int allTypesPresent = 1; // Flag to check if all event types are present
        for (int i = 0; i < numEventTypes; i++) {
            // Use bitwise operations to retrieve the event types from the EV field
            if (!(current->ev & (1 << eventTypes[i]))) {
                allTypesPresent = 0; // Set flag to 0 if any type is missing
                break;
            }
        }

        // If all event types are present
        if (allTypesPresent) {

            // Get event handler
            char *eventHandler = strstr(current->handlers, "event");
            if (eventHandler != NULL) {

                // Remove space and events to the right
                char *space = strchr(eventHandler, ' ');
                if (space != NULL) {
                    *space = '\0';
                }

                // Ensure enough space in buffer
                size_t requiredSize = strlen(eventHandlers) + strlen(eventHandler) + 2; // +2 for '\n' and '\0'
                if (requiredSize > bufferSize) {
                    bufferSize += 128;
                    char *temp = realloc(eventHandlers, bufferSize);
                    if (temp == NULL) {
                        printf("Failed to reallocate memory for event handlers\n");
                        free(eventHandlers);
                        // Free the allocated memory for devices
                        while (devices != NULL) {
                            current = devices;
                            devices = devices->next;
                            free(current);
                        }
                        return NULL;
                    }
                    eventHandlers = temp;
                }

                // Append the event handler to the list
                strcat(eventHandlers, eventHandler);
                strcat(eventHandlers, "\n");
            }
        }

        current = current->next;
    }

    free(devices);

    return eventHandlers;
}