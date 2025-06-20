# TeensyStripController - DOF Compatible Version

Firmware for a Teensy LC/3.1/3.2 to control WS2811/WS2812 based LED strips. **Updated to be fully compatible with the Direct Output Framework (DOF)** using flat RGB byte arrays.

![Teensy 3.1 with OctoWS2811 adaptor](http://www.pjrc.com/store/octo28_adaptor_6.jpg)

## What's New in This Version

This version has been **updated from the original TeensyStripController** to support the DOF-compatible protocol:

- **Enhanced Teensy Firmware**: The Arduino sketch now uses flat RGB byte arrays instead of individual RGB values, making it fully compatible with the Direct Output Framework
- **macOS Controller**: Complete C++ implementation of the protocol with comprehensive demo
- **Verified Compatibility**: Both Teensy firmware and macOS controller use the same command set and data format
- **PlatformIO Support**: Modern build system with automatic dependency management

## Hardware Support

### Teensy Models
- **Teensy LC**: 80 LEDs per strip maximum (640 total) - optimized for 8KB RAM
- **Teensy 3.1/3.2**: 1100 LEDs per strip maximum (8800 total)

### LED Strip Connections
- **8 LED strips** supported simultaneously
- **Pin assignments**: 2, 14, 7, 8, 6, 20, 21, 5
- **100Ω resistor** required on each data line

## Quick Start

### 1. Build and Upload Teensy Firmware

#### Using PlatformIO (Recommended)
```bash
# Install PlatformIO if not already installed
pip install platformio

# Upload to Teensy LC
platformio run --target upload --environment teensylc

# Upload to Teensy 3.1
platformio run --target upload --environment teensy31

# Upload to Teensy 3.2
platformio run --target upload --environment teensy32
```

#### Using Arduino IDE
1. Install [Arduino IDE](https://www.arduino.cc/en/software) and [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html)
2. Open `src/main.cpp` in Arduino IDE
3. Select your Teensy model from Tools → Board
4. Click Upload

### 2. Test with macOS Controller

```bash
# Build the controller
cd macos_controller
mkdir build && cd build
cmake ..
make

# Find your Teensy
./teensy_led_controller --list-ports

# Run the demo (replace with your port)
./teensy_led_controller --port /dev/cu.usbmodem123456 --demo
```

## Hardware Setup

### Required Connections
```
Teensy Pin 2 → [100Ω resistor] → LED Strip Data
12V Power Supply + → LED Strip VCC (DO NOT connect to Teensy!)
12V Power Supply - → LED Strip GND
Teensy GND → LED Strip GND (common ground required)
```

### All 8 Pin Assignments
- **Pin 2**: LED Strip #1 ⭐ (primary/tested)
- **Pin 14**: LED Strip #2
- **Pin 7**: LED Strip #3
- **Pin 8**: LED Strip #4
- **Pin 6**: LED Strip #5
- **Pin 20**: LED Strip #6
- **Pin 21**: LED Strip #7
- **Pin 5**: LED Strip #8

### Power Requirements
- **Every LED**: Up to 60mA current draw
- **Example**: 60 LEDs = 3.6A minimum power supply
- **Use thick wires** for power connections
- **Inject power** every 100-200 LEDs
- **Safety**: Incorrect wiring can cause fire hazard

## Protocol Commands

The DOF-compatible protocol uses these serial commands:

| Command | Description | Data Format |
|---------|-------------|-------------|
| `L` | Set LED strip length | `word: length` |
| `F` | Fill area with color | `word: first_led, word: count, 3_bytes: RGB` |
| `R` | Set LED data | `word: first_led, word: count, RGB_array` |
| `O` | Output data to strips | none |
| `C` | Clear all LEDs | none |
| `V` | Get firmware version | returns: `byte: major, byte: minor` |
| `M` | Get max LEDs per strip | returns: `word: max_leds` |

All commands return `A` (ACK) on success or `N` (NACK) on error.

## macOS Controller Features

### Command Line Usage
```bash
# List available serial ports
./teensy_led_controller --list-ports

# Run comprehensive demo
./teensy_led_controller --port /dev/cu.usbmodem123456 --demo

# Show help
./teensy_led_controller --help
```

### Demo Features
1. **Device Info**: Shows firmware version and LED limits
2. **Color Cycle**: Red → Green → Blue → Yellow → Magenta → Cyan → White → Off
3. **Rainbow Pattern**: Each LED shows a different rainbow color
4. **Fast Color Cycling**: Rapid color changes for 10 seconds
5. **Strobe Effect**: Bright white flashing
6. **Random Chaos**: Each LED gets a random color
7. **Cleanup**: Turns off all LEDs

### Customizing LED Count
To change the demo for your LED strip count:

1. Open `macos_controller/src/main.cpp`
2. Find line 47: `const uint16_t demo_length = 35;`
3. Change `35` to your LED count
4. Rebuild: `make` in the build directory

### Programming Example
```cpp
#include "teensy_controller.h"

TeensyController controller;
if (controller.connect("/dev/cu.usbmodem123456")) {
    // Set strip length
    controller.setStripLength(50);
    
    // Fill first 10 LEDs with red
    Color red(255, 0, 0);
    controller.fillLEDs(0, 10, red);
    
    // Set individual LED colors
    std::vector<Color> colors = {
        Color(255, 0, 0),    // Red
        Color(0, 255, 0),    // Green
        Color(0, 0, 255)     // Blue
    };
    controller.setLEDData(10, colors);
    
    // Output to strips
    controller.outputData();
}
```

## Configuration Options

### Teensy Firmware (`src/main.cpp`)
```cpp
// Maximum LEDs per strip (adjust for your Teensy model)
#define MaxLedsPerStrip 80      // Teensy LC: 80, Teensy 3.x: 1100

// Default strip length
word configuredStripLength = 80;

// Firmware version
#define FirmwareVersionMajor 2
#define FirmwareVersionMinor 0
```

## Built-in Test Mode

Ground **pin 17** during startup to run the built-in hardware test:
- Cycles through colors: Red → Green → Blue → Yellow → Pink → Orange → White → Off  
- Tests all connected strips simultaneously
- Each color displays for 3 seconds
- Built-in LED indicates test progress

## PlatformIO Configuration

The included `platformio.ini` supports multiple environments:

```ini
[env:teensylc]
platform = teensy
board = teensylc
framework = arduino
lib_deps = 
    paulstoffregen/OctoWS2811

[env:teensy31]
platform = teensy
board = teensy31
framework = arduino
lib_deps = 
    paulstoffregen/OctoWS2811

[env:teensy32]
platform = teensy
board = teensy32
framework = arduino
lib_deps = 
    paulstoffregen/OctoWS2811
```

## Memory Usage

| Program | RAM Usage | Flash Usage |
|---------|-----------|-------------|
| **Main Controller (Teensy LC)** | 70.4% (5,764 bytes) | 16.6% |
| **Main Controller (Teensy 3.x)** | ~20% | ~15% |

## Troubleshooting

### LED Strip Not Responding
1. **Check power supply**: Separate 12V supply required
2. **Verify connections**: Data pin through 100Ω resistor
3. **Common ground**: Teensy GND to LED strip GND
4. **Level shifting**: May need 3.3V→5V converter for some strips

### Serial Communication Issues
1. **Check port name**: Use `--list-ports` to find correct port
2. **Try different baud rates**: Default is 9600 (USB speed auto-negotiated)
3. **Reset Teensy**: Unplug and reconnect USB

### Memory Issues (Teensy LC)
1. **Reduce LED count**: Maximum 80 per strip
2. **Use fewer strips**: Total memory shared among all strips

## Dependencies

### Teensy Firmware
- **OctoWS2811 Library**: Automatically installed by PlatformIO
- **Arduino/Teensyduino**: For Arduino IDE compilation

### macOS Controller
- **libserialport**: Located at `/Users/jmillard/libdof/third-party/runtime-libs/macos/arm64/`
- **CMake 3.16+**
- **C++17 compiler**

## Direct Output Framework Integration

This controller is fully compatible with DOF R3+. Configure in DOF as:
- **Controller Type**: TeensyStripController
- **Port**: Your Teensy's serial port
- **LED Configuration**: Match your physical strip layout

## Integrated Products

The **Oak Micros Pinball Addressable LEDs (PAL)** board is a pre-built solution using this firmware:
- Teensy 3.2 with screw terminals
- Built-in test button
- 6 main LED outputs + 2 header outputs
- [User Guide](https://drive.google.com/open?id=1Zk_5RxsWX4VIPhT4XtGlj1rNlBTug39a) | [VPForums Thread](https://www.vpforums.org/index.php?showtopic=43482)

![Oak Micros PAL board](http://vpforums.org/imghost/24/pal_board.jpg)

## Performance Notes

**Update frequency estimation**:
- **Time per LED**: ~30 microseconds
- **Example**: 500 LEDs = 15ms = ~66 Hz maximum
- **Recommendation**: Stay above 30 Hz for smooth animation

## License

See LICENSE file in the repository.

## Links

- **Documentation**: [Project Wiki](https://github.com/DirectOutput/TeensyStripController/wiki)
- **Releases**: [Compiled Firmware](https://github.com/DirectOutput/TeensyStripController/releases)
- **DOF Integration**: [DOF Documentation](http://directoutput.github.io/DirectOutput/)