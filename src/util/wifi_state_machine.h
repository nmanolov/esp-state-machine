#include "n_state.h"
#include "composition.h"
#include <list>
#include <ESP8266WiFi.h>

enum WiFiSygnal {
  InitializeWifi,
  TimeoutWifi,
  TickWifi,
  ConnectedToWiFi,
  DisconnectedFromWifi,
};

struct WifiInfo {
  const char * ssid;
  const char * softApSsid;
  const char * password;
};

struct WifiContext {
  ESP8266WiFiClass & WiFi;
  WifiInfo & wifiInfo;
  Composition & workingStates;
};

typedef NState<WiFiSygnal, WifiContext> MyWiFiState;

MyWiFiState * setupWifi(MyWiFiState *, StateMachineHandle<WifiContext> &, const WiFiSygnal &);
MyWiFiState * waitingWifi(MyWiFiState *, StateMachineHandle<WifiContext> &, const WiFiSygnal &);
MyWiFiState * working(MyWiFiState *, StateMachineHandle<WifiContext> &, const WiFiSygnal &);

class WiFiSM : public NStateMachine<WiFiSygnal, WifiContext> {
  public:
    WiFiSM(WifiContext &);
    void setUp();
  private:
    WiFiEventHandler disconnectedHandler,
      connectedHandler;
};
