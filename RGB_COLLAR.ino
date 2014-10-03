/*********************************************************************
*  Code for running RGB collar (http://hackaday.io/project/3109)
*
*  For use with digispark (http://digistump.com)
*  can be used for other purposed
*  David Hopkins 2014, GNU GENERAL PUBLIC LICENSE
*
*  Note: this code is provided as is, parts of it are a bit of a mess,
*  and could be done better, but it works for the purpose intended.
*
**********************************************************************/

#include <Adafruit_NeoPixel.h>

#define NUM_LED 10

//pin 0 not used atm
#define LUX_PIN 1 // for brightness button
#define LUX_GND 2 // for brightness button
#define LED_PIN 3 // data pin for WS2811 string
#define BUT_GND 4 // for pattern select pin
#define BUT_PIN 5 // for pattern select pin

#define SPEED 100 // global pattern speed

boolean goingUp = true;
byte ledPosition = 0;//for colourwipe type effects
byte patPosition = 0;//for colourwheel type effects
byte patNum = 0;
byte numPat = 1;//number of patterns - changed later in selection code
byte lux = 16; // starting brightness

boolean buttonPressedFlag = false;
boolean luxButtonPressedFlag = false;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LED, LED_PIN, NEO_RGB + NEO_KHZ800);

uint32_t Col1 = strip.Color(150, 000, 255);
uint32_t Col2 = strip.Color(255, 000, 150);

void setup()
{
//simulated lux GND  
  pinMode(LUX_GND, OUTPUT);
  digitalWrite(LUX_GND, LOW);
  
//simulated button GND  
  pinMode(BUT_GND, OUTPUT);
  digitalWrite(BUT_GND, LOW);

//button pin  
  pinMode(BUT_PIN, INPUT);
  digitalWrite(BUT_PIN, HIGH); // turn on pullup r

//lux pin
  pinMode(LUX_PIN, INPUT);
  digitalWrite(LUX_PIN, HIGH); // turn on pullup r

  strip.begin();
  strip.setBrightness(lux);
  strip.show(); // Initialize all pixels to 'off'
}

void loop()
{
  doEffect();
  checkButton();
  checkLight();
}

void checkLight()
{
if(!digitalRead(LUX_PIN))
  {
    if(luxButtonPressedFlag == false)//if its first time press
    {
      luxButtonPressedFlag = true;
    }
  }
  else//if NOT pressed
  {			
    if(luxButtonPressedFlag == true)//if it WAS pressed
    {
      lux = lux + 16;
      strip.setBrightness(lux);
      luxButtonPressedFlag = false;
    }
  }
}

void checkButton()
{
  if(!digitalRead(BUT_PIN))
  {
    if(buttonPressedFlag == false)//if its first time press
    {
      buttonPressedFlag = true;
    }
  }
  else//if NOT pressed
  {			
    if(buttonPressedFlag == true)//if it WAS pressed
    {
      patNum++;
      if(patNum > numPat)
      {
        patNum = 0;
      }
      buttonPressedFlag = false;
    }
  }
}

void doEffect()
{
  switch(patNum)
  {
    case 1: allOneColour(strip.Color(255, 000, 000)); break;
    case 2: allOneColour(strip.Color(000, 255, 000)); break;
    case 3: allOneColour(strip.Color(000, 000, 255)); break;
    case 4: allOneColour(strip.Color(000, 255, 255)); break;
    case 5: allOneColour(strip.Color(255, 255, 000)); break;
    case 6: allOneColour(strip.Color(255, 000, 255)); break;
    case 7: allOneColour(strip.Color(255, 255, 255)); break;
    case 8: rainbow(); break;
    case 9: rainbowCycle(); break;
    case 10: twoColourFade(Col1,Col2); break;
    case 11: twoColourAltFade(Col1,Col2); break;
    default: allOneColour(strip.Color(000, 000, 000)); break;
  }
  numPat = 11;//change when adding a pattern
}

//  -  -  -  -  -  -  -  -  -  EFFECT FUNCTIONS  -  -  -  -  -  -  -  -

