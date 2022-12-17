#include <stdio.h>
#include <string.h>
#include "daisy_seed.h"
#include "daisysp.h"
#include "dev/oled_ssd130x.h"
#include "Menu.h"

using namespace daisy;
using namespace daisysp;

/** Typedef the OledDisplay to make syntax cleaner below 
 *  This is a 4Wire SPI Transport controlling an 128x64 sized SSDD1306
 * 
 *  There are several other premade test 
*/
using MyOledDisplay = OledDisplay<SSD130x4WireSpi128x64Driver>;

DaisySeed     hw;
MyOledDisplay display;
Menu          menu;
Encoder       encoderLeft;
Encoder       encoderRight;
Oscillator    osc1;
Oscillator    osc2;

bool  colorScheme = true;
int   osc1_octave = 1;
int   osc2_octave = 1;
float osc1_freq   = 20.0f;
float osc2_freq   = 40.0f;
bool  activeVoice = true;


void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    float sig;
    for(size_t i = 0; i < size; i += 2)
    {
        sig        = osc1.Process() + osc2.Process();
        out[i]     = sig * 0.5f;
        out[i + 1] = sig * 0.5f;
    }
}

int main(void)
{
    hw.Init();
    /** Configure the Display */
    MyOledDisplay::Config disp_cfg;
    disp_cfg.driver_config.transport_config.pin_config.dc    = hw.GetPin(9);
    disp_cfg.driver_config.transport_config.pin_config.reset = hw.GetPin(30);
    /** And Initialize */
    display.Init(disp_cfg);
    encoderLeft.Init(
        hw.GetPin(19), hw.GetPin(20), hw.GetPin(18), hw.AudioSampleRate());
    encoderRight.Init(
        hw.GetPin(16), hw.GetPin(17), hw.GetPin(15), hw.AudioSampleRate());
    menu.display = &display;
    osc1.Init(hw.AudioSampleRate());
    osc1.SetWaveform(osc1.WAVE_SAW);
    osc1.SetFreq(osc1_freq);
    osc2.Init(hw.AudioSampleRate());
    osc2.SetWaveform(osc2.WAVE_SAW);
    osc2.SetFreq(osc2_freq);
    hw.StartAudio(AudioCallback);
    while(1)
    {
        encoderLeft.Debounce();
        encoderRight.Debounce();
        menu.drawMenu((activeVoice) ? osc1_octave : osc2_octave,
                      (activeVoice) ? "VCO_1" : "VCO_2",
                      2);
        osc1_freq = 40.0f * powf(2.0f, osc1_octave);
        osc1.SetFreq(osc1_freq);
        osc2_freq = 40.0f * powf(2.0f, osc2_octave);
        osc2.SetFreq(osc2_freq);
        if(encoderLeft.RisingEdge())
        {
            menu.colorScheme = !menu.colorScheme;
            activeVoice      = !activeVoice;
        }

        if(activeVoice)
        {
            osc1_octave += encoderRight.Increment();
            if(osc1_octave > 4)
            {
                osc1_octave = 1;
            }
            if(osc1_octave < 1)
            {
                osc1_octave = 4;
            }
        }
        else
        {
            osc2_octave += encoderRight.Increment();
            if(osc2_octave > 4)
            {
                osc2_octave = 1;
            }
            if(osc2_octave < 1)
            {
                osc2_octave = 4;
            }
        }
    }
}