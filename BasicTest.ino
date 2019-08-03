// Simple WAV file player example
//
// Three types of output may be used, by configuring the code below.
//
//   1: Digital I2S - Normally used with the audio shield:
//         http://www.pjrc.com/store/teensy3_audio.html
//
//   2: Digital S/PDIF - Connect pin 22 to a S/PDIF transmitter
//         https://www.oshpark.com/shared_projects/KcDBKHta
//
//   3: Analog DAC - Connect the DAC pin to an amplified speaker
//         http://www.pjrc.com/teensy/gui/?info=AudioOutputAnalog
//
// To configure the output type, first uncomment one of the three
// output objects.  If not using the audio shield, comment out
// the sgtl5000_1 lines in setup(), so it does not wait forever
// trying to configure the SGTL5000 codec chip.
//
// The SD card may connect to different pins, depending on the
// hardware you are using.  Uncomment or configure the SD card
// pins to match your hardware.
//
// Data files to put on your SD card can be downloaded here:
//   http://www.pjrc.com/teensy/td_libs_AudioDataFiles.html
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

AudioPlaySdWav           playWav1;
// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
AudioOutputI2S           audioOutput;
//AudioOutputSPDIF       audioOutput;
//AudioOutputAnalog      audioOutput;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14


#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

uint32_t play_offset = 30000000;

void setup() {
  Serial.begin(9600);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
      display.println("Unable to access the SD card");
      display.display();
      
    }
  }
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("Mix");
  display.println("Headphone");
  display.display();
}

void playFile(const char *filename)
{
  Serial.print("Playing: ");
  Serial.println(filename);
  
  display.setTextSize(1);
  display.print("Playing: ");
  display.println(filename);
  display.display();
  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename, play_offset);

  // A brief delay for the library read WAV info
  delay(5);

  float duration = 0.0;
  float totalLength = 0.0;
  totalLength = playWav1.lengthMillis() / 1000.0;

  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {

    
    duration = playWav1.positionMillis() / 1000.0;
    
    

    Serial.print("Playing: ");
    Serial.println(filename);

    display.setCursor(0,0);
    display.clearDisplay();
  
    display.setTextSize(1);
    display.print("Playing: ");
    display.println(filename);
    display.print("Duration: ");
    display.print(duration);
    display.println("s");
    display.print("out of: ");
    display.print(totalLength);
    display.println("s");
    
    display.display();
    
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    //sgtl5000_1.volume(vol);
  }
}



void loop() {
  playFile("03THECHA.WAV");  // filenames are always uppercase 8.3 format
  delay(500);
  playFile("04HOMEFO.WAV");
  delay(500);
  playFile("10TOUCHM.WAV");
  delay(500);
}
