#pragma once
#ifndef ASTRA_PAGE__H
#define ASTRA_PAGE__H

#include <sqlite3.h>
#include <vector>
#include <astra/ui/item/menu/menu.h>

namespace cong
{
    class Wifi : public astra::Page
    {
        struct wifi
        {
            std::string SSID;
            std::string password;
        };

    private:
        std::vector<std::string> infoCache = {};
        std::vector<wifi> wifis = {
            {"NN14Z-WIFI", "14145678"},
        };
        int selected = 0;

    public:
        Wifi(const std::string& _title, const std::vector<unsigned char>& _pic);

    public:
        void init(const std::vector<float>& _camera) override;
        void deInit() override;

    public:
        void onLeft() override;
        void onRight() override;
        void onConfirm() override;

    public:
        void addInfo(std::string _msg);
        void clearInfo();
        void render(const std::vector<float>& _camera) override;
    };

    class AP : public astra::Page
    {
    private:
        std::vector<std::string> infoCache = {};
        std::string SSID = "ESP32S3-cong";
        std::string password = "12345678";
        bool enabled = false;

    public:
        AP(const std::string& _title, const std::vector<unsigned char>& _pic);

    public:
        void init(const std::vector<float>& _camera) override;
        void deInit() override;

    public:
        void onLeft() override;
        void onRight() override;
        void onConfirm() override;

    public:
        void addInfo(std::string _msg) { infoCache.push_back(_msg); };
        void clearInfo() { infoCache.clear(); };
        void render(const std::vector<float>& _camera) override;
    };

    class Directory : public astra::List
    {
    private:
        std::string path;
        bool initialized = false;

    public:
        Directory(const std::string& _title, const std::string& _path);

    public:
        void init(const std::vector<float>& _camera) override;
        void deInit() override;
    };

    class TxtPage : public astra::Page
    {
    private:
        String filepath;
        String indexpath;
        sqlite3 *db;

    public:
        TxtPage(const fs::File& file);

    public:
        void init(const std::vector<float>& _camera) override;
        void deInit() override;

    public:
        void onLeft() override;
        void onRight() override;
        void onConfirm() override;

    public:
        void render(const std::vector<float>& _camera) override;

    private:
        void renderTxt();
        void getTxt(int Y);
        void index();
        void writePage();

        void insertString(String key, String value);
        String queryString(String key);

    private:
        String txt[25];
        int page = 1;
        int maxPage = 1;
        String md5value = "";

        int step = 1;

        int rc;
        char *errMsg;
    };

    class JpegViewer : public astra::Page
    {
    private:
        String filepath;

    public:
        JpegViewer(const fs::File& file);

    public:
        void init(const std::vector<float>& _camera) override;
        void deInit() override;

    public:
        void onLeft() override;
        void onRight() override;
        void onConfirm() override;

    public:
        void render(const std::vector<float>& _camera) override;
    };

    class VideoPage : public astra::Page
    {
    private:
        String filepath;

    public:
        VideoPage(fs::File file);

    public:
        void init(const std::vector<float>& _camera) override;
        void deInit() override;

    public:
        void onLeft() override;
        void onRight() override;
        void onConfirm() override;

    public:
        void render(const std::vector<float>& _camera) override;
    };

    class BatteryPage : public astra::Page
    {
    public:
        BatteryPage(const std::string& _title, const std::vector<unsigned char>& _pic);

    public:
        void init(const std::vector<float>& _camera) override;

    public:
        void render(const std::vector<float>& _camera) override;
    };
}

#endif //ASTRA_PAGE__H
