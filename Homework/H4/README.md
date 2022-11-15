# Homework 4 - [Write digits on a 4 7-segment display with joystick and shift register](./H4.ino)
## Requirements
:diamond_shape_with_a_dot_inside: Components:  
a joystick, a 4 digit 7-segment display, a 74hc595 shift register, 2 1K ohms resistors for the shift register and 4 330 ohms resistors for the display   

:diamond_shape_with_a_dot_inside: General description:  
Use the joystick to move through the 4 digit 7 segment displays digits, press the button to lock in on the current digit and use the other axis to increment or decrement the number. Keep the
button pressed to reset all the digit values and the current position to the first digit (rightmost) in the first state.


The system has the following states:
1. <b> State 1: </b>the joystick axis can be used to cycle through the 4 digits;
using the other axis does nothing. A blinking decimal point shows
the current digit position. When pressing the button, the selected digit is locked in and the system goes to the second state.

2. <b> State 2: </b>in this state, the decimal point stays always on, no longer blinking and the axis can no longer be used to cycle through
the 4 digits. Instead, using the other axis, the number on the current digit can be incremented or decremented IN HEX (aka from 0
to F). Pressing the button again returns to the
previous state.

:large_orange_diamond: Other details:  
When changing the number, the increment must be done for each joystick movement - it should not continuosly increment if the joystick is kept in one position.  
Coding style is of utmost importance.  
Magic numbers are not accepted and consistentency in style is desired.   

:camera: Picture of the setup:     
TBA 

:film_projector: Video showcasing the functionality:  
TBA

