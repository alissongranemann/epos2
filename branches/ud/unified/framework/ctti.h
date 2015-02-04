//EPOS Unified components Compile-time Type Information

#ifndef __unified_ctti_h
#define __unified_ctti_h

#include "types.h"

namespace Implementation {

// Type -> Id
template<typename T>struct Type2Id  { enum { ID = UNKNOWN_TYPE_ID }; };

template<> struct Type2Id<Add>  { enum { ID = ADD_ID }; };
template<> struct Type2Id<MAC>  { enum { ID = MAC_ID }; };
template<> struct Type2Id<AES>  { enum { ID = AES_ID }; };
template<> struct Type2Id<Mult>  { enum { ID = MULT_ID }; };
template<> struct Type2Id<DTMF_Detector>  { enum { ID = DTMF_DETECTOR_ID }; };
template<> struct Type2Id<ADPCM_Codec>  { enum { ID = ADPCM_CODEC_ID }; };
template<> struct Type2Id<Sched<void,void> >  { enum { ID = SCHED_ID }; };
template<> struct Type2Id<Dummy_Callee>  { enum { ID = DUMMY_CALLEE_ID }; };
template<> struct Type2Id<Dummy_Caller>  { enum { ID = DUMMY_CALLER_ID }; };
template<> struct Type2Id<RSP_Controller>  { enum { ID = RSP_CONTROLLER_ID }; };
template<> struct Type2Id<RSP_DTMF>  { enum { ID = RSP_DTMF_ID }; };
template<> struct Type2Id<RSP_ADPCM>  { enum { ID = RSP_ADPCM_ID }; };
template<> struct Type2Id<RSP_AES>  { enum { ID = RSP_AES_ID }; };
template<> struct Type2Id<RSP_ETH>  { enum { ID = RSP_ETH_ID }; };
template<> struct Type2Id<Dummy_Component_For_Proxy>  { enum { ID = DUMMY_COMPONENT_FOR_PROXY_ID }; };
template<> struct Type2Id<Dummy_Component_For_Agent>  { enum { ID = DUMMY_COMPONENT_FOR_AGENT_ID }; };
template<> struct Type2Id<Dummy_HW_Area_Test>  { enum { ID = DUMMY_HW_AREA_TEST_ID }; };

// Id -> Type
template<Type_Id id> struct Id2Type { typedef Dummy<0> TYPE; };

template<> struct Id2Type<ADD_ID>   { typedef Add TYPE; };
template<> struct Id2Type<MAC_ID>   { typedef MAC TYPE; };
template<> struct Id2Type<AES_ID>   { typedef AES TYPE; };
template<> struct Id2Type<MULT_ID>   { typedef Mult TYPE; };
template<> struct Id2Type<DTMF_DETECTOR_ID>   { typedef DTMF_Detector TYPE; };
template<> struct Id2Type<SCHED_ID>   { typedef Sched<void,void> TYPE; };
template<> struct Id2Type<DUMMY_CALLEE_ID>   { typedef Dummy_Callee TYPE; };
template<> struct Id2Type<DUMMY_CALLER_ID>   { typedef Dummy_Caller TYPE; };
template<> struct Id2Type<RSP_CONTROLLER_ID>   { typedef RSP_Controller TYPE; };
template<> struct Id2Type<RSP_DTMF_ID>   { typedef RSP_DTMF TYPE; };
template<> struct Id2Type<RSP_ADPCM_ID>   { typedef RSP_ADPCM TYPE; };
template<> struct Id2Type<RSP_AES_ID>   { typedef RSP_AES TYPE; };
template<> struct Id2Type<RSP_ETH_ID>   { typedef RSP_ETH TYPE; };



}

#endif /* CTTI_H_ */
