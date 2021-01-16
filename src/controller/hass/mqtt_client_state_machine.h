#ifndef client_state_machine_h_
#define client_state_machine_h_

#include "n_state.h"
#include "integration/integration.h"
#include <list>
#include <AsyncMqttClient.h>

enum ClientSygnals {
  Initialize,
  Timeout,
  Tick,
  MqttConnected,
  MqttDisconnected,
};

struct MqttClientInfo {
  const IPAddress serverAddress;
  const int serverPort;
  const char *id;
  const char *username;
  const char *password;
};

struct ClientContext {
  AsyncMqttClient & mqttClient;
  std::list<Integration *> integrations;
  MqttClientInfo & mqttInfo;
  ClientContext(AsyncMqttClient &, std::list<Integration *> &, MqttClientInfo &);

  void resetTimeout();
  void increaseTimeout();
  unsigned int getTimeout() const;
private:
  unsigned int nextConnectionWait;

  static const unsigned int InitialWaitTime = 2000;
};

typedef NState<ClientSygnals, ClientContext> ClientState;

ClientState * setupClient(ClientState *, StateMachineHandle<ClientContext> &, const ClientSygnals &);
ClientState * connecting(ClientState *, StateMachineHandle<ClientContext> &, const ClientSygnals &);
ClientState * arbitraryDelay(ClientState *, StateMachineHandle<ClientContext> &, const ClientSygnals &);
ClientState * working(ClientState *, StateMachineHandle<ClientContext> &, const ClientSygnals &);

class ClientSM : public NStateMachine<ClientSygnals, ClientContext> {
  public:
    ClientSM(ClientContext &);
    void setUp();
};


#endif
