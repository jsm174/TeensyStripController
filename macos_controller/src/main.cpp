#include "teensy_controller.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>

void printUsage() {
    std::cout << "Usage: teensy_led_controller [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --list-ports    List available serial ports\n";
    std::cout << "  --port <name>   Connect to specified port (e.g., /dev/cu.usbmodem123456)\n";
    std::cout << "  --demo          Run a color demo\n";
    std::cout << "  --help          Show this help\n";
    std::cout << "\nExample:\n";
    std::cout << "  teensy_led_controller --list-ports\n";
    std::cout << "  teensy_led_controller --port /dev/cu.usbmodem123456 --demo\n";
}

void listPorts() {
    auto ports = TeensyController::listPorts();
    std::cout << "Available serial ports:\n";
    for (const auto& port : ports) {
        std::cout << "  " << port << "\n";
    }
    if (ports.empty()) {
        std::cout << "  No ports found\n";
    }
}

void runDemo(TeensyController& controller) {
    std::cout << "Running LED demo...\n";
    
    // Get device info
    uint8_t major, minor;
    uint16_t max_leds;
    
    if (controller.getVersion(major, minor)) {
        std::cout << "Firmware version: " << static_cast<int>(major) << "." << static_cast<int>(minor) << "\n";
    }
    
    if (controller.getMaxLEDs(max_leds)) {
        std::cout << "Max LEDs per strip: " << max_leds << "\n";
    }
    
    // Set strip length to 35 LEDs for demo
    const uint16_t demo_length = 35;
    std::cout << "Setting strip length to " << demo_length << " LEDs\n";
    if (!controller.setStripLength(demo_length)) {
        std::cerr << "Failed to set strip length\n";
        return;
    }
    
    // Demo colors
    std::vector<Color> demo_colors = {
        Color(255, 0, 0),    // Red
        Color(0, 255, 0),    // Green  
        Color(0, 0, 255),    // Blue
        Color(255, 255, 0),  // Yellow
        Color(255, 0, 255),  // Magenta
        Color(0, 255, 255),  // Cyan
        Color(255, 255, 255), // White
        Color(0, 0, 0)       // Off
    };
    
    std::vector<std::string> color_names = {
        "Red", "Green", "Blue", "Yellow", "Magenta", "Cyan", "White", "Off"
    };
    
    // Clear all LEDs first
    std::cout << "Clearing all LEDs\n";
    controller.clearAll();
    controller.outputData();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Cycle through colors
    for (size_t i = 0; i < demo_colors.size(); i++) {
        std::cout << "Setting all LEDs to " << color_names[i] << "\n";
        
        // Fill all LEDs with current color
        if (controller.fillLEDs(0, demo_length, demo_colors[i])) {
            controller.outputData();
        } else {
            std::cerr << "Failed to set LEDs to " << color_names[i] << "\n";
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    // Rainbow pattern
    std::cout << "Creating rainbow pattern\n";
    std::vector<Color> rainbow_colors;
    for (int i = 0; i < demo_length; i++) {
        float hue = (i * 360.0f) / demo_length;
        
        // Simple HSV to RGB conversion for rainbow effect
        float c = 1.0f;
        float x = c * (1.0f - std::abs(std::fmod(hue / 60.0f, 2.0f) - 1.0f));
        float r = 0, g = 0, b = 0;
        
        if (hue < 60) { r = c; g = x; b = 0; }
        else if (hue < 120) { r = x; g = c; b = 0; }
        else if (hue < 180) { r = 0; g = c; b = x; }
        else if (hue < 240) { r = 0; g = x; b = c; }
        else if (hue < 300) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
        
        rainbow_colors.emplace_back(
            static_cast<uint8_t>(r * 100),  // Dimmed for safety
            static_cast<uint8_t>(g * 100),
            static_cast<uint8_t>(b * 100)
        );
    }
    
    if (controller.setLEDData(0, rainbow_colors)) {
        controller.outputData();
    } else {
        std::cerr << "Failed to set rainbow pattern\n";
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    
    // Crazy fast color cycling
    std::cout << "Going CRAZY with fast color cycling!\n";
    std::vector<Color> crazy_colors = {
        Color(255, 0, 0),    // Red
        Color(0, 255, 0),    // Green  
        Color(0, 0, 255),    // Blue
        Color(255, 255, 0),  // Yellow
        Color(255, 0, 255),  // Magenta
        Color(0, 255, 255),  // Cyan
        Color(255, 128, 0),  // Orange
        Color(128, 0, 255),  // Purple
        Color(255, 192, 203), // Pink
        Color(0, 255, 128),  // Spring Green
        Color(255, 255, 255), // White
        Color(0, 0, 0)       // Off
    };
    
    // Fast cycling for 10 seconds
    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(10);
    int color_index = 0;
    
    while (std::chrono::steady_clock::now() < end_time) {
        // Fill all LEDs with current crazy color
        controller.fillLEDs(0, demo_length, crazy_colors[color_index]);
        controller.outputData();
        
        // Move to next color
        color_index = (color_index + 1) % crazy_colors.size();
        
        // Very fast - 50ms per color change
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Strobe effect - rapid on/off
    std::cout << "STROBE MODE!\n";
    Color strobe_color(255, 255, 255); // Bright white
    Color off_color(0, 0, 0);          // Off
    
    for (int i = 0; i < 20; i++) {
        // Flash on
        controller.fillLEDs(0, demo_length, strobe_color);
        controller.outputData();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Flash off
        controller.fillLEDs(0, demo_length, off_color);
        controller.outputData();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Crazy random individual LED colors
    std::cout << "Random LED chaos!\n";
    srand(static_cast<unsigned>(time(nullptr))); // Seed random number generator
    
    for (int cycle = 0; cycle < 50; cycle++) {
        std::vector<Color> random_colors;
        
        // Generate random color for each LED
        for (int led = 0; led < demo_length; led++) {
            random_colors.emplace_back(
                rand() % 256,  // Random red
                rand() % 256,  // Random green
                rand() % 256   // Random blue
            );
        }
        
        controller.setLEDData(0, random_colors);
        controller.outputData();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Turn off all LEDs
    std::cout << "Calming down... turning off all LEDs\n";
    controller.clearAll();
    controller.outputData();
    
    std::cout << "Demo complete! Hope that was CRAZY enough! 🎉\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    bool list_ports_flag = false;
    bool demo_flag = false;
    std::string port_name;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--list-ports") {
            list_ports_flag = true;
        } else if (arg == "--demo") {
            demo_flag = true;
        } else if (arg == "--port" && i + 1 < argc) {
            port_name = argv[++i];
        } else if (arg == "--help") {
            printUsage();
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            printUsage();
            return 1;
        }
    }
    
    if (list_ports_flag) {
        listPorts();
        return 0;
    }
    
    if (port_name.empty()) {
        std::cerr << "Port name required for demo mode\n";
        printUsage();
        return 1;
    }
    
    TeensyController controller;
    
    std::cout << "Connecting to " << port_name << "...\n";
    if (!controller.connect(port_name)) {
        std::cerr << "Failed to connect to " << port_name << "\n";
        return 1;
    }
    
    if (demo_flag) {
        runDemo(controller);
    }
    
    return 0;
}