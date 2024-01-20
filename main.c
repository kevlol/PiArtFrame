#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include "EPD_Test.h"   //Examples
#include "EPD_7in5_V2.h"
#include <time.h>
#include <iostream>
#include <chrono>

#include "mandelbrot.hpp"

using namespace std;
using namespace chrono;

static constexpr unsigned long SecondsBetweenImages = 3*60; 
void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_Module_Exit();

    exit(0);
}

int main(void)
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
    
    if(DEV_Module_Init()!=0){
        return -1;
    }

    printf("e-Paper Init...\r\n");
    EPD_7IN5_V2_Init();
    EPD_7IN5_V2_Clear();
    DEV_Delay_ms(500);

    // Compute image size 
    UWORD Imagesize = ((EPD_7IN5_V2_WIDTH % 8 == 0)? (EPD_7IN5_V2_WIDTH / 8 ): (EPD_7IN5_V2_WIDTH / 8 + 1)) * EPD_7IN5_V2_HEIGHT;
    UBYTE* img = NULL;

    // Allocate memory for image
    if((img = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for image memory...\r\n");
        return -1;
    }
    Paint_NewImage(img, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, 0, WHITE);
    Paint_SelectImage(img);

    MandelbrotSet mandelbrot;
    mandelbrot.InitMandelbrotSet();
    mandelbrot.SetRender(img);

    bool isFirstImage = true;
    int count = 0;
    while(true)
    {
        steady_clock::time_point beforeRender = steady_clock::now();
        cout << "Starting render..." << endl;
        mandelbrot.Render(EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT);
        cout << "Render complete!" << endl;
        steady_clock::time_point afterRender = steady_clock::now();
        
        if(isFirstImage)
        {
            isFirstImage = false;
        }
        else
        {
            while(duration_cast<std::chrono::seconds>(afterRender - beforeRender).count() < SecondsBetweenImages)
            {
                sleep(5);
                afterRender = steady_clock::now();
            }
        }
        auto renderDuration = duration_cast<std::chrono::seconds>(afterRender - beforeRender).count();
        cout << "Render time: " << renderDuration << "s" << endl;
        cout << "Drawing image..." << endl;

        EPD_7IN5_V2_Init();
        EPD_7IN5_V2_Clear();
        DEV_Delay_ms(500);
        EPD_7IN5_V2_Display(img);
        EPD_7IN5_V2_Sleep();
        cout << "Draw completed!" << endl;

        mandelbrot.ZoomOnInterestingArea();
        if(count % 50 == 0)
        {
            mandelbrot.InitMandelbrotSet();
        }
        cout << "Iterations: " << count << endl;
        count++;
    }

    return 0;
}
