#include "TFTHAL.h"

#include <lwip/sockets.h>
#include <WiFi.h>

bool isLefting = false;
bool isRighting = false;
bool isConfirmed = false;
bool isCanceled = false;

void TFTHAL::init() {
    tft_espi.init();

    button.begin(GPIO_NUM_17);
    button.setLongClickTime(400);
    button.setClickHandler([](Button2&) { isConfirmed = true; });
    button.setLongClickHandler([](Button2&) { isCanceled = true; });

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_17, 0);  // 0 表示低电平触发唤醒
    button.setDoubleClickHandler([](Button2&)
    {
        delay(150);
        esp_light_sleep_start();
    });//双击沉睡）

    rotary.begin(GPIO_NUM_18, GPIO_NUM_16, 4);
    rotary.setLeftRotationHandler([](ESPRotary&) ->void { isLefting = true; });
    rotary.setRightRotationHandler([](ESPRotary&) ->void { isRighting = true; });

    Serial.begin(115200);
}

// 实现屏幕开启和关闭
void TFTHAL::_screenOn() {
    pinMode(GPIO_NUM_2, OUTPUT);
    digitalWrite(GPIO_NUM_2, HIGH);
    tft_espi.fillScreen(TFT_BLACK);
}

void TFTHAL::_screenOff() {
    pinMode(GPIO_NUM_2, OUTPUT);
    digitalWrite(GPIO_NUM_2, LOW);
    tft_espi.fillScreen(TFT_BLACK);
}

void TFTHAL::_canvasClear() {
    sprite.createSprite(tft_espi.width(), tft_espi.height());
    sprite.startWrite();
    sprite.fillScreen(TFT_BLACK);
}

void TFTHAL::_canvasUpdate() {
    sprite.endWrite();
    tft_espi.setSwapBytes(true);
    sprite.pushSprite(0, 0);
    tft_espi.setSwapBytes(false);
}

// 获取画布缓冲区
void* TFTHAL::_getCanvasBuffer() {
    //return sprite.getBuffer();
}

uint8_t TFTHAL::_getBufferTileHeight() {
    return sprite.height();
}

uint8_t TFTHAL::_getBufferTileWidth() {
    return sprite.width();
}

void TFTHAL::_setSelectFont(const uint8_t _font) {
    sprite.setTextFont(_font);
    useU8g2 = false;
}

void TFTHAL::_setFont(const unsigned char *_font){
    u8g2.begin(sprite);
    u8g2.setBackgroundColor(TFT_BLACK);
    u8g2.setForegroundColor(TFT_WHITE);
    u8g2.setFontMode(0);
    u8g2.setFont(_font);
    useU8g2 = true;
}

uint8_t TFTHAL::_getFontWidth(std::string& _text) {
    if (useU8g2) return u8g2.getUTF8Width(_text.c_str());
    else return sprite.textWidth(_text.c_str());
}

uint8_t TFTHAL::_getFontHeight() {
    if (useU8g2) return u8g2.getFontAscent();
    else return sprite.fontHeight();
}

void TFTHAL::_setDrawType(uint8_t _type) {
    // 根据需求设置绘图类型，具体实现依赖于绘图库
}

void TFTHAL::_drawPixel(float _x, float _y) {
    sprite.drawPixel(_x, _y, TFT_WHITE);
}

void TFTHAL::_clearPixel(float _x, float _y) {
    sprite.drawPixel(_x, _y, TFT_BLACK);
}

void TFTHAL::_drawEnglish(float _x, float _y, const std::string& _text) {
    if (useU8g2) u8g2.drawStr(_x, _y, _text.c_str());
    else sprite.drawString(_text.c_str(), _x, _y);
}

void TFTHAL::_drawChinese(float _x, float _y, const std::string& _text) {
    if (useU8g2) u8g2.drawUTF8(_x, _y, _text.c_str());
    else sprite.drawString(_text.c_str(), _x, _y);
}

