// Driver for MX25V flash memory devices
// Single IO mode only
// TODO


// Supported devices:
// 512Kbit = MX25V{512, 5126, 512F}
// 1Mbit   = MX25V{1006, 1035}
// 2Mbit   = MX25V{2006, 2035}
// 4Mbit   = MX25V{4006, 4035}
// 8Mbit   = MX25V{8006, 8035}
// 16Mbit  = MX25V{1606, 1635}

#define 512Kbit 0x00FFFF
#define 1Mbit   0x01FFFF
#define 2Mbit   0x03FFFF
#define 4Mbit   0x07FFFF
#define 8Mbit   0x0FFFFF
#define 16Mbit  0x1FFFFF

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

template <uint32_t MAX_ADDR>
class FlashMemory {
private:
    uint8_t readStatusReg() {
        // TODO
    }
    
    FlashResult writeStatusReg(uint8_t reg) {
        // TODO
    }
    bool checkAligned(uint16_t testAddr, uint16_t size) {
        // TODO
    }
public:
    void init() {
        reset();
    }
    FlashResult readBytes(uint16_t startAddr, uint8_t* out, uint16_t len) {
        // TODO: Check startAddr + len <= MAX_ADDR
        // TODO: Send READ command
    }
    FlashResult writeBytes(uint16_t startAddr, uint8_t* in, uint16_t len) {
        // TODO: Check startAddr + len <= MAX_ADDR
        // TODO: If writing more than 256 bytes, separate into multiple PP commands
        // TODO: Configure write enable settings
        // TODO: Send PP command
    }
    /// Erase a 4k sector. The address must point to the start of a sector (i.e. be an integer multiple of 4096)
    FlashResult erase4k(uint16_t startAddr) {
        // TODO: Check startAddr < MAX_ADDR
        // TODO: Check alignment of startAddr
        // TODO: Configure write enable settings
        // TODO: Send SE command
    }
    /// Erase a 32k block. The address must point to the start of a 32k block (i.e. be an integer multiple of 32768)
    FlashResult erase32k(uint16_t startAddr) {
        // TODO: Check startAddr < MAX_ADDR
        // TODO: Check alignment of startAddr
        // TODO: Configure write enable settings
        // TODO: Send BE32K command
    }
    /// Erase a 64k block. The address must point to the start of a 64k block (i.e. be an integer multiple of 65536)
    FlashResult erase64k(uint16_t startAddr) {
        // TODO: Check startAddr < MAX_ADDR
        // TODO: Check alignment of startAddr
        // TODO: Configure write enable settings
        // TODO: Send BE64K command
    }
    /// Erase an address range. The start address, end address, and range must be integer multiples of 4096 bytes.
    FlashResult erase(uint16_t startAddr, uint16_t endAddr) {
        // TODO: Check startAddr and endAddr < MAX_ADDR
        // TODO: Check alignment of startAddr, endAddr
        // TODO: Configure write enable settings
        // TODO: Break erase up into 64k, 32k, or 4k chunks.
    }
    FlashResult eraseAll() {
        // TODO: Configure write enable settings
        // TODO: Send CE command
    }
    FlashResult reset() {
        // TODO: Send RSTEN then RST
    }
    bool isBusy() {
        // TODO: Check busy bit in status register
    }
    bool waitUntilReady() {
        while (isBusy());
    }
}
