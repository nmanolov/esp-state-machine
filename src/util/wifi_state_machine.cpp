#include "wifi_state_machine.h"
#include "util/logger/logger.h"

MyWiFiState * setupWifi(MyWiFiState * self, StateMachineHandle<WifiContext> & machine, const WiFiSygnal & sygnal) {
  auto & wifiInfo = machine.state.wifiInfo;
  auto & WiFi = machine.state.WiFi;
  switch (sygnal) {
  case InitializeWifi:
    delay(10);
    // We start by connecting to a WiFi network
    Logger.println();
    Logger.print("Connecting to ");
    Logger.println(wifiInfo.ssid);
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
        Logger.print(".");
        return self;
      }
      return self;
    case ConnectedToWiFi:
      Logger.println("");
      Logger.println("WiFi connected");
      Logger.println("IP address: ");
      Logger.println(WiFi.localIP());
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
    Logger.println("onStationModeGotIP");
    dispatch(ConnectedToWiFi, nullptr);
  });
  disconnectedHandler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected & event) {
    Logger.printf("wifi disconnected: %u\n", event.reason);
    dispatch(DisconnectedFromWifi, nullptr);
  });
}
