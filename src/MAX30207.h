#pragma once

namespace MaximWire {

class MAX30207 : public Device {
public:
    static constexpr uint8_t MODEL_CODE = 0x54;

    // public interface
    MAX30207() = default;

    MAX30207(const Address& address)
        : Device(address)
    {
    }

    MAX30207(const String& address)
	: Device(address)
    {
    }

    MAX30207(const uint8_t (&address)[Address::ADDRESS_SIZE])
        : Device(address)
    {
    }

    void Update(Bus& bus) { // it requires 93.75ms..750ms for background conversion (depending on resolution)
        if (Device::MatchRom(bus)) {
            bus.WriteByte(ECommands::ConvertT);
        }
    }

    template <typename T>
    T GetTemperature(Bus& bus);

    // implementation specific
    enum ECommands : uint8_t {
        ConvertT = 0x44,
        ReadScratchpad = 0XBE,
    };

    enum EResolution : uint8_t {
        Bits16 = 1
	    };

    struct ConfigurationRegister { 
        uint8_t Reserver:5;
        uint8_t Resolution:1;
    };

    static_assert(sizeof(ConfigurationRegister) == 1);

#pragma pack(push, 1)
    struct ScratchPad {
        int16_t Temperature;
        uint8_t Th;
        uint8_t Tl;
        ConfigurationRegister Configuration;
        uint8_t Reserved[3];
        uint8_t CRC8;
    };
#pragma pack(pop)

    static_assert(sizeof(ScratchPad) == 9);

    union ReadData {
        ScratchPad StructuredData;
        uint8_t RawData[9];
    };

    static constexpr int16_t UNKNOWN_TEMPERATURE = 0x550;
};

template <>
int16_t MAX30207::GetTemperature<int16_t>(Bus& bus) {
    ReadData data;
    data.StructuredData.Temperature = UNKNOWN_TEMPERATURE;
    for (int retries = 3; retries > 0; --retries) {
        if (Device::MatchRom(bus)) {
            bus.WriteByte(ECommands::ReadScratchpad);
            for (uint8_t& b : data.RawData) {
                b = bus.ReadByte();
            }
            if (data.StructuredData.CRC8 == Bus::CRC8(data.RawData, 8)) {
                switch (data.StructuredData.Configuration.Resolution) {
		    case EResolution::Bits16:
                        data.StructuredData.Temperature &= 0xFFFF;
                        break;
                }
                break;
            } else {
                data.StructuredData.Temperature = UNKNOWN_TEMPERATURE;
                continue;
            }
        }
    }
    return data.StructuredData.Temperature;
}

template <>
float MAX30207::GetTemperature<float>(Bus& bus) {
    int16_t temperature = GetTemperature<int16_t>(bus);
    if (temperature == UNKNOWN_TEMPERATURE) {
        return NAN;
    } else {
        return (float)temperature / 16;
    }
}

template <>
int MAX30207::GetTemperature<int>(Bus& bus) {
    int16_t temperature = GetTemperature<int16_t>(bus);
    if (temperature == UNKNOWN_TEMPERATURE) {
        return 0;
    } else {
        return temperature / 16;
    }
}

}
