#include "Menu.h"


void Menu::drawMenu(int octave, int indexPageOne, int shape, int shapeMod, int pitch)
{
    this->drawPageOne(octave, indexPageOne, shape, shapeMod, pitch);
}

//UI Pages
void Menu::drawPageOne(int octave,
                       int indexPageOne,
                       int shape,
                       int shapeMod,
                       int pitch)
{
    display->Fill(!this->colorScheme);
    display->SetCursor(display->Width() - 40, 7);
    display->WriteString(title, Font_7x10, this->colorScheme);
    display->SetCursor(3, (this->display->Height()) - 12);
    display->WriteString("Oct", Font_6x8, this->colorScheme);
    this->drawMeter(octave);
    display->SetCursor(30, (this->display->Height()) - 12);
    display->WriteString("Wav", Font_6x8, this->colorScheme);
    display->SetCursor(57, (this->display->Height()) - 12);
    display->WriteString("Pch", Font_6x8, this->colorScheme);
    display->SetCursor(82, (this->display->Height()) - 12);
    display->WriteString("Shp", Font_6x8, this->colorScheme);
    this->drawWaveSelector(shape);
    this->drawVSlider(61, 6, 8, 40, pitch, this->colorScheme);
    this->drawVSlider(86, 6, 8, 40, shapeMod, this->colorScheme);
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
        case 2:
            this->display->DrawRect(53,
                                    this->display->Height() - 15,
                                    77,
                                    this->display->Height() - 2,
                                    color);
            break;
        case 3:
            this->display->DrawRect(78,
                                    this->display->Height() - 15,
                                    102,
                                    this->display->Height() - 2,
                                    color);
            break;
    }
}

void Menu::drawVSlider(int  x,
                       int  y,
                       int  width,
                       int  height,
                       int  value,
                       bool color)
{
    this->display->DrawRect(x, y, x + width, y + height, this->colorScheme);
    this->display->DrawLine(x + (width / 2),
                            y + height,
                            x + (width / 2),
                            y + height - (height * value / 100),
                            color);
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
