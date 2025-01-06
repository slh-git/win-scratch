#pragma once
#ifndef VK_CODES_H
#define VK_CODES_H

#include <map>
#include <string>

class VKCodes {
public:
    static std::map<std::string, int> vkCodeMap;

    static int getVKCode(const std::string& vkName) {
        //use find so won't insert if not found
		auto it = vkCodeMap.find(vkName); 
        if (it != vkCodeMap.end()) {
            return it->second;
        }
        return -1; // return -1 if not found
    }
};

// Define the map to store the VK codes
std::map<std::string, int> VKCodes::vkCodeMap = {
    {"VK_LBUTTON", 0x01},
    {"VK_RBUTTON", 0x02},
    {"VK_CANCEL", 0x03},
    {"VK_MBUTTON", 0x04},
    {"VK_XBUTTON1", 0x05},
    {"VK_XBUTTON2", 0x06},
    {"VK_BACK", 0x08},
    {"VK_TAB", 0x09},
    {"VK_CLEAR", 0x0C},
    {"VK_RETURN", 0x0D},
    {"VK_SHIFT", 0x10},
    {"VK_CONTROL", 0x11},
    {"VK_MENU", 0x12},
    {"VK_PAUSE", 0x13},
    {"VK_CAPITAL", 0x14},
    {"VK_ESCAPE", 0x1B},
    {"VK_SPACE", 0x20},
    {"VK_PRIOR", 0x21},
    {"VK_NEXT", 0x22},
    {"VK_END", 0x23},
    {"VK_HOME", 0x24},
    {"VK_LEFT", 0x25},
    {"VK_UP", 0x26},
    {"VK_RIGHT", 0x27},
    {"VK_DOWN", 0x28},
    {"VK_SELECT", 0x29},
    {"VK_PRINT", 0x2A},
    {"VK_EXECUTE", 0x2B},
    {"VK_SNAPSHOT", 0x2C},
    {"VK_INSERT", 0x2D},
    {"VK_DELETE", 0x2E},
    {"VK_HELP", 0x2F},
    {"VK_LSHIFT", 0xA0},
    {"VK_RSHIFT", 0xA1},
    {"VK_LCONTROL", 0xA2},
    {"VK_RCONTROL", 0xA3},
    {"VK_LMENU", 0xA4},
    {"VK_RMENU", 0xA5},
    {"VK_BROWSER_BACK", 0xA6},
    {"VK_BROWSER_FORWARD", 0xA7},
    {"VK_BROWSER_REFRESH", 0xA8},
    {"VK_BROWSER_STOP", 0xA9},
    {"VK_BROWSER_SEARCH", 0xAA},
    {"VK_BROWSER_FAVORITES", 0xAB},
    {"VK_BROWSER_HOME", 0xAC},
    {"VK_VOLUME_MUTE", 0xAD},
    {"VK_VOLUME_DOWN", 0xAE},
    {"VK_VOLUME_UP", 0xAF},
    {"VK_MEDIA_NEXT_TRACK", 0xB0},
    {"VK_MEDIA_PREV_TRACK", 0xB1},
    {"VK_MEDIA_STOP", 0xB2},
    {"VK_MEDIA_PLAY_PAUSE", 0xB3},
    {"VK_LAUNCH_MAIL", 0xB4},
    {"VK_LAUNCH_MEDIA_SELECT", 0xB5},
    {"VK_LAUNCH_APP1", 0xB6},
    {"VK_LAUNCH_APP2", 0xB7},
    {"VK_OEM_CLEAR", 0xFE},
};

#endif // VK_CODES_H