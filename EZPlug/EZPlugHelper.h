//
//  EZPlugHelper.h
//  Thicket II
//
//  Created by morganpackard on 12/10/11.
//  Copyright (c) 2011 Interval Studios. All rights reserved.
//

#ifndef Thicket_II_EZPlugHelper_h
#define Thicket_II_EZPlugHelper_h


#include <sstream>

namespace EZPlug {
    template <class T>
    inline std::string toString (const T& t)
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }
    
    template <class T>
    inline std::string str(const char* chars, const T& t)
    {
        return std::string(chars).append(toString(t));
    }
}




#endif
