Non blocking delays
===================

The objective of this code is to use non blocking delays and handle a LED with different approaches

### Fixed delay
* Using a fixed delay of 100 ms, turn on the LED for 100 ms and then turn off the LED for 100 ms

### Variable delay
Create a custom structure to specify:

1. Period
2. Duty cycle
3. Repetitions
    
The LED On/Off pattern should follow the defined duty cycle

Based on this structure create an array of three elements and every time the repetitions of each element has been excedeed apply the LED cycle of the next item.
