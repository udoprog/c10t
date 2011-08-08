#include "statistics.hpp"

#define BORDER_X 50
#define BORDER_Y 50

BlocStatistics::BlocStatistics()
    : width(800), height(600)
{
    for(int i = 0; i<128; i++)
    {
         altitudeRegistry[i] = 0;
    }
}

void BlocStatistics::test()
{
    image_ptr graphImg;

    memory_image* image = new memory_image(width, height);

    graphImg.reset(image);

    color bgcolor(255, 255, 255, 255);
    color fgcolor(150,0,0,255);
    color axiscolor(0,0,0,255);

    // fill background
    graphImg->fill(bgcolor);

    int _w = width - BORDER_X;
    int _h = height - BORDER_Y;

    long maxVal = this->getMax();

    int x_step = _w / 128;

    int x=0, y=0, x0=BORDER_X, y0=_h;

    for(int i = 0; i<128; i++)
    {
       x =  BORDER_X + x_step*i;
       y = _h - (int)( ( (float)altitudeRegistry[i] / (float)maxVal ) * (_h-BORDER_Y) );
       graphImg->drawLine(x, y, x0, y0, fgcolor);
       x0 = x;
       y0 = y;
    }

    // draw axis
    graphImg->drawLine(BORDER_X, BORDER_Y, BORDER_X, _h, axiscolor);
    graphImg->drawLine(BORDER_X, _h, _w, _h, axiscolor);

    // draw axis labels
    for(int i=0; i<128; i++)
    {
        color _axiscolor = axiscolor;
        x =  BORDER_X + x_step*i;
        int size = 2;
        if(i == 63)
        {
            _axiscolor = color(0,0,255,255);
            size = 8;
        }
        if(i%10 == 0)
        {
            size = 5;
        }
        graphImg->drawLine(x, _h, x, _h+size, _axiscolor);

    }

    png_format::opt_type opts;
    graphImg->save<png_format>("graph.png", opts);
}


void BlocStatistics::registerBloc(Byte value, int altitude)
{
    altitudeRegistry[altitude] += 1;
}

long BlocStatistics::getMax()
{
    long max = 0;
    for(int i = 0; i<128; i++)
    {
        if(max<altitudeRegistry[i])
            max = altitudeRegistry[i];
    }
    return max;
}
