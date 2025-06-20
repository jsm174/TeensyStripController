# Teensy LED Controller - macOS Client

A C++ program to control LED strips connected to a Teensy LC running the TeensyStripController firmware.

## What's New

This version has been updated from the original TeensyStripController to support the **DOF-compatible protocol**:

- **Updated Teensy Firmware**: The Arduino sketch (`src/main.cpp`) now uses flat RGB byte arrays instead of individual RGB values, making it compatible with the Direct Output Framework
- **macOS Controller**: Fully implements the new protocol with proper RGB data transmission
- **Verified Compatibility**: Both Teensy and macOS controller use the same command set and data format

## Features

- **Serial Communication**: Uses libserialport for cross-platform serial communication
- **Protocol Support**: Full implementation of TeensyStripController protocol
- **Demo Mode**: Built-in color cycling and rainbow pattern demo
- **Device Info**: Query firmware version and LED limits

## Building

```bash
cd macos_controller
mkdir build && cd build
cmake ..
make
```

## Usage

### List Available Serial Ports
```bash
./teensy_led_controller --list-ports
```

### Run LED Demo
```bash
./teensy_led_controller --port /dev/cu.usbmodem104409301 --demo
```

### Help
```bash
./teensy_led_controller --help
```

## Protocol Commands

The program implements all TeensyStripController commands:

- **Set Strip Length**: Configure number of LEDs per strip
- **Fill LEDs**: Set a range of LEDs to a single color
- **Set LED Data**: Set individual LED colors
- **Output Data**: Send buffered data to LED strips
- **Clear All**: Turn off all LEDs
- **Get Version**: Query firmware version
- **Get Max LEDs**: Query maximum LEDs per strip

## Example Usage in Code

```cpp
#include "teensy_controller.h"

TeensyController controller;
if (controller.connect("/dev/cu.usbmodem104409301")) {
    // Set 10 LEDs per strip
    controller.setStripLength(10);
    
    // Fill first 5 LEDs with red
    Color red(255, 0, 0);
    controller.fillLEDs(0, 5, red);
    
    // Output to strips
    controller.outputData();
}
```

## LED Strip Connections

Connect your 12V LED strips to the Teensy LC:
- **Pin 2**: LED Strip #1 (data signal through 100Ω resistor)
- **12V Supply**: Connect to LED strip VCC (separate power supply)
- **Common Ground**: Connect Teensy GND to LED strip GND

## Dependencies

- **libserialport**: Located at `/Users/jmillard/libdof/third-party/runtime-libs/macos/arm64/`
- **CMake 3.16+**
- **C++17 compiler**

## Customizing for Your LED Strip

### Setting the Number of LEDs

The demo is configured for 35 LEDs by default. To change this for your strip:

1. Open `src/main.cpp` in your IDE
2. Find line 47: `const uint16_t demo_length = 35;`
3. Change `35` to your strip's LED count
4. Rebuild: `make` in the build directory

### Protocol Commands Used

The demo uses the DOF-compatible protocol commands:
- `L` - Set strip length
- `F` - Fill LEDs with solid color  
- `R` - Set individual LED data (RGB byte arrays)
- `O` - Output buffered data to strips
- `C` - Clear all LEDs
- `V` - Get firmware version
- `M` - Get max LEDs per strip

## Demo Features

The demo mode cycles through:
1. **Color Cycle**: Red → Green → Blue → Yellow → Magenta → Cyan → White → Off
2. **Rainbow Pattern**: Each LED shows a different color of the rainbow  
3. **Fast Color Cycling**: Rapid color changes for 10 seconds
4. **Strobe Effect**: Bright white flashing
5. **Random Chaos**: Each LED gets a random color
6. **Clear**: Turns off all LEDs

Perfect for testing your LED strip setup!

## Testing Your Setup

1. **Build the project**:
   ```bash
   cd macos_controller
   mkdir build && cd build
   cmake ..
   make
   ```

2. **Find your Teensy**:
   ```bash
   ./teensy_led_controller --list-ports
   ```

3. **Run the demo**:
   ```bash
   ./teensy_led_controller --port /dev/cu.usbmodem104409301 --demo
   ```

The demo will confirm your Teensy firmware version and LED count, then run through all the visual effects on your entire strip.