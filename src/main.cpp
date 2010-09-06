#include "resource.h"
#include "global.h"
#include "Level.h"
#include "render.h"
#include "IMG.h"

#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>

#include <string>
#include <vector>
#include <stack>
#include <list>
#include <iostream>
#include <fstream>

int Rotate;
int daynight;
int cave;
int exclude;
int slide;
int flip;

using namespace std;

//My changes:
//ListFiles rewrited to posix mode
//gui is destroy [oh god]
//only console
//what the hell is wchar_t??
//by Firemark :)

int CWorld = 0;

int cut = 0;
bool RENDER = false;
bool water = true;

bool ListFiles(string path, vector<string>& files,int &numfiles) {
	stack<string> directories;
	DIR *dir;
	dirent *ent; 
	string temp_str;
	string temp;
	
	directories.push(path);
	files.clear();
	
	cout << "1) Generating tree of files... " << flush;
	while ( !directories.empty() ) {
		path = directories.top();
		directories.pop();
       
		dir = opendir( path.c_str() ); 
	
        if (!dir) {
		cout << "directory does not exist: " << path << endl;
		return false;
	}
	
        while((ent = readdir(dir)) != NULL) 
        { 
		temp_str = ent->d_name;
		
		if ( temp_str != "." && temp_str != ".." ){
			temp = path + "/" + temp_str;
			//cout << temp << " ";
			if (ent->d_type == DT_DIR ){
				directories.push(temp);
			}
			if (ent->d_type == DT_REG ){
				files.push_back(temp);
				numfiles++;
			}
				
		}
        } 
        closedir(dir);
	}
	cout << "Done!" << endl;

	return true;
};

int write_image(settings_t *s, const char *filename, int width, int height, IMG *img, const char *title)
{
   int code = 0;
   FILE *fp;
   png_structp png_ptr;
   png_infop info_ptr;
   png_bytep row;

   //Now we open the file that the image will be written to. There's also a check to make sure the file opened was successful.

   // Open file for writing (binary mode)
   fp = fopen(filename, "wb");
   if (fp == NULL) {
      fprintf(stderr, "Could not open file %s for writing\n", filename);
      code = 1;
      goto finalise;
   }

   //Two libPNG structures are allocated and initialised.

   // * The write structure contains information about how the PNG file will be written (or read).
   // * The info structure contains information about the PNG image that will be written into the actual file. This allow programmes to find out characteristics of the image.

   // Initialize write structure
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      fprintf(stderr, "Could not allocate write struct\n");
      code = 1;
      goto finalise;
   }

   // Initialize info structure
   info_ptr = png_create_info_struct(png_ptr);

   if (info_ptr == NULL) {
      fprintf(stderr, "Could not allocate info struct\n");
      code = 1;
      goto finalise;
   }

   // This is a form of exception handling for C. Basically, after this little block of code, if any libPNG function fails, execution will jump back to the setjmp function with a non-zero value. The if statement is then entered.
   // Within this example code, the jump point is only set once at this point. Therefore, if an 'exception' occurs, it is not possible to determine from which libPNG function it was thrown. However, it is possible to repeat this block before each libPNG function call, defining a new point to jump back to with an appropriate response.
   // Setup Exception handling
   if (setjmp(png_jmpbuf(png_ptr))) {
      fprintf(stderr, "Error during png creation\n");
      code = 1;
      goto finalise;
   }

   //Various meta data for the image is now set, such as the size and the colour depth per channel.

   // A further piece of meta information is also set, an image title. There are various other bits of standard text that can be set, such as an author.

   png_init_io(png_ptr, fp);

   // Write header (8 bit colour depth)
   png_set_IHDR(png_ptr, info_ptr, width, height,
         8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

   // Set title
   if (title != NULL) {
      png_text title_text;
      title_text.compression = PNG_TEXT_COMPRESSION_NONE;
      title_text.key = (char *)"Title";
      title_text.text = (char *)title;
      png_set_text(png_ptr, info_ptr, &title_text, 1);
   }

   png_write_info(png_ptr, info_ptr);

   //Now the image data is written one row at a time. A single row buffer is created which is of the correct format. For each row, the floating-point image data is converted and written into the row buffer.

   // Allocate memory for one row (3 bytes per pixel - RGB)
   row = (png_bytep) malloc(3 * width * sizeof(png_byte));

   // Write image data
   int x, y;
   for (y=0 ; y<height ; y++) {
      for (x=0 ; x<width ; x++) {
    	  Color c = img->d[y*width + x];
    	  row[0 + x*3] = c.r;
    	  row[1 + x*3] = c.g;
    	  row[2 + x*3] = c.b;
      }

      png_write_row(png_ptr, row);
   }

   // End write
   png_write_end(png_ptr, NULL);

   //The last stage is just some cleaning up. This point is jumped to if there has been an error.
finalise:
   if (fp != NULL) fclose(fp);
   if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
   if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
   if (row != NULL) free(row);

   return code;
}

