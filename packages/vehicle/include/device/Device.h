/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/include/device/Device.h
 */

#ifndef RAM_VEHICLE_DEVICE_DEVICE_06_25_2007
#define RAM_VEHICLE_DEVICE_DEVICE_06_25_2007

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/Common.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

class RAM_EXPORT Device //: public IDevice
{
public:
    Device(std::string name);
    virtual ~Device() {};

    virtual std::string getName();
    
private:
    std::string m_name;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_DEVICE_06_25_2007
