/*
    EXAMPLE - 2-zone Hi-Hat with hihat Controller (FSR or TCRT5000)

    This sample code tesed with Roland VH-10.
    https://github.com/RyoKosaka/HelloDrum-arduino-Library
*/

#include <hellodrum.h>

// Using MIDI Library. If you want to use USB-MIDI, comment out the next two lines.
#include <MIDI.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
MIDI_CREATE_DEFAULT_INSTANCE();

// Uncomment the next two lines for using USB-MIDI with atmega32u4 or Teensy
//#include <USB-MIDI.h>
// USBMIDI_CREATE_DEFAULT_INSTANCE();

/*
Mux Breakout ----------- Arduino
S0 ------------------- 2
S1 ------------------- 3
S2 ------------------- 4
S2 ------------------- 5
Z -------------------- A0
 */
// Define MUX Pins

HelloDrumMUX_4067 mux(2, 3, 4, 5, 0); // D2, D3, D4, D5, A0

// Uncomment the next two lines for using USB-MIDI with atmega32u4 or Teensy
//#include <USB-MIDI.h>
// USBMIDI_CREATE_DEFAULT_INSTANCE();

// name your pad and define mux pin
// So, you need the same number of lines as the number of pads.
// The piezo named "hihat" is connected to the 0 mux pin.
// The FSR or TCRT5000 named "hihatPedal" is connected to the 1 mux pin.
HelloDrum kick(0);
HelloDrum snare(1, 2);
HelloDrum hihat(3, 4);
HelloDrum hihatPedal(5);
HelloDrum highTom(6);
HelloDrum midTom(7);
HelloDrum lowTom(8);
HelloDrum floorTom(9);
HelloDrum ride(10, 11);
HelloDrum splash(12);
HelloDrum rightCrash(13);
HelloDrum leftCrash(14);
HelloDrum bell(15);

// Set the DIGITAL pin number to which the buttons are connected.
HelloDrumButton button(6, 7, 8, 9, 10); //(EDIT,UP,DOWN,NEXT,BACK)

void setup()
{

  lcd.init(); // initialize the lcd
  lcd.backlight();

  // If you use Hairless MIDI, you have to comment out the next line.
  // MIDI.begin(10);

  // And uncomment the next two lines.
  MIDI.begin();
  Serial.begin(38400);

  // Give each pad a name to be displayed on the LCD.
  // It is necessary to make the order in exactly the same order as you named the pad first.
  kick.settingName("KICK");
  snare.settingName("SNARE");
  hihat.settingName("HIHAT");
  hihatPedal.settingName("HIHAT PEDAL");
  ride.settingName("RIDE");
  highTom.settingName("HIGH TOM");
  midTom.settingName("MID TOM");
  lowTom.settingName("LOW TOM");
  leftCrash.settingName("LEFT CRASH");
  rightCrash.settingName("RIGHT CRASH");
  bell.settingName("BELL");
  splash.settingName("SPLASH");
  floorTom.settingName("FLOOR TOM");

  // Load settings from EEPROM.
  // It is necessary to make the order in exactly the same order as you named the pad first.
  kick.loadMemory();
  snare.loadMemory();
  hihat.loadMemory();
  hihatPedal.loadMemory();
  ride.loadMemory();
  highTom.loadMemory();
  midTom.loadMemory();
  lowTom.loadMemory();
  leftCrash.loadMemory();
  rightCrash.loadMemory();
  bell.loadMemory();
  splash.loadMemory();
  floorTom.loadMemory();

  lcd.clear();
  lcd.print("Hello world!");
  lcd.setCursor(0, 1);
  lcd.print("Hello drum!");

 
}

