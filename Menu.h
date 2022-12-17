#ifndef Menu_H
#define Menu_H
#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"

using namespace daisy;

class Menu
{
    public:
    // variables
    OledDisplay<SSD130x4WireSpi128x64Driver>* display;
    bool colorScheme = true;
    const char* title;
    
    //functions
    void drawMenu(int octave, const char* title);
    void drawPageOne(int octave, const char* title);
    void drawMeter(int activeItem);
}


; // end of class Menu
#endif