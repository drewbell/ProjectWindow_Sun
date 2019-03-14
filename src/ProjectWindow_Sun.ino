/*
 * Project ProjectWindow_Sun
 * Description:
 * Author:
 * Date:
 */

/* Neopixel WS2812B datasheet: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
 *    Neopixels require 5V input logic level, must use buffer like HCT244
 *    All digital pins are 5V tolerant
 *    50mA each pixel with full RGB
 *    50ma * 60 LEDs = 3A -> power supply is 2A, so be careful
 */

/* Blynk 
 *    Example: https://examples.blynk.cc/?board=Particle%20Photon&shield=Particle%20WiFi&example=GettingStarted%2FVirtualPinRead
 */


// struct: rgb 
// rgb currentSensorColor
// rgb currentApp
// bool enabled

// V0   mode          input [see mode enum below] 
// V1   time          input HH:MM:SS AM/PM
// V2   red           input [0,255]
// V3   green         input [0,255]
// V4   blue          input [0,255]
// V5   brightness    input [0,255]
// V6   color temp    output:   [0, 4095]   Refreshes every 10s
// V7   ON/OFF        input [0,1]

/**************** DEFINES **********************************/
#include "Particle.h"
#include "blynk.h"
#include "neopixel.h"

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_COUNT 60
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

enum mode { OFF_MODE = 1, 
            RGB_MODE = 2, 
            TEMP_MODE = 3, 
            WINDOW_MODE = 4, 
            ALARM_MODE = 5,
};

struct RGB 
{   int red;
    int green; 
    int blue;
};

volatile struct RGB color;
volatile struct RGB last_color;
volatile struct RGB windowColor; 
volatile int mode = 1;
String alarmTime; 
volatile int brightness = 0; 
volatile bool blynkUpdateReady = true;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "19e91bbb75a24afe9895491df395c67d";

SerialLogHandler logHandler;

// Blynk Functions
// mode: this function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V0)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    mode = pinValue;  
    Log.info("New mode is %d.", mode);
    blynkUpdateReady = true;
}

// alarmTime - save as string? Need to handle  HH:MM:SS AM/PM
BLYNK_WRITE(V1)
{
    String pinValue = param.asString(); // assigning incoming value from pin V1 to a variable
    alarmTime = pinValue;
    Log.info("New time is %s.", alarmTime.c_str() );
    blynkUpdateReady = true;
}

// red
BLYNK_WRITE(V2)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    color.red = pinValue;
    Log.info("New red value is %d.", color.red);
    blynkUpdateReady = true;
}

// green 
BLYNK_WRITE(V3)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    color.green = pinValue;
    Log.info("New green value is %d.", color.green);
    blynkUpdateReady = true;
}

// blue 
BLYNK_WRITE(V4)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    color.blue = pinValue;
    Log.info("New blue value is %d.", color.blue);
    blynkUpdateReady = true;
}

// brightness 
BLYNK_WRITE(V5)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    brightness = pinValue;
    Log.info("New brightness value is %d.", brightness);
    blynkUpdateReady = true;
}

// setColor
void setColor(void){ 
    struct RGB newColor;
    switch(mode){
        // switch based on the mode
        case OFF_MODE:
            newColor.red = newColor.green = newColor.blue = 0;
            break;
        case RGB_MODE:
            newColor.red = color.red;
            newColor.green = color.green;
            newColor.blue = color.blue;
            break;
        case WINDOW_MODE:       //TODO(drew) pipe in windowColor data from the other photo
            newColor.red = windowColor.red;
            newColor.green = windowColor.green;
            newColor.blue = windowColor.blue;
            break;
        case ALARM_MODE:
            newColor.red = newColor.green = newColor.blue = 0;
            break;
        default:
            newColor.red = newColor.green = newColor.blue = 200;
    }
    
    for(int led = 0; led < PIXEL_COUNT; led++)
    {
        strip.setPixelColor(led, color.red, color.green, color.blue);
    }
    strip.show();
}

SYSTEM_MODE(AUTOMATIC);

// setup() runs once, when the device is first turned on.
void setup() {

    // Use primary serial over USB interface for logging output, not serial
    //Serial.begin(9600);
    Log.info("Starting setup."); 
    
    // Put initialization like pinMode and begin functions here.
    // set up pinmodes for the 3 mosfets
    // set up handling of iphone app
    delay(5000); // Allow board to settle

    // wifi
    WiFi.setCredentials("ATT6GW55sY", "4cgp3w4uik42");
    Particle.connect();         // connects wifi and particle cloud
    if( WiFi.ready() )
    {
        Log.info("WiFi ready."); 
    } else {
        Log.warn("WiFi setup failed.");
    }
    
    if( Particle.connected() )
    {
        Log.info("Particle connected to CLOUD."); 
    } else {
        Log.warn("Particle not connected to CLOUD.");
    }

    Blynk.begin(auth);

    // initialize strip to be dark
    strip.begin();
    strip.show();

    Log.info("Finishing setup setup."); 
}

void loop() {
    Blynk.run();
    if(blynkUpdateReady){
        setColor();  
        Log.info("Finishing setup setup."); 
    }
}
