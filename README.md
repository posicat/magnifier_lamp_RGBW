# magnifier_lamp_RGBW
Software for my replacement RGB+White LED for my desk magnifying lamp

(Feel free to post comments in Issues if you have any ideas, or questions, or run into any problems!)

My fluorescent lamp on my magnifying lamp (finally, it lasted 15 years) so now it's time to make it smarter!  I bought an LED ring 

Parts (so far)
  - Copied my breadboard design into a PCB layout (untested currently)
    - https://oshwlab.com/posicat/led-ring-light-controller
    - 3.3v regulator
    - ESP12F solder pads
    - Internal OR External rotary encoder (depending on the room available in the lamp)
    - External 5V power connects to the RGB LED pad
      - Primary power supplied to the ring
      - From there to this PCB as the PCB power requirements are far less.
  - Magnifying lamp with a round fluorescent light 
    - preferably a dead light, no need to waste a good light
  - LED Ring light 
    - Mine was titled : WS2812B Ring 5050 RGB SK6812 5050 RGBW Ring 93 241 Bits addressable leds DC5V
    - I used just the two outer rings (60 and 48 LEDS each), maybe you can buy just those, but I couldn't find them.
    - Measure to make sure they fit your lamp!
  - Rotary encoder
    - 360 rotation and a push button switch
  - ESP8266
    - Complete overkill for this project, but I wanted the extra memory, and WiFi could be fun.
  - 5v 4A regulator
    - I'm powering it via a 24V 30A power pack that powers my whole desk
    - This should let most people pick their own power supply, old laptop power suppoes are ideal!
    
  Features already in the code:
    - All-on white Light (default, startup condition)
    - Ability to light just a portion of the ring (arc size)
  
  Features I intend to add to this code
   - Ability to spin that portion of the ring that's lit (arc rotation)
    - Change colors, can be useful for making certain things more visible
  
  Optional extras I plan to add to my build:
    - A pair of USB cameras for recording projects (2 because I want to try getting 3D images)
    - USB hub for the cameras
