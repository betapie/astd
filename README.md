# astd
Neither the arduino language nor AVR have a good support of the c++ standard library. Everytime I come across
a feature, that I just don't want to miss, I am going to implement it here.

Currently these features are implemented:
- type_traits
  - support for move and forward
- memory
  - unique_ptr
- functional
  - simple_function which is less smart than std::function. It can bind function pointers, lambdas and
    functors. In its current state it is allocated and does not feature SVO. So use carefully.

# Usage
With visual micro:
Solution and project files are included in the repo. Add project file to a solution and create a
reference to it from you main project.

With Arduino IDE:
Copy src folder into your arduino library folder and rename it as you see fit. Include library in the IDE.
