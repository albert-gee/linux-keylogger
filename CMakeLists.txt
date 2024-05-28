cmake_minimum_required(VERSION 3.28)
project(Keylogger C)

set(CMAKE_C_STANDARD 23)

add_executable(Keylogger main.c
        include/proc_bus_input_devices/proc_bus_input_devices_parser.h
        include/proc_bus_input_devices/proc_bus_input_device_processor.h
        include/dev_input_events/dev_input_events_listener.h
        include/keylogger.h

        src/proc_bus_input_devices/proc_bus_input_device_processor.c
        src/proc_bus_input_devices/proc_bus_input_device_parser.c
        src/dev_input_events/dev_input_events_listener.c
        src/keylogger.c
)
