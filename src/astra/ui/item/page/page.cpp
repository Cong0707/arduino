#include "page.h"

#include <vector>
#include <WiFi.h>

namespace cong
{
    Wifi::Wifi(const std::string& _title, const std::vector<unsigned char>& _pic)
    {
        title = _title;
        pic = _pic;
    }

    void Wifi::init()
    {
        message = "Please select the Wifi";
    }

    void Wifi::deInit()
    {
    }

    void Wifi::onConfirm()
    {
        message = "connecting";
        ::WiFi.begin(wifis[selected].SSID.c_str(), wifis[selected].password.c_str());
    }

    void Wifi::onLeft()
    {
        if (selected != 0) selected -= 1;
        else selected = wifis.size() - 1;
    }

    void Wifi::onRight()
    {
        if (selected != wifis.size() - 1) selected += 1;
        else selected = 0;
    }

    void Wifi::render(const std::vector<float>& _camera)
    {
        if (WiFi.status() == WL_CONNECTED) wifiSelectedMessage = "connected";
        else wifiSelectedMessage = "SSID: " + wifis[selected].SSID + ", Password: " + wifis[selected].password;
        HAL::drawChinese(0, 20, message);
        HAL::drawChinese(0, 50, wifiSelectedMessage);
    }
}