int save_txt(string txtname, int cc, Level &foo)
{
    ofstream values(txtname.c_str());

    if(!values.is_open()) {
    	return 1;
    }

	values << "Map surface is: " << cc * 16 * 16 << " square meters" << endl;
	values << "Block amounts:" << endl;
	values << endl;
	values << "Air: " << foo.count[0] << endl;
	values << "Stone: " << foo.count[1] << endl;
	values << "Grass: " << foo.count[2] << endl;
	values << "Dirt: " << foo.count[3] << endl;
	values << "Snow: " << foo.count[78] << endl;
	values << endl;
	values << "Water: " << foo.count[8] + foo.count[9] << endl;
	values << "Ice: " << foo.count[79] << endl;
	values << "Lava: " << foo.count[10] + foo.count[11] << endl;
	values << endl;
	values << "Obisidian: " << foo.count[49] << endl;
	values << endl;
	values << "Trunk: " << foo.count[17] << endl;
	values << "Leaves: " << foo.count[18] << endl;
	values << "Wood: " << foo.count[5] << endl;
	values << "Cactus: " << foo.count[81] << endl;
	values << endl;
	values << "Sand: " << foo.count[12] << endl;
	values << "Gravel: " << foo.count[13] << endl;
	values << "Clay: " << foo.count[82] << endl;
	values << endl;
	values << "Gold Ore: " << foo.count[14] << endl;
	values << "Iron Ore: " << foo.count[15] << endl;
	values << "Coal Ore: " << foo.count[16] << endl;
	values << "Diamond Ore: " << foo.count[56] << endl;
	values << "Redstone: " << foo.count[73] + foo.count[74] << endl;
	values << endl;
	values << "Cobble: " << foo.count[4] << endl;
	values << "Glass: " << foo.count[20] << endl;
	values << "Cloth: " << foo.count[35] << endl;
	values << "Gold: " << foo.count[41] << endl;
	values << "Iron: " << foo.count[42] << endl;
	values << "Diamond: " << foo.count[57] << endl;
	values << endl;
	values << "Farmland: " << foo.count[60] << endl;
	values << "Crops: " << foo.count[59] << endl;
	values << "Reed: " << foo.count[83] << endl;
	values << "Torch: " << foo.count[50] << endl;
	values << "CraftTable: " << foo.count[58] << endl;
	values << "Chest: " << foo.count[54] << endl;
	values << "Furnace: " << foo.count[61] + foo.count[62] << endl;
	values << "Wooden Doors: " << foo.count[64] / 2 << endl;
	values << "Iron Doors: " << foo.count[71] << endl;
	values << "Signs: " << foo.count[63] + foo.count[68] << endl;
	values << "Ladder: " << foo.count[65] << endl;
	values << "Railtracks: " << foo.count[66] << endl;
	values << "Wooden Stairs: " << foo.count[53] << endl;
	values << "Rock Stairs: " << foo.count[67] << endl;
	values << "Lever: " << foo.count[69] << endl;
	values << "Buttons: " << foo.count[77] << endl;
	values << "Pressure Plates: " << foo.count[70] + foo.count[72] << endl;
	values << "Redstone Powder: " << foo.count[55] << endl;
	values << "Redstone Torches: " << foo.count[75] + foo.count[76] << endl;
	values << endl;
    values.close();
    return 0;
}

