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

DaisySeed                              hw;
MyOledDisplay                          display;
Menu                                   menu;
Encoder                                encoderLeft;
Encoder                                encoderRight;
Oscillator                             osc1;
Oscillator                             osc2;
Switch                                 leftButton;
Switch                                 rightButton;
MidiHandler<MidiUartTransport>         midi;
MidiHandler<MidiUartTransport>::Config midi_cfg;

bool  colorScheme = true;
int   osc1_octave = 1;
int   osc2_octave = 1;
float osc1_freq   = 0;
float osc2_freq   = 0;
float osc1_shape  = 0;
float osc2_shape  = 0;
float osc1_pitch  = 0;
float osc2_pitch  = 0;
bool  activeVoice = true;
int   indexPage1  = 0;


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

void HandleMidiMessage(MidiEvent m)
{
    switch(m.type)
    {
        case NoteOn:
        {
            NoteOnEvent p = m.AsNoteOn();
            if(m.data[1] != 0)
            {
                p = m.AsNoteOn();
                osc1.SetFreq(mtof(p.note + (osc1_octave * 12)));
                osc2.SetFreq(mtof(p.note + (osc2_octave * 12)));
                osc1.SetAmp((1));
                osc2.SetAmp((1));
            }
        }
        break;
        case NoteOff:
        {
            if(m.data[1] != 0)
            {
                osc1.SetAmp(0);
                osc2.SetAmp(0);
            }
        }
        default: break;
    }
}


int main(void)
{
    hw.Init();
    hw.usb_handle.Init(UsbHandle::FS_INTERNAL);
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
    osc1.SetAmp(0);
    osc2.Init(hw.AudioSampleRate());
    osc2.SetWaveform(osc2.WAVE_SAW);
    osc2.SetAmp(0);
    leftButton.Init(hw.GetPin(26), hw.AudioSampleRate());
    rightButton.Init(hw.GetPin(27), hw.AudioSampleRate());
    hw.StartAudio(AudioCallback);
    midi.Init(midi_cfg);
    midi.StartReceive();

    while(1)
    {
        midi.Listen();
        encoderLeft.Debounce();
        encoderRight.Debounce();
        leftButton.Debounce();
        rightButton.Debounce();
        menu.drawMenu((activeVoice) ? osc1_octave : osc2_octave,
                      (activeVoice) ? "VCO_1" : "VCO_2",
                      indexPage1,
                      (activeVoice) ? osc1_shape : osc2_shape,
                      (activeVoice) ? osc1_pitch : osc2_pitch);
        if(encoderLeft.RisingEdge())
        {
            menu.colorScheme = !menu.colorScheme;
            activeVoice      = !activeVoice;
        }

        if(indexPage1 == 0)
        {
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
        if(indexPage1 == 1)
        {
            if(activeVoice)
            {
                osc1_shape += encoderRight.Increment();
                if(osc1_shape > 2)
                {
                    osc1_shape = 0;
                }
                if(osc1_shape < 0)
                {
                    osc1_shape = 2;
                }
                switch((int)osc1_shape)
                {
                    case 0: osc1.SetWaveform(osc1.WAVE_SAW); break;
                    case 1: osc1.SetWaveform(osc1.WAVE_TRI); break;
                    case 2: osc1.SetWaveform(osc1.WAVE_SQUARE); break;
                }
            }
            else
            {
                osc2_shape += encoderRight.Increment();
                if(osc2_shape > 2)
                {
                    osc2_shape = 0;
                }
                if(osc2_shape < 0)
                {
                    osc2_shape = 2;
                }
                switch((int)osc2_shape)
                {
                    case 0: osc2.SetWaveform(osc2.WAVE_SAW); break;
                    case 1: osc2.SetWaveform(osc2.WAVE_TRI); break;
                    case 2: osc2.SetWaveform(osc2.WAVE_SQUARE); break;
                }
            }
        }

        if(indexPage1 == 2)
        {
            if(activeVoice)
            {
                osc1_freq += encoderRight.Increment();
                if(osc1_pitch > 100)
                {
                    osc1_pitch = 100;
                }
                if(osc1_pitch < 0)
                {
                    osc1_pitch = 0;
                }
                osc1_pitch += encoderRight.Increment();
            }
            else
            {
                osc2_freq += encoderRight.Increment();
                if(osc2_pitch > 100)
                {
                    osc2_pitch = 100;
                }
                if(osc2_pitch < 0)
                {
                    osc2_pitch = 0;
                }
                osc2_pitch += encoderRight.Increment();
            }
        }
        //Button
        if(leftButton.RisingEdge())
        {
            if(indexPage1 < 2)
            {
                indexPage1++;
            }
            else
            {
                indexPage1 = 0;
            }
        }
        if(rightButton.RisingEdge())
        {
            if(indexPage1 > 0)
            {
                indexPage1--;
            }
            else
            {
                indexPage1 = 2;
            }
        }
        if(midi.HasEvents())
        {
            HandleMidiMessage(midi.PopEvent());
        }
    }
}