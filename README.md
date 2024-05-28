
# Keylogger for Linux


## Overview

### Human Interface Devices

**Universal Serial Bus** (USB) is a communications architecture that allows a personal computer to communicate with different devices using a simple four-wire cable.

The Universal Serial Bus Specification defines device **classes of USB devices** [10]. A device may belong to one or more classes. Some examples of USB device classes are the following [8]:
- Display
- Communication
- Audio
- Mass storage
- HID

**Human Interface Devices** (HID) are used to interact with computers, such as mice, touchpads, tablets, or microphones [5]. The **USB HID Class** [8] describes USB HID devices.

All modern operating systems recognize standard USB HID devices without needing a specialized driver [6]. Computers can set up USB devices at startup or when the devices are plugged in.

The HID protocol for USB has been implemented on many buses other than USB, including Bluetooth and I2C.

HID devices communicate with the host computers through messages called **reports**. There are 3 report types [7]:
- *Input Report*: data sent from HID devices to hosts (typically when the state of a control changes)
- *Output Report*: data sent from hosts to HID device (for example to the LEDs on a keyboard)
- *Feature Report*: data that can be manually read and written, and are typically related to configuration information.

For example, a mouse sends IN reports about movements in X and Y directions. A keyboard sends IN reports about pressed regular and modifier keys. Keyboards can also receive OUT reports from the host computer to control LED indicators like the shift-lock.

**Report descriptors** are binary data supplied to the host computer by connected devices when plugged in. A report descriptor informs the computer about its device type and the specifics of the reports it sends and receives.

**HID usage tables** that are part of the report descriptors describe what HID devices are allowed to do. They contain a list with descriptions of Usages describing the intended meaning and use of a particular item in the report descriptor. 

For example, a usage is defined for the left button of a mouse. The report descriptor can define where in a report an application can find the current state of the mouse's left button. 

The usage tables are broken up into several name spaces, called **Usage Pages**. Each usage page describes a set of related usages to help organize the document. The combination of a usage and a usage page define the usage ID that uniquely identifies a specific usage in the usage tables [7].

### Keyboard Input Processing

A **scan code** is the data encapsulated within an IN report and sent from a keyboard to a computer through a USB interface to report which key has been pressesed or released. 

The computer's USB controller receives the report and generates a keyboard interrupt to allow the CPU to start the **Interrupt handler** from the Linux kernel (or **Interrupt Service Routine** from Windows kernel). This handler retrieves the scan code from the keyboard controller, translates it into a keycode, and makes it available to user programs through mechanisms like the Linux Input Subsystem which is a collection of drivers for input devices.

In a GUI-based system, the X server grabs keycodes from the kernel, transforms them into key symbols and metadata, and sends events to GUI applications. This layering allows the Linux kernel to ensure the locale and keyboard map settings are applied correctly. 

### Linux Kernel Input Subsystem

Input subsystem is a collection of drivers that is designed to support all input devices under Linux.

The main data structures and APIs of  the Input subsystem can be found at */usr/include/linux/input.h* file [9]. 

#### Input Devices

Every input device connected to the system is represented as a */dev/input/eventX* character device.

The */proc/bus/input/devices* virtual file contains detailed information about all the input devices. By parsing the data within this file, we can gain valuable insights into the hardware configurations associated with input devices.

For example, as we can see on the picture above, the list of handlers of the keyboard device includes event8. This handler represents a character device /dev/input/eventX that we can read.

We can confirm it by using the udevadm tool that displays information about the device using this event handler:

```bash
$ udevadm info /dev/input/event8
```

Another tool allows us to print the keys that are supported by the device, is evtest. It can also catch and print events when they are reported by the device. 

```bash
$ evtest /dev/input/event8
```

An input device is represented in the kernel as an instance of the struct input_dev. It includes the following fields:
- `name` represents the name of the device.
- `phys` is the physical path to the device in the system hierarchy.
- `evbit` is a bitmap of the types of events that are supported by the device. The list of events is available in the kernel source in the `/usr/include/linux/input-event-codes.h` file.
- `keybit` is for `EV_KEY` enabled devices and consists of a bitmap of keys/buttons that this device exposes; for example, `BTN_0`, `KEY_A`, `KEY_B`, and so on. The complete list of keys/buttons can be found in the `/usr/include/linux/input-event-codes.h` file.
- `relbit` is for `EV_REL` enabled devices and consists of a bitmap of relative axes for the device; for example, `REL_X`, `REL_Y`, `REL_Z`, and so on. Have a look at `/usr/include/linux/input-event-codes.h` for the complete list.
- `key` reflects the current state of the device's keys/buttons.
- `dev` is the `struct device` associated with this device (for device model).

The list of HID devices can also be found at `/sys/bus/hid/devices/`. We can read report descriptors for each device:

```bash
$ hexdump -C /sys/bus/hid/devices/0003\:093A\:2510.0002/report_descriptor
```

#### Input Events

We can read events from a character device at `/dev/input/event<X>` from the user space. The event packets are in the following format:

```c
struct input_event {
    struct timeval time;
    __u16 type;
    __u16 code;
    __s32 value;
}
```

