#include <BleMouse.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

BleMouse bleMouse;

// 定义用于按钮输入的GPIO引脚
#define BUTTON_PIN 0

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE Mouse...");

    // 初始化BLE鼠标
    bleMouse.begin();

    // 设置按钮引脚为输入
    pinMode(BUTTON_PIN, INPUT_PULLUP); // 使用内部上拉电阻
}

void mouseBack()
{
    // 移动到最底部
    for (unsigned int i = 0; i < 20; i++)
    {
        bleMouse.move(0, 50); // 大幅向下滑动
        delay(10);
    }

    // 稍微向上移动一些
    for (unsigned int i = 0; i < 5; i++)
    {
        bleMouse.move(0, -50);
        delay(10);
    }
}

void loop()
{
    // 检查按钮状态
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        mouseBack();
        delay(100);
        bleMouse.press(MOUSE_LEFT); // 按下鼠标左键
        delay(100);
        // 持续模拟向上滑动
        for (unsigned int i = 0; i < 50; i++)
        {
            bleMouse.move(0, -10); // x不变，y负方向移动，模拟向上滑
            delay(10);
        }

        bleMouse.release(MOUSE_LEFT); // 释放鼠标左键

        // 短暂延迟以避免过快处理
        delay(1000);
    }
}
