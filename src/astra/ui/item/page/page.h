#pragma once
#ifndef ASTRA_PAGE__H
#define ASTRA_PAGE__H

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
            {"exampleWifi", "examplePassWord"}
        };
        int selected = 0;

    public:
        Wifi(); //check box.
        Wifi(const std::string& _title, const std::vector<unsigned char>& _pic);

    public:
        bool check();
        bool uncheck();
        bool toggle();

    public:
        void init() override;
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
}

#endif //ASTRA_PAGE__H
