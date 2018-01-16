#ifndef MC_Lighting_DEVICE_H
#define MC_Lighting_DEVICE_H

#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <stdint.h>
#include "hardware/hardwareOutputDevice.h"

//The McLightingDevice talks to a McLighting esp8266.
//The channels are:
// Brightness
// Saturation
// Hue
class McLightingDevice : public HardwareOutputDevice
{
public:
    McLightingDevice();
    virtual ~McLightingDevice();

    //Configure the device.
    // Parameter: "ip": IP address of the bridge.
    virtual bool configure(std::unordered_map<string, string> settings);

    //Set a hardware channel output. Value is 0.0 to 1.0 for no to max output.
    virtual void setChannelData(int channel, float value);

    //Return the number of output channels supported by this device.
    virtual int getChannelCount();

private:
    class LightInfo
    {
    public:
        LightInfo() : dirty(true), brightness(0), r(0), g(0), b(0) {}

        bool dirty;
        int brightness;
        int r;
        int g;
        int b;
    };

    sf::Thread update_thread;
    sf::Mutex mutex;
    LightInfo light;

    bool run_thread;

    void updateLoop();

    string ip_address;
};

#endif//S_ACN_DMX_DEVICE_H
