/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/ConfigNodeImp.h
 */

#ifndef RAM_CORE_CONFIGNODEIMP_06_30_2006
#define RAM_CORE_CONFIGNODEIMP_06_30_2006

// STD Includes
#include <string>

// Project Includes
#include "core/include/Common.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

class RAM_EXPORT ConfigNodeImp
{
public:
    virtual ~ConfigNodeImp() {};
    
    /** Grab a section of the config like an array */
    virtual ConfigNodeImpPtr idx(int index) = 0;

    /** Grab a sub node with the same name */
    virtual ConfigNodeImpPtr map(std::string key) = 0;

    /** Convert the node to a string value */
    virtual std::string asString() = 0;

    /** Attempts conversion to string, if it fails return def */
    virtual std::string asString(const std::string& def) = 0;
    
    /** Convert the node to a double */
    virtual double asDouble() = 0;

    /** Attempts conversion to string, if it fails return def */
    virtual double asDouble(const double def) = 0;

    /** Convert the node to an int */
    virtual int asInt() = 0;

    /** Attempts conversion to int, if it fails return def */
    virtual int asInt(const int def) = 0;

    /** Returns the list of sub nodes of the current config node */
    virtual NodeNameList subNodes() = 0;

    /** The number of elements in an array or map node */
    virtual size_t size() = 0;
    
    /** Map a key to a given value */
    virtual void set(std::string key, std::string str) = 0;

    /** Map a key to a given value */
    virtual void set(std::string key, int value) = 0;

    /** Returns the config file in a python evalable format */
    virtual std::string toString() = 0;

    virtual void writeToFile(std::string, bool silent) = 0;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_CONFIGNODEIMP_06_30_2006



