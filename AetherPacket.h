#pragma once

// Imports
#include <cstdint>

// Types of data that we can send through packets
enum class PacketType : uint8_t {
    TEXT_MESSAGE = 0,
    FILE_TRANSFER = 1,
    CLIPBOARD_SYNC = 2,
    DISCONNECT_SIGNAL = 3
};

// Blueprint for the header
#pragma pack(push, 1) // to avoid padding (to keep things universal and lean)

struct AetherHeader {
    char magic[4] = {'A', 'E', 'T', 'H'}; // the magic signature to ensure it is a valid packet
    PacketType type;
    uint32_t payloadSize;
};

#pragma pack(pop)