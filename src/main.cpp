#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// 定义用于按钮输入的GPIO引脚
#define BUTTON_PIN 0

// BLE服务和特性UUID
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define READ_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define NOTIFY_CHARACTERISTIC_UUID "bc6fefd7-2a30-8a53-f76b-9937bc1ec4c1"

BLEServer *pServer = nullptr;
BLECharacteristic *pReadCharacteristic = nullptr;
BLECharacteristic *pNotifyIndicateCharacteristic = nullptr;

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

  // 创建主动读取的特征
  pReadCharacteristic = pService->createCharacteristic(READ_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  // pReadCharacteristic->setValue("Hello World TEST , Read Value!");

  // 创建通知的特征
  pNotifyIndicateCharacteristic = pService->createCharacteristic(NOTIFY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  pNotifyIndicateCharacteristic->addDescriptor(new BLE2902());

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
    unsigned long nowMillis = millis(); // 获取当前时间戳
    std::string nowStr = std::to_string(nowMillis);
    pReadCharacteristic->setValue("[*] Data from ESP32 ! Time: " + nowStr + " ms !");
    pNotifyIndicateCharacteristic->setValue(nowStr); // 设置特性的值为包含时间戳的数据
    pNotifyIndicateCharacteristic->notify();
    Serial.println("Btn pressed ... current nowMillis: " + String(nowMillis));
    delay(1000); // 间隔1秒发送一次数据
  }
}