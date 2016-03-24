#ifndef __cortex_m_bootloader_h__
#define __cortex_m_bootloader_h__

#include <usb.h>
#include <machine.h>
#include <machine/cortex_m/emote3_gptm.h>
#include <machine/cortex_m/emote3_flash.h>

__BEGIN_SYS

class Cortex_M_Bootloader
{
protected:
    enum STATUS {
        FINISHED = 42,
    };

public:
    static bool bootloader_finished() __attribute__((always_inline)) {
        return (*reinterpret_cast<volatile unsigned int *>(_traits::BOOTLOADER_STATUS_ADDRESS)) == FINISHED;
    }

    static void bootloader_finished(bool val) __attribute__((always_inline)) {
        *reinterpret_cast<volatile unsigned int *>(_traits::BOOTLOADER_STATUS_ADDRESS) = (val ? FINISHED : 0);
    }

    static unsigned int loaded_epos_stack_pointer() __attribute__((always_inline)) {
        return *reinterpret_cast<volatile unsigned int *>(_traits::LOADED_EPOS_STACK_POINTER_ADDRESS);
    }

    static void jump_to_epos() __attribute__((always_inline)) {
        CPU::int_disable();
        bootloader_finished(true);
        CPU::sp(loaded_epos_stack_pointer());
        auto entry_point_address = *reinterpret_cast<unsigned int *>(_traits::LOADED_EPOS_ENTRY_POINT_ADDRESS);
        reinterpret_cast<fptr>(entry_point_address)();
    }

    static bool vector_table_present() {
        auto entry_point = *reinterpret_cast<unsigned int *>(_traits::LOADED_EPOS_ENTRY_POINT_ADDRESS);
        auto int_handler = *reinterpret_cast<unsigned int *>(_traits::LOADED_EPOS_INT_HANDLER_ADDRESS);
        auto stack_pointer = loaded_epos_stack_pointer();

        return ((entry_point >= _traits::BOOTLOADER_IMAGE_LOW) && (entry_point < _traits::BOOTLOADER_IMAGE_TOP) && 
                (int_handler >= _traits::BOOTLOADER_IMAGE_LOW) && (int_handler < _traits::BOOTLOADER_IMAGE_TOP) && 
                (stack_pointer >= Traits<Machine>::MEM_BASE) && (stack_pointer <= Traits<Machine>::MEM_TOP+1));
    }

    static void jump_to_epos_int_handler() __attribute__((always_inline)) {
        auto address = *reinterpret_cast<unsigned int *>(_traits::LOADED_EPOS_INT_HANDLER_ADDRESS);
        reinterpret_cast<fptr>(address)();
    }

    struct Message {
        enum Message_Type {
            WRITE = 'w',
            ACK = 'a',
            HANDSHAKE_1 = 'h',
            HANDSHAKE_2 = 'H',
            END = 'e',
        };

        unsigned short sequence_number;
        char type;
        unsigned int address;
        unsigned int data;
        unsigned char checksum;

        Message() : sequence_number(0), type(0), address(0), data(0), checksum(0) { }

        unsigned char lrc() const {
            unsigned char lrc = 0;
            for(unsigned int i=0; i<sizeof(Message) - sizeof(char); i++)
                lrc += reinterpret_cast<const char *>(this)[i];
            return ((lrc ^ 0xff) + 1) & 0xff;
        }

        bool check(unsigned short seq_n) const { 
            return 
            (lrc() == checksum) &&
            (sequence_number == seq_n) && 
            ( 
                (type == WRITE)  ||
                (  ( (type == ACK) || (type == HANDSHAKE_1) || (type == HANDSHAKE_2) || (type == END) ) && (address == 0) && (data == 0)  )
            );
        }

        bool is_write_msg() const { return type == WRITE; }
        bool is_ack_msg() const { return type == ACK; }
        bool is_handshake1_msg() const { return type == HANDSHAKE_1; }
        bool is_handshake2_msg() const { return type == HANDSHAKE_2; }
        bool is_end_msg() const { return type == END; }
    } __attribute__((packed));

protected:
    typedef Traits<Cortex_M_Bootloader> _traits;

private:
    typedef void (*fptr)();
};


class eMote3_Bootloader_Common : public Cortex_M_Bootloader
{
public:
    eMote3_Bootloader_Common() : words_buffered(0) { }

    void run() { 
        if(handshake()) {
            execute();
        }
    }

protected:
    virtual bool get_message(Message * m) = 0;
    virtual void send_message(const Message & m) = 0;

    Message _message;
    Message _previous_message;

