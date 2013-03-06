photoboop-firmware
==================

PhotoBoop by Swimple Switch Labs

Modded by Bill Porter
www.billporter.info

Big change is using extrao IO pins in Canon mode to run an RGB LED button. The IR_LED, WIRED_FOCUS and WIRED_SHUTTER connections were re purpused for REDLED, GREENLED, and BLUELED respectivly. 

![Alt text](/photos/front.jpg "Optional title")

The button is a modified Adafruit LED pushbutton with a common anode RGB LED. 
The anode was connected to the IR LED + screw terminal, RED cathode to IR LED - screw terminal, Green and Blue to WIRED Shutter and focus. Resistors were wired in line between the control board and the LED. 

![Alt text](/photos/wiring.jpg "Optional title")

Two of the solder connections in each of the three transistors had to be shorted to connect the screw terminals directly to the AVR processor. 

The code was modified to add a #define RGBLEDMOD to trigger repurpusing the IO pins. A new function is called every time the code loops and the button is not pressed. This function will randomly play some various chirps through the piezo and change the LED color in hope to attract attention of people passing by. See it in action: http://www.youtube.com/watch?v=3_g0e5J53Xo Tones copied from RoboBrrd by RobotGrrl.  

I also altered the camera capture sequence in a quick and dirty way to change the behavior in a favorable way to me. Timer counts to 0, tells camera to focus, plays a tone and then tells camera to take a picture. 

File structure changed to make it Arduino friendly directly from Github. 
