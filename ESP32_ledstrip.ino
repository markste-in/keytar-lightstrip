//copy pasta from multiple sources

#include <FastLED.h>
#define NUM_LEDS 144
#define DATA_PIN 12
#define CLOCK_PIN 13
#define BRIGHTNESS  20

CRGB leds[NUM_LEDS];


void setup() { 
  Serial.begin(9600);
  keytar_setup();
  xTaskCreate(keytar_scan, "keytarscan", 20000, NULL, 5, NULL);
  FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS );
}
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void rainbow_beat(int a,int b, int len, int pat) {
  
  uint8_t beatA = beatsin8(a, 0, 255);                        // Starting hue
  uint8_t beatB = beatsin8(b, 0, 255);
  //            // Use FastLED's fill_rainbow routine.
  switch(pat){
    case 1:
      fill_rainbow(leds, NUM_LEDS, (beatA+beatB)/2, len);
      break;
    case 2:
      fill_gradient_RGB(leds, 1, CRGB(255,0,0), NUM_LEDS, CRGB::White);
      break;
    case 3:
      fill_gradient_RGB(leds, 1, CRGB(255, 0, 238), NUM_LEDS, CRGB::White);
      break;
    case 4:
      fill_gradient_RGB(leds, 1, CRGB::White, NUM_LEDS, CRGB(0,255,0));
      break;
    default:
      break;
      
  }
  
} // rainbow_beat()

int pins[] = {27, 33, 15, 32, 34, 39, 36, 4};
#define nrows 4
#define ncols 4
#define debounce_cycles 10
#define npins (nrows+ncols)
#define nkeys (ncols*nrows)
uint16_t keys;
uint16_t keyc; // 0-5, 4 times?
char keybounce[nkeys];

void keytar_setup() {
  for (int i = 0; i < npins; i++) {
    if (i < nrows) {
      pinMode(pins[i], OUTPUT);
    } else {
      pinMode(pins[i], INPUT_PULLDOWN);
    }
  }
}

void keytar_scan(void*) {
  while (true) {
    for (int i = 0; i < nrows; i++) {
      pinMode(pins[i], OUTPUT);
      digitalWrite(pins[i], HIGH);
      for (int j = 0; j < ncols; j++) {
        int k = (i * ncols) + j;
        if (digitalRead(pins[nrows + j])) {
          if (keybounce[k] > debounce_cycles) {
            keys |= 1 << k;
          } else {
            keybounce[k]++;
          }
        } else {
          if (keybounce[k] > 0) {
            keybounce[k]--;
          } else {

            keys &= ~(1 << k);
          }
        }
      }
      digitalWrite(pins[i], LOW);
      pinMode(pins[i], INPUT);

      //vTaskDelay(1);
    }
    vTaskDelay(1);
  }
}

int pressedKey()
{
  if (keys == 0)
    return 0;
  else
    return (int) (log(keys)/log(2)) + 1;

}

uint8_t pK =  0;
int freqA = 16;
int freqB = 16;
int len = 3;
int pattern = 1;
void loop() {

    rainbow_beat(freqA,freqB, len, pattern);

    FastLED.show();
    pK = pressedKey();
    if (pK >= 1 && pK <= 4){
      FastLED.setBrightness((pK ) * (16*4 - 50));
    }
    if (pK >= 5 && pK <= 8){
      freqB =  sq(pK -5 + 3);
    }

    if (pK >= 9 && pK <= 12){
      freqA =   sq((pK - 9 + 3));
    }
    if (pK >= 13 && pK <= 16){
      //len = (pK - 13 + 1) * 3;
      pattern = (pK - 13 + 1);
    }
//    Serial.println(pK);
//    Serial.println(freqA);
//    Serial.println(freqB);
//    Serial.println(len);
    delay(5);
}
  
