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

const int voice_number = 32;

struct voice
{
    Oscillator osc;
    int        mNoteNumber;
    int        mVelocity;
    bool       isActive;
};

DaisySeed                              hw;
MyOledDisplay                          display;
Menu                                   menu;
Encoder                                encoderLeft;
Encoder                                encoderRight;
voice                                  voice1[voice_number];
voice                                  voice2[voice_number];
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
float sig         = 0;

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    for(size_t i = 0; i < size; i += 2)
    {
        sig = 0;

        for(int j = 0; j < voice_number; j++)
        {
            if(voice1[j].isActive)
            {
                sig += voice1[j].osc.Process();
            }
            if(voice2[j].isActive)
            {
                sig += voice2[j].osc.Process();
            }
        }

        //cout how many active voices
        int activeVoices = 0;
        for(int j = 0; j < voice_number; j++)
        {
            if(voice1[j].isActive)
            {
                activeVoices++;
            }
            if(voice2[j].isActive)
            {
                activeVoices++;
            }
        }

        out[i]     = sig / 10;
        out[i + 1] = sig / 10;
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
                for(int i = 0; i < voice_number; i++)
                {
                    if(!voice1[i].isActive)
                    {
                        voice1[i].osc.SetFreq(mtof(p.note + (osc1_octave * 12))
                                              + osc1_pitch);
                        voice1[i].osc.SetAmp(p.velocity / 127.0f);
                        voice1[i].mNoteNumber = p.note;
                        voice1[i].mVelocity   = p.velocity;
                        voice1[i].isActive    = true;
                        break;
                    }
                }

                for(int i = 0; i < voice_number; i++)
                {
                    if(!voice2[i].isActive)
                    {
                        voice2[i].osc.SetFreq(mtof(p.note + (osc2_octave * 12))
                                              + osc2_pitch);
                        voice2[i].osc.SetAmp(p.velocity / 127.0f);
                        voice2[i].mNoteNumber = p.note;
                        voice2[i].mVelocity   = p.velocity;
                        voice2[i].isActive    = true;
                        break;
                    }
                }
            }
        }
        break;
        case NoteOff:
        {
            if(m.data[1] != 0)
            {
                NoteOffEvent p = m.AsNoteOff();
                for(int i = 0; i < voice_number; i++)
                {
                    if(voice1[i].mNoteNumber == p.note)
                    {
                        voice1[i].osc.SetAmp(0);
                        voice1[i].mNoteNumber = 0;
                        voice1[i].mVelocity   = 0;
                        voice1[i].isActive    = false;
                        break;
                    }
                }

                for(int i = 0; i < voice_number; i++)
                {
                    if(voice2[i].mNoteNumber == p.note)
                    {
                        voice2[i].osc.SetAmp(0);
                        voice2[i].mNoteNumber = 0;
                        voice2[i].mVelocity   = 0;
                        voice2[i].isActive    = false;
                        break;
                    }
                }
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
    for(int i = 0; i < voice_number; i++)
    {
        voice1[i].osc.Init(hw.AudioSampleRate());
        voice2[i].osc.Init(hw.AudioSampleRate());
        voice1[i].osc.SetWaveform(Oscillator::WAVE_SAW);
        voice2[i].osc.SetWaveform(Oscillator::WAVE_SAW);
        voice1[i].osc.SetFreq(0);
        voice2[i].osc.SetFreq(0);
        voice1[i].osc.SetAmp(0);
        voice2[i].osc.SetAmp(0);
        voice1[i].mNoteNumber = 0;
        voice2[i].mNoteNumber = 0;
        voice1[i].mVelocity   = 0;
        voice2[i].mVelocity   = 0;
        voice1[i].isActive    = false;
        voice2[i].isActive    = false;
    }
    leftButton.Init(hw.GetPin(26), hw.AudioSampleRate());
    rightButton.Init(hw.GetPin(27), hw.AudioSampleRate());
    hw.StartAudio(AudioCallback);
    midi.Init(midi_cfg);
    midi.StartReceive();

    while(1)
    {
        midi.Listen();
        while(midi.HasEvents())
        {
            HandleMidiMessage(midi.PopEvent());
        }


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
                    case 0:
                        for(int i = 0; i < voice_number; i++)
                        {
                            voice1[i].osc.SetWaveform(voice1[i].osc.WAVE_SAW);
                        }
                        break;
                    case 1:
                        for(int i = 0; i < voice_number; i++)
                        {
                            voice1[i].osc.SetWaveform(voice1[i].osc.WAVE_TRI);
                        }
                        break;
                    case 2:
                        for(int i = 0; i < voice_number; i++)
                        {
                            voice1[i].osc.SetWaveform(
                                voice1[i].osc.WAVE_SQUARE);
                        }
                        break;
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
                    case 0:
                        for(int i = 0; i < voice_number; i++)
                        {
                            voice2[i].osc.SetWaveform(voice2[i].osc.WAVE_SAW);
                        }
                        break;
                    case 1:
                        for(int i = 0; i < voice_number; i++)
                        {
                            voice2[i].osc.SetWaveform(voice2[i].osc.WAVE_TRI);
                        }
                        break;
                    case 2:
                        for(int i = 0; i < voice_number; i++)
                        {
                            voice2[i].osc.SetWaveform(
                                voice2[i].osc.WAVE_SQUARE);
                        }
                        break;
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
    }
}