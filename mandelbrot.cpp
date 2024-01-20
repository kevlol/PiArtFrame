#include "mandelbrot.hpp"
#include "GUI_Paint.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <tuple>
#include <random>

using namespace std;
void MandelbrotSet::InitMandelbrotSet()
{
    w = 4;
    h = 2;
    x = -1;
    y = 0;
    rendered = NULL;
    renderedResX = 0;
    renderedResY = 0;
}

void MandelbrotSet::SetRender(UBYTE* image)
{
    rendered = image;
}

void MandelbrotSet::Render(UWORD xResolution, UWORD yResolution)
{
    // Approximation for number of iterations
    int iter = (50 + max((long double)0.0, -log10(w)) * 100 );
    vector<vector<bool>> columns;

    for(int i = yResolution-1; i>=0; --i)
    {
        vector<bool> rows;
        for(int j = 0; j < xResolution; ++j)
        {
            long double p_x = this->x - this->w / 2.0 + (long double)j / (long double)xResolution * this->w;
            long double p_y = this->y - this->h / 2.0 + (long double)(i+1) / (long double)yResolution * this->h;
            rows.emplace_back(IsMandelPoint(p_x, p_y, iter));
        }
        columns.emplace_back(rows);
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

bool MandelbrotSet::IsMandelPoint(long double fX, long double fY, int iterations)
{
    long double z_x = fX;
    long double z_y = fY;

    for(int i = 0; i < iterations; ++i)
    {
        long double z_x_old = z_x;
        z_x = z_x * z_x - z_y * z_y + fX;
        z_y = 2.0 * z_x_old * z_y + fY;
        auto sumSquared = pow(z_x, 2) + pow(z_y, 2);
        if (sumSquared > 4)
        {
            return true;
        }
    }
    return false;
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
    tuple<long double, long double, long double> choice;
    vector<tuple<long double, long double, long double>> choices;

    auto uniformness = GetImprovedUniformnessOfArea(this->renderedResX / 2, this->renderedResY / 2, 0, 0, 2, 2);
    choice = {this->x - this->w/4, this->y + this->h/4, uniformness};
    choices.emplace_back(choice);

    uniformness = GetImprovedUniformnessOfArea(this->renderedResX / 2, this->renderedResY / 2, this->renderedResX / 2, 0, 2, 2);
    choice = {this->x + this->w/4, this->y + this->h/4, uniformness};
    choices.emplace_back(choice);

    uniformness = GetImprovedUniformnessOfArea(this->renderedResX / 2, this->renderedResY / 2, 0, this->renderedResY / 2, 2, 2);
    choice = {this->x - this->w/4, this->y - this->h/4, uniformness};
    choices.emplace_back(choice);

    uniformness = GetImprovedUniformnessOfArea(this->renderedResX / 2, this->renderedResY / 2, this->renderedResX / 2, this->renderedResY / 2, 2, 2);
    choice = {this->x + this->w/4, this->y - this->h/4, uniformness};
    choices.emplace_back(choice);     

    w = w / 2.0;
    h = h / 2.0;

    auto lessUniformChoices = choices;
    lessUniformChoices.erase(std::remove_if(
        lessUniformChoices.begin(),
        lessUniformChoices.end(),
        [](const tuple<long double, long double, long double>& x) { 
            return (std::get<2>(x) >= 0.85); 
        }), lessUniformChoices.end());

    auto topTierChoices = choices;
    topTierChoices.erase(std::remove_if(
        topTierChoices.begin(),
        topTierChoices.end(),
        [](const tuple<long double, long double, long double>& x) { 
            return (std::get<2>(x) >= 0.75); 
        }), topTierChoices.end());

    // Seed
    random_device rd;
    mt19937 g(rd());

    if(topTierChoices.size() > 0)
    {
            shuffle(topTierChoices.begin(), topTierChoices.end(), g);
            auto selection = topTierChoices[0];
            this->x = get<0>(selection);
            this->y = get<1>(selection);
    }
    else if (lessUniformChoices.size() > 0)
    {
            shuffle(lessUniformChoices.begin(), lessUniformChoices.end(), g);
            auto selection = lessUniformChoices[0];
            this->x = get<0>(selection);
            this->y = get<1>(selection);
    } 
    else
    {
            shuffle(choices.begin(), choices.end(), g);
            auto selection = choices[0];
            this->x = get<0>(selection);
            this->y = get<1>(selection);
    }
}

