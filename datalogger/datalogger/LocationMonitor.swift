//
//  locationMonitor.swift
//  datalogger
//
//  Created by Alex Adams on 5/17/22.
//

import Foundation
import CoreLocation

class LocationMonitor : ObservableObject {

    var locationManager: CLLocationManager?

    @Published var latitude: Double = 0.0
    @Published var longitude: Double = 0.0
    
    init(){
    
        locationManager?.requestAlwaysAuthorization()
    }
    
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
          
        let coordinates = locations.last

            // get lat and long
        latitude = coordinates?.coordinate.latitude ?? 0.0
        longitude = coordinates?.coordinate.longitude ?? 0.0
           
        }

}
