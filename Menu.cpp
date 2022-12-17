#include "Menu.h"


void Menu::drawMenu(int octave, const char* title)
{
    this->drawPageOne(octave, title);
}

void Menu::drawPageOne(int octave, const char* title)
{
    display->Fill(!this->colorScheme);
    display->SetCursor((display->Width()) / 3, 3);
    display->WriteString(title, Font_7x10, this->colorScheme);
    display->SetCursor(3, (this->display->Height()) - 10);
    display->WriteString("Oct", Font_6x8, this->colorScheme);
    this->drawMeter(octave);
    display->Update();
}

void Menu::drawMeter(int activeItem)
{
    for(int i = 1; i < 5; i++)
    {
        if(i == activeItem)
        {
            for(int j = 0; j < 4; j++)
            {
                this->display->DrawCircle(11,
                                          (this->display->Height())
                                              - (12 + (i * 10)),
                                          j,
                                          this->colorScheme);
            }
        }
        else
        {
            this->display->DrawCircle(11,
                                      (this->display->Height())
                                          - (12 + (i * 10)),
                                      3,
                                      this->colorScheme);
        }
    }
};