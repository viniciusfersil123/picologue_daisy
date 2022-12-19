#include "Menu.h"


void Menu::drawMenu(int         octave,
                    const char* title,
                    int         indexPageOne,
                    int         shape,
                    int         pitch)
{
    this->drawPageOne(octave, title, indexPageOne, shape, pitch);
}

//UI Pages
void Menu::drawPageOne(int         octave,
                       const char* title,
                       int         indexPageOne,
                       int         shape,
                       int         pitch)
{
    display->Fill(!this->colorScheme);
    display->SetCursor(display->Width() - 40, 7);
    display->WriteString(title, Font_7x10, this->colorScheme);
    display->SetCursor(3, (this->display->Height()) - 12);
    display->WriteString("Oct", Font_6x8, this->colorScheme);
    this->drawMeter(octave);
    display->SetCursor(27, (this->display->Height()) - 12);
    display->WriteString("Wave", Font_6x8, this->colorScheme);
    this->drawWaveSelector(shape);
    display->SetCursor(64, 20);
    display->WriteString("Pitch", Font_6x8, this->colorScheme);
    this->drawHSlider(64, 30, 60, 8, pitch, this->colorScheme);
    this->selector(indexPageOne, this->colorScheme);
    display->Update();
}


//UI elements

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

void Menu::drawWaveSelector(int shape)
{
    int x    = 34;
    int y    = (this->display->Height()) - 26;
    int size = 8;
    switch(shape)
    {
        case 0:
            this->display->DrawRect(x - size / 2,
                                    y,
                                    x + size + (size / 2),
                                    y + size,
                                    this->colorScheme,
                                    true);
            this->drawSawWaveIcon(x, y, size, !this->colorScheme);
            this->drawTriangleWaveIcon(x, y - 15, size, this->colorScheme);
            this->drawSquareWaveIcon(x, y - 30, size, this->colorScheme);
            break;
        case 1:
            this->display->DrawRect(x - size / 2,
                                    y - 15,
                                    x + size + (size / 2),
                                    y + size - 15,
                                    this->colorScheme,
                                    true);
            this->drawSawWaveIcon(x, y, size, this->colorScheme);
            this->drawTriangleWaveIcon(x, y - 15, size, !this->colorScheme);
            this->drawSquareWaveIcon(x, y - 30, size, this->colorScheme);
            break;
        case 2:
            this->display->DrawRect(x - size / 2,
                                    y - 30,
                                    x + size + (size / 2),
                                    y + size - 30,
                                    this->colorScheme,
                                    true);
            this->drawSawWaveIcon(x, y, size, this->colorScheme);
            this->drawTriangleWaveIcon(x, y - 15, size, this->colorScheme);
            this->drawSquareWaveIcon(x, y - 30, size, !this->colorScheme);
            break;
    }
}

void Menu::selector(int index, bool color)
{
    switch(index)
    {
        case 0:
            this->display->DrawRect(0,
                                    this->display->Height() - 15,
                                    23,
                                    this->display->Height() - 2,
                                    color);
            break;
        case 1:
            this->display->DrawRect(24,
                                    this->display->Height() - 15,
                                    52,
                                    this->display->Height() - 2,
                                    color);
            break;
        case 2: this->display->DrawRect(62, 16, 96, 28, color, false); break;
    }
}

void Menu::drawHSlider(int  x,
                       int  y,
                       int  width,
                       int  height,
                       int  value,
                       bool color)
{
    this->display->DrawRect(x, y, x + width, y + height, this->colorScheme);
    this->display->DrawRect(
        x + 1, y + 1, x + value, y + height - 1, color, true);
}

//UI icons

void Menu::drawSquareWaveIcon(int x, int y, int size, bool color)
{
    this->display->DrawLine(x, y, x, y + size, color);
    this->display->DrawLine(
        x + (size / 2), y + size, x + size, y + size, color);
    this->display->DrawLine(x + size, y + size, x + size, y, color);
    this->display->DrawLine(x + (size / 2), y, x, y, color);
    this->display->DrawLine(x + (size / 2), y, x + (size / 2), y + size, color);
}

void Menu::drawTriangleWaveIcon(int x, int y, int size, bool color)
{
    this->display->DrawLine(x, y + size, x + (size / 2), y, color);
    this->display->DrawLine(x + (size / 2), y, x + size, y + size, color);
}

void Menu::drawSawWaveIcon(int x, int y, int size, bool color)
{
    this->display->DrawLine(x, y, x, y + size, color);
    this->display->DrawLine(x + size, y, x + size, y + size, color);
    this->display->DrawLine(x, y + size, x + size, y, color);
}
