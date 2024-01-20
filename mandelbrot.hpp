
#include "DEV_Config.h"
#include <mpfr.h>

struct UniformnessChoice
{
    mpfr_t m_x;
    mpfr_t m_y;
    double uniformness = 0.0;
};

class MandelbrotSet
{
public:
    void InitMandelbrotSet();
    void Render(UWORD xResolution, UWORD yResolution);
    void SetRender(UBYTE* image);
    UBYTE* GetRender() { return rendered; };
    void ZoomOnInterestingArea();

private:
    bool IsMandelPoint(mpfr_t x, mpfr_t y, int iterations);
    unsigned long long GetUniformnessOfArea(double fW, double fH, int xOffset, int yOffset, int wDiv, int hDiv);
    bool IsAreaUniform(int xOffset, int yOffset, double fW, double fH,  int wDiv, int hDiv, double wStart, double hStart);
    double GetImprovedUniformnessOfArea(double fW, double fH, int xOffset, int yOffset, int wDiv, int hDiv);
    void IncreasePrecision(unsigned long long newPrecision);

    UBYTE* rendered;
    unsigned long long precision = 64;
    mpfr_t w;
    mpfr_t h;
    mpfr_t x;
    mpfr_t y;
    UWORD renderedResX;
    UWORD renderedResY;
};