The struct has the following elements:
- `time` is a timestamp that corresponds to the time when the event happened.
- `type` is the event type; for example, `EV_KEY` for a keypress or release, `EV_REL` for a relative moment, or `EV_ABS` for an absolute one. More types are defined in `/usr/include/linux/input-event-codes.h`.
- `code` is the event code; for example, `REL_X` or `KEY_BACKSPACE`. Again, a complete list can be found in `/usr/include/linux/input-event-codes.h`.
- `value` is the value that the event carries. For an `EV_REL` event type, it carries the relative change. For an `EV_ABS` (joysticks and so on) event type, it contains the absolute new value. For an `EV_KEY` event type, it is set to 0 for a key release, 1 for a keypress, and 2 for auto-repeat.


### Keyloggers

Keyloggers are a type of software that record (secretly in the case of malware) keystrokes typed by a user on their keyboard.

Keyloggers pose a serious threat to computer security and privacy because they can silently collect personal information like passwords and credit card numbers without the user's knowledge. They are difficult to detect by antiviruses and run in secret, making the user vulnerable to identity theft [1].

There are four main types of keyloggers:
1. **Hardware keyloggers:** Physical devices that sit between your keyboard and computer. They can be connected directly or installed inside the keyboard itself, making them hard to spot.
2. **Acoustic keyloggers:** Listen to the sound of your keystrokes using special equipment like parabolic microphones, even from a distance.
3. **Wireless keyloggers:** Use Bluetooth to capture keystrokes from wireless keyboards, but they need a receiver nearby to pick up the signals.
4. **Software keyloggers:** Work by intercepting data between your keyboard and computer, collecting keystrokes, and sending them to a remote location. They're often hidden in your system and can be hard to detect.

The following techniques can be used to develop keyloggers for Linux:
- **Keyboard Event Intercepting:** Monitoring the corresponding device files (`/dev/input/eventX`).
- **X Window System Hooks** [3]: Hooking into the X server or employing libraries like Xlib to monitor input events.
- **Kernel-Level Logging:** Developing a kernel module that hooks into the input subsystem.
- **System Call Interception:** Intercepting system calls like `read()` or `ioctl()`, which allows the attacker to monitor keyboard activity without modifying the kernel.

## Keylogger for Linux

Our Keylogger monitors event devices (*/dev/input/eventX*) and intercept keyboard events. This method allows us to capture keystrokes at a low level, regardless of the application being used, but it requires elevated privileges or root access to access the device files.

### Program Overview

Our program consists of the following modules:
1. **proc_bus_input_devices module:** This module parses the `/proc/bus/input/devices`, detect connected HID devices, and get event handlers of devices of specific types.
2. **dev_input_events_listener module:** This module listens for input events from the devices of specified event handlers and pass them to the `ProcessEventCallback` function.
3. **keylogger module:** Contains an implementation of `ProcessEventCallback` that receives keyboard events, extract the keys, and log them into a file.

All three modules are implemented in C. Each module is designed for easy replacement. For instance, a mouse logger can replace the keylogger, or event file determination can be implemented without relying on the proc file system.

#### proc_bus_input_devices

The file /proc/bus/input/devices holds detailed information regarding all input devices recognized by the system.

The proc_bus_input_devices module reads the file and parses the devices into a linked list of ProcBusInputDevice structs. It interprets lines based on their prefixes ('I', 'N', 'P', 'S', 'U', 'H', 'B').  Each node within this linked list encapsulates information about a single input device.

Then, the program iterates through the linked list and retrieves event handlers of input devices that support the event types that we specify when use this module.

The program processes the `EV` property of each device. This value is hexadecimal, e.g., for a keyboard, it can be 120013 or 12001f. The program uses bitwise operations to convert it into a bitmask of supported event types from `/usr/include/linux/input-event-codes.h` [13]. The following are some events we are interested in:
- **EV_KEY:** For devices that support sending key events (for example, keyboards, buttons, and so on).
- **EV_REL:** For devices that support sending relative positions (for example, mice, digitizers, and so on).
- **EV_ABS:** For devices that support sending absolute positions (for example, joysticks).

Upon locating a qualifying device, the program retrieves the associated event handler (e.g., `eventX`) linked with the keyboard. The collected event handlers are returned as a concatenated string.

Another approach would involve checking the Handlers of each device entry to determine if it contains both the string "kbd" (typically indicates a keyboard) and “sysrq” (Magic SysRq) [12]. Devices other than keyboards may use the “kbd” handler.

#### dev_input_events_listener

