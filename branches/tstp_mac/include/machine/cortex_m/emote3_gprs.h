#ifndef __emote3_gprs_h_
#define __emote3_gprs_h_

#include <machine/cortex_m/emote3.h>
#include <gpio.h>

__BEGIN_SYS

class M95 {
public:
    M95(GPIO &pwrkey, GPIO &status, UART &uart):
        pwrkey(pwrkey),
        status(status),
        uart(uart)
    {
        status.input();
        status.pull_down();
        pwrkey.output();
        pwrkey.clear();
        eMote3_GPTM::delay(1000000);
        power_off();
    }

    void power_on()
    {
        if (status.get()) {
            return;
        }

        auto initialized = false;
        pwrkey.set();
        // Leave pwrkey up until status is stable at 1.
        while (!initialized) {
            while (!status.get());
            eMote3_GPTM::delay(1000000);
            initialized = status.get();
        }
        pwrkey.clear();
   }

    void power_off()
    {
        if (!status.get()) {
            return;
        }

        // A 1-second pulse is what the manual says is fine for
        // turning off the device (Hardware Manual, 3.4.2.1)
        pwrkey.set();
        eMote3_GPTM::delay(1000000);
        pwrkey.clear();
        while (status.get());
    }

    // Assumes what is being sent is null-terminated.
    void send_command(const char *command)
    {
        for (auto p = command; *p; ++p) {
            uart.put(*p);
        }

        uart.put('\r');
    }

    // Doesn't assume what is being sent is null-terminated.
    // Usually the board will either expect the data size
    // to be pre-specified or will use a character as a
    // delimiter.
    void send_data(const char* data, unsigned int size)
    {
        for (auto i = 0u; i < size; ++i) {
            uart.put(data[i]);
        }
    }

    bool await_response(const char *expected, unsigned int max_timeout)
    {
        auto gptm = eMote3_GPTM{0, max_timeout};
        gptm.enable();

        {
            auto gptm = eMote3_GPTM{0, max_timeout};
            gptm.enable();
            // see if UART responds with anything before the timeout
            while (gptm.running()) {
                if (uart.has_data()) { break; }
            }

            if (!uart.has_data()) { return false; } // timeout
        }

        // the UART started responding, so we read until it stops
        auto i = 0u;
        auto result = false;
        while (true) {
            auto gptm = eMote3_GPTM{0, 300000};
            gptm.enable();
            while (gptm.running()) {
                if (uart.has_data()) { break; };
            }

            if (uart.has_data()) {
                auto c = uart.get();
                if (c == expected[i]) {
                    ++i;
                } else {
                    i = (c == expected[0]) ? 1 : 0;
                }

                if (expected[i] == '\0') {
                    // we got what we wanted, but if we stop now
                    // there will be garbage on the serial, so we
                    // read until the end
                    result = true;
                }
            } else {
                break; // end of message
            }
        }

        return result;
    }

private:
    GPIO &pwrkey;
    GPIO &status;
    UART &uart;
};

class eMote3_GPRS : private M95 {
public:
    eMote3_GPRS(GPIO &pwrkey, GPIO &status, UART &uart):
        M95{pwrkey, status, uart}
    {}

    void on()
    {
        power_on();
        set_baudrate();
        turn_off_echo();
    }

    void off() { power_off(); }

    void set_baudrate()
    {
        auto done = false;
        while (!done) {
            send_command("AT");
            done = await_response("OK", 300000);
        }
    }

    bool turn_off_echo()
    {
        send_command("ATE0");
        return await_response("OK", 300000);
    }

    bool sim_card_ready()
    {
        send_command("AT+CPIN?");
        return await_response("+CPIN: READY", 300000);
    }

    bool use_dns()
    {
        send_command("AT+QIDNSIP=1");
        return await_response("OK", 300000);
    }

    bool open_tcp(const char *address, const char *port)
    {
        char command[100] = "";
        strcat(command, "AT+QIOPEN=\"TCP\",\"");
        strcat(command, address);
        strcat(command, "\",");
        strcat(command, port);
        send_command(command);
        if (!await_response("OK", 300000)) { return false; }
        return await_response("CONNECT OK", 75000000);
    }

    bool send_tcp(const char *payload)
    {
        send_command("AT+QISEND");
        if (!await_response(">", 300000)) { return false; }

        char data[1460] = "";
        strcat(data, payload);
        strcat(data, "\x1a");
        send_command(data);
        return await_response("SEND OK", 3000000);
    }

    bool close_tcp()
    {
        send_command("AT+QICLOSE");
        return await_response("CLOSE OK", 300000);
    }

    bool set_http_url(const char *url)
    {
        auto length = strlen(url);
        char command[100] = "AT+QHTTPURL=";
        char buf[32] = "";
        buf[utoa(length, buf)] = '\0';
        strcat(command, buf);
        strcat(command, ",5");
        send_command(command);
        if (!await_response("CONNECT", 500000)) { return false; }
        send_command(url);
        return await_response("OK", 500000);
    }

    bool send_http_post(const char *data, unsigned int size)
    {
        char command[100] = "AT+QHTTPPOST=";
        char buf[32] = "";
        buf[utoa(size, buf)] = '\0';
        strcat(command, buf);
        strcat(command, ",5,5");
        send_command(command);
        if (!await_response("CONNECT", 30000000)) { return false; }
        send_data(data, size);
        return await_response("OK", 30000000);
    }

    bool send_http_post(const char *url, const char *data, unsigned int data_size)
    {
        if (!set_http_url(url)) { return false; };
        return send_http_post(data, data_size);
    }
};

__END_SYS

#endif
