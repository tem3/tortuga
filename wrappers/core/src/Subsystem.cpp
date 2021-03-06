/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/src/Subsystem.cpp
 */

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/EventHub.h"

namespace bp = boost::python;


static ram::core::SubsystemList pythonListToCpp(bp::list list)
{
    ram::core::SubsystemList result;

    int length = bp::len(list);
    for (int i = 0; i < length; ++i)
        result.push_back(bp::extract<ram::core::SubsystemPtr>(list[i]));

    return result;
}

class SubsystemWrapper : public ram::core::Subsystem,
                         public bp::wrapper< ram::core::Subsystem >
{
public:    
    SubsystemWrapper(
        std::string name,
        ram::core::EventHubPtr eventHub = ram::core::EventHubPtr()) :
        Subsystem(name, eventHub),
        bp::wrapper< ram::core::Subsystem >()
    {}

    SubsystemWrapper(std::string name,
                     ram::core::SubsystemList list) :
        Subsystem(name, list),
        bp::wrapper< ram::core::Subsystem >()
    {}

     SubsystemWrapper(std::string name,
                      bp::list list) :
         Subsystem(name, pythonListToCpp(list)),
         bp::wrapper< ram::core::Subsystem >()
    {}

    virtual void setPriority(ram::core::IUpdatable::Priority priority) {
        bp::override func_setPriority = this->get_override( "setPriority" );
        func_setPriority( priority );
    };
    
    virtual ram::core::IUpdatable::Priority getPriority() {
        bp::override func_getPriority = this->get_override( "getPriority" );
        return func_getPriority();
    };

    virtual void setAffinity(size_t affinity) {
        bp::override func_setAffinity = this->get_override( "setAffinity" );
        func_setAffinity( affinity );
    };
    
    virtual int getAffinity() {
        bp::override func_getAffinity = this->get_override( "getAffinity" );
        return func_getAffinity();
    };
    
    virtual void update( double timestep ){
        bp::override func_update = this->get_override( "update" );
        func_update( timestep );
    }
    
    virtual void background( int interval=-1 ){
        bp::override func_background = this->get_override( "background" );
        func_background( interval );
    }

    virtual bool backgrounded(  ){
        bp::override func_backgrounded = this->get_override( "backgrounded" );
        return func_backgrounded(  );
    }

    virtual void unbackground( bool join=false ){
        try {
            bp::override func_unbackground =
                this->get_override( "unbackground" );
            func_unbackground( join );
        } catch(bp::error_already_set err) {
            PyErr_Print();
            throw err;
        }
    }
};

void registerSubsystemClass()
{
    bp::class_<SubsystemWrapper,
        bp::bases<ram::core::IUpdatable, ram::core::EventPublisher>,
        boost::noncopyable >(
            "Subsystem",
            bp::init<std::string, bp::optional<ram::core::EventHubPtr> >(
                (bp::arg("name"),
                 bp::arg("eventHub") = ram::core::EventHubPtr())) )
        .def(bp::init<std::string, ram::core::SubsystemList>(
                 (bp::arg("name"),
                  bp::arg("deps") = ram::core::SubsystemList())) )
        .def(bp::init<std::string, bp::list>(
                 (bp::arg("name"),
                  bp::arg("list") = bp::list())) )
        .def("getName", &ram::core::Subsystem::getName)
        .add_property("name", &ram::core::Subsystem::getName);

    bp::register_ptr_to_python< ram::core::SubsystemPtr >();
    bp::implicitly_convertible< ram::core::SubsystemPtr, boost::shared_ptr< ram::core::EventPublisher > >();
/*    bp::implicitly_convertible< ram::core::SubsystemPtr, boost::shared_ptr< ram::core::IUpdatable > >();
    bp::register_ptr_to_python< const ram::core::SubsystemPtr >();
    bp::implicitly_convertible< const ram::core::SubsystemPtr, const boost::shared_ptr< ram::core::EventPublisher > >();
    bp::implicitly_convertible< const ram::core::SubsystemPtr, const boost::shared_ptr< ram::core::IUpdatable > >();*/
}
