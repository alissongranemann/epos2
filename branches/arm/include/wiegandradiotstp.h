#ifndef WIEGANDRADIOTSTP_H
#define WIEGANDRADIOTSTP_H

#include <system/config.h>
#include <gpio.h>
#include <utility/string.h>
#include <machine/cortex_m/emote3_gptm.h>



using namespace EPOS;

OStream cout;
GPIO * led;
GPIO * relay[6]; 


class Wiegand {
	private:
	
	GPIO * _relay;
	unsigned int data, facility,serial;
	unsigned int packageBitCount;
	unsigned int packageValue;
	unsigned int num_wiegand;
	unsigned int parity;
	
	int element;

	unsigned char bit;

	bool firstHalf;
	
	
	char facilitystr[3];
	char serialstr[5];
	char wiemsg;
	char device;
	char cmd;

	public:
	unsigned int doorstate;
	Wiegand(unsigned int num, GPIO * relay);
	void wiegandpin (unsigned int num);
	char messagemountwiegand(char device, unsigned int element, char cmd, char* wiemsg);
	void checkpermission(unsigned int lastwiegandmsg);
	void wiegandprocess();
	void receiveBit(int bit);
	
	void set_doorstate ();
	
    static const unsigned int MAX_MSG_SIZE = 15;
    static const unsigned int PROTOCOL_ID = 15;

    static const unsigned int BRIDGE_ADDRESS = 12;
    static const unsigned int WIEGAND_ADDRESS = 13;
	
	TSTP::ID_Code id;
};

Wiegand::Wiegand(unsigned int num, GPIO * relay) {
	num_wiegand = num;
	packageBitCount = 0;
	parity = 0;
	packageValue = 0;
	_relay = relay;
	doorstate = 0;
	firstHalf = false;
}


void Wiegand::receiveBit(int bit) {
	packageValue = (packageValue << 1)+bit;
	parity += bit;
	packageBitCount++;
	wiegandprocess();
}


char Wiegand::messagemountwiegand(char device, unsigned int element, char cmd, char* wiemsg){
    char msg[Wiegand::MAX_MSG_SIZE];
    memset(msg, 0, Wiegand::MAX_MSG_SIZE);
    msg[0] = ':';
    msg[1] = device;
    msg[2] = '0' + element;
    msg[3] = cmd;
    unsigned int idx = 4;
    while((*wiemsg) and (idx < Wiegand::MAX_MSG_SIZE)) {
        msg[idx++] = *wiemsg++;
    }

    if(idx < Wiegand::MAX_MSG_SIZE) {
        msg[idx++] = '\n';
        NIC nic;
        for(int i = 50; i-- and (nic.send(BRIDGE_ADDRESS, Wiegand::PROTOCOL_ID, msg, idx) < idx););
    }

	//function that mounts the messages on the following format ":DCV", where:
	//cout << ":" << device << element << cmd << wiemsg << endl;
}

void Wiegand::checkpermission(unsigned int lastwiegandmsg) {
	if (lastwiegandmsg == 59220) {
		set_doorstate ();
	} else if (lastwiegandmsg == 33060) {
		set_doorstate ();
	} else if (lastwiegandmsg == 50020) {
		set_doorstate ();
	} else {
		}
}

void Wiegand::set_doorstate () {
	led = new GPIO('c',3, GPIO::OUTPUT);	
	_relay->set(true);
	led->set(true);
	eMote3_GPTM::delay(500000);
	_relay->set(false);
	led->set(false);
}


void Wiegand::wiegandprocess() {
	if (packageBitCount==13){
		firstHalf = !firstHalf;
		if (firstHalf){
		 	 parity -= (packageValue & 0x1000) >> 12;
	         data=packageValue & 0x0FFF;
	         facility = (packageValue & 0x0FF0) >> 4;
	         serial = (packageValue & 0x0F) << 12;
	        // cout << "[0..13]";
	         if ((parity & 0x1) == ((packageValue >> 12) & 0x1)){
			 } else {
		         //   cout << ":PERR"; //parity error on the first half
		     	}
		        // cout << ":pack="<<packageValue<<",facility="<<facility<<endl;
		} else {
			parity -= (packageValue & 0x1);
		    data=(packageValue & 0x01FFE) >> 1;
		    serial |= data;
			//cout<<"[14..25]";
		    if ((parity & 0x1) != (packageValue & 0x1)){
		    } else {
                NIC nic;
                for(int i = 10; i-- and (nic.send(BRIDGE_ADDRESS, Wiegand::PROTOCOL_ID, ":PERR\n", sizeof(":PERR\n")) < sizeof(":PERR\n")););
                
		         //cout<<":PERR"; //parity error on the second half
		    }
		    		    	    
	       //  cout<<":pack="<<packageValue<<",serial="<<serial<<endl;

			char wiegandmsg[30];
			char facilitystr[3];
			char serialstr[5];

    		itoa(facility, facilitystr);
    		itoa(serial, serialstr);

			strcpy (wiegandmsg, reinterpret_cast<const char*>(facilitystr));
			strcat (wiegandmsg,":");
			strcat (wiegandmsg, reinterpret_cast<const char*>(serialstr));

			id = (facility << 16) + (serial);
			messagemountwiegand('W', num_wiegand, 'R', wiegandmsg);


			checkpermission(serial);

			//cout << ":W" <<  facility << ":" << serial << endl; //this cout it's the only one that is necessary to ScadaBR, besides of the Error cout. It uses the following format:
			  // :WFacilityCodeUserCode.
		}

		packageBitCount=0; //resets
		parity=0;
		packageValue=0;

    }
}

#endif

