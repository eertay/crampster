//
//  POMView.swift
//  MyResearchHub
//
//  Created by Yixuan Gao on 2021-02-13.
//

import Foundation
import SwiftUI
import SwiftUICharts

struct DataLoggerView:View{
    
//    @EnvironmentObject var activity: DataModel
    @State var run:Bool = false
    @State private var id: String = ""
    @State var activity = ActivityData()
    @StateObject var bleManager = BTManager()
    @State private var showData = true
    let chartStyle = ChartStyle(backgroundColor: Color.black, accentColor: Colors.OrangeStart, secondGradientColor: Colors.OrangeEnd,  textColor: Color.white, legendTextColor: Color.white, dropShadowColor: Color.white )
     
    
    var body: some View {
        
        VStack (spacing: 10.0) {
    
            HStack{
                Text("ID: ")
                TextField("pID_age_height_weight_sex", text: $id)
//              Text(bleManager.output)
//                  .font(.body)
//                  .frame(maxWidth: .infinity, alignment: .center)
//                Text("activity")
//                    .font(.body)
//                    .frame(maxWidth: .infinity, alignment: .center)
//                if activity.act != nil{
//                    Text(activity.act! as String)
//                        .font(.body)
//                        .frame(maxWidth: .infinity, alignment: .center)
//                }
            }
            HStack{
            if(showData){
                LineView(data: self.bleManager.dataShow, title: "Data",style: chartStyle).frame(width: UIScreen.main.bounds.size.width*0.9, height: UIScreen.main.bounds.size.height*0.1)
                }
            }
            Spacer()
            Spacer()
            Spacer()
            HStack{
                Button(action: {
                    self.bleManager.cramp = 1
                    print(bleManager.cramp)
                
                }) {
                    Text("Crampy")
                        .fontWeight(.bold)
                        .font(.system(size: 20, weight: .bold, design: .default))
                        .foregroundColor(.red)
                }
                    .buttonStyle(CustomButtonStyle())
            
            Button(action: {
                self.bleManager.cramp = 2
                print(bleManager.cramp)
            
            }) {
                Text("No Crampy")
                    .fontWeight(.bold)
                    .font(.system(size: 20, weight: .bold, design: .default))
                    .foregroundColor(.white)
                }
                .buttonStyle(CustomButtonStyle2())
            

           
                Button(action: {
                self.bleManager.cramp = 3
                print(bleManager.cramp)
                
                }) {
                    Text("No Label")
                        .fontWeight(.bold)
                        .font(.system(size: 20, weight: .bold, design: .default))
                        .foregroundColor(.black)
                }
                .buttonStyle(CustomButtonStyle3())
        
        }
          

            HStack {
                Button(action: {
                    self.bleManager.startScanning()
                    print(bleManager.peripherals)
                    print(bleManager.peripherals.count)
                
                }) {
                    if(bleManager.connected){
                        Text("Ready")
                            .fontWeight(.bold)
                            .font(.body)
                            .foregroundColor(.green)
                            .padding()
                            .overlay(
                                RoundedRectangle(cornerRadius: 20)
                                    .stroke(Color.blue, lineWidth: 5)
                            )
                    }
                    else{
                        Text("Scan")
                            .fontWeight(.bold)
                            .font(.body)
                            .foregroundColor(.red)
                            .padding()
                            .overlay(
                                RoundedRectangle(cornerRadius: 20)
                                    .stroke(Color.blue, lineWidth: 5)
                            )
                    }
                }
                Button(action: {
                    if(bleManager.connected){
                        bleManager.startRecording(userID: self.id, modelName:   "DataLogger",fileIdentifier: "0")
                        }
                    print("Record pressed")
                }){
                    if(bleManager.recording && bleManager.connected){
                        Text("Recording")
                            .fontWeight(.bold)
                            .font(.body)
               
                            .foregroundColor(.white)
                            .background(Color.red)
                            .padding()
                            .overlay(
                                RoundedRectangle(cornerRadius: 20)
                                    .stroke(Color.red, lineWidth: 3)
                            )
                    }else{
                        Text("Start Rec.")
                            .fontWeight(.bold)
                            .font(.body)
                            .foregroundColor(.red)
                            .padding()
                            .overlay(
                                RoundedRectangle(cornerRadius: 20)
                                    .stroke(Color.red, lineWidth: 3)
                            )
                    }
                    
                }
                
                Button(action: {
                    if(bleManager.connected){
                        bleManager.stopRecording()
                    }
                    print("Stop Pressed")
                    if(run){
                        run = false
                        }
                }) {
                    Text("Stop")
                        .fontWeight(.bold)
                        .font(.body)
                        .foregroundColor(.blue)
                        .padding()
                        .overlay(
                            RoundedRectangle(cornerRadius: 20)
                                .stroke(Color.blue, lineWidth: 5)
                        )
                }
         
                Toggle(isOn: $showData){
                    Text("Show Data")
                    .frame(maxWidth: .infinity, alignment: .trailing)
                    
                }
            }
        }
    }
}
struct CustomButtonStyle: ButtonStyle {
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .padding(5) // *make it thinner*
            .foregroundColor(.blue)
            .background(
                Rectangle()
                    .stroke(Color.blue, lineWidth: 4)
                    .background(Color.white)
                    .cornerRadius(10)
                    .frame(width: UIScreen.main.bounds.size.width*0.25, height: UIScreen.main.bounds.size.height*0.3)
                
            )
            .padding(.vertical, 125)
            .padding(.horizontal, 10)
            .opacity(configuration.isPressed ? 0.7 : 1)
    }
}
struct CustomButtonStyle2: ButtonStyle {
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .padding(5) // *make it thinner*
            .foregroundColor(.red)
            .background(
                Rectangle()
                    .stroke(Color.blue, lineWidth: 4)
                    .background(Color.blue)
                    .cornerRadius(10)
                    .frame(width: UIScreen.main.bounds.size.width*0.25, height: UIScreen.main.bounds.size.height*0.3)
                
            )
            .padding(.vertical, 125)
            .padding(.horizontal, 10)
            .opacity(configuration.isPressed ? 0.7 : 1)
    }
}
struct CustomButtonStyle3: ButtonStyle {
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .padding(5) // *make it thinner*
            .foregroundColor(.red)
            .background(
                Rectangle()
                    .stroke(Color.black, lineWidth: 4)
                    .background(Color.green)
                    .cornerRadius(10)
                    .frame(width: UIScreen.main.bounds.size.width*0.25, height: UIScreen.main.bounds.size.height*0.3)
                
            )
            .padding(.vertical, 125)
            .padding(.horizontal, 10)
            .opacity(configuration.isPressed ? 0.7 : 1)
    }
}
struct DataLoggerView_Preview:PreviewProvider{
    static var previews: some View {
        DataLoggerView().environmentObject(dataLoggerUserSettings())
    }
}
 