void do_work(settings_t *s, string path, string out) {
	string txtname = out + ".txt";
	string pngname = out + ".png";

	cout << "world: " << path << " " << endl;
	cout << "png: " << pngname << " " << endl;
	cout << "txt: " << txtname << " " << endl;
	
    vector<string> files;
	list<render> renderblocks;

	int counter = 0;
	int cc = 0;
	
	if (!ListFiles(path, files,counter)) {
		exit(1);
	}
	
	Level foo;
	
	cout << "2) Unpacking and drawing... " << flush;
        for (vector<string>::iterator it = files.begin(); it != files.end(); it++) {
			 
			 const render * temp = foo.LoadLevelFromFile(s, it->c_str(), s->slide, s->water, cut);
			 if(temp->isgood){
				cc++;
				renderblocks.push_back(*temp);
			 }
			//}

        }
	cout << "Done!" << endl;
	//}
		//int lool2 = renderblocks.size();
	
	int minx = 100000000;
	int miny = 100000000;
	int maxx = -100000000;
	int maxy = -100000000;
	for (list<render>::iterator pit=renderblocks.begin();pit!=renderblocks.end();pit++){
		//std::cout << "  " << it->x << " " << it->y;
		if(abs(pit->x) < 65536 && abs(pit->y) < 65536){
			
			if(pit->x < minx)
			minx = pit->x;
		if(pit->y < miny)
			miny = pit->y;
		
		if(pit->x > maxx)
			maxx = pit->x;
		if(pit->y > maxy)
			maxy = pit->y;
		
		}

	}
	//std::cout << "\n\nMAXIMUM X = " << maxx << " MAXIMUM Y = " << maxy;
	//std::cout << "\n\nMINIMUM X = " << minx << " MINIMUM Y = " << miny;
	int imageheight,imagewidth;
	
	IMG *image;

	if(slide == 70) {
		int cxx = ((((maxx-minx))+(maxy-miny)))*0.5;
		int cyy = (((maxx-minx))+(maxy-miny))*0.5;

		imageheight = (cxx)*32 + 288;
		imagewidth = (cyy)*32 + 288;
	}
	else
	{
		imageheight = (maxx-minx)*16 + 144;
		imagewidth = (maxy-miny)*16 + 144;
	}

	if(slide == 70) {
		image = new IMG(imageheight, imagewidth+128);
	}
	else if(slide == 69) {
		image = new IMG(imageheight, imagewidth);
	}
	else {
		image = new IMG(imageheight, imagewidth);
	}

	renderblocks.sort();
	renderblocks.reverse();
	
	for (list<render>::iterator rit=renderblocks.begin(); rit!=renderblocks.end(); rit++) {
		int xb = rit->x;
		int yb = rit->y;

		int xx = (xb-minx)*16;
		int yy = (yb-miny)*16;

		if(s->slide == 70){
			if(s->flip == 1)
				yy = abs((yb-miny)-(maxy-miny))*16;
			if(Rotate == 0)
				xx = abs((xb-minx)-(maxx-minx))*16;


	
			int temx = xx;
			int temy = yy;
			xx = imageheight/2+(temx)-(temy);
			yy = (temx)+(temy)+128;
		}

		if(slide == 69){
			if(s->flip == 1){
				if(s->Rotate == 0){
					yy = abs((yb-miny)-(maxy-miny))*16;
				} else {
					xx = (yb-miny)*16;
					yy = abs((xb-minx)-(maxx-minx))*16;
				}
			} else {
				if(s->Rotate == 1) {
					xx = (yb-miny)*16;
					yy = (xb-minx)*16;
				}
			}
		}

		if(xx >= 0 && yy >= 0 && xx < 40000 && yy < 40000){
			if(slide == 70){
				for(int xo = 0;xo < 33; xo++){
					for(int yo = 0;yo < 160;yo++){
						int x = xo;
						int y = yo;
						image->SetPixel(x+xx,y+yy-128, Blend(image->GetPixel(x+xx,y+yy-128), rit->Q->GetPixel(x,y), 128));
					}
				}
			} else {
				for(int xo = 0;xo < 16;xo++){
					for(int yo = 0;yo < 16+(slide > 68)*128;yo++){
						int x = xo;
						int y = yo;

						if(slide == 69) {
							image->SetPixel(x+xx,y+yy, Blend(image->GetPixel(x+xx,y+yy), rit->Q->GetPixel(x,y), 128));
						} else if(slide == 70) {
							image->SetPixel(x+xx,y+yy-128, Blend(image->GetPixel(x+xx,y+yy-128), rit->Q->GetPixel(x,y), 128));
						} else {
							image->SetPixel(x+xx, y+yy, rit->Q->GetPixel(x,y));
						}
					}
				}
			}
		}
	}

	cout << "3) Saving image " << pngname << "... " << flush;
	//corona::Image *Output = corona::CreateImage(imageheight, imagewidth, corona::PF_R8G8B8A8,MAP->d);
	//corona::SaveImage(pngname.c_str(), corona::FF_PNG,Output);
	
	if (write_image(s, pngname.c_str(), imagewidth, imageheight, image, "Title stuff") != 0) {
		cout << "failed!" << endl;
		exit(1);
	}

	cout << "done!";

	delete image;

	renderblocks.clear();
	cout << "Done!" << endl;

	cout << "4) Saving txt " << txtname << "... " << flush;

	if (save_txt(txtname, cc, foo) != 0) {
		cout << "failed!" << endl;
		exit(1);
    }

    cout << "done!" << endl;
}