void allOneColour(uint32_t c)
{
  for(int i=0; i<strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
  }
  strip.show();
}
/*
 //extra effect, not used
void twoColourBlended(uint32_t c1, uint32_t c2)
{//centre of strip colour 1, extremeties colour 2
  //break it down
  uint8_t r1 = (uint8_t)(c1 >> 16), g1 = (uint8_t)(c1 >>  8), b1 = (uint8_t)c1;
  uint8_t r2 = (uint8_t)(c2 >> 16), g2 = (uint8_t)(c2 >>  8), b2 = (uint8_t)c2;
  float A = 1.0/(strip.numPixels()/2);//reduce to decimal

  for(byte i=0; i<(strip.numPixels()/2); i++)//fill first half
  {
    float prop = i*A;
    float iprop = 1.0-prop;
    strip.setPixelColor(i, 
                        ((r1*prop) + (r2*iprop)),
                        ((g1*prop) + (g2*iprop)),
                        ((b1*prop) + (b2*iprop)));
  }

  for(byte i=0; i<(strip.numPixels()/2); i++)//fill second half
  {
    float prop = i*A;
    float iprop = 1.0-prop;
    strip.setPixelColor(i+(strip.numPixels()/2), 
                        ((r2*prop) + (r1*iprop)),
                        ((g2*prop) + (g1*iprop)),
                        ((b2*prop) + (b1*iprop)));
  }
  strip.show();
}
*/

void twoColourFade(uint32_t c1, uint32_t c2)
{//all one colour, fading to the other and back again 
  //break it down
  uint8_t r1 = (uint8_t)(c1 >> 16), g1 = (uint8_t)(c1 >>  8), b1 = (uint8_t)c1;
  uint8_t r2 = (uint8_t)(c2 >> 16), g2 = (uint8_t)(c2 >>  8), b2 = (uint8_t)c2;
  float prop = patPosition/256.0;//reduce to decimal
  float iprop = 1.0-prop;

  for(byte i=0; i<strip.numPixels(); i++)//fill ALL
  {
    if(goingUp)
      strip.setPixelColor(i, ((r1*prop) + (r2*iprop)),((g1*prop) + (g2*iprop)),((b1*prop) + (b2*iprop)));
    else
      strip.setPixelColor(i, ((r2*prop) + (r1*iprop)),((g2*prop) + (g1*iprop)),((b2*prop) + (b1*iprop)));
  }
  strip.show();
  delay(SPEED);
  patPosition++;//will overflow to 0
  if(patPosition == 0)
    goingUp = !goingUp; 
}



void twoColourAltFade(uint32_t c1, uint32_t c2)
{//alternate colours, fading between the two
//break it down
  uint8_t r1 = (uint8_t)(c1 >> 16), g1 = (uint8_t)(c1 >>  8), b1 = (uint8_t)c1;
  uint8_t r2 = (uint8_t)(c2 >> 16), g2 = (uint8_t)(c2 >>  8), b2 = (uint8_t)c2;
  float prop = patPosition/256.0;//reduce to decimal
  float iprop = 1.0-prop;
  uint8_t r1New = (r1*prop) + (r2*iprop);
  uint8_t g1New = (g1*prop) + (g2*iprop);
  uint8_t b1New = (b1*prop) + (b2*iprop);
  uint8_t r2New = (r2*prop) + (r1*iprop);
  uint8_t g2New = (g2*prop) + (g1*iprop);
  uint8_t b2New = (b2*prop) + (b1*iprop);
  for(byte i=0; i<strip.numPixels(); i=i+2)//fill evens
  {
    if(goingUp)
      strip.setPixelColor(i, r1New, g1New, b1New);
    else
      strip.setPixelColor(i, r2New, g2New, b2New);
  }
  for(byte i=1; i<strip.numPixels(); i=i+2)//fill odds
  {
    if(goingUp)
      strip.setPixelColor(i, r2New, g2New, b2New);
    else
      strip.setPixelColor(i, r1New, g1New, b1New);
  }
  strip.show();
  delay(SPEED);
  patPosition++;//will overflow to 0
  if(patPosition == 0)
    goingUp = !goingUp;
}

void rainbow()
{
  for(byte i=0; i<strip.numPixels(); i++)
  {
    strip.setPixelColor(i, Wheel((i+patPosition) & 255));
  }
  strip.show();
  delay(SPEED);
  patPosition++;//will overflow to 0
}


void rainbowCycle()
{// Slightly different, this makes the rainbow equally distributed throughout
  for(byte i=0; i< strip.numPixels(); i++)
  {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + patPosition) & 255));
  }
  strip.show();
  delay(SPEED);
  patPosition++;//will overflow to 0
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.

uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else if(WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } 
  else
  {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

