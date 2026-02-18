// Driver for MX25V flash memory devices
// Single IO mode only
// TODO: Test

// Supported devices:
// 512Kbit = MX25V{512, 5126, 512F}
// 1Mbit   = MX25V{1006, 1035}
// 2Mbit   = MX25V{2006, 2035}
// 4Mbit   = MX25V{4006, 4035}
// 8Mbit   = MX25V{8006, 8035}
// 16Mbit  = MX25V{1606, 1635}

#include "hal/blocking_spi.hpp"

#define _512Kbit 0x00FFFF
#define _1Mbit   0x01FFFF
#define _2Mbit   0x03FFFF
#define _4Mbit   0x07FFFF
#define _8Mbit   0x0FFFFF
#define _16Mbit  0x1FFFFF

enum class FlashResult {
    /// Operation completed successfully
    Ok,
    /// Address out of bounds
    ErrOutOfBounds,
    /// Address not aligned
    ErrNotAligned,
    /// The device is busy
    ErrBusy,
};

namespace detail {
    enum FlashCommand {
        Read = 0x03,
        ProgramPage = 0x02,
        SectorErase = 0x20,
        BlockErase32 = 0x52,
        BlockErase64 = 0xD8,
        ChipErase = 0x60,
        WriteEnable = 0x06,
        WriteDisable = 0x04,
        ReadStatus = 0x05,
        WriteStatus = 0x01,
        ResetEnable = 0x66,
        Reset = 0x99,
    };
};

