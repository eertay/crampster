//
//  AvailableBLEView.swift
//  datalogger
//
//  Created by Alex Adams on 4/21/22.
//

import Foundation
import SwiftUI

struct AvailableBLEView:View{
    @StateObject var bleManager = BTManager()
    @State var selectedDevice: String?
    
    var body: some View {
        VStack (spacing: 10) {
        
        List(bleManager.peripherals, id: \.name, selection: $selectedDevice) { peripheral in
            HStack {
                Text(peripheral.name)
                Spacer()
                Text(String(peripheral.rssi))
                }
            }.frame(height: 300)
        }
        VStack{
            NavigationLink(destination: DataLoggerView().environmentObject(dataLoggerUserSettings())) {
                Text("DataLogger")
                    .fontWeight(.bold)
                    .font(.title)
                    .padding()
                    .background(Color.orange)
                    .cornerRadius(40)
                    .foregroundColor(.white)
                    .padding(10)
                    .overlay(
                        RoundedRectangle(cornerRadius: 40)
                    .stroke(Color.purple, lineWidth: 5)
                )}
        }
        VStack (spacing: 10) {
            HStack {
                VStack (spacing: 10) {
                    Button(action: {
                        self.bleManager.startScanning()
                        print(bleManager.peripherals)
                        print(bleManager.peripherals.count)
                        
                    }) {
                        Text("Scan Devices")
                    }

                }.foregroundColor(Color.blue)
                    .padding()
                    .overlay(
                        RoundedRectangle(cornerRadius: 20)
                            .stroke(Color.blue, lineWidth: 2)
                    )
 
            }
        }
    }
}
