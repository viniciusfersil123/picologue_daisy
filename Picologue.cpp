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
Adsr                                   adsr[voice_number];
Switch                                 leftButton;
Switch                                 rightButton;
MidiHandler<MidiUartTransport>         midi;
MidiHandler<MidiUartTransport>::Config midi_cfg;

bool  colorScheme    = true;
int   osc1_octave    = 1;
int   osc2_octave    = 1;
float osc1_freq      = 0;
float osc2_freq      = 0;
float osc1_shape     = 0;
float osc2_shape     = 0;
float osc1_shape_mod = 0;
float osc2_shape_mod = 0;
float osc1_pitch     = 0;
float osc2_pitch     = 0;
float osc1_amp       = 0;
float osc2_amp       = 0;
bool  activeVoice    = true;
int   indexPage1     = 0;
int   indexPage2     = 0;
float sig            = 0;
int   currentPage    = 0;
int   attack         = 0;

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    float env_out;

    for(size_t i = 0; i < size; i += 2)
    {
        sig = 0;


        for(int j = 0; j < voice_number; j++)
        {
            env_out = adsr[j].Process(voice1[j].isActive);


            if(adsr[j].IsRunning())
            {
                if(osc1_shape == 2)
                {
                    voice1[j].osc.SetPw(osc1_shape_mod / 100);
                }
                voice1[j].osc.SetAmp(env_out);
                sig += voice1[j].osc.Process() * osc1_amp / 100;


                if(osc2_shape == 2)
                {
                    voice2[j].osc.SetPw(osc2_shape_mod / 100);
                }
                voice2[j].osc.SetAmp(env_out);
                sig += voice2[j].osc.Process() * osc2_amp / 100;
            }

            /*        if(voice1[j].osc.IsEOC())
                {
                    voice2[j].osc.Reset();
                } */
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
                    if(!adsr[i].IsRunning() && !voice1[i].isActive)
                    {
                        voice1[i].osc.SetFreq(mtof(p.note + (osc1_octave * 12))
                                              + osc1_pitch);
                        voice1[i].isActive    = true;
                        voice1[i].mNoteNumber = p.note;
                        break;
                    }
                }

                for(int i = 0; i < voice_number; i++)
                {
                    if(!adsr[i].IsRunning() && !voice2[i].isActive)
                    {
                        voice2[i].osc.SetFreq(mtof(p.note + (osc2_octave * 12))
                                              + osc2_pitch);
                        voice2[i].mNoteNumber = p.note;
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
                        voice1[i].mNoteNumber = 0;
                        voice1[i].isActive    = false;
                        break;
                    }
                }

                for(int i = 0; i < voice_number; i++)
                {
                    if(voice2[i].mNoteNumber == p.note)
                    {
                        voice2[i].mNoteNumber = 0;
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
        voice1[i].osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
        voice2[i].osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
        voice1[i].osc.SetFreq(0);
        voice2[i].osc.SetFreq(0);
        voice1[i].osc.SetAmp(0.5);
        voice2[i].osc.SetAmp(0.5);
        voice1[i].mNoteNumber = 0;
        voice2[i].mNoteNumber = 0;
        voice1[i].mVelocity   = 0;
        voice2[i].mVelocity   = 0;
        voice1[i].isActive    = false;
        voice2[i].isActive    = false;
        voice1[i].osc.SetPw(0.5);
        voice2[i].osc.SetPw(0.5);
        adsr[i].Init(hw.AudioSampleRate());
        adsr[i].Init(hw.AudioSampleRate());
        adsr[i].SetAttackTime(0.2);
        adsr[i].SetDecayTime(0.2);
        adsr[i].SetSustainLevel(0.8);
        adsr[i].SetReleaseTime(0.2);
    }
    osc1_amp = 50;
    osc2_amp = 50;
    osc1_shape_mod = 50;
    osc2_shape_mod = 50;
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
                      indexPage1,
                      (activeVoice) ? osc1_shape : osc2_shape,
                      (activeVoice) ? osc1_shape_mod : osc2_shape_mod,
                      (activeVoice) ? osc1_amp : osc2_amp,
                      (activeVoice) ? osc1_pitch : osc2_pitch,
                      currentPage,
                      attack);
        if(encoderLeft.RisingEdge())
        {
            menu.colorScheme = !menu.colorScheme;
            activeVoice      = !activeVoice;
        }
        if(currentPage < 0)
        {
            currentPage = 0;
        }
        else if(currentPage > 1)
        {
            currentPage = 1;
        }
        else
            currentPage += encoderLeft.Increment();
        if(currentPage == 0)
        {
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
                                voice1[i].osc.SetWaveform(
                                    voice1[i].osc.WAVE_POLYBLEP_SAW);
                            }
                            break;
                        case 1:
                            for(int i = 0; i < voice_number; i++)
                            {
                                voice1[i].osc.SetWaveform(
                                    voice1[i].osc.WAVE_TRI);
                            }
                            break;
                        case 2:
                            for(int i = 0; i < voice_number; i++)
                            {
                                voice1[i].osc.SetWaveform(
                                    voice1[i].osc.WAVE_POLYBLEP_SQUARE);
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
                                voice2[i].osc.SetWaveform(
                                    voice2[i].osc.WAVE_POLYBLEP_SAW);
                            }
                            break;
                        case 1:
                            for(int i = 0; i < voice_number; i++)
                            {
                                voice2[i].osc.SetWaveform(
                                    voice2[i].osc.WAVE_TRI);
                            }
                            break;
                        case 2:
                            for(int i = 0; i < voice_number; i++)
                            {
                                voice2[i].osc.SetWaveform(
                                    voice2[i].osc.WAVE_POLYBLEP_SQUARE);
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

            if(indexPage1 == 3)
            {
                if(activeVoice)
                {
                    osc1_shape_mod += encoderRight.Increment();
                    if(osc1_shape_mod > 100)
                    {
                        osc1_shape_mod = 100;
                    }
                    if(osc1_shape_mod < 0)
                    {
                        osc1_shape_mod = 0;
                    }
                }
                else
                {
                    osc2_shape_mod += encoderRight.Increment();
                    if(osc2_shape_mod > 100)
                    {
                        osc2_shape_mod = 100;
                    }
                    if(osc2_shape_mod < 0)
                    {
                        osc2_shape_mod = 0;
                    }
                }
            }

            if(indexPage1 == 4)
            {
                if(activeVoice)
                {
                    osc1_amp += encoderRight.Increment();
                    if(osc1_amp > 100)
                    {
                        osc1_amp = 100;
                    }
                    if(osc1_amp < 0)
                    {
                        osc1_amp = 0;
                    }
                }
                else
                {
                    osc2_amp += encoderRight.Increment();
                    if(osc2_amp > 100)
                    {
                        osc2_amp = 100;
                    }
                    if(osc2_amp < 0)
                    {
                        osc2_amp = 0;
                    }
                }
            }
        }

        if(currentPage == 1)
        {
            if(indexPage2 == 0)
            {
                attack += encoderRight.Increment();

                if(attack > 128 / 4)
                {
                    attack = 128 / 4;
                }
                if(attack < 0)
                {
                    attack = 0;
                }
                for(int i = 0; i < voice_number; i++)
                {
                    adsr[i].SetAttackTime(attack / 6 + 0.1);
                }
            }
        }

        //Button
        if(leftButton.RisingEdge())
        {
            if(indexPage1 < 4)
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
                indexPage1 = 4;
            }
        }
    }
}