#pragma once
#ifndef ASTRA_WIDGET__H
#define ASTRA_WIDGET__H

#include <vector>
#include <astra/ui/item/menu/menu.h>

#include "../item.h"

namespace astra
{
    class CheckBox : public Widget
    {
    private:
        bool isCheck;

    public:
        explicit CheckBox(
            const std::string& _title,
            bool& _value); //check box.

    public:
        bool check();
        bool uncheck();
        bool toggle();

    public:
        void init(const std::vector<float>& _camera) override;
        void deInit() override;

    public:
        void onLeft() override;
        void onRight() override;
        void onConfirm() override;

    public:
        void renderIndicator(float _x, float _y, const std::vector<float>& _camera) override;

    public:
        void render(const std::vector<float>& _camera) override;
    };

    class PopUp : public Widget
    {
    public:
        typedef struct Position
        {
            float x, xTrg;
            float y, yTrg;
        } Position;

        Position position{};

    private:
        std::string text;
        std::vector<std::string> options;
        unsigned char direction;
        unsigned char boundary;

    public:
        // 0: left 1: top 2: right 3: bottom
        PopUp(const std::string& _title,
            const std::string& _text,
            unsigned char _direction,
            const std::vector<std::string>& _options,
            unsigned char& _value); //pop up.

    public:
        void selectNext();
        void selectPreview();
        bool select(unsigned char _index);

    public:
        void init(const std::vector<float>& _camera) override;
        void deInit() override;

    public:
        void onLeft() override;
        void onRight() override;
        void onConfirm() override;

    public:
        void renderIndicator(float _x, float _y, const std::vector<float>& _camera) override;

    public:
        void render(const std::vector<float>& _camera) override;
    };

    class Slider : public Widget
    {
    public:
        typedef struct Position
        {
            float x, xTrg;
            float y, yTrg;

            float l, lTrg; //slider
        } Position;

        Position position{};

    private:
        std::string text;
        unsigned char maxLength;
        unsigned char min;
        unsigned char max;
        unsigned char step;

        bool valueOverflow;

        unsigned char lengthIndicator;

    public:
        Slider(const std::string& _title,
               const std::string& _text,
               unsigned char _min,
               unsigned char _max,
               unsigned char _step,
               unsigned char& _value); //slider.

    public:
        unsigned char add();
        unsigned char sub();

    public:
        void init(const std::vector<float>& _camera) override;
        void deInit() override;

    public:
        void onLeft() override;
        void onRight() override;
        void onConfirm() override;

    public:
        void renderIndicator(float _x, float _y, const std::vector<float>& _camera) override;

    public:
        void render(const std::vector<float>& _camera) override;
    };
}

#endif //ASTRA_WIDGET__H
