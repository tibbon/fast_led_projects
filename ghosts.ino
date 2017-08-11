#include "FastLED.h"                                         

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
 
// Fixed global definitions.
#define LED_DT 11                                             // Data pin to connect to the strip.
#define LED_CK 13
#define COLOR_ORDER BGR                                       
#define LED_TYPE APA102                                       // Don't forget to change LEDS.addLeds
#define NUM_LEDS 600                                           // Number of LED's.
#define NUM_GHOSTS 8

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array
uint8_t global_dir = 0;

typedef struct
 {
     uint8_t hue;
     uint8_t dir;
     uint8_t pos;
     boolean alive = false;
     uint8_t time_alive = 0;
     uint8_t speed;
 }  ghost;

ghost ghosts[NUM_GHOSTS];

void setup() {
  delay(100);                                                 
  Serial.begin(57600);
  Serial.write("Initializing\n");
  LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER, DATA_RATE_MHZ(2)>(leds, NUM_LEDS); 
  //set_max_power_in_volts_and_milliamps(5, 100);
}


void loop () {
  spawn_news_ghosts_if_needed();
  update_ghosts();
  FastLED.delay(11);
  nscale8(leds,NUM_LEDS,236);
  FastLED.show();                   
}


void spawn_ghost() {
  uint8_t ghost_hue = beatsin8(20, 0, 255);
  for(int i = 0; i < NUM_GHOSTS; i++) {
    if (!ghosts[i].alive) {
      //Serial.write("Spawning a new ghost\n");
      ghosts[i].alive = true;
      ghosts[i].hue = ghost_hue;
      ghosts[i].time_alive = random8();
      ghosts[i].pos = random16(NUM_LEDS);
      
      if (global_dir == 0) {
        ghosts[i].dir = 0;
        global_dir = 1;
      } else {
        ghosts[i].dir = 1;
        global_dir = 0;
      }
      ghosts[i].speed = 1;
    }
  }
}

void spawn_news_ghosts_if_needed() {
  int alive_count = 0;
  for(int i = 0; i < NUM_GHOSTS; i++) {
    if(ghosts[i].alive) {
      alive_count++;
    }
  }
  if (alive_count < NUM_GHOSTS) {
    spawn_ghost();
  }
}


void update_ghosts() {
  uint8_t ghost_hue;
  for(int i = 0; i < NUM_GHOSTS; i++) {
    ghost_hue = beatsin8(10 * i, 0, 255);
    
    ghosts[i].hue = ghost_hue;
    
    // If it's still spawning
    if (ghosts[i].time_alive < 255) {
      ghosts[i].time_alive++;
      // Play spawning animation
      leds[ghosts[i].pos].setHSV(ghost_hue, 255, ghosts[i].time_alive);
      FastLED.show();
    } else {
      // Check against all others ghosts
      for(int x = 0; x < NUM_GHOSTS; x++) {
        if(x != i) {
          if(ghosts[x].time_alive == 255) {
            if(ghosts[i].pos == ghosts[x].pos) {
              if(ghosts[i].dir == 0) {
                ghosts[i].dir = 1;
              } else {
                ghosts[i].dir = 0;
              }
              //Serial.print("Collision\n");
              // Animate a kill/burst
              // Kill the slower one
              leds[ghosts[i].pos].setHue(ghost_hue);
              leds[ghosts[i].pos + 1].setHue(ghost_hue - 10);
              leds[ghosts[i].pos + 2].setHue(ghost_hue - 20);
              leds[ghosts[i].pos - 1].setHue(ghost_hue + 10);
              leds[ghosts[i].pos - 2].setHue(ghost_hue + 20);
              FastLED.show();
              FastLED.delay(10);
              FastLED.show();
              
              if(ghosts[i].speed > ghosts[x].speed) {
                if(ghosts[i].speed < 2) {
                  ghosts[i].speed++;
                  ghosts[x].alive = false;
                } else {
                  ghosts[i].alive = false;
                }
              } else {
                if(ghosts[x].speed < 2) {
                  ghosts[x].speed++;
                  ghosts[i].alive = false;
                } else {
                  ghosts[x].alive = false;
                }
                
              }
            }
          }
        }    
      }

      // Move and cycle it
      if (ghosts[i].alive) {
        if (ghosts[i].dir == 1) {
          if (ghosts[i].pos < NUM_LEDS) {
            ghosts[i].pos += ghosts[i].speed;
          } else {
            ghosts[i].hue = ghost_hue;
            ghosts[i].dir = 0;
          }
          leds[ghosts[i].pos].setHue(ghosts[i].hue);
          if (ghosts[i].speed > 1) {
            leds[ghosts[i].pos - 1].setHue(ghosts[i].hue);
          }
        }
    
         if (ghosts[i].dir == 0) {
          if (ghosts[i].pos > 0) {
            ghosts[i].pos -= ghosts[i].speed;
          } else {
            ghosts[i].hue = ghost_hue;
            ghosts[i].dir = 1;
          }
          leds[ghosts[i].pos].setHue(ghosts[i].hue);
          if (ghosts[i].speed > 1) {
            leds[ghosts[i].pos + 1].setHue(ghosts[i].hue);
          }
        }
        
        
        //Serial.write("Updating Ghost\n");
        FastLED.show();
      }
    }
  }
}

