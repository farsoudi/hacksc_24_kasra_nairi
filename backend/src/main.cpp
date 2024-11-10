#include "BluetoothSerial.h"
#include "esp_pm.h"      // For esp_pm_configure()
#include <Arduino.h>
#include <GXEPD2_BW.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

#define CS_PIN 17
#define RST_PIN 5
#define DC_PIN 16
#define DIN_PIN 23
#define CLK_PIN 18
#define BUSY_PIN 19

#define WIDTH 800
#define HEIGHT 480
#define BITS_PER_UNIT 32
#define TOTAL_PIXELS (WIDTH * HEIGHT) //800 * 480 = 384000

#define RX_BUFFER_SIZE 2048 // Increase buffer size

//Define 3 States 
#define WAITING_TO_READ 2  //when program receives \n, change state to 1 
#define READ_PIC 1 //continue to receive data until I've received all TOTAL_PIXELS, and once that condition is met, change state to 3 
#define WRITING_PIC 3

//initial state 
int state = WAITING_TO_READ;
int receivedBits = 0; //tracks how many bits have been received 

String device_name = "DrawFrame";
esp_pm_lock_handle_t pm_cpu_lock; //handle for CPU lock to prevent the esp32 from entering light sleep mode during bluetooth communication 

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT; //SerialBT declared as an instance of BluetoothSerial, provides serial-like functionalities over Bluetooth
//GxEPD2_750_T7(int16_t cs, int16_t dc, int16_t rst, int16_t busy);
GxEPD2_BW<GxEPD2_750_T7, 480> display(GxEPD2_750_T7(CS_PIN, DC_PIN, RST_PIN, BUSY_PIN));


bool isConnected = false;

void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

void processData(uint8_t *data, size_t len) {
  // Process the received data
  for (size_t i = 0; i < len; i++) { //Within this loop, we decide what to do with the chunk
    Serial.write(data[i]);
  }
}


void setup() 
{
  Serial.begin(115200); //baud rate
  SerialBT.begin(device_name);  //Bluetooth device name
  esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "cpu_lock", &pm_cpu_lock);
  esp_pm_lock_acquire(pm_cpu_lock); // Acquire the lock to prevent light sleep

  SerialBT.register_callback(btCallback);

  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());

  SPI.begin(CLK_PIN, -1, DIN_PIN, CS_PIN);

  display.init();
  display.setRotation(1); // Set rotation if needed
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(0, 10);
  display.print("Hello, ePaper!");
  display.display();
}

uint8_t rxBuffer[RX_BUFFER_SIZE];
size_t rxBufferIndex = 0;

void loop() {


  //Recieving data from the bluetooth serial connection:
  if (SerialBT.available()) {
    while (SerialBT.available() && rxBufferIndex < RX_BUFFER_SIZE) {
      rxBuffer[rxBufferIndex++] = SerialBT.read();
    }
    if (rxBufferIndex == RX_BUFFER_SIZE) {
      // Buffer is full, process data
      processData(rxBuffer, rxBufferIndex);
      rxBufferIndex = 0; // Reset buffer index
    }
  }
}

void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    isConnected = true;
    Serial.println("Bluetooth connected");
  } else if (event == ESP_SPP_CLOSE_EVT) {
    isConnected = false;
    Serial.println("Bluetooth disconnected");
    SerialBT.end();
    SerialBT.begin(device_name);
  }
}