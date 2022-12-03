#include <stdio.h>
#include <string.h>
#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"
#include "Menu.h"

using namespace daisy;

/** Typedef the OledDisplay to make syntax cleaner below 
 *  This is a 4Wire SPI Transport controlling an 128x64 sized SSDD1306
 * 
 *  There are several other premade test 
*/
using MyOledDisplay = OledDisplay<SSD130x4WireSpi128x64Driver>;

DaisySeed     hw;
MyOledDisplay display;
Menu          menu;

int main(void)
{
    hw.Init();
    /** Configure the Display */
    MyOledDisplay::Config disp_cfg;
    disp_cfg.driver_config.transport_config.pin_config.dc    = hw.GetPin(9);
    disp_cfg.driver_config.transport_config.pin_config.reset = hw.GetPin(30);
    /** And Initialize */
    display.Init(disp_cfg);
    menu.display = &display;
    /* char strbuff[128];
	sprintf(strbuff, "VCO I"); */
    while(1)
    {
        menu.drawMenu();
    }
}