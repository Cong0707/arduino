//
// Created by Fir on 2024/2/8.
//

#include <cstring>
#include "hal.h"

#include <vector>

HAL* HAL::hal = nullptr;

HAL* HAL::get()
{
    return hal;
}

bool HAL::check()
{
    return hal != nullptr;
}

bool HAL::inject(HAL* _hal)
{
    if (_hal == nullptr)
    {
        return false;
    }

    _hal->init();
    hal = _hal;
    return true;
}

void HAL::destroy()
{
    if (hal == nullptr) return;

    delete hal;
    hal = nullptr;
}

/**
 * @brief log printer. 自动换行的日志输出
 *
 * @param _msg message want to print. 要输出的信息
 * @note cannot execute within a loop. 不能在循环内执行
 */
void HAL::_printInfo(std::string _msg)
{
    static std::vector<std::string> _infoCache = {};
    static const unsigned char _max = getSystemConfig().screenHeight / getFontHeight();
    static const unsigned char _fontHeight = getFontHeight();

    if (_infoCache.size() >= _max) _infoCache.clear();
    _infoCache.push_back(_msg);

    canvasClear();
    setDrawType(2); //反色显示
    for (unsigned char i = 0; i < _infoCache.size(); i++)
    {
        drawChinese(0, _fontHeight + i * (1 + _fontHeight), _infoCache[i]);
    }
    canvasUpdate();
    setDrawType(1); //回归实色显示
}
