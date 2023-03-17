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
volatile uint16_t input = 0x00;
volatile uint16_t airtemp = 0x00;
volatile uint16_t bodytemp = 0x00;
volatile uint32_t irled_inp = 0x00;
volatile uint32_t redled_inp = 0x00;
volatile uint16_t heartr = 0x00;
volatile uint16_t ox = 0x00;



volatile char b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, i1, i2 = 0x00;
uint16_t temp1 = 0;
uint16_t temp2 = 0;
uint16_t indicator = 0;

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
                    BLECharacteristic::PROPERTY_WRITE
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_READ
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}


int get_temp1() {
  sensor1.setMeasurementMode(TMP117_MODE_ONE_SHOT);
  sensors_event_t temp1;
  sensor1.getEvent(&temp1); //fill the empty event object with the current measurements
  return temp1.temperature;
}

int get_temp2() {
  sensor2.setMeasurementMode(TMP117_MODE_ONE_SHOT);
  sensors_event_t temp2;
  sensor2.getEvent(&temp2); //fill the empty event object with the current measurements
  return temp2.temperature;
}


uint32_t get_irled() {
  // restart wire, Not sure if it'll work

  return body.irLed;
}

uint32_t get_redled() {
  //body = bioHub.readSensorBpm();
  return body.redLed;
}

uint16_t get_bpm() {
  //body = bioHub.readSensorBpm();
  return body.heartRate;
}

uint16_t get_ox() {
  //body = bioHub.readSensorBpm();
  return body.oxygen;
}



void setup(){
  Serial.begin(115200);
  Serial.println("starting");
  // Setup code for TMP sensors
  
  Wire.begin();
  analogReadResolution(12);



  // // Try to initialize!
  // if (!sensor1.begin(0x49) & !sensor2.begin()) {
  //   Serial.println("Failed to find TMP117 chip");
  //   while (1) { delay(10); }
  // }
  // Serial.println("TMP117 Found!");

  //initialize both sensors
  int pulseOx_result = bioHub.begin();
  int temp1_result = sensor1.begin(0x49); // Air temp
  int temp2_result = sensor2.begin();


  //check to see if they started
  if (pulseOx_result == 0 && temp1_result == 0 && temp2_result == 0) {// Zero errors!
    Serial.println("All sensors started!");
  }

  //double check temperatures until found
  if (!sensor1.begin(0x49) || !sensor2.begin()) {
    Serial.println("Failed to find one of the TMP117 chips");
    while (1) { delay(10); }
  }


  Serial.println("Both TMP117s Found!");

  //configure pulseox sensor intto BPM mode
  Serial.println("Configuring PulseOx Sensor...."); 
  int error = bioHub.configSensorBpm(MODE_TWO); // Configure Sensor and BPM mode , MODE_TWO also available
  if (error == 0){// Zero errors.
    Serial.println("PulseOx Sensor configured.");
  }
  else {
    Serial.println("Error configuring pulse ox sensor.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }

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


  // Initialize pulse ox
  body = bioHub.readSensorBpm();

  //set the intial temp readings
  airtemp = get_temp1();
  // bodytemp = get_temp2();

  delay(1000);
  ble_init();
  oldtime = millis();

}

void loop() {

  // Timer Counter
  // static uint32_t oldtime = millis();

  if ( (millis()-oldtime) >= 1000 ) {
    airtemp = get_temp1();
    bodytemp = get_temp2();
    // j = j + 2;

    // Update time
    oldtime = millis();
  }



  // Indicator
  indicator = 0;

  i1 = indicator >> 0 & 0xFF;
  i2 = indicator >> 8 & 0xFF;
  Serial.println(indicator);


  dataFrame[j] = i1;
  dataFrame[j+1] = i2;


  // Send temp data

  b1 = airtemp >> 0 & 0xFF; 
  b2 = airtemp >> 8 & 0xFF; 

  dataFrame[j+2] = b1;
  dataFrame[j+3] = b2;

  Serial.println(airtemp);



  // Indicator
  indicator = 1;

  i1 = indicator >> 0 & 0xFF;
  i2 = indicator >> 8 & 0xFF;
  Serial.println(indicator);


  dataFrame[j+3] = i1;
  dataFrame[j+4] = i2;


  // Send temp data

  b3 = bodytemp >> 0 & 0xFF; 
  b4 = bodytemp >> 8 & 0xFF; 

  dataFrame[j+5] = b1;
  dataFrame[j+6] = b2;

  Serial.println(bodytemp);
  // Try getting heartrate
  // irled_inp = get_bpm();
  // Serial.println(irled_inp);

  irled_inp = get_irled();


  // Indicator
  indicator = 2;

  i1 = indicator >> 0 & 0xFF;
  i2 = indicator >> 8 & 0xFF;

  Serial.println(indicator);


  dataFrame[j+7] = i1;
  dataFrame[j+8] = i2;


  b3 = irled_inp >> 0 & 0xFF; 
  b4 = irled_inp >> 8 & 0xFF; 
  b5 = irled_inp >> 16 & 0xFF; 
  b6 = irled_inp >> 24 & 0xFF; 

  dataFrame[j+9] = b3;
  dataFrame[j+10] = b4;
  dataFrame[j+11] = b5;
  dataFrame[j+12]= b6;

  Serial.println(irled_inp);


  redled_inp = get_redled();


  // Indicator
  indicator = 3;

  i1 = indicator >> 0 & 0xFF;
  i2 = indicator >> 8 & 0xFF;

  Serial.println(indicator);


  dataFrame[j+13] = i1;
  dataFrame[j+14] = i2;


  b3 = redled_inp >> 0 & 0xFF; 
  b4 = redled_inp >> 8 & 0xFF; 
  b5 = redled_inp >> 16 & 0xFF; 
  b6 = redled_inp >> 24 & 0xFF; 

  dataFrame[j+15] = b3;
  dataFrame[j+16] = b4;
  dataFrame[j+17] = b5;
  dataFrame[j+18] = b6;

  Serial.println(redled_inp);

  heartr = get_heartra

  // Indicator
  indicator = 4;

  i1 = indicator >> 0 & 0xFF;
  i2 = indicator >> 8 & 0xFF;
  Serial.println(indicator);


  dataFrame[j+19] = i1;
  dataFrame[j+20] = i2;


  // Send temp data

  b3 = bodytemp >> 0 & 0xFF; 
  b4 = bodytemp >> 8 & 0xFF; 

  dataFrame[j+5] = b1;
  dataFrame[j+6] = b2;

  Serial.println(bodytemp);
  j = j + 19;


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

  delay(1000);

}