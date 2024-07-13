#include "../hal.h"
#include "u8g2_for_tft_espi/U8g2_for_TFT_eSPI.h"
#include <TFT_eSPI.h>

class TFTHAL : public HAL {
protected:
    TFT_eSPI tft_espi = TFT_eSPI();
    TFT_eSprite sprite = TFT_eSprite(&tft_espi);
    U8g2_for_TFT_eSPI u8g2;
    bool useU8g2 = false;
public:
    void init() override {
        tft_espi.init();
        pinMode(GPIO_NUM_10, INPUT_PULLDOWN);
        pinMode(GPIO_NUM_11, INPUT_PULLDOWN);
        pinMode(GPIO_NUM_12, INPUT_PULLDOWN);
    }
public:
    void _screenOn() override;
    void _screenOff() override;
public:
    void* _getCanvasBuffer() override;
    uint8_t _getBufferTileHeight() override;
    uint8_t _getBufferTileWidth() override;
    void _canvasUpdate() override;
    void _canvasClear() override;
    void _setSelectFont(uint8_t _font) override;
    void _setFont(const unsigned char * _font) override;
    uint8_t _getFontWidth(std::string& _text) override;
    uint8_t _getFontHeight() override;
    void _setDrawType(uint8_t _type) override;
    void _drawPixel(float _x, float _y) override;
    void _clearPixel(float _x, float _y) override;
    void _drawEnglish(float _x, float _y, const std::string& _text) override;
    void _drawChinese(float _x, float _y, const std::string& _text) override;
    void _drawVDottedLine(float _x, float _y, float _h) override;
    void _drawHDottedLine(float _x, float _y, float _l) override;
    void _drawVLine(float _x, float _y, float _h) override;
    void _drawHLine(float _x, float _y, float _l) override;
    void _drawXBMP(float _x, float _y, float _w, float _h, const uint8_t* _bitMap) override;
    void _drawBox(float _x, float _y, float _w, float _h) override;
    void _drawRBox(float _x, float _y, float _w, float _h, float _r) override;
    void _drawFrame(float _x, float _y, float _w, float _h) override;
    void _drawRFrame(float _x, float _y, float _w, float _h, float _r) override;

public:
    void _delay(unsigned long _mill) override;
    unsigned long _millis() override;
    unsigned long _getTick() override;
    unsigned long _getRandomSeed() override;

public:
    void _beep(float _freq) override;
    void _beepStop() override;
    void _setBeepVol(uint8_t _vol) override;

public:
    bool _isLeft() override;
    bool _isRight() override;
    bool _isConfirm() override;
    void _startKeyScan() override;

public:
    void _updateConfig() override;
};