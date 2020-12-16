#include "mqtt_client_state_machine.h"
#include <algorithm>
#include <logger.h>

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
  Logger.printf("setupClient: %u\r\n", sygnal);
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
  Logger.printf("connecting: %d\n", sygnal);
  switch(sygnal) {
    case Initialize: {
      Logger.print("Current timeout: %u\n");
      Logger.println(machine.state.getTimeout());
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
  Logger.printf("arbitraryDelay: %u\n", sygnal);
  
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
  Logger.printf("working: %u\n", sygnal);
  ClientContext & context = machine.state;
  switch(sygnal) {
    case Initialize: {
      machine.setStateTickTime(10000);
      auto & mqttClient = context.mqttClient;
      for (auto integration : context.integrations) {
        integration->reconnect();
        const char *commandTopic = integration->getCommandTopic();
        Logger.println(commandTopic);
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
    Logger.println("Subscribe acknowledged.");
    Logger.print("  packetId: ");
    Logger.println((uint)packetId);
    Logger.print("  qos: ");
    Logger.println((uint)qos);
  });
  mqttClient.onConnect([this](bool sessionPresent) {
    Logger.println("onConnect");
    dispatch(MqttConnected, nullptr);
  });
  mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason){
    Logger.print("onDisconnected: ");
    Logger.println((char)reason);
    dispatch(MqttDisconnected, nullptr);
  });
  mqttClient.onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t length, size_t index, size_t total){
    char * buffer = new char[length + 1];
    strncpy(buffer, payload, length); 
    buffer[length] = '\0'; // Null terminator used to terminate the char array
    Logger.print("Message arrived on topic: [");
    Logger.print(topic);
    Logger.print("], ");
    Logger.println(buffer);

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
