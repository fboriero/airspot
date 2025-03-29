#include "edp.h"

/* Simple firmware for a ESP32 displaying a static image on an EPaper Screen.
 *
 * Write an image into a header file using a 3...2...1...0 format per pixel,
 * for 4 bits color (16 colors - well, greys.) MSB first.  At 80 MHz, screen
 * clears execute in 1.075 seconds and images are drawn in 1.531 seconds.
 */

 
 EpdiyHighlevelState hl;
 
 void idf_setup() {
     Wire.begin(39, 40);
 
     epd_init(&DEMO_BOARD, &ED047TC1, EPD_LUT_64K);
     // Set VCOM for boards that allow to set this in software (in mV).
     // This will print an error if unsupported. In this case,
     // set VCOM using the hardware potentiometer and delete this line.
     epd_set_vcom(1560);
 
     hl = epd_hl_init(WAVEFORM);
 
     // Default orientation is EPD_ROT_LANDSCAPE
     epd_set_rotation(EPD_ROT_INVERTED_PORTRAIT);
 
     printf(
         "Dimensions after rotation, width: %d height: %d\n\n", epd_rotated_display_width(),
         epd_rotated_display_height()
     );
 
     // The display bus settings for V7 may be conservative, you can manually
     // override the bus speed to tune for speed, i.e., if you set the PSRAM speed
     // to 120MHz.
     // epd_set_lcd_pixel_clock_MHz(17);
 
     heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
     heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
 }
 
 static inline void checkError(enum EpdDrawError err) {
     if (err != EPD_DRAW_SUCCESS) {
         ESP_LOGE("demo", "draw error: %X", err);
     }
 }
 
 void showGpsInfo(double infoLat, double infoLon)
 {
     const EpdFont* font;
     font = &FiraSans_20;
 
     //initalize EPD
     uint8_t* fb = epd_hl_get_framebuffer(&hl);
     epd_poweron();
     epd_clear();
     int temperature = epd_ambient_temperature();
     epd_poweroff();
     int cursor_x = epd_rotated_display_width() / 2;
     int cursor_y = epd_rotated_display_height() / 2 - 200;
 
     EpdFontProperties font_props = epd_font_properties_default();
     font_props.flags = EPD_DRAW_ALIGN_CENTER;
 
     EpdRect clear_area = {
         .x = 0,
         .y = epd_rotated_display_height() / 2 + 100,
         .width = epd_rotated_display_width(),
         .height = 300,
     };
 
     epd_fill_rect(clear_area, 0xFF, fb);
 
 
     char buffer[50]; // Assicurati che il buffer sia abbastanza grande!
     sprintf(buffer, "GPS lat: %.2f, GPS long: %.2f", infoLat, infoLon);
     const char* strGpsInfo = buffer;
 
     epd_write_string(
         font, strGpsInfo, &cursor_x, &cursor_y, fb, &font_props
     );
     
     epd_poweron();
     checkError(epd_hl_update_screen(&hl, MODE_GL16, temperature));
     epd_poweroff();
     printf("test1");
     delay(5000);
 }