#include "page.h"

#include <SD_MMC.h>
#include <vector>
#include <WiFi.h>
#include <cstring>
#include <EEPROM.h>
#include <TJpg_Decoder.h>
#include <MD5Builder.h>
#include <sqlite3.h>

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

    void Wifi::deInit()
    {
    }

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

        if (infoCache.empty())
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

    void AP::deInit()
    {
    }

    void AP::onConfirm()
    {
        clearInfo();
        addInfo("热点正在开启");
        WiFi.softAP(SSID.c_str(), password.c_str());
        enabled = true;
        clearInfo();
    }

    void AP::onLeft()
    {
    }

    void AP::onRight()
    {
    }

    void AP::render(const std::vector<float>& _camera)
    {
        HAL::canvasClear();

        if (infoCache.empty())
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
        }; //TODO
        path = _path;
    }

    void Directory::init(const std::vector<float>& _camera)
    {
        pinMode(GPIO_NUM_38, PULLDOWN);
        SD_MMC.setPins(GPIO_NUM_40, GPIO_NUM_39, GPIO_NUM_41);

        SD_MMC.begin("/sdcard", true, false, 80000, 255);

        if (!initialized)
        {
            File root = SD_MMC.open(path.c_str());

            File file = root.openNextFile("r");
            while (file)
            {
                if (file.isDirectory())
                {
                    addItem(new Directory(file.name(), file.path()));
                }
                else
                {
                    if (String(file.name()).endsWith(".txt")) addItem(new TxtPage(file));
                    else if (String(file.name()).endsWith(".index")) addItem(new TxtPage(file));//TODO for test only
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

    void Directory::deInit()
    {
    }

    TxtPage::TxtPage(const File& file)
    {
        title = file.name();
        filepath = file.path();
    }

    void TxtPage::insertString(String key, String value)
    {
        int rc = sqlite3_exec(db, (String("INSERT OR REPLACE INTO Strings (KEY, VALUE) VALUES ('") + key + String("', '") + value + String("');")).c_str(), 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }

    String TxtPage::queryString(String key) {
        String query = "SELECT Value FROM Strings WHERE KEY='" + key + "';";

        Serial.println(query);

        String value = "";
        char* errMsg = 0;

        // 定义 lambda 来处理查询结果
        auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
            String* result = reinterpret_cast<String*>(data);
            if (argc > 0 && argv[0]) { // 检查是否有返回值
                *result = argv[0]; // 取第一个返回值（Value列）
            }
            return 0;
        };

        // 执行SQL语句，使用lambda获取结果
        int rc = sqlite3_exec(db, query.c_str(), callback, &value, &errMsg);

        if (rc != SQLITE_OK) {
            Serial.printf("Failed to execute query: %s\n", errMsg);
            sqlite3_free(errMsg); // 释放错误信息
        } else {
            if (value == "") {
                Serial.println("No result found");
            } else
            {
                Serial.println(key + String(" = ") + value);
            }
        }

        return value;
    }

    void TxtPage::init(const std::vector<float>& _camera)
    {
        HAL::canvasClear();
        EEPROM.begin(1024);

        indexpath = "/sdcard";
        indexpath += filepath;
        indexpath += String(".db");

        sqlite3_initialize();

        rc = sqlite3_open_v2(indexpath.c_str(), &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr);
        if (rc) {
            Serial.printf("Can't open database: %s\n", sqlite3_errmsg(db));
            return;
        }

        Serial.println("Calculating md5");
        HAL::printInfo("正在校验文件");
        File file = SD_MMC.open(filepath, "r");
        MD5Builder md5Builder = MD5Builder();
        while (file.available())
        {
            char buffer[512];
            int bytesRead = file.readBytes(buffer, sizeof(buffer));
            md5Builder.add((uint8_t*)buffer, bytesRead); // 转换 char* 为 uint8_t*
        }
        md5Builder.calculate();
        md5value = md5Builder.toString();
        Serial.println("Calculating md5 finished");
        HAL::printInfo("正在检查索引是否匹配");

        String md5 = "";
        md5 = queryString(String("MD5"));

        Serial.println("Read index finished");

        if (md5 != md5value) {
            HAL::printInfo("正在生成索引");
            Serial.println("Index started");
            index();
            Serial.println("Index finished");
        }

        HAL::printInfo("正在获取书签");
        page = queryString("PAGE").toInt();

        HAL::printInfo("正在获取文件信息");
        maxPage = queryString("TOTAL").toInt();

        HAL::printInfo("加载完成");
        HAL::canvasClear();
        getTxt(page);
        renderTxt();
    }

    void TxtPage::index()
    {
        unsigned long i = 0, l = 0, d = 0, y = 0;

        File file = SD_MMC.open(filepath, "r");

        int s = file.size();
        int v;

        Serial.println("Dropping tables");

        rc = sqlite3_exec(db, "DROP TABLE IF EXISTS Strings;", 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        } else {
            Serial.println("Table deleted successfully");
        }

        Serial.println("Creating tables");

        rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Strings (KEY TEXT PRIMARY KEY, VALUE TEXT);", 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        } else {
            Serial.println("Table created successfully");
        }

        Serial.println("Creating index");

        rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }

        Serial.println("Begin transaction");

        insertString("MD5", md5value);

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
            if (d > 57)
            {
                d = 0;
                l++;
            }
            if (l >= 19)
            {
                long t = file.position();
                float value = (float(t) / float(s)) * 100;
                if (v != int(value))
                {
                    v = int(value);
                    Serial.print("处理进度：");
                    Serial.print(int(value));
                    Serial.println("%");
                    HAL::printInfo(String("Processed " + String(int(value)) + " %").c_str());
                }

                y++;
                insertString("L" + String(y), String(file.position()));
                l = 0;

                if (int(value) == 99) break;//TODO
            }
        }
        Serial.println("index finished");
        maxPage = y;
        insertString("TOTAL", String(maxPage));

        rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }

        Serial.println("End transaction");
    }

    void TxtPage::getTxt(int Y)
    {
        int i = 0, l = 0, d = 0;
        int address = 0;

        File file = SD_MMC.open(filepath);

        address = queryString(String("L") + String(Y)).toInt();

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

            if (d > 57)
            {
                d = 0;
                l++;
                txt[l] = "";
            }
            if (l >= 19)
            {
                break;
            }
        }
    }

    void TxtPage::writePage()
    {
        insertString(String("PAGE"), String(page));
    }


    void TxtPage::onConfirm()
    {
        if (step == 1) step = 5;
        else if (step == 5) step = 10;
        else if (step == 10) step = 100;
        else if (step == 100) step = 1000;
        else if (step == 1000) step = 10000;
        else if (step == 10000) step = 1;
        HAL::canvasClear();
        renderTxt();
    }

    void TxtPage::onLeft()
    {
        HAL::canvasClear();
        if (page > step - 1) page -= step; else page = 0;
        getTxt(page);
        writePage();
        renderTxt();
    }

    void TxtPage::onRight()
    {
        HAL::canvasClear();
        page += step;
        getTxt(page);
        writePage();
        renderTxt();
    }

    void TxtPage::renderTxt()
    {
        HAL::setDrawType(1);

        HAL::drawChinese(120, 14, String("x" + String(step)).c_str());

        std::string pageInfo = ("页：" + String(page + 1) + "/" + String(maxPage)).c_str();
        HAL::drawChinese(300 - (pageInfo.length() * HAL::getFontWidth(pageInfo)), 14, pageInfo);

        for (int j = 0; j < 24; j++)
        {
            HAL::drawChinese(0, 16 * (j + 2), txt[j].c_str());
        }
    }

    void TxtPage::deInit()
    {
        sqlite3_close(db);
    }

    void TxtPage::render(const std::vector<float>& _camera)
    {
    }


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
            if (_y >= 320) return false; //TODO
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

        const uint16_t blackW = (systemConfig.screenWeight - w) / 2;
        const uint16_t blackH = (systemConfig.screenHeight - h) / 2;

        TJpgDec.drawSdJpg(blackW, blackH, SD_MMC.open(filepath.c_str()));
    }

    void JpegViewer::render(const std::vector<float>& _camera)
    {
    }

    void JpegViewer::deInit()
    {
    }

    void JpegViewer::onLeft()
    {
    }

    void JpegViewer::onRight()
    {
    }

    void JpegViewer::onConfirm()
    {
    }


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

    BatteryPage::BatteryPage(const std::string& _title, const std::vector<unsigned char>& _pic)
    {
        title = _title;
        pic = _pic;
    }

    void BatteryPage::init(const std::vector<float>& _camera)
    {
        HAL::canvasClear();
    }

    void BatteryPage::render(const std::vector<float>& _camera)
    {
        HAL::delay(200);
        HAL::drawChinese(1, 14, ("电池电压：" + String(analogRead(GPIO_NUM_1) * (3.3 / 4095.0) * 5.0) + "V").c_str());
    }


}