void TFTHAL::_drawVDottedLine(float _x, float _y, float _h) {
    for (int i = 0; i < _h; i += 2) {
        sprite.drawPixel(_x, _y + i, TFT_WHITE);
    }
}

void TFTHAL::_drawHDottedLine(float _x, float _y, float _l) {
    for (int i = 0; i < _l; i += 2) {
        sprite.drawPixel(_x + i, _y, TFT_WHITE);
    }
}

void TFTHAL::_drawVLine(float _x, float _y, float _h) {
    sprite.drawFastVLine(_x, _y, _h, TFT_WHITE);
}

void TFTHAL::_drawHLine(float _x, float _y, float _l) {
    sprite.drawFastHLine(_x, _y, _l, TFT_WHITE);
}

void TFTHAL::_drawXBMP(float _x, float _y, float _w, float _h, const uint8_t* _bitMap) {
    sprite.startWrite();  // 开始写入显示屏

    int byteWidth = (_w + 7) / 8;  // 每行的字节数
    uint8_t byte = 0;

    sprite.startWrite();  // 开始写入

    for (int j = 0; j < _h; j++) {    // 遍历每一行
        for (int i = 0; i < _w; i++) {  // 遍历每一列
            if (i & 7) {  // 位操作，取出当前位
                byte >>= 1;
            } else {  // 取出新的字节
                byte = _bitMap[j * byteWidth + i / 8];
            }
            if (byte & 0x01) {  // 如果当前位为1，绘制点
                sprite.drawPixel(_x + i, _y + j, TFT_WHITE);
            }
        }
    }

    sprite.endWrite();  // 结束写入
}

void TFTHAL::_drawImage(int16_t _x, int16_t _y, uint16_t _w, uint16_t _h, uint16_t* _bitMap)
{
    sprite.pushImage(_x, _y, _w, _h, _bitMap);
}


void TFTHAL::_drawBox(float _x, float _y, float _w, float _h) {
    sprite.drawRect(_x, _y, _w, _h, TFT_WHITE);
}

void TFTHAL::_drawRBox(float _x, float _y, float _w, float _h, float _r) {
    sprite.drawRoundRect(_x, _y, _w, _h, _r, TFT_WHITE);
}

void TFTHAL::_drawFrame(float _x, float _y, float _w, float _h) {
    sprite.drawRect(_x, _y, _w, _h, TFT_WHITE);
}

void TFTHAL::_drawRFrame(float _x, float _y, float _w, float _h, float _r) {
    sprite.drawRoundRect(_x, _y, _w, _h, _r, TFT_WHITE);
}

// 延时
void TFTHAL::_delay(unsigned long _mill) {
    ::delay(_mill);
}

unsigned long TFTHAL::_millis() {
    return ::millis();
}

unsigned long TFTHAL::_getTick() {
    return ::millis();
}

unsigned long TFTHAL::_getRandomSeed() {
    return esp_random();
}

// 蜂鸣器控制
void TFTHAL::_beep(float _freq) {
    // 根据具体硬件实现蜂鸣器控制
}

void TFTHAL::_beepStop() {
    // 停止蜂鸣器
}

void TFTHAL::_setBeepVol(uint8_t _vol) {
    // 设置蜂鸣器音量
}

void TFTHAL::_startKeyScan() {
    button.loop();
    rotary.loop();
}

bool TFTHAL::_isLeft() {
    if (isLefting)
    {
        isLefting = false;
        return true;
    }
    return false;
}

bool TFTHAL::_isRight() {
    if (isRighting)
    {
        isRighting = false;
        return true;
    }
    return false;
}

bool TFTHAL::_isConfirm() {
    if (isConfirmed)
    {
        isConfirmed = false;
        return true;
    }
    return false;
}

bool TFTHAL::_isCancel() {
    if (isCanceled)
    {
        isCanceled = false;
        return true;
    }
    return false;
}

// 更新配置
void TFTHAL::_updateConfig() {
    // 根据需求实现配置更新
}
