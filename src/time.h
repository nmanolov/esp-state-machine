#ifndef time_h_
#define time_h

#include "n_state.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define NTP_PACKET_SIZE 48
const char * ntpServerName = "0.wled.pool.ntp.org";
bool useAMPM = true;
byte currentTimezone = 0;
int utcOffsetSecs = 0;
uint16_t ntpLocalPort = 2390;

enum TimeSygnal {
  timeInitialize,
  timeTimeout,
  timeTick,
};

struct TimeContext {
  WiFiUDP ntpUdp;
  IPAddress ntpServerIP;
  unsigned long ntpLastSyncTime = 999000000L;
  unsigned long ntpPacketSentTime = 999000000L;
  unsigned long ntpLastreceivedTime = 0;
};

typedef NState<TimeSygnal, TimeContext> TimeState;

void sendNTPPacket(TimeContext & context){
  Serial.println("Sending packet");
  WiFiUDP & udp = context.ntpUdp;
  IPAddress & serverIP = context.ntpServerIP;

  if (!serverIP.fromString(ntpServerName)) //see if server is IP or domain
  {
    WiFi.hostByName(ntpServerName, serverIP, 750);
  }

  byte pbuf[NTP_PACKET_SIZE];
  memset(pbuf, 0, NTP_PACKET_SIZE);

  pbuf[0] = 0b11100011;   // LI, Version, Mode
  pbuf[1] = 0;     // Stratum, or type of clock
  pbuf[2] = 6;     // Polling Interval
  pbuf[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  pbuf[12]  = 49;
  pbuf[13]  = 0x4E;
  pbuf[14]  = 49;
  pbuf[15]  = 52;

  udp.beginPacket(serverIP, 123); //NTP requests are to port 123
  udp.write(pbuf, NTP_PACKET_SIZE);
  udp.endPacket();
}

bool checkNTPResponse( TimeContext & context){
  WiFiUDP & ntpUdp = context.ntpUdp;
  int bytesAvailable = ntpUdp.parsePacket();
  if (!bytesAvailable) {
    return false;
  }
  byte pbuf[NTP_PACKET_SIZE];
  ntpUdp.read(pbuf, NTP_PACKET_SIZE); // read the packet into the buffer

  unsigned long highWord = word(pbuf[40], pbuf[41]);
  unsigned long lowWord = word(pbuf[42], pbuf[43]);
  if (highWord == 0 && lowWord == 0) return false;
  
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  unsigned long epoch = secsSince1900 - 2208988799UL; //subtract 70 years -1sec (on avg. more precision)
  
  Serial.print("Current Time is: ");
  Serial.println(epoch);
  context.ntpLastreceivedTime = epoch;
  return true;
}

TimeState * sendPacket(TimeState * self, StateMachineHandle<TimeContext> &, const TimeSygnal & sygnal);
TimeState * checkPacket(TimeState * self, StateMachineHandle<TimeContext> &, const TimeSygnal & sygnal);
TimeState * waiting(TimeState * self, StateMachineHandle<TimeContext> &, const TimeSygnal & sygnal);

TimeState * sendPacket(TimeState * self, StateMachineHandle<TimeContext> & machine, const TimeSygnal & sygnal) {
  TimeContext & context = machine.state;
  switch(sygnal) {
    case timeInitialize:
      context.ntpUdp.begin(ntpLocalPort);
      sendNTPPacket(context);
      return new TimeState(&checkPacket);
    default:
      return nullptr;
  }
}

TimeState * checkPacket(TimeState * self, StateMachineHandle<TimeContext> &machine, const TimeSygnal & sygnal) {
  TimeContext & context = machine.state;
  switch(sygnal) {
    case timeInitialize:
      machine.setStateTickTime(20);
      machine.setStateTimeout(10000);
    case timeTick: {
      if(checkNTPResponse(context)) {
        return new TimeState(&waiting);
      }
      return self;
    }
    case timeTimeout: {
      return new TimeState(&sendPacket);
    }
    default:
      return nullptr;
  }
}

TimeState * waiting(TimeState * self, StateMachineHandle<TimeContext> &machine, const TimeSygnal & sygnal) {
  switch(sygnal) {
    case timeInitialize:
      machine.setStateTimeout(10000);
      return self;
    case timeTimeout: {
      return new TimeState(&sendPacket);
    }
    default:
      return nullptr;
  }
}



class TimeStateMachine : public NStateMachine<TimeSygnal, TimeContext> {
  public:
    TimeStateMachine(TimeContext & context)
    : NStateMachine<TimeSygnal, TimeContext>(
      context,
      &sendPacket,
      {timeInitialize, timeTimeout, timeTick}) {
    }
};

#endif
