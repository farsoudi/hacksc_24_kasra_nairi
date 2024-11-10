#include "BluetoothSerial.h"
#include "esp_pm.h"      // For esp_pm_configure()

String device_name = "DrawFrame";
esp_pm_lock_handle_t pm_cpu_lock; //handle for CPU lock to prevent the esp32 from entering light sleep mode during bluetooth communication 

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT; //SerialBT declared as an instance of BluetoothSerial, provides serial-like functionalities over Bluetooth

void setup() {
  Serial.begin(115200); //baud rate
  SerialBT.begin(device_name);  //Bluetooth device name
  esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "cpu_lock", &pm_cpu_lock);
  esp_pm_lock_acquire(pm_cpu_lock); // Acquire the lock to prevent light sleep
  // SerialBT.deleteAllBondedDevices(); // Uncomment this to delete paired devices; Must be called after begin
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
}

void loop() 
{
  SerialBT.write('t');  //sends character 't' via Bluetooth at the start of each loop iteration 
  if (Serial.available()) //checks if there is data available from the Serial connection (USB) 
  {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) 
  {
    Serial.write(SerialBT.read());
  }
  delay(20);

  
}