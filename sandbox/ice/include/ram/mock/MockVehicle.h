#ifndef _RAM_MOCK_MOCKVEHICLE_H
#define _RAM_MOCK_MOCKVEHICLE_H

#include <ram.h>
#include "MockIMU.h"
#include "MockPowerSource.h"
#include "MockDepthSensor.h"

namespace ram {
    namespace mock {
        class MockVehicle : virtual public vehicle::IVehicle
        {
        private:
            vehicle::DeviceDictionary devices;
            vehicle::IIMUPrx getFirstIMU()
            {
                for (vehicle::DeviceDictionary::iterator iter = devices.begin();
                    iter != devices.end() ; iter++)
                    if (iter->second->ice_isA("::ram::vehicle::IIMU"))
                        return vehicle::IIMUPrx::uncheckedCast(iter->second);
            }
            vehicle::IDepthSensorPrx getFirstDepthSensor()
            {
                for (vehicle::DeviceDictionary::iterator iter = devices.begin();
                    iter != devices.end() ; iter++)
                    if (iter->second->ice_isA("::ram::vehicle::IDepthSensor"))
                        return vehicle::IDepthSensorPrx::uncheckedCast(iter->second);
            }
            
            void addDevice(vehicle::IDevicePrx prx)
            { devices[prx->getName()] = prx; }
        public:
            MockVehicle(const ::Ice::Current&c)
            {
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockIMU("Yoyodyne Propulsion Systems IMU"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockDepthSensor("Absolute Pressure Sensor"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockPowerSource("Battery 1"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockDepthSensor("Bottom Ranging Sonar"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockPowerSource("Acme Thermonuclear Reactor"))));
                addDevice(vehicle::IDevicePrx::uncheckedCast(c.adapter->addWithUUID(new MockDepthSensor("Surface Ranging Sonar"))));
            }
            
            inline virtual vehicle::DeviceDictionary getDevices(const ::Ice::Current&c)
            { return devices; }
            
            inline virtual double getDepth(const ::Ice::Current&c)
            { return getFirstDepthSensor()->getDepth(); }
            
            inline virtual math::Vector3Ptr getLinearAcceleration(const ::Ice::Current&c)
            { return getFirstIMU()->getLinearAcceleration(); }
            
            inline virtual math::Vector3Ptr getAngularRate(const ::Ice::Current&c)
            { return getFirstIMU()->getAngularRate(); }
            
            inline virtual math::QuaternionPtr getOrientation(const ::Ice::Current&c)
            { return getFirstIMU()->getOrientation(); }
            
            inline virtual void applyForcesAndTorques(const ::Ice::Current&c)
            { /* do nothing */ }
            
            inline virtual void safeThrusters(const ::Ice::Current&c)
            { std::cout << "Thrusters safed!" << std::endl; }
            
            inline virtual void unsafeThrusters(const ::Ice::Current&c)
            { std::cout << "Thrusters unsafed!" << std::endl; }
            
            inline virtual void dropMarker(const ::Ice::Current&c)
            { std::cout << "Marker dropped!" << std::endl; }
        };
    }
}

#endif
