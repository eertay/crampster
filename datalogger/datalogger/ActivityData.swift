//
//  activityTracker.swift
//  datalogger
//
//  Created by Alex Adams on 4/21/22.
//

import Foundation
import CoreMotion

class ActivityData : ObservableObject {
    var activityManager : CMMotionActivityManager?
    @Published var activities = [CMMotionActivity]()
    @Published var act : String?
    init() {
        if !CMMotionActivityManager.isActivityAvailable() {
            print("Activity Monitoring not available")
            return
        }
        activityManager = CMMotionActivityManager()
        if activityManager == nil {  // check just to be sure initialised OK
            print("Unable to initialise Activity Manager")
            return
        }
        activityManager!.startActivityUpdates(to: OperationQueue.main) { (motion) in
            guard let newMotion = motion else { return }
            self.activities.append(newMotion)
            if newMotion.stationary{self.act = "stationary"}
            else if newMotion.walking{self.act = "walking"}
            else if newMotion.running{self.act = "running"}
            else if newMotion.automotive{self.act = "driving"}
            else if newMotion.unknown{self.act = "unknown"}
            else if newMotion.cycling{self.act = "cycling"}
            else {self.act = "_"}
//            let a = self.act! as String
//            print(a)
        }
    }
}
