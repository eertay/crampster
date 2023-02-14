//
//  BLEManager.swift
//  BLE_Background
//
//  Created by Mohammad Afaneh on 5/6/20.
//  Copyright © 2020 NovelBits. All rights reserved.
//

import Foundation
import CoreBluetooth
import SwiftUI

struct Peripheral: Identifiable {
    let id: Int
    let name: String
    let rssi: Int
}


class BTManager: NSObject, ObservableObject, CBCentralManagerDelegate{

    
    let bleService = CBUUID.init(string: "00000001-1000-2000-3000-111122223333");
    let bleChar = CBUUID.init(string: "00000003-1000-2000-3000-111122223333")
    
    var data:Data?
    private var outputChar: CBCharacteristic?
    var myStream: MyStreamer?

    var myCentral: CBCentralManager!
    @Published var isSwitchedOn = false
    @Published var pairedDevice:CBPeripheral?=nil
    @Published var peripherals = [Peripheral]()
  
    @Published var connected = false
    @Published var recording = false
    @Published var cramp = 3
    @State var activity = ActivityData()
    @State var location = LocationMonitor()
    
    @Published var output:String = "0"
        override init() {
            super.init()
     
            myCentral = CBCentralManager(delegate: self, queue: nil)
            myCentral.delegate = self
//            location.init()
        }

    @Published var dataShow:[Double] = Array(repeating:0.0, count:20)
    
    func startRecording(userID:String,modelName:String,fileIdentifier:String){
        recording = true
        myStream = MyStreamer(Prefix: "Logger_"+userID,modelName:modelName,fileIdentifier:fileIdentifier)
        let characteristic = outputChar! as CBCharacteristic
        if(characteristic.uuid  == bleChar){
            var parameter = NSInteger(1)
            let data = NSData(bytes: &parameter, length: 1)
            pairedDevice?.writeValue(data as Data, for: characteristic,type: CBCharacteristicWriteType.withResponse)
        }
    }
    
    func stopRecording(){
        let characteristic = outputChar! as CBCharacteristic
        recording = false
        var parameter = NSInteger(0)
        if(characteristic.uuid  == bleChar){
            let data = NSData(bytes: &parameter, length: 1)
            pairedDevice?.writeValue(data as Data, for: characteristic,type: CBCharacteristicWriteType.withResponse)
        }
    }
  
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
         if central.state == .poweredOn {
             isSwitchedOn = true
         }
         else {
             isSwitchedOn = false
         }
    }


    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        var peripheralName: String!
       
        if let name = advertisementData[CBAdvertisementDataLocalNameKey] as? String {
            peripheralName = name
            let newPeripheral = Peripheral(id: peripherals.count, name: peripheralName, rssi: RSSI.intValue)
            print(newPeripheral)
            peripherals.append(newPeripheral)
            print(peripherals)
            if(peripheralName == "crampster"){
                myCentral.stopScan();
                myCentral.connect(peripheral)
                pairedDevice=peripheral
                print("connected to your device")
                connected = true
//                pairedDevice?.discoverServices([bleChar])
            }
        }
        
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected to \(peripheral.name ?? "UNKNOWN")")
        peripheral.delegate = self
        peripheral.discoverServices(nil)
        
    }
    
    func startScanning() {
        print("startScanning")
        myCentral.scanForPeripherals(withServices: nil,
                                     options: [CBCentralManagerScanOptionAllowDuplicatesKey : false])
        
     }
    
    
    func cleanup(peripheral: CBPeripheral){
        myCentral.cancelPeripheralConnection(peripheral)
    }
    
    func stopScanning() {
        print("stopScanning")
        myCentral.stopScan()
    }
    
}
extension BTManager : CBPeripheralDelegate {
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        print("Discovered services for \(peripheral.name ?? "UNKNOWN")")
        
        guard let services = peripheral.services else {
            return
        }
        for service in services {
            peripheral.discoverCharacteristics(nil, for: service)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        print("Discovered characteristics for \(peripheral.name ?? "UNKNOWN")")
        
        guard let characteristics = service.characteristics else {
            return
        }
        for ch in characteristics {
            switch ch.uuid {
                case bleChar:
                    outputChar = ch
                    // subscribe to notification events for the output characteristic
                    peripheral.setNotifyValue(true, for: ch)
                default:
                    break
            }
        }
        
        DispatchQueue.main.async {
            self.connected = true
            self.output = "Connected."
        }
    }

    func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        print("Notification state changed to \(characteristic.isNotifying)")
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        print("Characteristic updated: \(characteristic.uuid)")
        if characteristic.uuid == bleChar, let data = characteristic.value {
            let dataArray = [UInt8](data)
            var combinedData = "" as String
            var i = 0
            let nElements = dataArray.count
            while i < nElements{

                let  lsb = dataArray[i]
                let  msb = dataArray[i+1]
                let airtemp = UInt16(msb) << 8 | UInt16(lsb)

                combinedData = "\(airtemp)"

                let testString = String(airtemp)

                print("Air temp:")
                print(testString)

                self.output = testString

                // Body temp
                let  lsb2 = dataArray[i+2]
                let  msb2 = dataArray[i+3]
                let bodytemp = UInt16(msb2) << 8 | UInt16(lsb2)

                combinedData = "\(bodytemp)"

                let testString2 = String(bodytemp)
                self.output = testString2


                print("Body temp:")
                print(testString2)


                let  lsb3 =  dataArray[i+4]
                let  midb =  dataArray[i+5]
                let  msb3 =  dataArray[i+6]



                let irled = UInt32(msb3) << 16 | UInt32(midb) << 8 | UInt32(lsb3)

                combinedData = "\(irled)" //combinedData + "irled " + "\(irled)"

                let testString4 = String(irled)
                self.output = testString4

                print("IRLED:")
                print(testString4)
                
                
                let  lsb4 =  dataArray[i+7]
                let  midb4 =  dataArray[i+8]
                let  msb4 =  dataArray[i+9]



                let redled = UInt32(msb4) << 16 | UInt32(midb4) << 8 | UInt32(lsb4)

                combinedData = "\(redled)" //combinedData + "irled " + "\(irled)"



                let testString5 = String(redled)
                self.output = testString5

                
                print("REDLED:")
                print(testString5)

                var act = "-"
                var lat = "-"
                var long = "-"
                
                if activity.act != nil{
                    act = activity.act! as String
                }
       
                lat =  String(location.latitude)
                long =  String(location.longitude)
                
                if (self.dataShow.count > 600){
                        self.dataShow.removeFirst()
                    }
                self.dataShow.append(Double(self.output) ?? 0)
                if recording {
                    let timestamp = NSDate().timeIntervalSince1970
                    let writtingString: String = "\(timestamp), \(cramp), \(airtemp), \(bodytemp), \(irled), \(redled), \(lat), \(long), \(act)\n"
                    
//                    let writtingString:String = "\(timestamp), \(cramp),\(mic), \(temp1), \(temp2),\(irled), \(redled), \(lat), \(long), \(act)\n"
                    self.myStream!.write(writtingString)
                    print(writtingString)
                    
                }
                //i = 0
                
               i = i + 10
            }
        }
    }
}
