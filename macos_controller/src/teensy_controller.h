#pragma once

#include <string>
#include <vector>
#include <libserialport.h>

struct Color {
    uint8_t r, g, b;
    
    Color(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0) 
        : r(red), g(green), b(blue) {}
    
    // Convert to 24-bit RGB integer
    uint32_t toRGB() const {
        return (static_cast<uint32_t>(r) << 16) | 
               (static_cast<uint32_t>(g) << 8) | 
               static_cast<uint32_t>(b);
    }
};

class TeensyController {
public:
    TeensyController();
    ~TeensyController();
    
    // Connection management
    bool connect(const std::string& port_name);
    void disconnect();
    bool isConnected() const;
    
    // LED strip commands
    bool setStripLength(uint16_t length);
    bool fillLEDs(uint16_t first_led, uint16_t count, const Color& color);
    bool setLEDData(uint16_t first_led, const std::vector<Color>& colors);
    bool outputData();
    bool clearAll();
    
    // Info commands
    bool getVersion(uint8_t& major, uint8_t& minor);
    bool getMaxLEDs(uint16_t& max_leds);
    
    // Utility functions
    static std::vector<std::string> listPorts();
    
private:
    struct sp_port* port_;
    bool connected_;
    
    // Low-level communication
    bool sendByte(uint8_t byte);
    bool sendWord(uint16_t word);
    bool sendColor(const Color& color);
    bool readByte(uint8_t& byte, int timeout_ms = 1000);
    bool waitForAck(int timeout_ms = 1000);
    
    // Protocol commands
    enum Command {
        CMD_SET_LENGTH = 'L',
        CMD_FILL = 'F',
        CMD_RECEIVE_DATA = 'R',
        CMD_OUTPUT = 'O',
        CMD_CLEAR = 'C',
        CMD_VERSION = 'V',
        CMD_MAX_LEDS = 'M'
    };
};