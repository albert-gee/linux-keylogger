#ifndef DEV_INPUT_EVENTS_LISTENER_H
#define DEV_INPUT_EVENTS_LISTENER_H

/**
 * Monitor the file descriptors for events.
 * @param pollingRequests The array of file descriptors to monitor.
 * @param count The number of file descriptors to monitor.
 * @param processEventCallback The callback function to process the input event.
 * @return 1 if the function executed successfully, 0 otherwise.
 */
typedef int (*ProcessEventCallback)(struct input_event ev);

/**
 * Get the file descriptor of the input event handler.
 * @param eventHandler The event handler to get the file descriptor for, e.g. event8.
 * @return The file descriptor of the input event handler.
 */
int getFileDescriptorByEventHandler(char *eventHandler);

/**
 * Listen for input events from the devices of specified event handlers.
 * @param eventHandlers The event handlers to listen for input events from.
 * @param processEventCallback The callback function to process the input event.
 */
void listen_dev_input_events(char *eventHandlers, ProcessEventCallback processEventCallback);

#endif //DEV_INPUT_EVENTS_LISTENER_H
