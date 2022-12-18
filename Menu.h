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
    bool                                      colorScheme = true;
    const char*                               title;

    //functions
    void drawMenu(int octave, const char* title, int indexPageOne, int shape);
    void
    drawPageOne(int octave, const char* title, int indexPageOne, int shape);
    void drawMeter(int activeItem);
    void drawWaveSelector(int shape);
    void drawSquareWaveIcon(int x, int y, int size, bool color);
    void drawTriangleWaveIcon(int x, int y, int size, bool color);
    void drawSawWaveIcon(int x, int y, int size, bool color);
    void selector(int index, bool color);
}


; // end of class Menu
#endif