template <uint32_t MAX_ADDR, typename Spi, typename ChipSel>
class FlashMemory {
private:
    static FlashResult sendCommand(uint8_t* payload, uint16_t payloadLen) {
        if (isBusy()) {
            return FlashResult::ErrBusy;
        }
        ChipSel cs;
        Spi::write(payload, payloadLen, cs);
        return FlashResult::Ok;
    }
    static uint8_t readStatusReg() {
        uint8_t payload[2] = {detail::FlashCommand::ReadStatus, 0};
        ChipSel cs;
        Spi::transferInPlace(payload, 2, cs);
        return payload[1];
    }
    static FlashResult writeStatusReg(uint8_t reg) {
        uint8_t payload[2] = {detail::FlashCommand::WriteStatus, reg};
        return sendCommand(payload, 2);
    }
    static FlashResult writeEnable() {
        uint8_t command[1] = {detail::FlashCommand::WriteEnable};
        return sendCommand(command, 1);
    }
    static FlashResult writePage(uint32_t startAddr, uint8_t* bytes, uint16_t len) {
        FlashResult res = writeEnable();
        if (res != FlashResult::Ok) {
            return res;
        }
        uint8_t command[4] = {
            detail::FlashCommand::ProgramPage,
            uint8_t(startAddr >> 16),
            uint8_t(startAddr >> 8),
            uint8_t(startAddr)
        };
        
        ChipSel::setLow();
            #pragma diag_suppress 1544
            for (uint8_t i = 0; i < 4; i++) {
                Spi::writeByte(command[i]);
            }
            for (uint16_t i = 0; i < len; i++) {
                Spi::writeByte(bytes[i]);
            }
            #pragma diag_default 1544
            Spi::flush();
        ChipSel::setHigh();
        return FlashResult::Ok;
    }
    // 'size' must be non-zero
    static bool isAligned(uint16_t testAddr, uint16_t size) {
        return testAddr % size == 0;
    }
    static FlashResult internalErase(uint32_t startAddr, uint32_t alignment, detail::FlashCommand cmd) {
        if (startAddr >= MAX_ADDR) {
            return FlashResult::ErrOutOfBounds;
        }
        if (!isAligned(startAddr, alignment)) {
            return FlashResult::ErrNotAligned;
        }
        FlashResult res = writeEnable();
        if (res != FlashResult::Ok) {
            return res;
        }
        uint8_t command[4] = {
            cmd,
            startAddr >> 16, 
            startAddr >> 8, 
            startAddr
        };
        return sendCommand(command, 4);
    }
public:
    static void init(Spi& spi, ChipSel& cs) {
        reset();
    }
    static FlashResult readBytes(uint32_t startAddr, uint8_t* recv, uint16_t len) {
        if (startAddr + len > MAX_ADDR) {
            return FlashResult::ErrOutOfBounds;
        }
        uint8_t command[4] = {
            detail::FlashCommand::Read,
            uint8_t(startAddr >> 16), 
            uint8_t(startAddr >> 8), 
            uint8_t(startAddr)
        };
        ChipSel cs;
        Spi::transfer(command, 4, recv, len, cs);
        return FlashResult::Ok;
    }
    static FlashResult writeBytes(uint32_t startAddr, uint8_t* send, uint16_t len) {
        if (startAddr + len > MAX_ADDR) {
            return FlashResult::ErrOutOfBounds;
        }
        // Write first (possibly not aligned) page
        const uint32_t PAGE_SIZE = 256;
        uint16_t partialLen = PAGE_SIZE - (startAddr & 0xFF);
        partialLen = partialLen < len ? partialLen : len;
        FlashResult res = writePage(startAddr, send, partialLen);
        if (res != FlashResult::Ok) {
            return res;
        }
        
        // Write the rest of the (now aligned) pages
        while (true) {
            startAddr += partialLen;
            send += partialLen;
            len -= partialLen;
            if (len == 0) {
                break;
            }
            partialLen = len > 256 ? 256 : len;
            FlashResult res = writePage(startAddr, send, partialLen);
            if (res != FlashResult::Ok) {
                return res;
            }
        }
        return FlashResult::Ok;
    }
    /// Erase a 4k sector. The address must point to the start of a sector (i.e. be an integer multiple of 4096)
    static FlashResult erase4k(uint32_t startAddr) {
        const uint32_t SECTOR_SIZE = 4096;
        return internalErase(startAddr, SECTOR_SIZE, detail::FlashCommand::SectorErase);
    }
    /// Erase a 32k block. The address must point to the start of a 32k block (i.e. be an integer multiple of 32768)
    static FlashResult erase32k(uint32_t startAddr) {
        const uint32_t BLOCK_32_SIZE = 32768;
        return internalErase(startAddr, BLOCK_32_SIZE, detail::FlashCommand::BlockErase32);
    }
    /// Erase a 64k block. The address must point to the start of a 64k block (i.e. be an integer multiple of 65536)
    static FlashResult erase64k(uint32_t startAddr) {
        const uint32_t BLOCK_64_SIZE = 65536;
        return internalErase(startAddr, BLOCK_64_SIZE, detail::FlashCommand::BlockErase64);
    }
    /// Erase an address range. The start address, end address, and range must be integer multiples of 4096 bytes.
    static FlashResult erase(uint32_t startAddr, uint32_t endAddr) {
        // TODO: Check startAddr and endAddr < MAX_ADDR
        // TODO: Check alignment of startAddr, endAddr
        // TODO: Configure write enable settings
        // TODO: Break erase up into 64k, 32k, or 4k chunks.
    }
    static FlashResult eraseAll() {
        FlashResult res = writeEnable();
        if (res != FlashResult::Ok) {
            return res;
        }
        uint8_t command[1] = {detail::FlashCommand::ChipErase};
        return sendCommand(command, 1);
    }
    static FlashResult reset() {
        uint8_t command[1] = {detail::FlashCommand::ResetEnable};
        FlashResult res = sendCommand(command, 1);
        if (res != FlashResult::Ok) {
            return res;
        }
        command[0] = detail::FlashCommand::Reset;
        return sendCommand(command, 1);
    }
    static bool isBusy() {
        uint8_t statusReg = readStatusReg();
        return statusReg & 1;
    }
    static void waitUntilReady() {
        while (isBusy());
    }
};
