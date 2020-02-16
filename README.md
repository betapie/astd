# astd
Neither the arduino language nor AVR have a good support of the c++ standard library. There are a few implementations out there but none
of them seem to support the more modern features of the stl. Everytime I come across a feature, that I just don't want to miss, I am
going to implement it here. This is in no way meant to be or become a complete port of the stl.

Currently these features are implemented:
- type_traits
  - support for move and forward
  - enable_if and conditional with a few types
- memory
  - unique_ptr (no allocator, use with array types blocked)
- functional
  - simple_function which is less smart than std::function. It can bind function pointers, lambdas and
    functors. In its current state it is allocated and does not feature SVO. So use carefully.
- array
- bitset

# Remarks
This library is designed to work without exceptions. Methods that would normally throw will print an error message to the serial port if
open and reset the board via the watchdog. Assertions will do the same in debug mode. In release they will gladly run into undefined
behaviour.

There are some features in the AVR already. This includes a String class which resembles a std::basic_string<char>. So instead of
reimplementing a feature like string (which will cause headaches anyway due to memory reallocations), this library will use the AVR
implementations.

# Usage
With visual micro:
Solution and project files are included in the repo. Add project file to a solution and create a
reference to it from you main project.

With Arduino IDE:
Copy src folder into your arduino library folder and rename it as you see fit. Include library in the IDE.
