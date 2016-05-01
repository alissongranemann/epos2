// EPOS PC Keyboard Mediator Declarations

#ifndef __pc_keyboard_h
#define __pc_keyboard_h

#include <cpu.h>
#include <keyboard.h>

__BEGIN_SYS

// Intel 8042 Keyboard Controller
class i8042
{
public:
    // I/O ports
    enum {
        DATA            = 0x60,
        STATUS		= 0x64,
        COMMAND         = 0x64 // same as STATUS, but for write operations
    };

    // Status register flags
    enum {
        OUT_BUF_FULL    = 0x01, // The byte in the output buffer hasn't been received yet
        IN_BUF_FULL     = 0x02, // A byte is ready in the input buffer
        IGNORE          = 0x04, // Ignored bit
        PORT            = 0x08, // 0 => PORT = 0x64, 1 => PORT = 0x60
        ACTIVE          = 0x10, // Keyboard is active
        OUT_TIME_OUT    = 0x20, // Error: time out when outputting
        IN_TIME_OUT     = 0x40, // Error: time out when inputing
        PARITY          = 0x80  // Error: parity don't match
    };

    // Commands
    typedef unsigned char Command;
    enum {
        SET_LEDS        = 0xed,
        SET_RATE        = 0xf3,
        ACK             = 0xfa
    };

    // LEDs
    enum {
        SCROLL          = 0x1,
        NUM             = 0x2,
        CAPS            = 0x4
    };

public:
    i8042() {}

    static int status() { return(CPU::in8(STATUS)); }
    static bool command(const Command & cmd) {
        // Wait for the controller's input buffer to get empty
        while((CPU::in8(STATUS) & IN_BUF_FULL));

        // Send command
        CPU::out8(COMMAND, cmd);

        // Wait for a keyboard controller reaction
        while(!(CPU::in8(STATUS) & OUT_BUF_FULL));

        // Check for an ACK
        return (CPU::in8(DATA) == ACK);
    }
    static int data() { return(CPU::in8(DATA)); }

    static int scancode() {
        while(!(status() & OUT_BUF_FULL));
        return data();
    }

    static bool set_leds(unsigned char leds = (SCROLL | NUM | CAPS)) {
        return (command(SET_LEDS) && command(leds));
    }
    static int set_rate(int delay, int rate) {
        return (command(SET_RATE) && command(((delay << 5) | rate) & 0xff));
    }
    static void flush() {
        do
        {
            // Wait for the controller to get reeady
            while(status() & OUT_BUF_FULL)
                data(); // Consume a byte
        } while(status() & IN_BUF_FULL);
    }
};

class PC_Keyboard: public Keyboard_Common, private i8042
{
    friend class PC_Setup;

private:
    struct Scancode {
       unsigned char normal;
       unsigned char shift;
       unsigned char ctrl;
       unsigned char alt;
    };

    enum {
        CTRL,
        ALT,
        SHIFT,
        CAPS
    };

public:
    PC_Keyboard() {}

    static char getc();

//    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
//        Engine::int_enable(receive, send, line, modem);
//    }
//    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
//        Engine::int_disable(receive, send, line, modem);
//    }

    static void reboot();

private:
    static void init() { flush(); }

    static char upper(char c) { return ((c >= 'a') && (c <= 'z')) ? (c -'a' + 'A') : c; }

private:
    static unsigned int _flags;
    static Scancode _scancodes[255];
};

__END_SYS

#endif