void do_help() {
	cout << "Usage: cart5 <world-directory> <output.png> [options]" << endl;
	cout << "Valid options:" << endl
		<< "W - water; C - cave mode" << endl
		<< "R - rotate; F - flip  " << endl
		<< "D - day; d -day/night; N - night " << endl
		<< "Hc - heightcolor; Hg - heightgray; Or - Ore " << endl
		<< "Ob - oblique; Oa - oblique angle" << endl
		<< "E - exclude " << endl << ":" << flush;
}

int main(int argc, char *argv[]){
	string flag = "";

	if (argc < 3) {
		do_help();
		exit(1);
	}

	string path(argv[1]);
	string out(argv[2]);
	
	cout << "Cartography rewritten for linux by Firemark [pozdrawiam halp]" << std::endl;

	settings_t *settings = new settings_t();
	settings->slide = -1;
	settings->water = 0;
	settings->cave = 0;
	settings->Rotate = 0;
	settings->flip = 0;
	settings->daynight = 0;

	for (int i = 3; i < argc; i++) {
		string opt(argv[i]);
		
		if (opt.compare("W") == 0) {
			settings->water = 1;
		}
	
		else if ( opt.compare("C") == 0) {
			settings->cave = 1;
		}
		else if ( opt.compare("R") == 0) {
			settings->Rotate = 1;
		}
		else if ( opt.compare("F") == 0) {
			settings->flip = 1;
		}
		
		else if ( opt.compare("D") == 0)
			settings->daynight = 0;
		else if ( opt.compare("d") == 0)
			settings->daynight = 1;
		else if ( opt.find("n") == 0)
			settings->daynight = 2;
		else if ( opt.compare("Hc") == 0)
			settings->slide = -3;
		else if ( opt.compare("Hg") == 0)
			settings->slide = -4;
		else if ( opt.compare("Or") == 0)
			settings->slide = -5;
		else if ( opt.compare("Ob") == 0)
			//yeah funny
			// i lolled
			settings->slide = 69;
		else if ( opt.compare("Oa") == 0)
			settings->slide = 70;
		else if ( flag.find("E") != string::npos ) {
			cout << "Write number to exclude [0-128]" << endl << ":" << flush;
			cin >> exclude;
		}
		else {
			cerr << "Unkown option: " << opt << endl;
			exit(1);
		}
	}
	
	slide = 	settings->slide;
	water = 	settings->water;
	cave = 		settings->cave;
	Rotate = 	settings->Rotate;
	flip = 		settings->flip;
	daynight = 	settings->daynight;

	do_work(settings, path, out);
	
	return 0;
};
