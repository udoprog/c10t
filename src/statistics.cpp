#include "statistics.hpp"

BlocStatistics::BlocStatistics()
{

}

void BlocStatistics::test()
{
    image_ptr graphImg;

    memory_image* image = new memory_image(300, 300);

    graphImg.reset(image);

    color color(255, 255, 255, 255);
    graphImg->fill(color);

    png_format::opt_type opts;
    opts.comment = "C10T_COMMENT";

    graphImg->save<png_format>("graph.png", opts);
}


void BlocStatistics::registerBloc(Byte value, Byte height)
{

}
