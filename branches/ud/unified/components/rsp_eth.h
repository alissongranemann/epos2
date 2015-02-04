
#ifndef __rsp_eth_unified_h
#define __rsp_eth_unified_h

#include "component.h"
#include "rsp_aes.h"
#ifndef HIGH_LEVEL_SYNTHESIS
#include <utility/ostream.h>
#endif

namespace Implementation {

class RSP_ETH : public Component{

public:
	enum {
		OP_START = 0xF0
	};

private:
	System::RSP_AES aes;

	safe_pkt_t pkt;

	enum {
	    N_PKTS = 44
	};


public:
	RSP_ETH(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char iid[Traits<RSP_ETH>::n_ids])
	:Component(rx_ch, tx_ch, iid[0]),
	 aes(rx_ch, tx_ch, &iid[1]){}


public:
	int start(){
	    /*#ifndef HIGH_LEVEL_SYNTHESIS
        System::OStream cout;
        cout << "RSP_ETH::start()\n";
        #endif*/

	    PKT_INIT: for (int i = 0; i < 16; ++i) {
	        pkt.data[i] = i;
        }

		RUN: for (int i = 0; i < N_PKTS; ++i) {
		    aes.decipher(pkt);
		}

		return N_PKTS;
	}
};

PROXY_BEGIN(RSP_ETH)
    int start(){
        return Base::call_r<RSP_ETH::OP_START,int>();
    }
PROXY_END

AGENT_BEGIN(RSP_ETH)
    D_CALL_R_0(start, OP_START, int)
AGENT_END

};

DECLARE_COMPONENT(RSP_ETH);


#endif /* MULT_H_ */
