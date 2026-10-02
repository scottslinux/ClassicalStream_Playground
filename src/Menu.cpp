#include "Menu.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "Tuner.h"




//----------------------------------------------------------------
//Define Display via a member initializer
Menu::Menu(Adafruit_ILI9341& tftdisplay, XPT2046_Touchscreen& touchSense):Display(tftdisplay),touchScreen(touchSense)
{   
    //calculate the menu offest in relation to the tft display
    menuoffX=(240-menuWidth)/2; 
    menuoffY=(320-menuHeight)/2;
    

}
//----------------------------------------------------------------
Menu::~Menu()
{

}
//----------------------------------------------------------------
//  ⁡⁣⁢⁣𝗦𝗶𝗺𝗽𝗹𝗲 𝗠𝗲𝗻𝘂 𝗗𝗿𝗮𝘄 𝗳𝗿𝗼𝗺 𝗹𝗶𝘀𝘁 𝗼𝗳 𝘀𝘁𝗿𝗶𝗻𝗴𝘀 (𝘁𝗮𝗴𝘀 𝗮𝗻𝗱 𝗼𝘁𝗵𝗲𝗿 𝗰𝗵𝗼𝗶𝗰𝗲𝘀)⁡

void Menu::drawMenu(const vector<String>& items)
{
    choices=items.size();  //define table size
    choices=constrain(choices,0,10);    //limit it to 10 lines- may scroll later
    cols=1;

    uint16_t w,h;
    int16_t xpos,ypos;

    //determine the width of an average string in digital7 font
    Display.setFont(&digital_712pt7b);
    String spacingstring="MAX STATION SIZE 123";
    Display.getTextBounds(spacingstring,0,0,&xpos,&ypos,&w,&h);
    cellheight=h*1.5;   //add padding to the height of the cell
    int maxpixels=w;

    //draw a grid and menu items
    for(int index=0;index<choices;index++)
    {
        //Display.drawRect(menuoffX,menuoffY+index*cellheight,menuWidth,cellheight,ILI9341_GREEN);
        Display.getTextBounds(items[index],0,0,&xpos,&ypos,&w,&h);

        //index+1 prints the text on the lower border. Y pos indicates the floor
        
        

        if(items[index].length()<maxchars)
        {
            Display.setCursor(menuWidth/2-w/2+menuoffX,cellheight*(index+1)+menuoffY-3);
            Display.print(items[index].c_str());
        }
                else{
                        Display.setCursor(menuWidth/2-maxpixels/2+menuoffX,cellheight*(index+1)+menuoffY-3);
                        Display.print(items[index].substring(0,maxchars).c_str());
                }

        
        
    }

}
//----------------------------------------------------------------
//      ⁡⁣𝗢𝘃𝗲𝗿𝗹𝗼𝗮𝗱𝗲𝗱 𝗱𝗿𝗮𝘄𝗠𝗲𝗻𝘂 𝗠𝗲𝘁𝗵𝗼𝗱 𝘁𝗼 𝗔𝗰𝗰𝗲𝗽𝘁 𝗦𝘁𝗮𝘁𝗶𝗼𝗻𝗶𝗻𝗳𝗼 𝗦𝘁𝗿𝘂𝗰𝘁⁡

void Menu::drawMenu(const vector<Stationinfo>& items)
{
    choices=items.size();  //define table size
    choices=constrain(choices,0,10);    //limit it to 10 lines- may scroll later
    cols=1;

    uint16_t w,h;
    int16_t xpos,ypos;

    //determine the width of an average string in digital7 font
    Display.setFont(&digital_712pt7b);
    String spacingstring="MAX STATION SIZE 123";
    Display.getTextBounds(spacingstring,0,0,&xpos,&ypos,&w,&h);
    cellheight=h*1.5;   //add padding to the height of the cell
    int maxpixels=w;

    //draw a grid and menu items
    for(int index=0;index<choices;index++)
    {
        //Display.drawRect(menuoffX,menuoffY+index*cellheight,menuWidth,cellheight,ILI9341_GREEN);
        Display.getTextBounds(items[index].Name,0,0,&xpos,&ypos,&w,&h);

        //index+1 prints the text on the lower border. Y pos indicates the floor
        
        

        if(items[index].Name.length()<maxchars)
        {
            Display.setCursor(menuWidth/2-w/2+menuoffX,cellheight*(index+1)+menuoffY-3);
            Display.print(items[index].Name.c_str());
        }
                else{
                        Display.setCursor(menuWidth/2-maxpixels/2+menuoffX,cellheight*(index+1)+menuoffY-3);
                        Display.print(items[index].Name.substring(0,maxchars).c_str());
                }

        
        
    }
    int x=select_from_Menu();

}

//------------------------------------------------------------------------------
int Menu::select_from_Menu()
{


if(touchScreen.touched())
    {
        TS_Point p;
        p=touchScreen.getPoint();

        int screenX=map(p.y,350,3800,0,239);        //flipping X and Y
        int screenY=map(p.x,3800,300,0,319);
        screenX=constrain(screenX,0,239);
        screenY=constrain(screenY,0,319);

        Display.fillCircle(screenX,screenY,2,ILI9341_CYAN);

     
    }

    return 0;
}

