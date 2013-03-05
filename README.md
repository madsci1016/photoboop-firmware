photoboop-firmware
==================

PhotoBoop by Swimple Switch Labs

Modded by Bill Porter
www.billporter.info

Big change is using extrao IO pins in Canon mode to run an RGB LED button. The IR_LED, WIRED_FOCUS and WIRED_SHUTTER 
connections were re purpused for REDLED, GREENLED, and BLUELED respectivly. 

The button is a modified Adafruit LED pushbutton with a common anode RGB LED. 
The anode was connected to the IR LED + screw terminal, RED cathode to IR LED - screw terminal, Green and Blue to 
WIRED Shutter and focus. Resistors were wired in line between the control board and the LED. 



