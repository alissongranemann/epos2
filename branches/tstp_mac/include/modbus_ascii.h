#ifndef __modbus_ascii_h
#define __modbus_ascii_h

#include <system/config.h>

__BEGIN_SYS

class Modbus_ASCII
{
private:
	static const char char_map[16];

public:
	static const unsigned int MSG_LEN = 96;
	enum Commands {
		READ_COILS              = 0x01,
		READ_HOLDING_REGISTER   = 0x03,
		WRITE_SINGLE_COIL       = 0x05,
		WRITE_SINGLE_REGISTER   = 0x06,
		WRITE_MULTIPLE_COILS    = 0x15,
		WRITE_REGISTER          = 0x16,
		READ_MULTIPLE_REGISTERS = 0x23
	};

	class Modbus_ASCII_Feeder
	{
	public:
		virtual ~Modbus_ASCII_Feeder() {};

		void registerModbus(Modbus_ASCII * modbus) { _modbus = modbus; }

	protected:
		void notify(const char * c)
		{
			_modbus->receive(c);
		}
		Modbus_ASCII * _modbus;
	};

	class Modbus_ASCII_Sender
	{
	public:
		virtual ~Modbus_ASCII_Sender() {}

		virtual void send(const char * c, int len) = 0;
	};

	Modbus_ASCII(Modbus_ASCII_Sender * sender, unsigned char addr, int max_data_size = 32)
	  : _sender(sender), _my_address(addr)
    {
		_data_in = new unsigned char [max_data_size];
		_data_out = new char [max_data_size];
    }

	virtual ~Modbus_ASCII()
	{
		delete [] _data_in;
		delete [] _data_out;
	}

	void receive(const char * cmd)
	{
        last_msg = cmd;
		int i = 0;
		while(state_machine(cmd[i++]));
	}

    const char * last_msg;
    int last_msg_len;

    void ack()
    {
        _sender->send(last_msg, last_msg_len);
    }

    static bool check_format(const char * msg, unsigned int len)
    {
		unsigned char lrc = 0;
        if((len > MSG_LEN) || (msg[0] != ':'))
        {
            return false;
        }

        unsigned int end;
        for(unsigned int i=1; i<len; i++)
        {
            if(msg[i] == '\r')
            {
                if(msg[i+1] == '\n')
                {
                    end = i;
                    if(((end-1) % 2)) // Size is not even
                    {
                        return false;
                    }
                    break;
                }
                else
                {
                    return false;
                }
            }
            else if( !(
                        ((msg[i] >= 'a') && (msg[i] <= 'f')) ||
                        ((msg[i] >= 'A') && (msg[i] <= 'F')) ||
                        ((msg[i] >= '0') && (msg[i] <= '9'))
                      ))
            {
                return false;
            }
        }
            
        for(unsigned int i=1; i<end; i+=2)
        {
            lrc += decode(msg[i], msg[i+1]);
        }
		
        lrc = ((lrc ^ 0xff) + 1) & 0xff;

        return lrc == 0;
    }

	void send(unsigned char addr, unsigned char cmd, unsigned char * data, int data_len)
	{
		unsigned char lrc = 0;

        // ScadaBR doesn't recognize 1-byte data messages
        int padding = 0;
        if((cmd != READ_COILS) && (data_len < 2))
            padding = 2 - data_len;

		int i = 0;
		_data_out[i++] = ':';

		encode(addr, &_data_out[i], &_data_out[i+1]);
        i += 2;
		lrc += addr;

		encode(cmd, &_data_out[i], &_data_out[i+1]);
        i += 2;
		lrc += cmd;

		encode(data_len + padding, &_data_out[i], &_data_out[i+1]);
        i += 2;
		lrc += data_len + padding;

        // ScadaBR doesn't recognize 1-byte data messages
		for(int j = 0; j < padding; ++j)
        {
			encode(0, &_data_out[i], &_data_out[i+1]);
            i += 2;
        }

		for(int j = 0; j < data_len; ++j)
		{
			encode(data[j], &_data_out[i], &_data_out[i+1]); 
            i += 2;
			lrc += data[j];
		}

		lrc = ((lrc ^ 0xff) + 1) & 0xff;
		encode(lrc, &_data_out[i], &_data_out[i+1]);
        i += 2;


		_data_out[i++] = '\r';
		_data_out[i++] = '\n';

		_sender->send(_data_out, i);
	}

	static void encode(unsigned char c, char * ascii1, char * ascii2)
	{
		*ascii2 = char_map[c&0x0f];
		*ascii1 = char_map[c>>4];
	}
	static unsigned char dec_one(unsigned char c)
	{
		if(c >= '0' && c <= '9')
			return c - '0';
		else if(c >= 'A' && c <= 'F')
			return c - 'A' + 0x0a;
		else if(c >= 'a' && c <= 'f')
			return c - 'a' + 0x0a;
		return c;
	}
	static unsigned char decode(unsigned char h, unsigned char l)
	{
		return ((dec_one(h) << 4) | dec_one(l));
	}

	unsigned char myAddress() { return _my_address; }

	virtual void handle_command(unsigned char cmd, unsigned char * data, int data_len) = 0;

private:

	int data_index;
	unsigned char cmd;
	unsigned char data_len;
	unsigned char _lrc;
	unsigned char buff;

	int state_machine(unsigned char c)
	{
	    static unsigned char STATE = 0;
		switch(STATE) {
		case 0:
            last_msg_len = 0;
			data_index = 0;
			data_len = 0;
			if(c == ':') 
            { 
                STATE = 1; 
                last_msg_len = 1; 
            }
			break;
		case 1:
            last_msg_len++;
			buff = c;
			STATE = 2;
			break;
		case 2:
            last_msg_len++;
			buff = decode(buff, c);
			if(buff == _my_address) STATE = 3;
			else STATE = 0;
			break;
		case 3:
            last_msg_len++;
			buff = c;
			STATE = 4;
			break;
		case 4:
            last_msg_len++;
			cmd = decode(buff, c);
			STATE = 5;
			break;
		case 5:
            last_msg_len++;
			if(c == '\r')
				STATE = 7;
			else {
				buff = c;
				STATE = 6;
			}
			break;
		case 6:
            last_msg_len++;
			_data_in[data_len++] = decode(buff, c);
			STATE = 5;
			break;
		case 7:
            last_msg_len++;
			if(c != '\n') STATE = 0;
			_lrc = _my_address + cmd;
			for(int i = 0; i < (data_len-1); ++i)
				_lrc += _data_in[i];
			_lrc = ((_lrc ^ 0xff) + 1) & 0xff;
			STATE = 0;
			if(_lrc == _data_in[data_len-1]) handle_command(cmd, _data_in, data_len-1);
			break;
		}

		return STATE;
	}

	Modbus_ASCII_Sender * _sender;
	unsigned char _my_address;
	unsigned char * _data_in;
	char * _data_out;
};

__END_SYS

#endif
