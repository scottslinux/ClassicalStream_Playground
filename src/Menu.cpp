#include "Menu.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "Tuner.h"
#include <violin200x150.h>
#include <saxophone_135x180.h>
#include <guitar_135x180.h>
#include <microphone_130x173.h>
#include <bpu_40x40.h>
#include <bpd_40x40.h>
#include <bpauseu40x40.h>
#include <bpaused40x40.h>
#include <backd40x40.h>
#include <backu40x40.h>
#include <volu40x40.h>
#include <vol_lessd40x40.h>
#include <favu40x40.h>
#include <radiodial_240x87.h>






//----------------------------------------------------------------
//Define Display via a member initializer
Menu::Menu(Adafruit_ILI9341& tftdisplay, XPT2046_Touchscreen& touchSense):Display(tftdisplay),touchScreen(touchSense)
{   
    //calculate the menu offest in relation to the tft display
    menuoffX=(240-menuWidth)/2; 
    menuoffY=(320-menuHeight)/2;

    screenArt=ArtImage::microphone;
    

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
    currmenucount=choices;

    uint16_t w,h;
    int16_t xpos,ypos;

    uint16_t Violet=Display.color565(64,45,229);
    uint16_t BrightGreen=Display.color565(22,212,20);

    //determine the width of an average string in digital7 font
    Display.setFont(&digital_712pt7b);
    Display.setTextColor(BrightGreen);
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
    currmenucount=choices;

    uint16_t Violet=Display.color565(122,111,220);


    uint16_t w,h;
    int16_t xpos,ypos;

    //determine the width of an average string in digital7 font
    Display.setFont(&digital_712pt7b);
    Display.setTextColor(Violet);
    String spacingstring="MAX STATION SIZE 123";
    Display.getTextBounds(spacingstring,0,0,&xpos,&ypos,&w,&h);
    cellheight=h*1.5;   //⁡⁣⁣⁢𝗮𝗱𝗱 𝗽𝗮𝗱𝗱𝗶𝗻𝗴 𝘁𝗼 𝘁𝗵𝗲 𝗵𝗲𝗶𝗴𝗵𝘁 𝗼𝗳 𝘁𝗵𝗲 𝗰𝗲𝗹𝗹⁡
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
    

}

//------------------------------------------------------------------------------
int Menu::select_from_Menu()
{
    int screenX, screenY;
    int choice=-999; //start with no touch
if(touchScreen.touched())
    {
        TS_Point p;
        p=touchScreen.getPoint();

        int rawX=map(p.y,350,3800,0,239);        //flipping X and Y
        int rawY=map(p.x,3800,300,0,319);
        screenX=constrain(rawX,0,239);           //actual screen coords
        screenY=constrain(rawY,0,319);


        //touch is activated..is the position within the max 10 menu positions
    
        //⁡⁣⁢⁣𝗧𝗿𝗮𝗻𝘀𝗹𝗮𝘁𝗲 𝗽𝗼𝘀𝗶𝘁𝗶𝗼𝗻 𝘁𝗼 𝗺𝗲𝗻𝘂 𝗰𝗵𝗼𝗶𝗰𝗲...𝘁𝗵𝗲 𝗺𝗮𝗴𝗶𝗰!⁡
        if((screenX>menuoffX) && (screenX<menuoffX+menuWidth)
            &&(screenY>menuoffY)&&(screenY<cellheight*10+menuoffY))
        {
            

            //in the choice zone
            choice=(screenY-menuoffY)/cellheight;
            

            if(choice>=currmenucount)
                return(-999);
                else
                    return choice;
        
        }

        //touch was outside of menu. Check for tool activations. Is the touch in that zone

        //for(int x=0;x<5;x++)
          //  Display.drawRect(x*40+5,280,40+5,40,ILI9341_RED);

        if((screenX>=5 && screenX<=225)&&(screenY>=280))    //in the zone
        {
            int tool=(screenX)/45;    //each icon is 40 wide. should be 0 thru 4

            //return 100 +tool to signal this is not a music choice but a tool choice

            return (100+tool);

        }
        
        

    }
        


return -999;


}
//------------------------------------------------------------------------------
void Menu::pulseCircle(bool flag)
{
    if(flag)
      Display.fillCircle(223,25,5,ILI9341_GREEN);
        else
            Display.fillCircle(223,25,5,ILI9341_RED);
           
    
}
//-----------------------------------------------------------------------------
//      Audio Controls
void Menu::draWAudioControls()
{
    

    //Draw UI Controls
    
    
    
    Display.setFont(&FreeMonoBold12pt7b);
    Display.setCursor(27,295);
    Display.setTextColor(ILI9341_BLACK);
    Display.print("X");

    Display.setCursor(58,295);
    Display.print("PLAY");

    Display.setCursor(125,295);
    Display.print("-");
    Display.setCursor(156,295);
    Display.print("+");

    Display.setCursor(210,295);
    Display.print("<");
}
//------------------------------------------------
void Menu::drawFrame()
{
    
    Display.fillScreen(ILI9341_BLACK);
    //Draw UI Frame
    Display.fillRoundRect(0,0,230,280,10,ILI9341_WHITE);
    Display.fillRoundRect(6,6,218,270,10,ILI9341_BLACK);

    draWAudioControls();
    
    Display.drawRGBBitmap(5,280,bpu_40x40,40,40);
    Display.drawRGBBitmap(50,280,vol_lessd40x40,40,40);
    Display.drawRGBBitmap(95,280,volu40x40,40,40);
    Display.drawRGBBitmap(140,280,favu40x40,40,40);
    Display.drawRGBBitmap(185,280,backd40x40,40,40);
    

}
//-----------------------------------------------------
void Menu::drawart(ArtImage currimage)
{
    switch (currimage)
    {
    case ArtImage::microphone:
        Display.drawRGBBitmap(50,100,microphone_130x173,130,173);
        break;
    case ArtImage::guitar:
        Display.drawRGBBitmap(50,100,guitar_135x180,130,180);
        break;
    case ArtImage::saxophone:
        Display.drawRGBBitmap(50,100,saxophone_135x180,135,180);
        break;
    case ArtImage::violin:
        Display.drawRGBBitmap(20,100,violin200x150,200,150);
        break;

    default:
        break;
    }
}   
//---------------------------------------------------------
void Menu::drawart(int currimage)
{
    currGenre=currimage;    //register this for use later

    switch (currimage)
    {
    
    case 0:
        Display.drawRGBBitmap(50,50,guitar_135x180,135,180);
        break;
    case 1:
        Display.drawRGBBitmap(50,50,microphone_130x173,130,173);
        break;
    case 2:
        Display.drawRGBBitmap(20,50,violin200x150,200,150);
        break;
        
    case 3:
        Display.drawRGBBitmap(50,50,saxophone_135x180,135,180);
        break;
    
        case 4:
        Display.drawRGBBitmap(0,100,radiodial,240,87);
    

    default:
        break;
    }
}   
//------------------------------------------------------------
void Menu::clearMenu()
{
    Display.fillScreen(ILI9341_BLACK);


}