This module monitors input events from specified event handlers in the */dev/input/* directory and processes these events using a callback function. It first constructs the full paths to the event files, opens them, and then uses the poll system call to listen for events on these file descriptors. When an event is detected, it reads the event and processes it using a user-provided callback function.

Since a system can utilize more than one keyboard, we need to monitor multiple file descriptors. We use “*poll*” system calls to efficiently wait for events on multiple file descriptors without having to constantly check each one.

#### keylogger

We use the `input_event` structure defined in `/usr/include/linux/input.h` which encapsulates events from keyboards [9]:

```c
struct input_event {
    struct timeval time; // time that the event was generated
    unsigned short type; // type of event occurred
    unsigned short code; // code of pressed key
    unsigned int value; // a subtype of event
};
```

For a keylogger, we are only interested in keyboard events that have a type of "EV_KEY". The values (subtypes) of such events can be 0 for "key release", 1 for "key press", or 2 for “key repeat”.

The code values aren't direct translations to ASCII characters. For instance, the pressed key “a” corresponds to a code value of KEY_A, which is 30 in decimal. However, the ASCII value for “a” is 61. An array of KeyCode structures maps key codes to their corresponding key names. The decodeKey function retrieves the key name for a given key code. The following Key Codes are supported:

- **Standard Keys**: Includes key codes for alphanumeric keys, function keys, control keys, and others.
- **Custom Keys**: Defines custom codes for special keys, like <RIGHTUNSHIFT> and <LEFTUNSHIFT> for shift key releases.

The full list of key codes can be found at */usr/include/linux/input-event-codes.h*.

The *logKeyToFile* function appends standard keys and names of custom keys to a log file.

### Compiling and Running the Program

1. Create a build directory and navigate into it:
```bash
mkdir build
cd build
```

2. Run CMake to generate the build files:
```bash
cmake ..
```

3. After the build files are generated, compile the program using make:
```bash
make
```

4. Run the program:
```bash
sudo ./Assigment3
```

The output will be generated in the *build/keylog.txt* file.

## Legal and Ethical Implications

The use of keyloggers raises significant legal and ethical concerns regarding user privacy, data protection, and cybersecurity. 

From a legal standpoint, the use of keyloggers without explicit consent violates privacy laws in many jurisdictions. Collection of sensitive information, such as passwords and financial data, may constitute identity theft or fraud, leading to severe legal repercussions for perpetrators.

Ethically, the use of keyloggers raises questions about individual autonomy and informed consent. Employers, for instance, may be tempted to deploy keylogging software to monitor employee activities without their knowledge, infringing upon their privacy rights. Similarly, individuals may install keyloggers on shared computers or devices to surreptitiously monitor the online activities of family members or acquaintances, violating their trust and autonomy. 

## References

[1] Y. Abukar, M. A. Maarof, F. M. Hassan, and A. S. Mohamed, “Survey of Keylogger Technologies,” ResearchGate, Feb. 2014, [Online]. Available: https://www.researchgate.net/publication/309230926_Survey_of_Keylogger_Technologies (accessed May 14, 2024).  

[2] Wikipedia contributors, “Virtual keyboard,” Wikipedia, Apr. 15, 2024. https://en.wikipedia.org/wiki/Virtual_keyboard (accessed May 14, 2024).

[3] B. Day, “Hacker's Corner: Complete Guide to Keylogging in Linux - Part 1,” Linux Security, Apr. 25, 2024. https://linuxsecurity.com/features/complete-guide-to-keylogging-in-linux-part-1

[4] “Custom HID devices in CircuitPython,” Adafruit Learning System, Oct. 01, 2021. https://learn.adafruit.com/custom-hid-devices-in-circuitpython/report-descriptors (accessed May 20, 2024).

[5]  “Introduction to HID report descriptors — the Linux Kernel documentation.” https://kernel.org/doc/html/latest/hid/hidintro.html (accessed May 20, 2024).

[6] Wikipedia contributors, “Human interface device,” Wikipedia, Dec. 24, 2023. https://en.wikipedia.org/wiki/Human_interface_device (accessed May 20, 2024).

[7] Mhopkins-Msft, “Introduction to Human Interface Devices (HID) - Windows drivers,” Microsoft Learn, Dec. 08, 2023. https://learn.microsoft.com/en-us/windows-hardware/drivers/hid/

[8] M. Bergman et al., “Device Class Definition for Human Interface Devices (HID),” Jun. 2001. Accessed: May 21, 2024. [Online]. Available: https://usb.org/sites/default/files/hid1_11.pdf

[9] J. Madieu, “Linux Device Driver Development - Second Edition,” O’Reilly Online Learning. https://www.oreilly.com/library/view/linux-device-driver/9781803240060

[10] “Universal Serial Bus Specification,” in 4.8 USB Devices, Revision 2.0., Compaq Computer Corporation, Hewlett-Packard Company, Intel Corporation, Lucent Technologies Inc, Microsoft Corporation, NEC Corporation, Koninklijke Philips Electronics N.V., 2000, p. 22. Accessed: May 22, 2024. [Online]. Available: https://www.usb.org/document-library/usb-20-specification

[11] “Read keyboard inputs in Linux,” Unix & Linux Stack Exchange, Apr. 21, 2019. https://unix.stackexchange.com/questions/513655/read-keyboard-inputs-in-linux (accessed May 25, 2024).

[12] Wikipedia contributors, “Magic SysRq key,” Wikipedia, Jan. 10, 2024. https://en.wikipedia.org/wiki/Magic_SysRq_key

[13] “Explain EV in /proc/bus/input/devices data,” Unix & Linux Stack Exchange, May 06, 2013. https://unix.stackexchange.com/questions/74903/explain-ev-in-proc-bus-input-devices-data (accessed May 27, 2024).
