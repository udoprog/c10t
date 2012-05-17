#include "altitude_graph.hpp"
#include "text.hpp"

using namespace std;
namespace fs = boost::filesystem;

#define BORDER_X 50
#define BORDER_Y 50

AltitudeGraph::AltitudeGraph(settings_t& _s)
    :  s(_s), width(800), height(600)
{
    altitudeRegistry.reset(new long[mc::MapY]);
    for(int i = 0; i < mc::MapY; i++)
    {
         altitudeRegistry[i] = 0;
    }
}

void AltitudeGraph::createGraph()
{
    image_ptr graphImg;

    memory_image* image = new memory_image(width, height);

    graphImg.reset(image);

    color bgcolor(255, 255, 255, 255);
    color fgcolor(150,0,0,255);
    color axiscolor(0,0,0,255);
    color seacolor(0,0,255,255);

    text::font_face ffsea(s.ttf_path, 8, seacolor);
    ffsea.init();
    text::font_face ff12(s.ttf_path, 12, axiscolor);
    ff12.init();

    // fill background
    graphImg->fill(bgcolor);

    int _w = width - BORDER_X;
    int _h = height - BORDER_Y;

    long maxVal = this->getMax();

    int x_step = _w / mc::MapY;

    std::stringstream maxss;
    maxss << "MAX = " << maxVal;
    ff12.draw(graphImg, "NB of blocks", BORDER_X - 30 , BORDER_Y - 25);
    ff12.draw(graphImg, maxss.str(), BORDER_X - 20 , BORDER_Y - 10);

    ff12.draw(graphImg, "Altitude", _w - 2*BORDER_X , _h + 17);

    int x=0, y=0, x0=BORDER_X, y0=_h;

    for(int i = 0; i < mc::MapY; i++)
    {
       x =  BORDER_X + x_step*i;
       y = _h - (int)( ( (float)altitudeRegistry[i] / (float)maxVal ) * (_h-BORDER_Y) );
       graphImg->draw_line(x, y, x0, y0, fgcolor);
       x0 = x;
       y0 = y;
    }

    // draw axis
    graphImg->draw_line(BORDER_X, BORDER_Y, BORDER_X, _h, axiscolor);
    graphImg->draw_line(BORDER_X, _h, _w, _h, axiscolor);

    // draw axis labels
    for(int i=0; i < mc::MapY; i++)
    {
        color _axiscolor = axiscolor;
        x =  BORDER_X + x_step*i;
        int size = 2;
        if(i == 63)
        {
            _axiscolor = seacolor;
            ffsea.draw(graphImg, "Sea", x+4, _h+25);
            size = 25;
        }
        if(i%10 == 0)
        {
            size = 5;
        }
        graphImg->draw_line(x, _h, x, _h+size, _axiscolor);
    }

    png_format::opt_type opts;
    graphImg->save<png_format>(s.statistics_path.string() + "_graph.png", opts);
}


void AltitudeGraph::registerBloc(Byte value, int altitude)
{
    altitudeRegistry[altitude] += 1;
}

long AltitudeGraph::getMax()
{
    long max = 0;
    for(int i = 0; i < mc::MapY; i++)
    {
        if(max<altitudeRegistry[i])
            max = altitudeRegistry[i];
    }
    return max;
}
