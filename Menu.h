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
    
    //functions
    void drawMenu();
}


; // end of class Menu
#endif