Command Parser using FSM and UART
===================

The objective of this code is implement a Command Parser using a FSM and capturing the input from the user using UART

The default uart configuration is:

BaudRate:       9600
WordLength:     0
StopBits:       0
Parity:         1536
HwFlowCtl:      0
Mode:           12
OverSampling:   0

Once the UART connection has been established, the user could enter one of the following commands:
HELP:                       Print the available commands
STATUS                      Print the LED state
LED [ON | OFF | TOGGLE]     Modify the state of the LED


The command parser FSM is able to identify errors in the user input like:

* Line too long
* Wrong syntax
* Bad arguments
* Unkown error


Author: Andres Urian Florez