void loop()
{

  /////////// 1. LCD and SETTING MODE /////////////

  bool buttonPush = button.GetPushState();
  bool editStart = button.GetEditState();
  bool editDone = button.GetEditdoneState();
  bool display = button.GetDisplayState();

  const char *padName = button.GetPadName();
  const char *item = button.GetSettingItem();
  int settingValue = button.GetSettingValue();
  int velocity = button.GetVelocity();
  const char *hitPad = button.GetHitPad();

  button.readButtonState();

  kick.settingEnable();
  snare.settingEnable();
  hihat.settingEnable();
  hihatPedal.settingEnable();
  ride.settingEnable();
  highTom.settingEnable();
  midTom.settingEnable();
  lowTom.settingEnable();
  leftCrash.settingEnable();
  rightCrash.settingEnable();
  bell.settingEnable();
  splash.settingEnable();
  floorTom.settingEnable();

  if (buttonPush == true)
  {
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  if (editStart == true)
  {
    lcd.clear();
    lcd.print("EDIT START");
    delay(500);
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  if (editDone == true)
  {
    lcd.clear();
    lcd.print("EDIT DONE");
    delay(500);
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  // show hitted pad name and velocity to LCD
  if (display == true)
  {
    int velocity = button.GetVelocity();
    const char *hitPad = button.GetHitPad();

    lcd.clear();
    lcd.print(hitPad);
    lcd.setCursor(0, 1);
    lcd.print(velocity);

    if (hihat.hit == true)
    {
      // open
      if (hihatPedal.openHH == true)
      {
        lcd.setCursor(15, 0);
        lcd.print("O");
      }
      // close
      else if (hihatPedal.closeHH == true)
      {
        lcd.setCursor(15, 0);
        lcd.print("C");
      }
    }
  }

  // show hitted pad name and velocity to LCD
  // Immediately after I2C communication, scanning of piezo is stop.
  // So if you change the display right after you hit pad, it affects the sensing.
  // I recommend leaving it as a comment.
  /*
      if (display == true)
      {
          lcd.clear();
          lcd.print(hitPad);
          lcd.setCursor(0, 1);
          lcd.print(velocity);
      }
      */

  ////////// 2. SENSING & SENDING MIDI////////////

  // Sensing each pad.
  mux.scan();

  kick.singlePiezoMUX();
  snare.dualPiezoMUX();
  hihat.HHMUX();
  hihatPedal.hihatControlMUX();
  ride.cymbal3zoneMUX();
  highTom.singlePiezoMUX();
  midTom.singlePiezoMUX();
  lowTom.singlePiezoMUX();
  floorTom.singlePiezoMUX();
  bell.singlePiezoMUX();
  splash.singlePiezoMUX();
  leftCrash.singlePiezoMUX();
  rightCrash.singlePiezoMUX();

  // MIDI signals are transmitted with this IF statement.
  // bow

  // KICK//
  if (kick.hit == true)
  {
    MIDI.sendNoteOn(kick.note, kick.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(kick.note, 0, 10);
  }

  // SNARE//
  if (snare.hit == true)
  {
    MIDI.sendNoteOn(snare.note, snare.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(snare.note, 0, 10);
  }
  else if (snare.hitRim == true)
  {
    if (snare.velocity > 60) // Rim shot
    {
      MIDI.sendNoteOn(snare.noteRim, snare.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(snare.noteRim, 0, 10);
    }
    else // Side Stick
    {
      MIDI.sendNoteOn(snare.noteCup, snare.velocity * 2, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(snare.noteCup, 0, 10);
    }
  }

  // HIHAT//
  if (hihat.hit == true)
  {
    // check open or close
    // 1.open
    if (hihatPedal.openHH == true)
    {
      MIDI.sendNoteOn(hihat.noteOpen, hihat.velocity, 10); //(note of open, velocity, channel)
      MIDI.sendNoteOff(hihat.noteOpen, 0, 10);
    }
    // 2.close
    else if (hihatPedal.closeHH == true)
    {
      MIDI.sendNoteOn(hihat.noteClose, hihat.velocity, 10); //(note of close, velocity, channel)
      MIDI.sendNoteOff(hihat.noteClose, 0, 10);
    }
  }

  // edge
  else if (hihat.hitRim == true)
  {
    // check open or close
    // 1.open
    if (hihatPedal.openHH == true)
    {
      MIDI.sendNoteOn(hihat.noteRim, hihat.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(hihat.noteRim, 0, 10);
    }
    // 2.close
    else if (hihatPedal.closeHH == true)
    {
      MIDI.sendNoteOn(hihat.noteRim, hihat.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(hihat.noteRim, 0, 10);
    }
  }

  // HIHAT CONTROLLER//
  // when hihat is closed
  if (hihatPedal.hit == true)
  {
    MIDI.sendNoteOn(hihatPedal.note, hihatPedal.velocity, 10); //(note of pedal, velocity, channel)
    MIDI.sendNoteOff(hihatPedal.note, 0, 10);
  }

  // sending state of pedal with controll change
  if (hihatPedal.moving == true)
  {
    MIDI.sendControlChange(4, hihatPedal.pedalCC, 10);
  }

  // RIDE//
  // 1.bow
  if (ride.hit == true)
  {
    MIDI.sendNoteOn(ride.note, ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ride.note, 0, 10);
  }

  // 2.edge
  else if (ride.hitRim == true)
  {
    MIDI.sendNoteOn(ride.noteRim, ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ride.noteRim, 0, 10);
  }

  // 3.cup
  else if (ride.hitCup == true)
  {
    MIDI.sendNoteOn(ride.noteCup, ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ride.noteCup, 0, 10);
  }

  // 4.choke
  if (ride.choke == true)
  {
    MIDI.sendAfterTouch(ride.note, 127, 10);
    MIDI.sendAfterTouch(ride.noteRim, 127, 10);
    MIDI.sendAfterTouch(ride.noteCup, 127, 10);
    MIDI.sendAfterTouch(ride.note, 0, 10);
    MIDI.sendAfterTouch(ride.noteRim, 0, 10);
    MIDI.sendAfterTouch(ride.noteCup, 0, 10);

    /*
    MIDI.sendPolyPressure(ride.note, 127, 10);
    MIDI.sendPolyPressure(ride.noteRim, 127, 10);
    MIDI.sendPolyPressure(ride.noteCup, 127, 10);
    MIDI.sendPolyPressure(ride.note, 0, 10);
    MIDI.sendPolyPressure(ride.noteRim, 0, 10);
    MIDI.sendPolyPressure(ride.noteCup, 0, 10);
    */
  }

  // TOM//
  if (highTom.hit == true)
  {
    MIDI.sendNoteOn(highTom.note, highTom.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(highTom.note, 0, 10);
  }
  if (midTom.hit == true)
  {
    MIDI.sendNoteOn(midTom.note, midTom.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(midTom.note, 0, 10);
  }
  if (lowTom.hit == true)
  {
    MIDI.sendNoteOn(lowTom.note, lowTom.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(lowTom.note, 0, 10);
  }
  if (floorTom.hit == true)
  {
    MIDI.sendNoteOn(floorTom.note, floorTom.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(floorTom.note, 0, 10);
  }
  if (bell.hit == true)
  {
    MIDI.sendNoteOn(bell.note, bell.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(bell.note, 0, 10);
  }
  if (bell.hit == true)
  {
    MIDI.sendNoteOn(bell.note, bell.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(bell.note, 0, 10);
  }
}
