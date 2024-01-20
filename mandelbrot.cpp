#include "mandelbrot.hpp"
#include "GUI_Paint.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <tuple>
#include <random>
#include <iostream>

using namespace std;
void MandelbrotSet::InitMandelbrotSet()
{
    precision = 10;
    mpfr_init2(w, precision);
    mpfr_init2(h, precision);
    mpfr_init2(x, precision);
    mpfr_init2(y, precision);
    mpfr_set_d(w, 4.0, MPFR_RNDN);
    mpfr_set_d(h, 2.0, MPFR_RNDN);
    mpfr_set_d(x, -1.0, MPFR_RNDN);
    mpfr_set_d(y, 0.0, MPFR_RNDN);
    
    rendered = NULL;
    renderedResX = 0;
    renderedResY = 0;
}

void MandelbrotSet::SetRender(UBYTE* image)
{
    rendered = image;
}

void MandelbrotSet::IncreasePrecision(unsigned long long newPrecision)
{
    // Init temporaries with new precision
    mpfr_t nW, nH, nX, nY;
    mpfr_init2(nW, newPrecision);
    mpfr_init2(nH, newPrecision);
    mpfr_init2(nX, newPrecision);
    mpfr_init2(nY, newPrecision);
    mpfr_set(nW, w, MPFR_RNDN);
    mpfr_set(nH, h, MPFR_RNDN);
    mpfr_set(nX, x, MPFR_RNDN);
    mpfr_set(nY, y, MPFR_RNDN);

    // Clear and initialize members
    mpfr_clear(w);
    mpfr_clear(h);
    mpfr_clear(x);
    mpfr_clear(y);
    mpfr_init2(w, newPrecision);
    mpfr_init2(h, newPrecision);
    mpfr_init2(x, newPrecision);
    mpfr_init2(y, newPrecision);  
    mpfr_set(w, nW, MPFR_RNDN);
    mpfr_set(h, nH, MPFR_RNDN);
    mpfr_set(x, nX, MPFR_RNDN);
    mpfr_set(y, nY, MPFR_RNDN);

    precision = newPrecision;
}

void MandelbrotSet::Render(UWORD xResolution, UWORD yResolution)
{
    mpfr_t logW;
    mpfr_init2(logW, precision);
    mpfr_log10(logW, w, MPFR_RNDN);
    mpfr_neg(logW, logW, MPFR_RNDN);
    double dLogW = mpfr_get_d(logW, MPFR_RNDN);


    // Approximation for number of iterations
    int iter = (50 + max(0.0, dLogW) * 100 );
    unsigned long long newPrecision = (8 + max(0.0, dLogW));
    if(newPrecision > precision)
        IncreasePrecision(newPrecision);

    vector<vector<bool>> columns;

    int ctr = 0;
    for(int i = yResolution-1; i>=0; --i)
    {
        vector<bool> rows;
        for(int j = 0; j < xResolution; ++j)
        {
            mpfr_t p_x, p_y, halfW, halfH, two, tmp, tmpX, tmpY;

            mpfr_init2(p_x, precision);
            mpfr_init2(p_y, precision);
            mpfr_init2(tmp, precision);
            mpfr_init2(tmpX, precision);
            mpfr_init2(tmpY, precision);            

            mpfr_init_set(halfW, w, MPFR_RNDN);
            mpfr_init_set(halfH, h, MPFR_RNDN);
            mpfr_init_set_d (two, 2.0, MPFR_RNDN);

            //double p_x = this->x - this->w / 2.0 + (double)j / (double)xResolution * this->w;
            mpfr_div(halfW, halfW, two, MPFR_RNDN);
            mpfr_div(halfH, halfH, two, MPFR_RNDN);

            mpfr_set(tmpX, x, MPFR_RNDN);
            mpfr_sub(tmpX, tmpX, halfW, MPFR_RNDN );

            double jDiv = (double)j / (double)xResolution;
            mpfr_set_d(tmp, jDiv, MPFR_RNDN);
            mpfr_mul(tmp, tmp, w, MPFR_RNDN);
            mpfr_add(p_x, tmpX, tmp, MPFR_RNDN);


            //double p_y = this->y - this->h / 2.0 + (double)(i+1) / (double)yResolution * this->h;
            mpfr_set(tmpY, y, MPFR_RNDN);
            mpfr_sub(tmpY, tmpY, halfH, MPFR_RNDN );

            double iDiv = (double)(i+1) / (double)yResolution;
            mpfr_set_d(tmp, iDiv, MPFR_RNDN);
            mpfr_mul(tmp, tmp, h, MPFR_RNDN);
            mpfr_add(p_y, tmpY, tmp, MPFR_RNDN);

            rows.emplace_back(IsMandelPoint(p_x, p_y, iter));
        }
        columns.emplace_back(rows);
        //if(ctr %100 == 0)
            //cout << "Ctr: " << ctr << endl;
        ctr++;
    }

    renderedResX = xResolution;
    renderedResY = yResolution;

    // Update rendered image
    for(unsigned int y = 0; y < columns.size(); ++y)
    {
        auto row = columns[y];
        for(unsigned int x = 0; x < row.size(); ++x)
        {
            auto bitSet = row[x];
            if(bitSet)
            {
                Paint_SetPixel(x, y, WHITE);
            }
            else
            {
                Paint_SetPixel(x, y, BLACK);
            }
        }
    }
}

