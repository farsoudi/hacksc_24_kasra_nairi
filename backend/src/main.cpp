#include <Arduino.h>
#include <BluetoothSerial.h> //Bluetooth Library. Bluetooth communication on ESP32, enabling microcontroller to send and receive data via Bluetooth
//#include <SPI.h> //facilitates communication between ESP32 and Serial Peripheral Interface (used for connecting devices: Displays )
//#include <GXEPD2.h> //To control the Waveshare e-paper display. Provides functions to draw on the screen, clear it, refresh it 
BluetoothSerial SerialBT; //object handles Bluetooth communications. Allows ESP32 to connect to a Bluetooth device and send/receive data 
//GXEPD2<GxEPD2_154_C21, GxEPD2::BEST> display(GxEPD2_154_C21(15, 4, 16, 17)); // EXAMPLE PINS. Object interacts with Waveshare e-paper display. Template parameters define type of e-paper used, pin numers for connecting display to ESP32 
// put function declarations here:

void setup() 
{ //Initializes Bluetooth communication (waiting for a connection with a device). Sets up the e-paper display (Waveshare) and prepares it to show content 
  // put your setup code here, to run once:
  // Start the serial communication for debugging
  Serial.begin(115200); //initializes serial communication for debugging (using a USB connection to computer). Outputs messages to Serial Monitor 

  // Initialize Bluetooth Serial
  SerialBT.begin("DrawFrame"); // The name ESP32 will broadcast for Bluetooth pairing
  Serial.println("Bluetooth Started. Waiting for connection...");

  // Initialize the display
//  display.init();  //initializes e-paper display
 // display.setRotation(1); //sets orientation of display (rotating 90)
  //display.fillScreen(GxEPD_WHITE); //clears display and sets background to white 
 // display.display(); //refreshes e-paper screen to show updates 
}

void loop() 
{ //Continusouly checks for incoming data over Bluetooth/ When data is received, it processes the data (prints it) and refreshes display with new content. 
  //Display is cleraed each time before drawing the new data to make sure it shows the most recent imaage via Bluetooth Wha
  // put your main code here, to run repeatedly:
  // Check if Bluetooth data is available
  if (SerialBT.available())  //checks if there is incoming data from bluetooth dev. If data, return true and ESP32 will process
  {
    String incomingData = SerialBT.readString(); //Receives bluetooth data as a string 
    Serial.println("Received Bluetooth data:"); 
    Serial.println(incomingData); 
    //String drawingData = SerialBT.readString(); // Receive drawing data
    //Serial.println("Received drawing data:"); //outputs received drawing data to Serial Monitor for debugging. Helps check if ESP32 is receiving data correctly 
   // Serial.println(drawingData); // ""

    // Process the drawing data (you would likely need to convert it to pixel values)
    // and then update the display
    //display.fillScreen(GxEPD_WHITE);  // Clear the display, white background
    // Example of drawing, this depends on your data format
   // display.setCursor(0, 0); //moves cursor to the top left of display, ready to print new data 
   // display.print(drawingData); // Prints received data onto the display 
   // display.display(); // Refresh display screen with new pixels (new drawing data)
  }
  SerialBT.println("Hello from ESP32!");
}

