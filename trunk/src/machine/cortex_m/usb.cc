// EPOS Cortex_M USB Mediator Implementation
// TODO: this code seems to have been copied from somewhere else. It does not conform to EPOS coding quality, although it is functional. A carefull EPOS programmer should clean it up in the future!

#include <usb.h>
#include <ic.h>

__USING_SYS

// Class attributes
const char * Cortex_M_USB::_send_buffer = reinterpret_cast<const char *>(0);
unsigned int Cortex_M_USB::_send_buffer_size = 0;


// Class methods
void Cortex_M_USB::disable()
{
    flush();
    reg(CTRL) &= ~USBEN;
    Cortex_M_Model::usb_disable();
}

char Cortex_M_USB::get()
{
    char ret = 0;
    while(Cortex_M_USB::get_data(&ret, 1) == 0);
    return ret;
}

unsigned int Cortex_M_USB::get_data(char * out, unsigned int max_size)
{
    if(Traits<Cortex_M_USB>::blocking) {
        while(!ready_to_get());

        unsigned int i;
        unsigned int sz = (reg(CNTH) << 8) | reg(CNT0_CNTL);
        for(i = 0; (i<sz) && (i<max_size); i++)
            out[i] = reg(F4);
        if(i>=sz)
            reg(CSOL) &= ~CSOL_OUTPKTRDY;
        return i;
    }
    else {
        if(ready_to_get()) // Sets index to 4
        {
            unsigned int i;
            unsigned int sz = (reg(CNTH) << 8) | reg(CNT0_CNTL);
            for(i = 0; (i<sz) && (i<max_size); i++)
                out[i] = reg(F4);
            if(i>=sz)
                reg(CSOL) &= ~CSOL_OUTPKTRDY;
            return i;
        }
        else
            return 0;
    }
}

void Cortex_M_USB::flush(unsigned int index/*=3*/)
{
    Reg32 old_index = reg(INDEX);
    reg(INDEX) = index;
    do_flush();
    reg(INDEX) = old_index;
}

void Cortex_M_USB::put(char c)
{
    if(not ready()) {
        if(Traits<Cortex_M_USB>::blocking) {
            while(not ready());
            flush();
        }
        else {
            return;
        }
    }

    Reg32 old_index = reg(INDEX);
    reg(INDEX) = 3;
    while(reg(CS0_CSIL) & CSIL_INPKTRDY) {
        if(Traits<Cortex_M_USB>::blocking) {
            while(!ready());
        }
        else if(!ready()) {
            reg(INDEX) = old_index;
            return;
        }
    }
    reg(F3) = c;
    // If newline, signal that a packet is ready. Otherwise, let the hardware signal when the FIFO is full
    if(c == '\n') {
        do_flush();
    }
    reg(INDEX) = old_index;
}

bool Cortex_M_USB::handle_ep0(const USB_2_0::Request::Device_Request & data)
{    
    switch(data.bRequest)
    {
        case SET_ADDRESS:
            if(auto d = data.morph<Request::Set_Address>())
            {
                db<Cortex_M_USB>(TRC) << *d << endl;
                reg(ADDR) = d->device_address;
                _state = USB_2_0::STATE::ADDRESS;
                return true;
            }
            break;

        case GET_DESCRIPTOR:
            if(auto d = data.morph<Request::Get_Descriptor>())
            {
                db<Cortex_M_USB>(TRC) << *d << endl;
                if(d->descriptor_type == DESC_DEVICE)
                {
                    _send_buffer = reinterpret_cast<const char *>(&_device_descriptor);
                    _send_buffer_size = (sizeof _device_descriptor) > (d->descriptor_length) ? (d->descriptor_length) : (sizeof _device_descriptor);
                    return true;
                }

                // DESC_DEVICE_QUALIFIER not supported

                if(d->descriptor_type == DESC_CONFIGURATION)
                {
                    _send_buffer = reinterpret_cast<const char *>(&_config);
                    _send_buffer_size = (sizeof _config) > (d->descriptor_length) ? (d->descriptor_length) : (sizeof _config);
                    return true;
                }
            }
            break;

        case SET_CONFIGURATION:
            if(auto d = data.morph<Request::Set_Configuration>())
            {
                db<Cortex_M_USB>(TRC) << *d << endl;
                bool ret = (d->configuration_number == 1);
                if(ret) {
                    _state = USB_2_0::STATE::CONFIGURED;
                }
                return ret;
            }
            break;

        case CDC::GET_LINE_CODING:
            if(auto d = data.morph<CDC::Request::Get_Line_Coding>())
            {
                static CDC::Request::Get_Line_Coding::Data_Format data;// = new CDC::Request::Get_Line_Coding::Data_Format();
                data.dwDTERate = 1/Traits<Cortex_M_UART>::DEF_BAUD_RATE;
                switch(Traits<Cortex_M_UART>::DEF_STOP_BITS)
                {            
                    case 1: data.bCharFormat = 0; break;
                    case 2: data.bCharFormat = 2; break;
                    default: return false;
                }
                data.bParityType = Traits<Cortex_M_UART>::DEF_PARITY;
                data.bDataBits = Traits<Cortex_M_UART>::DEF_DATA_BITS;

                _send_buffer = reinterpret_cast<const char *>(&data);
                _send_buffer_size = (sizeof (data)) > (d->size_of_data) ? (d->size_of_data) : (sizeof (data));
                return true;
            }
            break;

        case CDC::SET_LINE_CODING:
            return true;

        case CDC::SET_CONTROL_LINE_STATE:
            if(auto d = data.morph<CDC::Request::Set_Control_Line_State>())
            {
                db<Cortex_M_USB>(TRC) << *d << endl;
                _ready_to_print = d->DTE_present;
                return true;
            }
            break;

        default:
                break;
    }

    return false;
}

