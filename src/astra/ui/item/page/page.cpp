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
        clearInfo();
    }

    void Wifi::deInit() {}

    void Wifi::onConfirm()
    {
        clearInfo();
        if (!WiFi.isConnected())
        {
            addInfo("正在连接");
            ::WiFi.begin(wifis[selected].SSID.c_str(), wifis[selected].password.c_str());
            addInfo(".");
            while(!WiFi.isConnected())
            {
                delay(500);
                infoCache[infoCache.size() - 1] = infoCache[infoCache.size() - 1] + ".";
            }
            clearInfo();
        }
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

    void Wifi::addInfo(std::string _msg)
    {
        infoCache.push_back(_msg);
    }

    void Wifi::clearInfo() { infoCache.clear(); }

    void Wifi::render(const std::vector<float>& _camera)
    {
        if ( infoCache.empty() )
        {
            if (WiFi.isConnected())
            {
                addInfo("Wifi连接成功");
                addInfo("");
                addInfo("网络名称: " + std::string(WiFi.SSID().c_str()));
                addInfo("IP: " + std::string(WiFi.localIP().toString().c_str()));
                if (WiFi.enableIpV6()) addInfo("IPv6: " + std::string(WiFi.localIPv6().toString().c_str()));
                addInfo("Mac: " + std::string(WiFi.macAddress().c_str()));
                addInfo("网关IP: " + std::string(WiFi.gatewayIP().toString().c_str()));
            }
            else
            {
                addInfo("请选择Wifi");
                addInfo("");
                addInfo("网络名称: " + std::string(wifis[selected].SSID.c_str()));
                addInfo("密码: "/* + std::string(wifis[selected].password.c_str())*/);
            }
        }
        static const unsigned char _fontHeight = HAL::getFontHeight();
        HAL::canvasClear();
        for (unsigned char i = 0; i < infoCache.size(); i++)
        {
            HAL::drawChinese(0, _fontHeight + i * (5 + _fontHeight) + 10, infoCache[i]);
        }
        HAL::canvasUpdate();
    }
}
