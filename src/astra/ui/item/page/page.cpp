#include "page.h"

#include <SD.h>
#include <SD_MMC.h>
#include <vector>
#include <WiFi.h>
#include <cstring>
#include <TJpg_Decoder.h>

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
        HAL::canvasClear();

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
        HAL::canvasClear();

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

            File file = root.openNextFile("r");
            while (file) {
                if (file.isDirectory()) {
                    addItem(new FileList(file.name(), file.path()));
                } else {
                    if (String(file.name()).endsWith(".txt")) addItem(new TxtPage(file));
                    else if (String(file.name()).endsWith(".jpg")) addItem(new ImagePage(file));
                    else if (String(file.name()).endsWith(".mp4")) addItem(new VideoPage(file));
                    else addItem(new List(file.name()));
                }
                file.close();
                file = root.openNextFile();
            }
            initialized = true;
        }

        forePosInit();
        childPosInit(_camera);
    }

    void FileList::deInit() {}

    TxtPage::TxtPage(const File& file)
    {
        title = file.name();
        filepath = file.path();
    }

    void TxtPage::init(const std::vector<float>& _camera)
    {
        File file = SD_MMC.open(filepath.c_str());

    }

    void TxtPage::onConfirm()
    {

    }

    void TxtPage::onLeft()
    {

    }

    void TxtPage::onRight()
    {

    }

    void TxtPage::render(const std::vector<float>& _camera)
    {

    }

    void TxtPage::deInit()
    {

    }


    ImagePage::ImagePage(const File& file)
    {
        title = file.name();
        filepath = file.path();
    }

    void ImagePage::init(const std::vector<float>& _camera)
    {
        HAL::canvasClear();

        // The byte order can be swapped (set true for TFT_eSPI)
        TJpgDec.setSwapBytes(true);

        // The decoder must be given the exact name of the rendering function above
        TJpgDec.setCallback([](int16_t _x, int16_t _y, uint16_t _w, uint16_t _h, uint16_t* _bitmap) -> bool
        {
            if ( _y >= 320 ) return false;//TODO
            HAL::drawImage(_x, _y, _w, _h, _bitmap);
            return true;
        });

        // Get the width and height in pixels of the jpeg if you wish
        uint16_t w = 0, h = 0, scale = 8;
        TJpgDec.getFsJpgSize(&w, &h, SD_MMC.open(filepath.c_str())); // Note name preceded with "/"

        while (scale != 0)
        {
            scale >>= 1;
            if (w * scale > systemConfig.screenWeight && h * scale > systemConfig.screenHeight) break;
        }

        TJpgDec.setJpgScale(scale);

        const uint16_t blackW = ( systemConfig.screenWeight - w ) / 2;
        const uint16_t blackH = ( systemConfig.screenHeight - h ) / 2;

        TJpgDec.drawSdJpg(blackW, blackH, SD_MMC.open(filepath.c_str()));
    }

    void ImagePage::render(const std::vector<float>& _camera){}
    void ImagePage::deInit(){}
    void ImagePage::onLeft(){}
    void ImagePage::onRight(){}
    void ImagePage::onConfirm(){}


    VideoPage::VideoPage(File file)
    {
        title = file.name();
        filepath = file.path();
    }

    void VideoPage::init(const std::vector<float>& _camera)
    {

    }

    void VideoPage::onConfirm()
    {

    }

    void VideoPage::onLeft()
    {

    }

    void VideoPage::onRight()
    {

    }

    void VideoPage::render(const std::vector<float>& _camera)
    {

    }

    void VideoPage::deInit()
    {

    }
}
