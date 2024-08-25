//
// Created by Fir on 2024/2/2.
//

#include "launcher.h"

namespace astra
{
    void Launcher::popInfo(std::string _info, uint16_t _time)
    {
        static bool init = false;
        static unsigned long long int beginTime = this->time;;
        static bool onRender = false;

        if (!init)
        {
            init = true;
            beginTime = this->time;
            onRender = true;
        }

        float wPop = HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin; //宽度
        float hPop = HAL::getFontHeight() + 2 * getUIConfig().popMargin; //高度
        float yPop = 0 - hPop - 8; //从屏幕上方滑入
        float yPopTrg = (HAL::getSystemConfig().screenHeight - hPop) / 3; //目标位置 中间偏上
        float xPop = (HAL::getSystemConfig().screenWeight - wPop) / 2; //居中

        while (onRender)
        {
            time++;

            HAL::canvasClear();
            /*渲染一帧*/
            currentMenu->render(camera->getPosition());
            selector->render(camera->getPosition());
            camera->update(currentMenu, selector);
            /*渲染一帧*/

            HAL::setDrawType(0);
            HAL::drawRBox(xPop - 4, yPop - 4, wPop + 8, hPop + 8, getUIConfig().popRadius + 2);
            HAL::setDrawType(1); //反色显示
            HAL::drawRFrame(xPop - 1, yPop - 1, wPop + 2, hPop + 2, getUIConfig().popRadius); //绘制一个圆角矩形
            HAL::drawEnglish(xPop + getUIConfig().popMargin,
                             yPop + getUIConfig().popMargin + HAL::getFontHeight(),
                             _info); //绘制文字
            HAL::canvasUpdate();

            Animation::move(&yPop, yPopTrg, getUIConfig().popSpeed); //动画

            if (time - beginTime >= _time) yPopTrg = 0 - hPop - 8; //滑出

            if (yPop == 0 - hPop - 8)
            {
                onRender = false; //退出条件
                init = false;
            }
        }
    }

    void Launcher::init(Menu* _rootPage)
    {
        currentMenu = _rootPage;

        camera = new Camera(0, 0);
        _rootPage->init(camera->getPosition());

        selector = new Selector();
        selector->inject(_rootPage);

        camera->init(_rootPage->getType());
    }

    /**
     * @brief 打开选中的页面
     *
     * @return 是否成功打开
     * @warning 仅可调用一次
     */
    bool Launcher::open()
    {
        //如果当前页面指向的当前item没有后继 那就返回false
        if (currentMenu->getNextMenu() == nullptr)
        {
            popInfo("unreferenced page!", 300);
            return false;
        }
        else if (currentMenu->getNextMenu()->getType() == "Divider")
        {
            return false;
        }
        else
        {
            /*
            if (currentMenu->getNextMenu()->getType() != "Page")
            {
                if (currentMenu->getNextMenu()->getItemNum() <= 0)
                {
                    popInfo("empty page!", 300);
                    return false;
                }
            }
            */

            currentMenu->rememberCameraPos(camera->getPositionTrg());

            currentMenu->deInit(); //先析构（退场动画）再挪动指针

            currentMenu = currentMenu->getNextMenu();
            currentMenu->init(camera->getPosition());

            selector->inject(currentMenu);

            return true;
        }
    }

    /**
     * @brief 关闭选中的页面
     *
     * @return 是否成功关闭
     * @warning 仅可调用一次
     */
    bool Launcher::close()
    {
        if (currentMenu->getPreview() == nullptr)
        {
            popInfo("unreferenced page!", 300);
            return false;
        }
        if (currentMenu->getPreview()->getItemNum() <= 0)
        {
            popInfo("empty page!", 300);
            return false;
        }

        currentMenu->rememberCameraPos(camera->getPositionTrg());

        currentMenu->deInit(); //先析构（退场动画）再挪动指针

        currentMenu = currentMenu->getPreview();
        currentMenu->init(camera->getPosition());

        selector->inject(currentMenu);

        return true;
    }

    void Launcher::update()
    {
        HAL::canvasClear();

        currentMenu->render(camera->getPosition());
        if (currentMenu->getType() == "Widget") currentMenu->render(camera->getPosition());
        selector->render(camera->getPosition());
        camera->update(currentMenu, selector);


        //if (time == 100) selector->go(0); //test

        /*
        if (time == 200) selector->go(1); //test
        if (time == 300) selector->go(2); //test
        if (time == 400) selector->go(3); //test
        if (time == 500) selector->go(4); //test
        */
        /*
        if (time == 200) open(); //test

        if (time == 300) selector->go(1); //test
        if (time == 400) open();
        if (time == 700) close();

        if (time == 800) selector->go(2); //test
        if (time == 900) open();
        if (time == 1200) close();

        if (time == 1300) selector->go(3); //test
        if (time == 1400) open();
        if (time == 1700) close();

        if (time == 1800) close();
        if (time == 1900) selector->go(0); //test
        */

        //if (time >= 700) time = 0; //test

        if (currentMenu->getType() == "Widget")
        {
            if (HAL::isLeft()) static_cast<Widget*>(currentMenu)->onLeft();
            else if (HAL::isRight()) static_cast<Widget*>(currentMenu)->onRight();
            else if (HAL::isConfirm()) static_cast<Widget*>(currentMenu)->onConfirm();
            else if (HAL::isCancel()) close();
        }
        else if (currentMenu->getType() == "Page")
        {
            if (HAL::isLeft()) static_cast<Page*>(currentMenu)->onLeft();
            else if (HAL::isRight()) static_cast<Page*>(currentMenu)->onRight();
            else if (HAL::isConfirm()) static_cast<Page*>(currentMenu)->onConfirm();
            else if (HAL::isCancel()) close();
        }
        else
        {
            if (HAL::isLeft()) selector->goPreview();
            else if (HAL::isRight()) selector->goNext();
            else if (HAL::isConfirm()) open();
            else if (HAL::isCancel()) close();
        }


        HAL::canvasUpdate();

        //time++;
        time = HAL::millis() / 1000;
    }
}
