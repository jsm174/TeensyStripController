#include "teensy_controller.h"
#include <iostream>
#include <chrono>
#include <thread>

TeensyController::TeensyController() : port_(nullptr), connected_(false) {
}

TeensyController::~TeensyController() {
    disconnect();
}

bool TeensyController::connect(const std::string& port_name) {
    if (connected_) {
        disconnect();
    }
    
    // Get port by name
    enum sp_return result = sp_get_port_by_name(port_name.c_str(), &port_);
    if (result != SP_OK) {
        std::cerr << "Error finding port " << port_name << std::endl;
        return false;
    }
    
    // Open port
    result = sp_open(port_, SP_MODE_READ_WRITE);
    if (result != SP_OK) {
        std::cerr << "Error opening port " << port_name << std::endl;
        sp_free_port(port_);
        port_ = nullptr;
        return false;
    }
    
    // Configure port: 9600 baud, 8N1
    sp_set_baudrate(port_, 9600);
    sp_set_bits(port_, 8);
    sp_set_parity(port_, SP_PARITY_NONE);
    sp_set_stopbits(port_, 1);
    sp_set_flowcontrol(port_, SP_FLOWCONTROL_NONE);
    
    // Wait for device to be ready
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    
    // Flush any existing data
    sp_flush(port_, SP_BUF_BOTH);
    
    connected_ = true;
    std::cout << "Connected to " << port_name << std::endl;
    return true;
}

void TeensyController::disconnect() {
    if (port_) {
        sp_close(port_);
        sp_free_port(port_);
        port_ = nullptr;
    }
    connected_ = false;
}

bool TeensyController::isConnected() const {
    return connected_;
}

bool TeensyController::setStripLength(uint16_t length) {
    if (!connected_) return false;
    
    if (!sendByte(CMD_SET_LENGTH)) return false;
    if (!sendWord(length)) return false;
    return waitForAck();
}

bool TeensyController::fillLEDs(uint16_t first_led, uint16_t count, const Color& color) {
    if (!connected_) return false;
    
    if (!sendByte(CMD_FILL)) return false;
    if (!sendWord(first_led)) return false;
    if (!sendWord(count)) return false;
    if (!sendColor(color)) return false;
    return waitForAck();
}

bool TeensyController::setLEDData(uint16_t first_led, const std::vector<Color>& colors) {
    if (!connected_ || colors.empty()) return false;
    
    if (!sendByte(CMD_RECEIVE_DATA)) return false;
    if (!sendWord(first_led)) return false;
    if (!sendWord(static_cast<uint16_t>(colors.size()))) return false;
    
    for (const auto& color : colors) {
        if (!sendColor(color)) return false;
    }
    
    return waitForAck();
}

bool TeensyController::outputData() {
    if (!connected_) return false;
    
    if (!sendByte(CMD_OUTPUT)) return false;
    return waitForAck();
}

bool TeensyController::clearAll() {
    if (!connected_) return false;
    
    if (!sendByte(CMD_CLEAR)) return false;
    return waitForAck();
}

bool TeensyController::getVersion(uint8_t& major, uint8_t& minor) {
    if (!connected_) return false;
    
    if (!sendByte(CMD_VERSION)) return false;
    if (!readByte(major)) return false;
    if (!readByte(minor)) return false;
    return waitForAck();
}

bool TeensyController::getMaxLEDs(uint16_t& max_leds) {
    if (!connected_) return false;
    
    if (!sendByte(CMD_MAX_LEDS)) return false;
    
    uint8_t high_byte, low_byte;
    if (!readByte(high_byte)) return false;
    if (!readByte(low_byte)) return false;
    
    max_leds = (static_cast<uint16_t>(high_byte) << 8) | low_byte;
    return waitForAck();
}

std::vector<std::string> TeensyController::listPorts() {
    std::vector<std::string> ports;
    struct sp_port** port_list;
    
    enum sp_return result = sp_list_ports(&port_list);
    if (result != SP_OK) {
        return ports;
    }
    
    for (int i = 0; port_list[i] != nullptr; i++) {
        const char* port_name = sp_get_port_name(port_list[i]);
        if (port_name) {
            ports.push_back(std::string(port_name));
        }
    }
    
    sp_free_port_list(port_list);
    return ports;
}

bool TeensyController::sendByte(uint8_t byte) {
    if (!port_) return false;
    
    enum sp_return result = sp_blocking_write(port_, &byte, 1, 1000);
    return result == 1;
}

bool TeensyController::sendWord(uint16_t word) {
    // Send high byte first, then low byte
    uint8_t high_byte = (word >> 8) & 0xFF;
    uint8_t low_byte = word & 0xFF;
    
    return sendByte(high_byte) && sendByte(low_byte);
}

bool TeensyController::sendColor(const Color& color) {
    // Send as RGB (24-bit)
    return sendByte(color.r) && sendByte(color.g) && sendByte(color.b);
}

bool TeensyController::readByte(uint8_t& byte, int timeout_ms) {
    if (!port_) return false;
    
    enum sp_return result = sp_blocking_read(port_, &byte, 1, timeout_ms);
    return result == 1;
}

bool TeensyController::waitForAck(int timeout_ms) {
    uint8_t response;
    if (!readByte(response, timeout_ms)) {
        std::cerr << "Timeout waiting for response" << std::endl;
        return false;
    }
    
    if (response == 'A') {
        return true;
    } else if (response == 'N') {
        std::cerr << "Received NACK from device" << std::endl;
        return false;
    } else {
        std::cerr << "Unexpected response: " << static_cast<int>(response) << std::endl;
        return false;
    }
}