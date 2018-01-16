#include "mcLightingDevice.h"
#include "hardware/serialDriver.h"
#include "logging.h"
#include <unistd.h>

McLightingDevice::McLightingDevice()
: update_thread(&McLightingDevice::updateLoop, this)
{
    run_thread = false;
}

McLightingDevice::~McLightingDevice()
{
    if (run_thread)
    {
        run_thread = false;
        update_thread.wait();
    }
}

bool McLightingDevice::configure(std::unordered_map<string, string> settings)
{
    if (settings.find("ip") != settings.end())
    {
        ip_address = settings["ip"];
    }

    sf::Http http(ip_address);
    sf::Http::Response response = http.sendRequest(sf::Http::Request("/status"));

    if (response.getStatus() != sf::Http::Response::Ok)
    {
        LOG(WARNING) << "Failed to validate username on philips hue bridge: " << response.getStatus();
        LOG(WARNING) << response.getBody();

        if (response.getStatus() == sf::Http::Response::ConnectionFailed)
            return false;
        if (response.getStatus() == sf::Http::Response::NotFound)
            return false;
    }

    run_thread = true;
    update_thread.launch();
    return true;
}

void McLightingDevice::setChannelData(int channel, float value)
{

    switch(channel % 4)
    {
    case 0: if (light.brightness != value * 100) light.dirty = true; light.brightness = value * 100; break;
    case 1: if (light.r != value * 255) light.dirty = true; light.r = value * 255; break;
    case 2: if (light.g != value * 255) light.dirty = true; light.g = value * 255; break;
    case 3: if (light.b != value * 255) light.dirty = true; light.b = value * 255; break;
    }
}

int McLightingDevice::getChannelCount()
{
    return 4;
}

void McLightingDevice::updateLoop()
{
    sf::Http http(ip_address);

    while(run_thread)
    {
        if (light.dirty)
        {
            LightInfo info;
            {
                sf::Lock lock(mutex);
                light.dirty = false;
                info = light;
            }
            string rest_string;
            rest_string = "/all?r="+string(info.r)+"&g="+string(info.g)+"&b="+string(info.b);
            sf::Http::Response response = http.sendRequest(sf::Http::Request(rest_string, sf::Http::Request::Get));
            if (response.getStatus() != sf::Http::Response::Ok)
            {
                LOG(WARNING) << "Failed to set mc Lighting light: " << response.getStatus();
                LOG(WARNING) << response.getBody();
            }
            LOG(INFO) << "McLightingDevice: " << rest_string;
        }
        sf::sleep(sf::milliseconds(50));
    }
}
