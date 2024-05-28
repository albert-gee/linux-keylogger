#ifndef PROC_BUS_INPUT_DEVICE_PROCESSOR_H
#define PROC_BUS_INPUT_DEVICE_PROCESSOR_H

/**
 * Get the event handlers of input devices that support the specified event types.
 * The list of event types can be found in /usr/include/linux/input-event-codes.h
 * @param eventTypes - an array of event types to search for.
 * @param numEventTypes - the number of event types in the array.
 * @return
 */
char *getInputDeviceEventHandlers(const int eventTypes[], int numEventTypes);

#endif //PROC_BUS_INPUT_DEVICE_PROCESSOR_H