bool MandelbrotSet::IsMandelPoint(mpfr_t fX, mpfr_t fY, int iterations)
{   
    mpfr_t z_x, z_y, z_x2, z_y2, z_x_old, two;

    mpfr_init_set(z_x, fX, MPFR_RNDN);
    mpfr_init_set(z_y, fY, MPFR_RNDN);
    mpfr_init2(z_x_old, precision);
    mpfr_init2(z_x2, precision);
    mpfr_init2(z_y2, precision);
    mpfr_init_set_d (two, 2.0, MPFR_RNDN);

    for(int i = 0; i < iterations; ++i)
    {
        mpfr_set(z_x_old, z_x, MPFR_RNDN);
        mpfr_mul(z_x2, z_x, z_x, MPFR_RNDN);
        mpfr_mul(z_y2, z_y, z_y, MPFR_RNDN);

        // z_x = z_x * z_x - z_y * z_y + fX;
        mpfr_sub(z_x, z_x2, z_y2, MPFR_RNDN);
        mpfr_add(z_x, z_x, fX, MPFR_RNDN);

        // z_y = 2.0 * z_x_old * z_y + fY;
        mpfr_mul(z_y, z_y, z_x_old, MPFR_RNDN);
        mpfr_mul(z_y, two, z_y, MPFR_RNDN);
        mpfr_add(z_y, z_y, fY, MPFR_RNDN);

        mpfr_t sumSquared, xSumSqr, ySumSqr;
        mpfr_init2(sumSquared, precision);
        mpfr_init2(xSumSqr, precision);
        mpfr_init2(ySumSqr, precision);

        mpfr_mul(xSumSqr, z_x, z_x, MPFR_RNDN);
        mpfr_mul(ySumSqr, z_y, z_y, MPFR_RNDN);

        mpfr_add(sumSquared, xSumSqr, ySumSqr, MPFR_RNDN);

        if (mpfr_cmp_d(sumSquared, 4.0) > 0)
        {
            return true;
        }
    }
    return false;
}

unsigned long long MandelbrotSet::GetUniformnessOfArea(double fW, double fH, int xOffset, int yOffset, int wDiv, int hDiv)
{
    unsigned long long uniformness = 0;
    for(int wStart = 0; wStart < wDiv; ++wStart)
    {
        for(int hStart = 0; hStart < hDiv; ++hStart)
        {
            if(IsAreaUniform(xOffset, yOffset, fW, fH, wDiv, hDiv, wStart, hStart))
            {
                ++uniformness;
            }
        }
    }

    return uniformness;
}

bool MandelbrotSet::IsAreaUniform(int xOffset, int yOffset, double fW, double fH,  int wDiv, int hDiv, double wStart, double hStart)
{
    int yInit = yOffset + static_cast<int>(fH / hDiv) * hStart;
    int xInit = xOffset + static_cast<int>(fW / wDiv) * wStart;
    auto firstPoint = Paint_GetPixel(xInit , yInit);

    for(unsigned int i = 0; i < static_cast<unsigned int>(fW / wDiv); ++i )
    {
        for(unsigned int j = 0; j < static_cast<unsigned int>(fH / hDiv); ++j )
        {
            int yTest = yOffset + static_cast<int>(fH / hDiv) * hStart + j;
            int xTest = xOffset + static_cast<int>(fW / wDiv) * wStart + i;
            auto testPoint = Paint_GetPixel(xTest , yTest);
            if(testPoint != firstPoint)
                return false;
        }        
    }

    return true;
}


double MandelbrotSet::GetImprovedUniformnessOfArea(double fW, double fH, int xOffset, int yOffset, int wDiv, int hDiv)
{
    unsigned long long numWhite = 0;
    unsigned long long numBlack = 0;
    double totalPixels = fW*fH;

    for(int wStart = 0; wStart < fW; ++wStart)
    {
        for(int hStart = 0; hStart < fH; ++hStart)
        {
            int xPointIndex = xOffset+wStart;
            int yPointIndex = yOffset+hStart;
            if(Paint_GetPixel(xPointIndex , yPointIndex) == WHITE)
                numWhite++;
            else
                numBlack++;
        }
    }

    return max((double)numWhite / totalPixels, double(numBlack) / totalPixels);
}


