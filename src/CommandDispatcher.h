#pragma once
#include "TransportProtocol.h"
#include "EventQueue.h"
#include "ParamBinding.h"
class CommandDispatcher {
public:
  CommandDispatcher(Param* p,uint8_t n,EventQueue<16>&q):_p(p),_n(n),_q(q){}
  void handlePacket(const uint8_t pkt[8]){
    if(pkt[0]!=PROTO_SYNC_BYTE||pkt[1]!=PROTO_VERSION||proto_crc8(pkt,7)!=pkt[7]) return;
    switch(pkt[2]){
      case MSG_CMD_GET_SNAPSHOT:
        for(uint8_t i=0;i<_n;i++){ Event e{EVT_PARAM_CHANGED,i,_p[i].value}; _q.push(e);} break;
      case MSG_CMD_RESET_PARAM:
        if(pkt[3]<_n){ _p[pkt[3]].resetToDefault(); Event e{EVT_PARAM_CHANGED,pkt[3],_p[pkt[3]].value}; _q.push(e);} break;
      case MSG_CMD_RESET_ALL:
        for(uint8_t i=0;i<_n;i++){ _p[i].resetToDefault(); Event e{EVT_PARAM_CHANGED,i,_p[i].value}; _q.push(e);} break;
      default: break;
    }
  }
private:
  Param* _p; uint8_t _n; EventQueue<16>& _q;
};
