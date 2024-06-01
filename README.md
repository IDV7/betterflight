**Note:** the main branch is OUT OF DATE, to see the latest changes check each individual branch (software, hardware & configurator) more info below.

# Betterflight
Quadcopter flight controller hardware, software and configurator.

## Project layout :
- 3 different main folders exist, hardware, software and configurator. Each with there respective documents and there respective branch. The latest "unstable" changes are in these sub-main branches. [Main](https://github.com/IDV7/betterflight) is stable.

  ### [software](https://github.com/IDV7/betterflight/tree/main/software) :
  - This is the folder with the STM32 sourcecode, it contains the actual flight control solftware written in C.
  - Cmake is used for building the bineary.

  ### [configurator](https://github.com/IDV7/betterflight/tree/main/configurator) :
  - The configurator is PC software, written in C++ using the [QT Framework](https://www.qt.io/product/qt6) that allows the user to change basic settings on the FC, such as rates, motor direction, motor order, PID values, ...
  - It also contains features to speed up the development such as a serial montitor, a flash button that will handle dfu, gyro/pid debugger.

  ### [hardware](https://github.com/IDV7/betterflight/tree/main/hardware)
  - This contains the Schematics & PCB files the code is written for.
  - The hardware is kept compatible with [betaflight](https://betaflight.com/)



## Compatibility:

- [Betaflight](https://betaflight.com/), the hardware is kept compatible with Betaflight, you have to flash it with the (foxeer v3 mini target)
  
