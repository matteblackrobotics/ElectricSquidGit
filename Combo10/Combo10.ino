// Electric Giant Squid
// Range finders in eyes determine LED behavior based on proximity
// Twinke for sleep mode
// Rainbow cycle for awake
// Pulse for engenged
// Flash for threatened

// New change here 7.25.20
// Test here
// Test here


// Initial set up
// -------------------------- NEO Pixel & LED Strips Hardware ------------------ //
#include <Adafruit_NeoPixel.h>
#define Strip1Pin 2 // LED Strip
#define Strip2Pin 3 // LED Strip

#define StripPixels 92 // Pixels in strip

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(StripPixels, Strip1Pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(StripPixels, Strip2Pin, NEO_GRB + NEO_KHZ800);

// --------------------------------- MaxSonar Hardware -------------------------------- //
// LV-MaxSonar-EZ-V0
// Analog Reading =  Inches
// Analog pin for reading voltage from range finder is set on the board
const int anRangePinLeft = 1;
const int anRangePinRight = 0;

// Variable to store values = range in inches
int anVoltLeft;
int anVoltRight;

int MinDistance = 5; // Min Range
int zone1 = 10; // Flash
int zone2 = 30; // Pulse
int zone3 = 50; // RainbowCycle
// Twinkle is beyond zone3
int MaxDistance = 80; // Max Range

int behavior; // (3=twinkle, 2=rainbowcycle, 1=pulse, 0=flash)

// ----- Analog smoothing ------- //
const int numReadings = 8; // num of readings to smooth
int readingsL[numReadings];
int readIndexL = 0;
int readingstotalL = 0;
int readingsaverageL = 0;
int smoothingdelay = 2;

int readIndexR = 0;
int readingstotalR =  0;
int readingsaverageR = 0;
int readingsR[numReadings];

// --------------------------------- TWINKLE ----------------------------- //
// Random number generator begins LED sin wave for random leds
// ------ LED Checker Function For Twinkle -------- //
int randomMax = 50; // ramdom number maximum
int threshold = 1;  // random chance threshold
int armarray[StripPixels]; // Array of LED values the length of LED strip

// ------ Sin Array -------- //
int slices = 20;    // slices of brightness
int lightval[20]; // Array of brightness values # long
int amplitude = 60;  // Amplitude of brightness for sin array, float becasue of sine curve
float halfpi = 1.5708;
float pi = 3.141592;
int arraymax = slices; // length of lightval array

// --------------------------------- RAINBOWCYCLE  ----------------------------- //
// Cycles through 1 rainbow of colors before checking range again
float bright_mult = .09; // brightness multiplier (0 to 1)
int numCycles = 1;
int rainbowDelay = 2;

// -------------------------------- PULSE ------------------------ //
int pixelMax = 92;
int brightness = 80;
int delta = 40; //brightness step
int ambient = 4; //ambient brightness
int delaytime;

// -------------------------------- FLASH ----------------------------- //
int pulses = 3; // Number of flashes per burst
int delay1 = 20; // Delay between flashes
int delay2 = 150; // Delay between bursts
int flashbrightness = 40;

// --------------------------------------- VOID SETUP ------------------------------------------ //
void setup()
{
  strip1.begin(); // Initialize Strips
  strip2.begin(); // Initialize Strips

  clearAll; // Clear all LED strips

  strip1.show();  // Set all to 0
  strip2.show();  // Set all to 0

  Serial.begin(9600);  // Begin serial


 // ------------------------- TWINKLE --------------------------- //
 //Set arm all array to 0
 for(int i=0; i<=StripPixels; i++)
  {
    armarray[i] = 0;
  }

  // Build Sin array containing a number of slices
  for(int i = 0; i<slices; i++)
  {
    float sinVal = amplitude * sin(i * pi / slices);
    lightval[i] = (int) sinVal;
  }
}

// ----------------------------------------- VOID LOOP ------------------------------------------- //
void loop()
{

  // --------------------------------- Range Checker ------------------------------- //
  // subtract the last reading:
  readingstotalL = readingstotalL - readingsL[readIndexL];

  // --------------------- Left Range Detection ------------------- //
  anVoltLeft = analogRead(anRangePinLeft)/2;
  // Bounds detection
  if(anVoltLeft < MinDistance)
  {
    anVoltLeft = MinDistance;
  }
  if(anVoltLeft > MaxDistance)
  {
    anVoltLeft = MaxDistance;
  }
    // assignes to read Index
    readingsL[readIndexL] = anVoltLeft;
    // add the reading to the total:
    readingstotalL = readingstotalL + readingsL[readIndexL];
    // advance to the next position in the array:
    readIndexL = readIndexL + 1;

    // if we're at the end of the array...
    if (readIndexL >= numReadings)
    {
    // ...wrap around to the beginning:
    readIndexL = 0;
    }
  // calculate the average:
  readingsaverageL = readingstotalL / numReadings;
  // send it to the computer as ASCII digits
  Serial.print("Avg Left: ");
  Serial.print(readingsaverageL);
  Serial.print("in   ");
  delay(smoothingdelay);        // delay in between reads for stability

  // --------------------- Right Range Detection ------------------- //
  // subtract the last reading:
  readingstotalR = readingstotalR - readingsR[readIndexR];

  anVoltRight = analogRead(anRangePinRight)/2;
  if(anVoltRight < MinDistance)
  {
    anVoltRight = MinDistance;
  }
  if(anVoltRight > MaxDistance)
  {
    anVoltRight = MaxDistance;
  }

  // assignes to read Index
    readingsR[readIndexR] = anVoltRight;
    // add the reading to the total:
    readingstotalR = readingstotalR + readingsR[readIndexR];
    // advance to the next position in the array:
    readIndexR = readIndexR + 1;

    // if we're at the end of the array...
    if (readIndexR >= numReadings)
    {
    // ...wrap around to the beginning:
    readIndexR = 0;
    }
  // calculate the average:
  readingsaverageR = readingstotalR / numReadings;
  // send it to the computer as ASCII digits
  Serial.print("Avg Right: ");
  Serial.print(readingsaverageR);
  Serial.print("in ");
  delay(smoothingdelay);        // delay in between reads for stability

  // ------------------------- Set Behavior ------------------------------- //

  // For Twinkle
  if(readingsaverageL >= zone3 || readingsaverageR >= zone3)
  {
    behavior = 3;
  }

  // For RainbowCycle
  if(readingsaverageL < zone3 && readingsaverageL >= zone2 || readingsaverageR < zone3 && readingsaverageR >= zone2)
  {
    behavior = 2;
  }

  // For Pulse
  if(readingsaverageL < zone2 && readingsaverageL >= zone1 || readingsaverageR < zone2 && readingsaverageR >= zone1)
  {
    behavior = 1;
  }

  // For Flash
  if(readingsaverageL < zone1 || readingsaverageR < zone1)
  {
    behavior = 0;
  }

  Serial.print("  behavior = ");
  Serial.println(behavior);

  // ---------------------------- TWINKLE ------------------------ //
  // Loop over all pixels and set value of each one
  if(behavior == 3)
  {
    for(int i=0; i<=StripPixels; i++)
    {
     // Update armarray for one timestep
     armarray[i] = ledchecker(armarray[i], randomMax, threshold);
     int templightval = lightval[armarray[i]];

     // Assign pixel value to all LED strips. Eventually, create two armarrays; One for length 92, one for 50
     strip1.setPixelColor(i,templightval,templightval,templightval);
     strip2.setPixelColor(i,templightval,templightval,templightval);
    }
    strip1.show();
    strip2.show();
    delay(100);

  } // END TWINKLE

  // --------------------------------- RAINBOWCYCLE ----------------------------- //
  if(behavior == 2)
  {
    // --------------------------------- Range Checker ------------------------------- //
    // --------------------- Left Range Detection ------------------- //
    anVoltLeft = analogRead(anRangePinLeft)/2;
    if(anVoltLeft < MinDistance)
    {
      anVoltLeft = MinDistance;
    }
    if(anVoltLeft > MaxDistance)
    {
      anVoltLeft = MaxDistance;
    }
    Serial.println("[RainbowCycle Range Check]");
    Serial.print("Left: ");
    Serial.print(anVoltLeft);
    Serial.print("in  ");

    // --------------------- Right Range Detection ------------------- //
    anVoltRight = analogRead(anRangePinRight)/2;
    if(anVoltRight < MinDistance)
    {
      anVoltRight = MinDistance;
    }
    if(anVoltRight > MaxDistance)
    {
      anVoltRight = MaxDistance;
    }
    Serial.print("Right: ");
    Serial.print(anVoltRight);
    Serial.println("in");

    // ------------------------- Set Behavior ------------------------------- //

    // If it is still within zone, continue rainbow, else return to void loop
    if(anVoltLeft < zone3 && anVoltLeft >= zone2 || anVoltRight < zone3 && anVoltRight >= zone2)
    {
    	// MAYBE MAKE THE DELAY 1 SO IT GOES FASTER???
     rainbowCycle(2, 1, bright_mult);      // rainbowCycle(delay, numCycles, bright_mult);
    }
    else
    {
      return;
    }

  } // END RAINBOWCYCLE

  // ------------------------- PULSE --------------------------- //
  if(behavior == 1)
  {
    for(int i=0; i<=pixelMax; i++)
    {
      // Read range finder again
      anVoltLeft = analogRead(anRangePinLeft)/2;

      if(anVoltLeft < MinDistance)
      {
        anVoltLeft = MinDistance;
      }

      if(anVoltLeft > MaxDistance)
      {
        anVoltLeft = MaxDistance;
      }

      // delay(0);
      // ------------------------- Strips ------------------------//
      // strip.setPixelColor(n, R, G, B): n = pixel number, R(0-255), G(0-255), B(0-255)
      strip1.setPixelColor(i-1,brightness-delta,brightness-delta,brightness-delta);
      strip1.setPixelColor(i,brightness,brightness,brightness);
      strip1.setPixelColor(i+1,brightness-delta,brightness-delta,brightness-delta);

      strip2.setPixelColor(i-1,brightness-delta,brightness-delta,brightness-delta);
      strip2.setPixelColor(i,brightness,brightness,brightness);
      strip2.setPixelColor(i+1,brightness-delta,brightness-delta,brightness-delta);

      strip1.show();
      strip2.show();

      strip1.setPixelColor(i-1,ambient,ambient,ambient);
      strip1.setPixelColor(i,ambient,ambient,ambient);
      strip1.setPixelColor(i+1,ambient,ambient,ambient);

      strip2.setPixelColor(i-1,ambient,ambient,ambient);
      strip2.setPixelColor(i,ambient,ambient,ambient);
      strip2.setPixelColor(i+1,ambient,ambient,ambient);

      // closer the range, faster the pulse
      delaytime = map(anVoltLeft, MinDistance, MaxDistance, 0, 50);

      // Keeps Delaytime Postive
      if (delaytime < 0)
      {
        delaytime = 0;
      }
    }
  } // END PULSE

  // ------------------------------------ FLASH ------------------------------- //
  if(behavior == 0)
  {
    for(int j; j<=pulses; j++)
    {
      for(int i=0; i<=pixelMax; i++)
      {
        strip1.setPixelColor(i,flashbrightness,flashbrightness,flashbrightness);
        strip2.setPixelColor(i,flashbrightness,flashbrightness,flashbrightness);
      }
      strip1.show();
      strip2.show();
      delay(delay1);

      for(int i=0; i<=pixelMax; i++)
      {
        strip1.setPixelColor(i,ambient,ambient,ambient);
        strip2.setPixelColor(i,ambient,ambient,ambient);
      }
      strip1.show();
      strip2.show();
      delay(delay1);
    }
    delay(delay2);

  } // END FLASH


} // End Void Loop


// -------------------------------------------------- FUNCTIONS ---------------------------------------------- //
// --------------------------------- LED checker function -------------------------------- //
int ledchecker(int currentVal, int randomMax, int threshold)
{
  // First, handle random chance if the index value is 0
  if(currentVal == 0)
  {
    // If random chance passes, return 1, move to index posiiton 1
    int rand1 = random(randomMax);
    if(rand1 < threshold)
    {
     return 1;
    }
    // If random chance fails, return 0
    else
    {
     return 0;
    }
  }
  // If the index value is not 0, then increment the index
  else
  {
    // if the index is the last value, reset it to zero
    if(currentVal == arraymax-1)
    {
      return 0;
    }
    // If not the last value in index, increment index
    else
    {
      return currentVal+1;
    }
  }
}

// ------------------------ rainbowCycle Function ------------------------------ //
// Slightly different, this makes the rainbow equally distributed throughout
// uint_t is a text data type
void rainbowCycle(uint8_t wait, int numCycles, float bright_mult)
{
  uint16_t i, j; // assigning data types
  for(j=0; j<256*numCycles; j++) // cycles of all colors on wheel
  {
    for(i=0; i<strip1.numPixels(); i++) // For all the pixels in the strip
    {
      // the '&' is a bit-wise 'AND' operator
      strip1.setPixelColor(i, Wheel(((i * 256 / strip1.numPixels()) + j) & 255, bright_mult));
    }

    for(i=0; i<strip2.numPixels(); i++) // For all the pixels in the strip
    {
      // the '&' is a bit-wise 'AND' operator
      strip2.setPixelColor(i, Wheel(((i * 256 / strip2.numPixels()) + j) & 255, bright_mult));
    }
    strip1.show();
    strip2.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, float max_prop)
{
  if(WheelPos < 85)
  {
   // strip.Color(r,g,b)
   return strip1.Color((int)(max_prop * WheelPos * 3), (int)(max_prop * (255 - WheelPos * 3)), 0);
  }
  else if(WheelPos < 170)
  {
   WheelPos -= 85;
   return strip1.Color((int)(max_prop * (255 - WheelPos * 3)), 0, (int)(max_prop * WheelPos * 3));
  }
  else
  {
   WheelPos -= 170;
   return strip1.Color(0, (int)(max_prop * WheelPos * 3), (int)(max_prop * (255 - WheelPos * 3)));
  }
}


// ------------- Clear All Function ------------ //
void clearAll()
{
   for(int i=0; i<=92; i++)
  {
    strip1.setPixelColor(i,0,0,0);
    strip2.setPixelColor(i,0,0,0);
  }
  strip1.show();
  strip2.show();
}
