//
//  ContentView.swift
//  datalogger
//
//  Created by Alex Adams on 4/20/22.
//
import Foundation
import SwiftUI

struct ContentView: View {
    
    var body: some View {
    
        NavigationView {
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
//
//                NavigationLink(destination: AvailableBLEView().environmentObject(bleAvailable())) {
//                    Text("Select Device")
//                        .fontWeight(.bold)
//                        .font(.title)
//                        .padding()
//                        .background(Color.orange)
//                        .cornerRadius(40)
//                        .foregroundColor(.white)
//                        .padding(10)
//                        .overlay(
//                            RoundedRectangle(cornerRadius: 40)
//                        .stroke(Color.purple, lineWidth: 5)
//            )}
            }
        }
    }
}

class dataLoggerUserSettings: ObservableObject{
    
    @Published var userID:String = ""

}
class bleAvailable: ObservableObject{
    
    @Published var devices:String = ""

}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
