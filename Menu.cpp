#include "Menu.h"

void Menu::drawMenu()
{
    display->Fill(false);
    display->SetCursor((display->Width())/3, 0);
    display->WriteString("VCO I", Font_7x10, true);
    display->Update();
}