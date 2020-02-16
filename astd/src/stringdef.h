#pragma once
#ifndef ASTD_STRINGDEF
#define ASTD_STRINGDEF

/*
This is not part of the STL in any way. Instead of defining its own string class,
this library will make use of the avr String class. To still make use of type information
or integral constants like astd::string::npos, they will be defined here
*/

namespace astd
{
    namespace string
    {
        // these are the types the avr String class uses...
        using value_type = char;
        using size_type = unsigned int;
        constexpr auto npos = static_cast<size_type>(-1);
    }
}

#endif // ASTD_STRINGDEF