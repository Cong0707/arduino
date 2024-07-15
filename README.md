## WIP

### 基于arduino和astra UI编写的不知道用来干嘛的东西

### 硬件配置

点击可跳转购买页面 非广告 仅供参考

_[esp32 s3 n16r8 开发板](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4a402e8dklSa6K&id=718248966902&_u=f20ek70nkl5e0c)_

_[st3375 1.8寸tft屏幕](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4a402e8dklSa6K&id=583173435654&_u=f20ek70nkl979b)_

_[SIQ-02FVS3 编码器](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4a402e8dklSa6K&id=724980992821&_u=f20ek70nkle197)_

_[插线配置](插线.jpg)_ 按钮为gpio 10 11 12 拉高电平为按下

### 开发环境

Clion + platformIO 

_[安装教程参考](https://blog.csdn.net/qq_26700087/article/details/121444631)_

其实就是下个clion 然后装个platformio 再进clion装个配套插件 导入项目就行

在pio同步完成后可将src内的User_Setup配置文件覆盖进tft_espi的配置文件内 插线配置在上面有