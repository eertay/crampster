#include <Wire.h> 
#include <Adafruit_Sensor.h>
#include <Adafruit_TMP117.h> // TEMP sensor
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <SparkFun_Bio_Sensor_Hub_Library.h>

#define FRAMESIZE  170
// Pulseox: Reset pin, MFIO pin
int resPin = 4;
int mfioPin = 5;
// Possible widths: 69, 118, 215, 411us
int width = 411;  //pulseox
// Possible samples: 50, 100, 200, 400, 800, 1000, 1600, 3200 samples/second
// Not every sample amount is possible with every width; check out our hookup
// guide for more information.
int samples = 200;  //pulseox
int pulseWidthVal; //pulseox
int sampleVal; //pulseox

Adafruit_TMP117 sensor1;
Adafruit_TMP117 sensor2;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body; 

// input
// volatile uint16_t input = 0x00;
volatile uint16_t airtemp = 0x00;
volatile uint16_t bodytemp = 0x00;
volatile uint32_t irled_inp = 0x00;
volatile uint32_t redled_inp = 0x00;

volatile char b1, b2, b3, b4, b5, b6, b7, b8, b9, b10 = 0x00;
uint16_t temp1 = 0;
uint16_t temp2 = 0;

uint8_t dataFrame[FRAMESIZE]; 
int j = 0;


  // Timer Counter
uint32_t oldtime = 0x00;

// UUUID
#define SERVICE_UUID "00000001-1000-2000-3000-111122223333"
#define CHARACTERISTIC_UUID_RX "00000002-1000-2000-3000-111122223333"
#define CHARACTERISTIC_UUID_TX "00000003-1000-2000-3000-111122223333"

// BLE vars
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0; 
BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length()
        ; i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};

void ble_init() {
  // Create the BLE Device
  BLEDevice::init("crampster");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_WRITE |
                    BLECharacteristic::PROPERTY_READ
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  // BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
	// 										 CHARACTERISTIC_UUID_RX,
	// 										BLECharacteristic::PROPERTY_READ
	// 									);

   pTxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}


int get_temp1() {
  // sensor1.setMeasurementMode(TMP117_MODE_ONE_SHOT);
  sensors_event_t temp1;
  sensor1.getEvent(&temp1); //fill the empty event object with the current measurements
  return temp1.temperature;
}

int get_temp2() {
  // sensor2.setMeasurementMode(TMP117_MODE_ONE_SHOT);
  sensors_event_t temp2;
  sensor2.getEvent(&temp2); //fill the empty event object with the current measurements
  return temp2.temperature;
}


uint32_t get_irled() {

  body = bioHub.readSensor();
  return body.irLed;
}

uint32_t get_redled() {
  body = bioHub.readSensor();
  return body.redLed;
}
//caluclate this on the backend, waste of resources
// uint16_t get_bpm() {
//   body = bioHub.readSensorBpm();
//   return body.heartRate;
// }