    bool handshake() {
        if(Traits<Cortex_M_Bootloader>::HANDSHAKE_WAITING_LIMIT == 0) {
            do {
                while(!get_message(&_message));
            } while(!(_message.check(0) && _message.is_handshake1_msg()));
        } else {
            eMote3_GPTM timer(1, Traits<Cortex_M_Bootloader>::HANDSHAKE_WAITING_LIMIT);
            timer.enable();
            do {
                bool got_msg;
                do {
                    if(not timer.running()) {
                        return false;
                    }
                    got_msg = get_message(&_message);
                } while(not got_msg);
            } while(not (_message.check(0) and _message.is_handshake1_msg()));
        }

        // Reply
        _message.sequence_number++;
        _message.type = Message::HANDSHAKE_2;
        _message.checksum = _message.lrc();
        send_message(_message);

        return true;
    }

    void execute() {
        unsigned short sequence_number = 2;

        bool msg_ok = false;
        while(not msg_ok) {
            while(not get_message(&_previous_message))
                ;

            if(_previous_message.check(sequence_number)) {
                msg_ok = true;
                send_message(_previous_message); // ack
                if(_previous_message.is_end_msg()) {
                    write_buffer();
                    return;
                }
                sequence_number++;
            }
        }

        while(true) {
            while(not get_message(&_message))
                ;

            // Previous message's ACK was wrong
            if(_message.check(sequence_number - 1)) {
                send_message(_message); // ack
                if(_message.is_end_msg()) {
                    write_buffer();
                    return;
                }
                _previous_message = _message;
            }
            else if(_message.check(sequence_number)) {
                send_message(_message); // ack

                // Commit previous message
                if(_previous_message.is_write_msg()) {
                    auto addr = _previous_message.address;
                    if((addr >= _traits::BOOTLOADER_IMAGE_LOW) and (addr < _traits::BOOTLOADER_IMAGE_TOP)) {
                        if(!bufferize(_previous_message)) {
                            write_buffer();
                            bufferize(_previous_message);
                        }
                    }
                }

                if(_message.is_end_msg()) {
                    write_buffer();
                    return;
                }

                _previous_message = _message;
                sequence_number++;
            }
        }
    }

    void write_buffer() {
        if(words_buffered > 0) {
            auto n_bytes = words_buffered * sizeof(unsigned int);
            eMote3_Flash::write(next_buffer_address - n_bytes - eMote3_Flash::base_address(), data_buffer, n_bytes);
            words_buffered = 0;
        }
    }

    bool bufferize(const Message & m) {        
        if(words_buffered >= _traits::BUFFER_SIZE) {
            return false;
        }
        if(words_buffered == 0) {
            next_buffer_address = m.address;
        }

        if(next_buffer_address == m.address) {
            data_buffer[words_buffered++] = m.data;
            next_buffer_address += sizeof(unsigned int);
            return true;
        } else {
            return false;
        }
    }

    unsigned int data_buffer[_traits::BUFFER_SIZE];
    unsigned int words_buffered;
    unsigned int next_buffer_address;
};

class eMote3_Bootloader : public eMote3_Bootloader_Common, NIC::Observer {
public:
    eMote3_Bootloader(unsigned int channel = _traits::NIC_CHANNEL) : eMote3_Bootloader_Common(), _nic(), _peer(_nic.broadcast()), _received_msg(0), _reply_via_nic(false) {
        _nic.stop_listening();
        _nic.channel(channel);
        _nic.attach(this, _traits::NIC_PROTOCOL);
        _nic.listen();
    }

    void send_message(const Message & m) {
        if(_reply_via_nic) {
            _nic.send(_peer, _traits::NIC_PROTOCOL, reinterpret_cast<const char *>(&m), sizeof(Message));
            _reply_via_nic = false;
        } else {
            for(unsigned int i=0; i<sizeof(Message); i++)
                USB::put(reinterpret_cast<const char *>(&m)[i]);
            USB::flush();
        }
    }

    void update(NIC::Observed * o, NIC::Protocol p, NIC::Buffer * b) {
        if((b->size() == sizeof(Message)) and not _received_msg) {
            _received_msg = b;
        } else {
            _nic.free(b);
        }
    }

private:
    bool get_message(Message * m) {
        if(_received_msg) {
            memcpy(m, _received_msg->frame()->data<Message>(), sizeof(Message));
            _peer = _received_msg->frame()->src();
            _nic.free(_received_msg);
            _received_msg = 0;
            _reply_via_nic = true;
            return true;
        } else {
            return USB::get_data(reinterpret_cast<char *>(m), sizeof(Message)) == sizeof(Message);
        }
        return false;
    }

    NIC _nic;
    NIC::Address _peer;
    NIC::Buffer * _received_msg;
    bool _reply_via_nic;
};

__END_SYS

#endif
