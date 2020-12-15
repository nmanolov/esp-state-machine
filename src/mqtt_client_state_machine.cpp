#include "mqtt_client_state_machine.h"
#include <algorithm>

 ClientContext::ClientContext(
   AsyncMqttClient & pAsyncMqttClient,
   std::list<Integration *> & pIntegrations,
   MqttClientInfo & pClientInfo) : 
mqttClient(pAsyncMqttClient),
integrations(pIntegrations),
mqttInfo(pClientInfo), 
nextConnectionWait(ClientContext::InitialWaitTime) {}

void ClientContext::resetTimeout() {
  nextConnectionWait = ClientContext::InitialWaitTime;
}
void ClientContext::increaseTimeout() {
  nextConnectionWait = std::min(nextConnectionWait * 2, 32000u);
}

unsigned int ClientContext::getTimeout() const {
  return nextConnectionWait;
}

ClientState * setupClient(ClientState * self, StateMachineHandle<ClientContext> & machine, const ClientSygnals & sygnal) {
  Serial.print("setupClient: ");
  Serial.println(sygnal);
  ClientContext & context = machine.state;
  auto & mqttClient = context.mqttClient;
  auto & info = machine.state.mqttInfo;
  switch(sygnal) {
    case Initialize: {
      machine.setStateTimeout(1);
      mqttClient.setServer(info.serverAddress, info.serverPort);
      mqttClient.setCredentials(info.username, info.password);
      mqttClient.setClientId(info.id);
      mqttClient.connect();
      return self;
    }
    case Timeout:
      return new ClientState(&connecting);
    default:
      return self;
  }
}

ClientState * connecting(ClientState * self, StateMachineHandle<ClientContext> & machine, const ClientSygnals & sygnal) {
  Serial.print("connecting: ");
  Serial.println(sygnal);
  switch(sygnal) {
    case Initialize: {
      Serial.print("Current timeout: ");
      Serial.println(machine.state.getTimeout());
      machine.setStateTimeout(machine.state.getTimeout());
      return self;
    }
    case MqttConnected:
      machine.state.resetTimeout();
      return new ClientState(&arbitraryDelay);
    case Timeout:
      machine.state.increaseTimeout();
      return new ClientState(&setupClient);
    default:
      return self;
  }
}

ClientState * arbitraryDelay(ClientState * self, StateMachineHandle<ClientContext> & machine, const ClientSygnals & sygnal) {
  Serial.print("arbitraryDelay: ");
  Serial.println(sygnal);
  
  switch(sygnal) {
    case Initialize:
      machine.setStateTimeout(10);
      return self;
    case Timeout:
      return new ClientState(&working);
    default:
      return self;
  }
}

ClientState * working(ClientState * self, StateMachineHandle<ClientContext> & machine, const ClientSygnals & sygnal) {
  Serial.print("working: ");
  Serial.println(sygnal);
  ClientContext & context = machine.state;
  switch(sygnal) {
    case Initialize: {
      machine.setStateTickTime(10000);
      auto & mqttClient = context.mqttClient;
      for (auto integration : context.integrations) {
        integration->reconnect();
        const char *commandTopic = integration->getCommandTopic();
        Serial.println(commandTopic);
        if(commandTopic == nullptr) {
          continue;
        }
        mqttClient.subscribe(commandTopic, 0);
        delete[] commandTopic;
      }
      for (auto integration : context.integrations) {
        integration->setUp();
      }
      return self;
    }
    case MqttDisconnected:
      return new ClientState(&setupClient);
    case Tick:
    case Timeout:
      if(!machine.state.mqttClient.connected()) {
        return new ClientState(&setupClient);
      }
    default:
      return self;
  }
}

ClientSM::ClientSM(ClientContext & pContext)
    : NStateMachine<ClientSygnals, ClientContext>(
      pContext,
      &setupClient,
      {Initialize, Timeout, Tick}
    ) { }

void ClientSM::setUp() {
  NStateMachine<ClientSygnals, ClientContext>::setUp();
  auto & mqttClient = context.mqttClient;
  mqttClient.onSubscribe([](uint16_t packetId, uint8_t qos){
    Serial.println("Subscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
    Serial.print("  qos: ");
    Serial.println(qos);
  });
  mqttClient.onConnect([this](bool sessionPresent) {
    Serial.println("onConnect");
    dispatch(MqttConnected, nullptr);
  });
  mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason){
    Serial.print("onDisconnected: ");
    Serial.println((int8_t)reason);
    dispatch(MqttDisconnected, nullptr);
  });
  mqttClient.onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t length, size_t index, size_t total){
    char * buffer = new char[length + 1];
    strncpy(buffer, payload, length); 
    buffer[length] = '\0'; // Null terminator used to terminate the char array
    Serial.print("Message arrived on topic: [");
    Serial.print(topic);
    Serial.print("], ");
    Serial.println(buffer);

    for (auto integration : context.integrations) {
      const char *const msCommandTopic = integration->getCommandTopic();
      if(msCommandTopic == nullptr) {
        continue;
      }
      if (strcmp(topic, msCommandTopic) == 0) {
        integration->handleCommand(buffer);
        delete[] msCommandTopic;
        break;
      }
      delete[] msCommandTopic;
    }
  });
}
