#include <FastLED.h>

#define LED_PIN 6
#define NUM_LEDS 50
CRGB leds[NUM_LEDS];

// define the power state
bool isPoweredOn = false; // default to off

// define the current color, pattern, and speed
CRGB currentColor = CRGB::Red; // default color
int currentPattern = 0;        // default pattern
int patternSpeed = 50;         // default speed (1-100)

void setup() {
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(200);
  Serial.begin(9600);
  clearAll();
}

void loop() {
  // handle serial commands
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'O') { // turn on LEDs
      isPoweredOn = true;
      runCurrentPattern();
    } else if (command == 'F') { // turn off LEDs
      isPoweredOn = false;
      fadeOut();
    } else if (command == 'B') { // set brightness
      int level = Serial.parseInt();
      FastLED.setBrightness(constrain(level, 0, 255));
      FastLED.show();
    } else if (command == 'C') { // set color
      int r = Serial.parseInt();
      int g = Serial.parseInt();
      int b = Serial.parseInt();
      currentColor = CRGB(r, g, b);
    } else if (command == 'P') { // change pattern
      currentPattern = Serial.parseInt();
    } else if (command == 'S') { // set speed
      patternSpeed = Serial.parseInt();
      patternSpeed = constrain(patternSpeed, 1, 100); // clamp to valid range
    }
  }

  if (isPoweredOn) {
    runCurrentPattern();
  }
}

// function to clear all LEDs
void clearAll() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

// function to fade out LEDs
void fadeOut() {
  for (int brightness = FastLED.getBrightness(); brightness >= 0;
       brightness--) {
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(10); // controls fade-out speed
  }
  clearAll();
}

void runCurrentPattern() {
  switch (currentPattern) {
  case 0:
    solidColor();
    break;
  case 1:
    rainbow();
    break;
  case 2:
    twinkle();
    break;
  case 3:
    cascade();
    break;
  case 4:
    chase();
    break;
  case 5:
    quadrantLighting();
    break;
  case 6:
    multiColorQuadrantLighting();
    break;
  default:
    solidColor();
    break; // default to solid color pattern
  }
}

// solid color pattern
void solidColor() {
  fill_solid(leds, NUM_LEDS, currentColor);
  FastLED.show();
}

// rainbow pattern
void rainbow() {
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue + (i * 10), 255, 255);
  }
  FastLED.show();
  hue++;
  delay(map(patternSpeed, 1, 100, 100, 10)); // adjust delay based on speed
}

// twinkle pattern
void twinkle() {
  clearAll();
  for (int i = 0; i < NUM_LEDS / 5; i++) {
    leds[random(NUM_LEDS)] = CHSV(random(255), 255, 255); // Random colors
  }
  FastLED.show();
  delay(map(patternSpeed, 1, 100, 300, 30)); // adjust delay based on speed
}

// cascade pattern
void cascade() {
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS; i += 5) {
    for (int j = 0; j < 5 && i + j < NUM_LEDS; j++) {
      leds[i + j] = CHSV(hue, 255, 255); // Use a gradient of colors
    }
    FastLED.show();
    delay(map(patternSpeed, 1, 100, 150, 15)); // adjust delay based on speed
  }
  hue += 10; // change hue for variety
}

// chase pattern
void chase() {
  static int pos = 0;
  static uint8_t hue = 0;
  clearAll();
  for (int i = 0; i < 5; i++) { // light up a segment of LEDs
    leds[(pos + i) % NUM_LEDS] = CHSV(hue, 255, 255);
  }
  FastLED.show();
  pos = (pos + 1) % NUM_LEDS;
  hue += 5;                                  // slowly shift colors
  delay(map(patternSpeed, 1, 100, 150, 15)); // adjust delay based on speed
}

// quadrant lighting pattern
void quadrantLighting() {
  static int currentQuadrant = 0;
  int quadrantSize = NUM_LEDS / 4; // size of one quadrant
  clearAll();
  for (int i = currentQuadrant * quadrantSize;
       i < (currentQuadrant + 1) * quadrantSize; i++) {
    leds[i] = currentColor; // light up current quadrant
  }
  FastLED.show();
  currentQuadrant = (currentQuadrant + 1) % 4; // move to the next quadrant
  delay(map(patternSpeed, 1, 100, 300, 30));   // adjust delay based on speed
}

void multiColorQuadrantLighting() {
  static int currentQuadrant = 0;
  int quadrantSize = NUM_LEDS / 4;
  CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Blue,
                   CRGB::Yellow}; // colors for each quadrant
  clearAll();
  for (int i = currentQuadrant * quadrantSize;
       i < (currentQuadrant + 1) * quadrantSize; i++) {
    leds[i] = colors[currentQuadrant % 4]; // cycle through the colors array
  }
  FastLED.show();
  currentQuadrant = (currentQuadrant + 1) % 4;
  delay(map(patternSpeed, 1, 100, 300, 30));
}