
#ifndef __rsp_aes_unified_h
#define __rsp_aes_unified_h

#include "component.h"
#include "rsp_adpcm.h"
#include "src/aes.h"
#ifndef HIGH_LEVEL_SYNTHESIS
#include <utility/ostream.h>
#endif

namespace Implementation {

class RSP_AES : public Component, private AES_Common {

public:
	enum {
		OP_CIPHER = 0xF0,
		OP_DECIPHER = 0xF1
	};

private:
	System::RSP_ADPCM adpcm;

public:
	RSP_AES(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char iid[Traits<RSP_AES>::n_ids])
	:Component(rx_ch, tx_ch, iid[0]),
	 adpcm(rx_ch, tx_ch, &iid[1]){}


public:
	void decipher(safe_pkt_t pkt){
	    /*#ifndef HIGH_LEVEL_SYNTHESIS
        System::OStream cout;
        cout << "RSP_AES::decipher(" << (void*)pkt.data[0] << ",...,"<< (void*)pkt.data[15] << ")\n";
        #endif*/

	    AES_Common::add_key(AES_Common::DEFAULT_CIPHER_KEY);

	    AES_Common::cipher_block(pkt.data, pkt.data);

	    adpcm.decode(pkt);
	}
};

PROXY_BEGIN(RSP_AES)
    void decipher(safe_pkt_t pkt){
        Base::call<RSP_AES::OP_DECIPHER>(pkt);
    }
PROXY_END

AGENT_BEGIN(RSP_AES)
    D_CALL_1(decipher, OP_DECIPHER, safe_pkt_t)
AGENT_END

};

DECLARE_COMPONENT(RSP_AES);


#endif /* MULT_H_ */