void setup(){
  Serial.begin(115200);
  Serial.println("starting");
  // Setup code for TMP sensors
  
  Wire.begin();
  // analogReadResolution(12);

  //initialize both sensors
  int pulseOx_result = bioHub.begin();
  int temp1_result = sensor1.begin(0x49); // Air temp
  int temp2_result = sensor2.begin(0x48);

  //check to see if they started
  if (pulseOx_result == 0 && temp1_result == 0 && temp2_result == 0) {// Zero errors!
    Serial.println("All sensors started!");
  }

  //configure pulseox sensor intto sensor mode
  Serial.println("Configuring PulseOx Sensor...."); 
  int error = bioHub.configSensor(); // Configure Sensormode , most basic mode returning only LED values
  if (error == 0){// Zero errors.
    Serial.println("PulseOx Sensor configured.");
  }
  else {
    Serial.println("Error configuring pulse ox sensor.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }
  sensor1.setMeasurementMode(TMP117_MODE_ONE_SHOT);
  sensor1.setAveragedSampleCount(TMP117_AVERAGE_1X);  //Start with 1 but try more later
  // sensor1.setAveragedSampleCount(TMP117_AVERAGE_8X);
  // sensor1.setAveragedSampleCount(TMP117_AVERAGE_32X);
  sensor1.setReadDelay(TMP117_DELAY_0_MS);

  sensor2.setMeasurementMode(TMP117_MODE_ONE_SHOT);
  sensor2.setAveragedSampleCount(TMP117_AVERAGE_1X);
  // sensor2.setAveragedSampleCount(TMP117_AVERAGE_8X);
  // sensor2.setAveragedSampleCount(TMP117_AVERAGE_32X);
  sensor2.setReadDelay(TMP117_DELAY_0_MS);

  // Set pulse width.
  error = bioHub.setPulseWidth(width);
  if (error == 0){// Zero errors.
    Serial.println("Pulse Width Set.");
  }
  else {
    Serial.println("Could not set Pulse Width.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }

  // Check that the pulse width was set. 
  pulseWidthVal = bioHub.readPulseWidth();
  Serial.print("Pulse Width: ");
  Serial.println(pulseWidthVal);

  // Set sample rate per second. Remember that not every sample rate is
  // available with every pulse width. Check hookup guide for more information.  
  error = bioHub.setSampleRate(samples);
  if (error == 0){// Zero errors.
    Serial.println("Sample Rate Set.");
  }
  else {
    Serial.println("Could not set Sample Rate!");
    Serial.print("Error: "); 
    Serial.println(error); 
  }

  // Check sample rate.
  sampleVal = bioHub.readSampleRate();
  Serial.print("Sample rate is set to: ");
  Serial.println(sampleVal); 
  
  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. 
  Serial.println("Loading up the buffer with data....");
  //set the intial temp readings
  airtemp = get_temp1();
  bodytemp = get_temp2();

  delay(1000);
  ble_init();
  oldtime = millis();

}

void loop() {
//Need to update the value every 1 second, but write to the array every sample/iterationm
  if ( (millis()-oldtime) >= 1000 ) {
    airtemp = get_temp1();
    bodytemp = get_temp2();
    // Update time
    oldtime = millis();
  }
  b1 = airtemp >> 0 & 0xFF; 
  b2 = airtemp >> 8 & 0xFF; 

  b3 = bodytemp >> 0 & 0xFF; 
  b4 = bodytemp >> 8 & 0xFF; 
  
  dataFrame[j] = b1;
  dataFrame[j+1] = b2;
  dataFrame[j+2] = b3;
  dataFrame[j+3] = b4;

  irled_inp = get_irled();
  // Serial.println(irled_inp);
  b5 = irled_inp >> 0 & 0xFF; 
  b6 = irled_inp >> 8 & 0xFF; 
  b7 = irled_inp >> 16 & 0xFF; 
  // b6 = irled_inp >> 24 & 0xFF; //Don't think you need this, values is only 24bit even though the int is 32

  dataFrame[j+4] = b5;
  dataFrame[j+5] = b6;
  dataFrame[j+6] = b7;

  redled_inp = get_redled();
  // Serial.println(redled_inp);
  b8 = redled_inp >> 0 & 0xFF; 
  b9 = redled_inp >> 8 & 0xFF; 
  b10 = redled_inp >> 16 & 0xFF; 

  dataFrame[j+7] = b8;
  dataFrame[j+8] = b9;
  dataFrame[j+9] = b10;

  j = j + 10;
  // delay(5); //samples at 200hz

  // delay(500); // to change next

  if (deviceConnected && j == FRAMESIZE) {
    
    pTxCharacteristic->setValue(dataFrame, sizeof(dataFrame)); 
    pTxCharacteristic->notify();
    txValue++;
    j = 0;
    delay(10);
    
  }
  


  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }

}
