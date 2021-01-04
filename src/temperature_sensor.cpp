#include "temperature_sensor.h"
#include "logger.h"

TS_Context::TS_Context(int pPin) : pin(pPin), wire(pPin), celsius(22) {
}

void TS_Context::setTemperature(float pCelsius) {
  celsius = pCelsius;
  this->notifyObservers(*this);
}

float TS_Context::getTemperature() const {
  return celsius;
}

TemperatureSensor::TemperatureSensor(TS_Context &pContext)
    : NStateMachine<TS_Sygnals, TS_Context>(
          pContext, &searchForAddresses, {TS_Initialize, TS_Timeout, TS_Tick}) {
}

TS_State *searchForAddresses(TS_State *self,
                             StateMachineHandle<TS_Context> &machine,
                             const TS_Sygnals &sygnal) {
  auto &state = machine.state;
  auto &wire = state.wire;
  auto addr = state.addr;
  switch (sygnal) {
  case TS_Initialize: {
    machine.setStateTimeout(10);
    Logger.println("Searching");
    if (!wire.search(addr)) {
      Logger.println("No more addresses.");
      Logger.println();
      return new TS_State(&reseting);
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
      Logger.println("CRC is not valid!");
      return new TS_State(&searchForAddresses);
    }

    // the first ROM byte indicates which chip
    switch (addr[0]) {
    case 0x10:
      Logger.println("  Chip = DS18S20"); // or old DS1820
      state.type_s = 1;
      break;
    case 0x28:
      Logger.println("  Chip = DS18B20");
      state.type_s = 0;
      break;
    case 0x22:
      Logger.println("  Chip = DS1822");
      state.type_s = 0;
      break;
    default:
      Logger.println("Device is not a DS18x20 family device.");
      return new TS_State(&searchForAddresses);
    }
    return new TS_State(&writing);
  }
  case TS_Timeout: {
    return new TS_State(&searchForAddresses);
  }
  default:
    return self;
  }
  return self;
}

TS_State *reseting(TS_State *self, StateMachineHandle<TS_Context> &machine,
                   const TS_Sygnals &sygnal) {
  auto &wire = machine.state.wire;
  switch (sygnal) {
  case TS_Initialize: {
    machine.setStateTimeout(250);
    wire.reset_search();
    Logger.println("reset search");
    machine.forceLoop();
    return self;
  }
  case TS_Timeout: {
    return new TS_State(&searchForAddresses);
  }
  default:
    return self;
  }
}

TS_State *writing(TS_State *self, StateMachineHandle<TS_Context> &machine,
                  const TS_Sygnals &sygnal) {
  auto &wire = machine.state.wire;
  auto addr = machine.state.addr;
  switch (sygnal) {
  case TS_Initialize: {
    wire.reset();
    wire.select(addr);
    wire.write(0x44, 1);
    machine.setStateTimeout(1000);
    machine.forceLoop();
    return self; // start conversion, with parasite power on at the end
  }
  case TS_Timeout: {
    return new TS_State(&processing);
  }
  default: return self;
  }
}

TS_State *processing(TS_State *self, StateMachineHandle<TS_Context> &machine,
                     const TS_Sygnals &sygnal) {
  auto &state = machine.state;
  auto &wire = state.wire;
  byte data[12];
  switch (sygnal) {
  case TS_Initialize: {
    machine.setStateTimeout(10);
    byte present = wire.reset();
    wire.select(state.addr);
    wire.write(0xBE); // Read Scratchpad

    Logger.print("  Data = ");
    Logger.print(present, HEX);
    Logger.print(" ");
    for (byte i = 0; i < 9; i++) { // we need 9 bytes
      data[i] = wire.read();
      Logger.print(data[i], HEX);
      Logger.print(" ");
    }
    Logger.print(" CRC=");
    Logger.print(OneWire::crc8(data, 8), HEX);
    Logger.println();

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (state.type_s) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00)
        raw = raw & ~7; // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20)
        raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40)
        raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }
    const float celsius = (float)raw / 16.0;

    state.setTemperature(celsius);
    Logger.printf("Current temperature: %f\n\r", celsius);

    return self;
  }
  case TS_Timeout: {
    return new TS_State(&searchForAddresses);
  }
  default: return self;
  }
}
