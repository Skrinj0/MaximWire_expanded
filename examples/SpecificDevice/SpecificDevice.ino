#include <MaximWireExpanded.h>

#define PIN_BUS 9

MaximWireExpanded::Bus bus(PIN_BUS);
MaximWireExpanded::DS18B20 device("28FF6BC322170323");

void setup() {
    Serial.begin(9600);
}

void loop() {
    float temp = device.GetTemperature<float>(bus);
    if (!isnan(temp)) {
        Serial.println(temp);
    }
    device.Update(bus);
    delay(1000);
}
