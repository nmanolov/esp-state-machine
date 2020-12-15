#include "wifi_state_machine.h"

MyWiFiState * setupWifi(MyWiFiState * self, StateMachineHandle<WifiContext> & machine, const WiFiSygnal & sygnal) {
  auto & wifiInfo = machine.state.wifiInfo;
  auto & WiFi = machine.state.WiFi;
  switch (sygnal) {
  case InitializeWifi:
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifiInfo.ssid);
    WiFi.softAP(wifiInfo.softApSsid, wifiInfo.password);
    WiFi.begin(wifiInfo.ssid, wifiInfo.password);
    return new MyWiFiState(&waitingWifi);
  default:
    return self;
  }
};

MyWiFiState * waitingWifi(MyWiFiState * self, StateMachineHandle<WifiContext> & machine, const WiFiSygnal & sygnal){
  auto & WiFi = machine.state.WiFi;
  switch(sygnal) {
    case InitializeWifi:
      machine.setStateTickTime(500);
      machine.setStateTimeout(10000);
    case TickWifi:
      if (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        return self;
      }
      return self;
    case ConnectedToWiFi:
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      return new MyWiFiState(&working);
    case TimeoutWifi: {
      return new MyWiFiState(&setupWifi);
    }
    default:
      return nullptr;
  }
}
MyWiFiState * working(MyWiFiState * self, StateMachineHandle<WifiContext> & machine, const WiFiSygnal & sygnal){
  auto & innerStates = machine.state.workingStates;
  switch(sygnal) {
    case InitializeWifi:
      machine.forceLoop();
      innerStates.setUp();
      return self;
    case TickWifi:
      innerStates.tick();
      return self;
    case DisconnectedFromWifi:
      // return new MyWiFiState(&setupWifi);
    default:
      return self;
  }
}

WiFiSM::WiFiSM(WifiContext & pContext)
    : NStateMachine<WiFiSygnal, WifiContext>(
      pContext,
      &setupWifi,
      { InitializeWifi, TimeoutWifi, TickWifi }
    ) { }

void WiFiSM::setUp() {
  NStateMachine<WiFiSygnal, WifiContext>::setUp();
  auto & WiFi = context.WiFi;
  connectedHandler = WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP & event) {
    Serial.println("onStationModeGotIP");
    dispatch(ConnectedToWiFi, nullptr);
  });
  disconnectedHandler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected & event) {
    Serial.print("wifi disconnected: ");
    Serial.println(event.reason);
    dispatch(DisconnectedFromWifi, nullptr);
  });
}