void MandelbrotSet::ZoomOnInterestingArea()
{   
    UniformnessChoice choice;
    vector<UniformnessChoice> choices;

    mpfr_t four, two, wDiv4, hDiv4, tmpX, tmpY;

    mpfr_init2(choice.m_x, precision);
    mpfr_init2(choice.m_y, precision);
    mpfr_init_set_d(four, 4.0, MPFR_RNDN);
    mpfr_init_set_d(two, 2.0, MPFR_RNDN);
    mpfr_init_set(wDiv4, w, MPFR_RNDN);
    mpfr_init_set(hDiv4, h, MPFR_RNDN);
    mpfr_init2(tmpX, precision);
    mpfr_init2(tmpY, precision);

    mpfr_div(wDiv4, wDiv4, four, MPFR_RNDN);
    mpfr_div(hDiv4, hDiv4, four, MPFR_RNDN);

    auto uniformness = GetImprovedUniformnessOfArea(this->renderedResX / 2, this->renderedResY / 2, 0, 0, 2, 2);

    mpfr_sub(tmpX, x, wDiv4, MPFR_RNDN);
    mpfr_add(tmpY, y, hDiv4, MPFR_RNDN);
    mpfr_set(choice.m_x, tmpX, MPFR_RNDN);
    mpfr_set(choice.m_y, tmpY, MPFR_RNDN);
    choice.uniformness = uniformness;
    choices.emplace_back(choice);

    uniformness = GetImprovedUniformnessOfArea(this->renderedResX / 2, this->renderedResY / 2, this->renderedResX / 2, 0, 2, 2);
    mpfr_add(tmpX, x, wDiv4, MPFR_RNDN);
    mpfr_add(tmpY, y, hDiv4, MPFR_RNDN);
    mpfr_set(choice.m_x, tmpX, MPFR_RNDN);
    mpfr_set(choice.m_y, tmpY, MPFR_RNDN);
    choice.uniformness = uniformness;
    choices.emplace_back(choice);

    uniformness = GetImprovedUniformnessOfArea(this->renderedResX / 2, this->renderedResY / 2, 0, this->renderedResY / 2, 2, 2);
    mpfr_sub(tmpX, x, wDiv4, MPFR_RNDN);
    mpfr_sub(tmpY, y, hDiv4, MPFR_RNDN);
    mpfr_set(choice.m_x, tmpX, MPFR_RNDN);
    mpfr_set(choice.m_y, tmpY, MPFR_RNDN);
    choice.uniformness = uniformness;
    choices.emplace_back(choice);

    uniformness = GetImprovedUniformnessOfArea(this->renderedResX / 2, this->renderedResY / 2, this->renderedResX / 2, this->renderedResY / 2, 2, 2);
    mpfr_add(tmpX, x, wDiv4, MPFR_RNDN);
    mpfr_sub(tmpY, y, hDiv4, MPFR_RNDN);
    mpfr_set(choice.m_x, tmpX, MPFR_RNDN);
    mpfr_set(choice.m_y, tmpY, MPFR_RNDN);
    choice.uniformness = uniformness;
    choices.emplace_back(choice);     

    mpfr_div(w, w, two, MPFR_RNDN);
    mpfr_div(h, h, two, MPFR_RNDN);

    auto lessUniformChoices = choices;
    lessUniformChoices.erase(std::remove_if(
        lessUniformChoices.begin(),
        lessUniformChoices.end(),
        [](const UniformnessChoice& x) { 
            return (x.uniformness >= 0.85); 
        }), lessUniformChoices.end());

    auto topTierChoices = choices;
    topTierChoices.erase(std::remove_if(
        topTierChoices.begin(),
        topTierChoices.end(),
        [](const UniformnessChoice& x) { 
            return (x.uniformness >= 0.75); 
        }), topTierChoices.end());

    // Seed
    random_device rd;
    mt19937 g(rd());

    if(topTierChoices.size() > 0)
    {
            shuffle(topTierChoices.begin(), topTierChoices.end(), g);
            auto selection = topTierChoices[0];
            mpfr_set(x, selection.m_x, MPFR_RNDN);
            mpfr_set(y, selection.m_y, MPFR_RNDN);
    }
    else if (lessUniformChoices.size() > 0)
    {
            shuffle(lessUniformChoices.begin(), lessUniformChoices.end(), g);
            auto selection = lessUniformChoices[0];
            mpfr_set(x, selection.m_x, MPFR_RNDN);
            mpfr_set(y, selection.m_y, MPFR_RNDN);
    } 
    else
    {
            shuffle(choices.begin(), choices.end(), g);
            auto selection = choices[0];
            mpfr_set(x, selection.m_x, MPFR_RNDN);
            mpfr_set(y, selection.m_y, MPFR_RNDN);
    }
}

