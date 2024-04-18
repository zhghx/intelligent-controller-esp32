#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// 定义用于按钮输入的GPIO引脚
#define BUTTON_PIN 0

// BLE服务和特性UUID
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;

// 按钮的状态检测
volatile bool buttonPressed = false;
unsigned long buttonPressTime = 0;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("Device connected");
  }

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("Device disconnected");
    // 断开连接后重新开始广播，以便设备可以再次被发现
    BLEDevice::startAdvertising();
    Serial.println("Restarting advertising...");
  }
};

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // 初始化BLE设备
  BLEDevice::init("zhghx_esp_32");

  // 创建BLE服务器
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);

  pCharacteristic->setValue("Hello World TEST 111111111");
  pService->start();

  // 开始广播
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // 设置广告间隔的最小值
  pAdvertising->setMaxPreferred(0x12); // 设置广告间隔的最大值
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop()
{
  // 检测按钮是否按下
  if (digitalRead(BUTTON_PIN) == LOW)
  {
    unsigned long now = millis();                              // 获取当前时间戳
    char buf[50];                                              // 创建足够大的缓冲区来存储消息和时间戳
    sprintf(buf, "[*] Data from ESP32 ! Time: %lu ms !", now); // 格式化字符串包含时间戳
    pCharacteristic->setValue(buf);                            // 设置特性的值为包含时间戳的数据
    pCharacteristic->notify();
    Serial.println("Btn pressed ... ");
    delay(1000); // 间隔1秒发送一次数据
  }
}