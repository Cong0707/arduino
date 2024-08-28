#include "page.h"

#include <SD_MMC.h>
#include <vector>
#include <WiFi.h>
#include <cstring>
#include <EEPROM.h>
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

    Directory::Directory(const std::string& _title, const std::string& _path)
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

    void Directory::init(const std::vector<float>& _camera)
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
                    addItem(new Directory(file.name(), file.path()));
                } else {
                    if (String(file.name()).endsWith(".txt")) addItem(new TxtPage(file));
                    else if (String(file.name()).endsWith(".jpg")) addItem(new JpegViewer(file));
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

    void Directory::deInit() {}

    TxtPage::TxtPage(const File& file)
    {
        title = file.name();
        filepath = file.path();
    }

    void TxtPage::init(const std::vector<float>& _camera)
    {
        HAL::canvasClear();
        EEPROM.begin(1024);
        page = readString(1).toInt();
        maxPage = readString(10).toInt();

        SD_MMC.open(filepath, "r");

        String indexPath = filepath;
        indexPath.replace(".txt", ".txt.index");
        File fileSY = SD_MMC.open(indexPath, "r");
        if (!fileSY) index(indexPath);

        this->indexpath = indexPath;

        getTxt(page);
        renderTxt();
    }

    void TxtPage::getTxt(int Y)
    {
        int i = 0, l = 0, d = 0;
        int address = 0;

        if (Y != 0)
        {
            File SYfile = SD_MMC.open(indexpath, "r");
            String ST_L;
            while (SYfile.available())
            {
                byte ST_T = SYfile.read();
                ST_L += (char)ST_T;
                if (ST_T == '\n')
                {
                    if (ST_L.substring(ST_L.indexOf("L") + 1, ST_L.indexOf(":")).toInt() == Y)
                    {
                        address = ST_L.substring(ST_L.indexOf(":") + 1, ST_L.indexOf("\n")).toInt();
                        break;
                    }
                    else
                    {
                        ST_L = "";
                    }
                }
            }
        }
        File file = SD_MMC.open(filepath, "r");
        file.seek(address, SeekSet);
        txt[0] = "";
        while (file.available())
        {
            byte t = file.read();
            if ((t <= 0xF7) && (t >= 0xB0))
            {
                txt[l] += (char)t;
                txt[l] += (char)file.read();
                txt[l] += (char)file.read();
                i += 3;
                d += 3;
            }
            else if (t == 0xE3)
            {
                txt[l] += (char)t;
                txt[l] += (char)file.read();
                txt[l] += (char)file.read();
                i += 3;
                d += 3;
            }
            else if (t == '\n')
            {
                i++;
                l++;
                d = 0;
                txt[l] = "";
            }
            else
            {
                txt[l] += (char)t;
                i++;
                d++;
            }

            if (d > 60)
            {
                d = 0;
                l++;
                txt[l] = "";
            }
            if (l >= 24)
            {
                break;
            }
        }
    }

    void TxtPage::index(String indexPath)
    {
        unsigned long i = 0, l = 0, d = 0, y = 0;
        int s = SD_MMC.open(filepath, "r").size();
        String adds;
        int v;

        File SYfile = SD_MMC.open(indexPath, "a");
        File file = SD_MMC.open(filepath, "r");
        while (file.available())
        {
            byte t = file.read();
            if ((t <= 0xF7) && (t >= 0xB0))
            {
                t = file.read();
                t = file.read();
                i += 3;
                d += 3;
            }
            else if (t == 0xE3)
            {
                txt[l] += (char)t;
                txt[l] += (char)file.read();
                txt[l] += (char)file.read();
                i += 3;
                d += 3;
            }
            else if (t == '\n')
            {
                i++;
                l++;
                d = 0;
            }
            else
            {
                txt[l] += (char)t;
                i++;
                d++;
            }
            if (d > 20)
            {
                d = 0;
                l++;
            }
            if (l >= 20)
            {
                long t = file.position();
                float value = (float(t) / float(s)) * 100;
                if (v != int(value))
                {
                    v = int(value);
                    String info = "处理进度：" + String(value) + "%";
                    Serial.println(info);
                    HAL::printInfo(info.c_str());
                }

                y++;
                adds = "L" + String(y) + ":" + String(file.position()) + "\n";
                SYfile.print(adds);
                l = 0;
            }
        }
        maxPage = y;
        writeString(10, String(maxPage));
    }

    void TxtPage::writeString(int a, String str)
    {
        for (int i = 0; i < str.length(); i++)
        {
            EEPROM.write(a + i, str[i]);
        }
        EEPROM.write(a + str.length(), 0);
        EEPROM.commit();
    }

    String TxtPage::readString(int a)
    {
        String str;
        char ch = EEPROM.read(a);
        while (ch != 0)
        {
            str += ch;
            a += 1;
            ch = EEPROM.read(a);
        }
        return str;
    }

    void TxtPage::onConfirm()
    {

    }

    void TxtPage::onLeft()
    {
        HAL::canvasClear();
        if (page > 0) page--;
        getTxt(page);
        writeString(1, String(page));
        renderTxt();
    }

    void TxtPage::onRight()
    {
        HAL::canvasClear();
        page++;
        getTxt(page);
        writeString(1, String(page));
        renderTxt();
    }

    void TxtPage::renderTxt()
    {
        HAL::setDrawType(1);

        std::string pageInfo = ("页：" + String(page + 1) + "/" + String(maxPage)).c_str();
        HAL::drawChinese(300 - (pageInfo.length() * HAL::getFontWidth(pageInfo)), 14, pageInfo);

        for (int j = 0; j < 24; j++)
        {
            HAL::drawChinese(0, 16 * (j + 2), txt[j].c_str());
        }
    }

    void TxtPage::deInit(){}
    void TxtPage::render(const std::vector<float>& _camera){}


    JpegViewer::JpegViewer(const File& file)
    {
        title = file.name();
        filepath = file.path();
    }

    void JpegViewer::init(const std::vector<float>& _camera)
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

    void JpegViewer::render(const std::vector<float>& _camera){}
    void JpegViewer::deInit(){}
    void JpegViewer::onLeft(){}
    void JpegViewer::onRight(){}
    void JpegViewer::onConfirm(){}


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
