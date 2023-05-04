#include <MaximWireExpanded.h>

#define PIN_BUS 9

MaximWireExpanded::Bus bus(PIN_BUS);
MaximWireExpanded::DS18B20 device;

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (device.IsValid()) {
        float temp = device.GetTemperature<float>(bus);
        if (!isnan(temp)) {
            Serial.println(temp);
            device.Update(bus);
        } else {
            Serial.print("LOST ");
            Serial.println(device.ToString());
            device.Reset();
        }
    } else {
        if (bus.Discover().FindNextDevice(device) && device.GetModelCode() == MaximWireExpanded::DS18B20::MODEL_CODE) {
            Serial.print("FOUND ");
            Serial.println(device.ToString());
            device.Update(bus);
        } else {
            Serial.println("NOTHING FOUND");
            device.Reset();
        }
    }
    delay(1000);
}
