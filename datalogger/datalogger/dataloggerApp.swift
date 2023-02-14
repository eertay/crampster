//
//  dataloggerApp.swift
//  datalogger
//
//  Created by Alex Adams on 4/20/22.
//

import SwiftUI
import Foundation

@main
struct dataloggerApp: App {
    var settings = dataLoggerUserSettings()
    
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}