void Cortex_M_USB::int_handler(const IC::Interrupt_Id & interrupt)
{
    Reg32 index = reg(INDEX); // Save old index

    //db<Cortex_M_USB>(TRC) << "Cortex_M_USB::int_handler" << endl;
    //db<Cortex_M_USB>(TRC) << "CS0_CSIL = " << reg(CS0_CSIL) << endl;
    //db<Cortex_M_USB>(TRC) << "CIF = " << cif << endl;
    //db<Cortex_M_USB>(TRC) << "OIF = " << reg(OIF) << endl;
    //db<Cortex_M_USB>(TRC) << "IIF = " << reg(IIF) << endl;

    Reg32 flags;
    if((flags = reg(CIF))) // USB interrupt flags are cleared when read
    {
        if(flags & INT_RESET)
            reset();
        db<Cortex_M_USB>(TRC) << "CIF = " << flags << endl;
    }

    if((flags = reg(IIF))) // USB interrupt flags are cleared when read
    {
        if(flags & (1 << 0)) // Endpoint 0 interrupt
        {
            reg(INDEX) = 0;
            if(reg(CS0_CSIL) & CS0_OUTPKTRDY) // Data present
            {
                db<Cortex_M_USB>(TRC) << "Endpoint 0 command received:";
                // Read command from endpoint 0 FIFO
                USB_2_0::Request::Device_Request data;
                Reg8 fifocnt = reg(CNT0_CNTL);
                for(unsigned int i=0; (i < 8) && (i < fifocnt); i++)
                {
                    data[i] = reg(F0);
                    db<Cortex_M_USB>(TRC) << " " << (int) data[i];
                }
                db<Cortex_M_USB>(TRC) << endl;

                db<Cortex_M_USB>(TRC) << data << endl;

                reg(CS0_CSIL) |= CS0_CLROUTPKTRDY; // Signal that the command was read
                if(handle_ep0(data))
                {
                    if(!_send_buffer)
                        reg(CS0_CSIL) |= CS0_DATAEND;                    
                    db<Cortex_M_USB>(TRC) << "command processed" << endl;
                }
                else
                {
                    // Signal that the command could not be executed
                    reg(CS0_CSIL) |= CS0_SENDSTALL;
                    db<Cortex_M_USB>(WRN) << "Cortex_M_USB::int_handler: command NOT processed" << endl;
                }
            }

            // Send part of the requested buffer that fits in the FIFO
            if(_send_buffer)
            {
                for(unsigned int i=0; (_send_buffer_size > 0) && (i<_max_packet_ep0); i++, _send_buffer++, _send_buffer_size--)
                    reg(F0) = *_send_buffer;

                if(_send_buffer_size == 0)
                {
                    // Signal that packet is ready and no further data is expected for this request
                    reg(CS0_CSIL) |= CS0_INPKTRDY | CS0_DATAEND;                    
                    _send_buffer = reinterpret_cast<const char *>(0);
                }
                else
                    // Signal that packet is ready
                    reg(CS0_CSIL) |= CS0_INPKTRDY;
            }
        }


        db<Cortex_M_USB>(TRC) << "IIF = " << flags << endl;
    }
    if((flags = reg(OIF))) // USB interrupt flags are cleared when read
    {
        db<Cortex_M_USB>(TRC) << "OIF = " << flags << endl;
    }
    reg(INDEX) = index; // Restore old index
}
