This repository contains the code for my closed loop Protean project. It's still a work in progress.

See [the linked repository](https://github.com/MarkGhebrial/nerf-tachometer-board) for the tachometer PCB.

## Why closed loop?
- Closed loop allows me to precisely tune the rpm of the wheels without sacrificing spinup time. A simple pwm and mosfet board can give you some level of speed control, but only full closed loop control allows you to use the full power of the motor to get the wheels up to any speed.
- Closed loop also allows me to ensure both motors are spinning at the exact same rpm, which should increase accuracy.
- When I eventually add a solenoid pusher (currently I'm using the protean's manual pusher), closed loop will allow me to implement a single stage trigger that will fire a dart at the exact moment that the wheels reach full speed.

## How does it work?
- A semicircle of silver sharpie has been added to the top of each flywheel. IR reflectivity sensors are positioned above each wheel. When a sensor detects a change in the wheel's reflectivity, it triggers an interrupt on the microcontroller. The microcontroller measures the time between the interrupts and uses that to compute the rpm of the wheel.
- A PID controller + feedforward model running on the microcontroller precisely controls the speed of the motor using the real time rpm data.

## What about the electronics?
- An Adafruit ItsyBitsy M0 express is the brains of the whole operation.
- Each motor is controlled by a dedicated MOSFET. A dual channel MOSFET driver allows the MCU to safely control the MOSFETS.
- A voltage divider allows the microcontroller to measure the voltage of the battery.
- I designed a PCB that makes it easier to wire up the IR sensors.
- Battery: currently a 3s lipo. Voltage regulators are capable of 4s.

## What about the hardware?
- The pusher, grip, and cage of the protean are unchanged. I modified the magwell to make more space underneath the motors. I designed a new stock that has space for both the battery and the electronics. I modified the reciever cover to add mounts for the IR sensor PCB.
- Motors: Custom 42k rpm motors. Courtesy of tinkershot 
- Wheels: 1st gen manshees from Roboman Automation

## Planned features
- Automatic tuning of the feed forward model.
- Quick and easy configuration changes via UART or USB.
