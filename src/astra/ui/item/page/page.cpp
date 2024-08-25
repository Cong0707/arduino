#include "page.h"

#include <SD.h>
#include <SD_MMC.h>
#include <vector>
#include <WiFi.h>

namespace cong
{
    Wifi::Wifi(const std::string& _title, const std::vector<unsigned char>& _pic)
    {
        title = _title;
        pic = _pic;
    }

    void Wifi::init(const std::vector<float>& _camera)
    {
        clearInfo();
        WiFi.mode(WIFI_MODE_APSTA);
    }

    void Wifi::deInit() {}

    void Wifi::onConfirm()
    {
        clearInfo();
        if (!WiFi.isConnected())
        {
            addInfo("正在连接");
            ::WiFi.begin(wifis[selected].SSID.c_str(), wifis[selected].password.c_str());
            clearInfo();
        }
    }

    void Wifi::onLeft()
    {
        if (selected != 0) selected -= 1;
        else selected = wifis.size() - 1;
        clearInfo();
    }

    void Wifi::onRight()
    {
        if (selected != wifis.size() - 1) selected += 1;
        else selected = 0;
        clearInfo();
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

    AP::AP(const std::string& _title, const std::vector<unsigned char>& _pic)
    {
        title = _title;
        pic = _pic;
    }

    void AP::init(const std::vector<float>& _camera)
    {
        clearInfo();
        WiFi.mode(WIFI_MODE_APSTA);
    }

    void AP::deInit() {}

    void AP::onConfirm()
    {
        clearInfo();
        addInfo("热点正在开启");
        WiFi.softAP(SSID.c_str(), password.c_str());
        enabled = true;
        clearInfo();
    }

    void AP::onLeft() {}

    void AP::onRight() {}

    void AP::render(const std::vector<float>& _camera)
    {
        if ( infoCache.empty() )
        {
            if (enabled)
            {
                addInfo("热点已开启");
                addInfo("");
                addInfo("网络名称: " + std::string(WiFi.softAPSSID().c_str()));
                addInfo("网络密码: " + std::string(password));
                addInfo("网关IP: " + std::string(WiFi.softAPNetworkID().toString().c_str()));
            }
            else
            {
                addInfo("热点已关闭");
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

    FileList::FileList(const std::string& _title, const std::string& _path)
    {
        title = _title;
        pic = {
            0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F,
            0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF0, 0xE7, 0x3F, 0x7F, 0xE0, 0xE7, 0x3F,
            0x7F, 0xE0, 0xC3, 0x3F, 0x7F, 0xE0, 0xC3, 0x3F, 0x7F, 0xE0, 0xC3, 0x3F, 0x7F, 0xE0, 0xE7, 0x3F,
            0xFF, 0xF0, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F,
            0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xC3, 0x3F, 0xFF, 0xF9, 0x81, 0x3F, 0xFF, 0xF0, 0x81, 0x3F,
            0xFF, 0xF0, 0x81, 0x3F, 0xFF, 0xF0, 0x81, 0x3F, 0xFF, 0xF9, 0x81, 0x3F, 0xFF, 0xF9, 0xC3, 0x3F,
            0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F,
            0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F
        };//TODO
        path = _path;
    }

    void FileList::init(const std::vector<float>& _camera)
    {
        pinMode(GPIO_NUM_38, PULLDOWN);
        SD_MMC.setPins(GPIO_NUM_40, GPIO_NUM_39, GPIO_NUM_41);

        SD_MMC.begin("/sdcard", true);

        if (!initialized)
        {
            File root = SD_MMC.open(path.c_str());

            File file = root.openNextFile();
            while (file) {
                if (file.isDirectory()) {
                    addItem(new FileList(file.name(), file.path()));
                } else {
                    addItem(new List(file.name()));
                }
                file = root.openNextFile();
            }
            initialized = true;
        }

        forePosInit();
        childPosInit(_camera);
    }

    void FileList::deInit() {}

}
