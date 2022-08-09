/*
ESP32S3 DevkitC
USBHostSheild 2.0 as shown here: https://www.pjrc.com/teensy/td_libs_USBHostShield.html
SD Card Reader
Rotary Encoder Knob
Two Momentary Microswitches
ILI9341 (designed for a 240x135 ST7789VW originally)

 USB Host Shield must be modified to power the VBUS at 5v instead of 3.3v
The resistor next to "2k2" must be removed.
A wire from 5v to the VBUS contact behind the USB port must be soldered. 
RST must be wired to 3.3v on the USB host board
*/

// PIN ASSIGNMENTS

// USB and LCD all hooked to HSPI
#define HSPI_MOSI 7
#define HSPI_MISO 10
#define HSPI_CLK 6

// SD hooked up to FSPI
#define FSPI_MOSI 47
#define FSPI_MISO 21
#define FSPI_CLK 48

// buttons are microswitch
// momentary pushbuttons
// closed high.
#define BUTTON_A 38
#define BUTTON_B 39
// a rotary encoder is attached
// as indicated
#define ENC_CLK 37
#define ENC_DATA 36

// LCD is configured as follows
#define LCD_CS 11
#define LCD_DC 4
#define LCD_RST 8
// BL is hooked to +3.3v
#define LCD_BL -1
#define LCD_ROTATION 3

// USB host CS
#define USB_CS 5

// SD Card reader CS
#define SD_CS 1

#include <Arduino.h>
#include <ESP32Encoder.h>
#include <SD.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <midiusb.h>
#include <usbh_midi.h>
#include <usbhub.h>
#include <gfx.hpp>
#include <htcw_button.hpp>
#include <ili9341.hpp>
#include <message_queue.hpp>
#include <sfx.hpp>
#include <tft_io.hpp>
#include <thread.hpp>

#include "midi_esptinyusb.hpp"
#include "telegrama.hpp"
using namespace arduino;
using namespace sfx;
using namespace gfx;
using namespace freertos;
using bus_t = tft_spi_ex<HSPI, LCD_CS, HSPI_MOSI, HSPI_MISO, HSPI_CLK, SPI_MODE0, true>;
using lcd_t = ili9342c<LCD_DC, LCD_RST, LCD_BL, bus_t, LCD_ROTATION, false, 400, 200>;
using color_t = color<typename lcd_t::pixel_type>;

midi_esptinyusb midi_out;
USB Usb;
USBHub Hub(&Usb);
USBH_MIDI midi_in(&Usb);

button<BUTTON_A> button_a;
button<BUTTON_B> button_b;
ESP32Encoder encoder;
lcd_t lcd;
void usb_in_init() {
    char buf[20];
    uint16_t vid = midi_in.idVendor();
    uint16_t pid = midi_in.idProduct();
    sprintf(buf, "VID:%04X, PID:%04X", vid, pid);
    Serial.println(buf);
}
void wait_and_restart() {
    button_a.update();
    button_b.update();
    while(!button_a.pressed() && !button_b.pressed()) {
        button_a.update();
        button_b.update();
        vTaskDelay(1);
    }
    ESP.restart();
}
void setup() {
    Serial.begin(115200);
    spi_container<FSPI>::instance().begin(FSPI_CLK, FSPI_MISO, FSPI_MOSI);
    spi_container<HSPI>::instance().begin(HSPI_CLK, HSPI_MISO, HSPI_MOSI);
    button_a.initialize();
    button_b.initialize();
    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder.attachFullQuad(ENC_CLK, ENC_DATA);
    SPIFFS.begin(false);
    lcd.initialize();
}

void loop() {
}