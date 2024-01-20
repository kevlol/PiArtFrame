
#include "DEV_Config.h"

class MandelbrotSet
{
public:
    void InitMandelbrotSet();
    void Render(UWORD xResolution, UWORD yResolution);
    void SetRender(UBYTE* image);
    UBYTE* GetRender() { return rendered; };
    void ZoomOnInterestingArea();

private:
    bool IsMandelPoint(long double x, long double y, int iterations);
    double GetImprovedUniformnessOfArea(double fW, double fH, int xOffset, int yOffset, int wDiv, int hDiv);

    UBYTE* rendered;
    long double w;
    long double h;
    long double x;
    long double y;
    UWORD renderedResX;
    UWORD renderedResY;
};

