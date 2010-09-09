#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include <string>
#include <vector>
#include <stack>
#include <list>
#include <queue>
#include <iostream>
#include <fstream>

#include <png.h>

#include "resource.h"
#include "global.h"
#include "Level.h"
#include "Image.h"

int Rotate;
int daynight;
int cave;
int exclude;
int slide;
int flip;
int water;

using namespace std;

//My changes:
//ListFiles rewrited to posix mode
//gui is destroy [oh god]
//only console
//what the hell is wchar_t??
//by Firemark :)

int CWorld = 0;

int cut = 0;

class dirlist {
private:
  queue<string> directories;
  queue<string> files;

public:
  dirlist(string path) {
    directories.push(path);
  }
  
  bool hasnext() {
    if (!files.empty()) {
      return true;
    }

    if (directories.empty()) {
      return false;
    }
    
    // work until you find any files
    while (files.empty() && !directories.empty()) {
      string path = directories.front();
      directories.pop();
      
      DIR *dir = opendir(path.c_str()); 
      
      if (!dir) {
        return false;
      }
      
      dirent *ent; 
    
      while((ent = readdir(dir)) != NULL)
      {
        string temp_str = ent->d_name;

        if (temp_str.compare(".") == 0) {
          continue;
        }
        
        if (temp_str.compare("..") == 0) {
          continue;
        }
        
        if (ent->d_type == DT_DIR) {
          directories.push(path + "/" + temp_str);
        }
        else if (ent->d_type == DT_REG) {
          files.push(path + "/" + temp_str);
        }
      }
      
      closedir(dir);
    }
    
    return !files.empty();
  }

  string next() {
    string next = files.front();
    files.pop();
    return next;
  }
};

int write_image(settings_t *s, const char *filename, Image &img, const char *title)
{
   int code = 0;
   FILE *fp;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
   png_bytep row = NULL;

   fp = fopen(filename, "wb");

   if (fp == NULL) {
      code = 1;
      goto finalise;
   }

   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      code = 1;
      goto finalise;
   }

   info_ptr = png_create_info_struct(png_ptr);

   if (info_ptr == NULL) {
      code = 1;
      goto finalise;
   }

   if (setjmp(png_jmpbuf(png_ptr))) {
      code = 1;
      goto finalise;
   }

   png_init_io(png_ptr, fp);

   png_set_IHDR(png_ptr, info_ptr, img.get_width(), img.get_height(),
         8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
   if (title != NULL) {
      png_text title_text;
      title_text.compression = PNG_TEXT_COMPRESSION_NONE;
      title_text.key = (char *)"Title";
      title_text.text = (char *)title;
      png_set_text(png_ptr, info_ptr, &title_text, 1);
   }

   png_write_info(png_ptr, info_ptr);

   row = (png_bytep) malloc(4 * img.get_width() * sizeof(png_byte));

   int x, y;
   for (y=0 ; y<img.get_width(); y++) {
      for (x=0 ; x<img.get_width(); x++) {
        Color c = img.get_pixel(x, y);
        row[0 + x*4] = c.r;
        row[1 + x*4] = c.g;
        row[2 + x*4] = c.b;
        row[3 + x*4] = 0xff;
      }

      png_write_row(png_ptr, row);
   }
  
   png_write_end(png_ptr, NULL);

finalise:
   if (fp != NULL) {
     fclose(fp);
   }
    
   if (info_ptr != NULL) {
     png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
   }

   if (png_ptr != NULL) {
     png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
   }

   if (row != NULL) {
     free(row);
   }

   return code;
}

int save_txt(string txtname, int cc)
{
//    ofstream values(txtname.c_str());

//    if(!values.is_open()) {
//      return 1;
//    }

//  values << "Map surface is: " << cc * 16 * 16 << " square meters" << endl;
//  values << "Block amounts:" << endl;
//  values << endl;
//  values << "Air: " << foo.count[0] << endl;
//  values << "Stone: " << foo.count[1] << endl;
//  values << "Grass: " << foo.count[2] << endl;
//  values << "Dirt: " << foo.count[3] << endl;
//  values << "Snow: " << foo.count[78] << endl;
//  values << endl;
//  values << "Water: " << foo.count[8] + foo.count[9] << endl;
//  values << "Ice: " << foo.count[79] << endl;
//  values << "Lava: " << foo.count[10] + foo.count[11] << endl;
//  values << endl;
//  values << "Obisidian: " << foo.count[49] << endl;
//  values << endl;
//  values << "Trunk: " << foo.count[17] << endl;
//  values << "Leaves: " << foo.count[18] << endl;
//  values << "Wood: " << foo.count[5] << endl;
//  values << "Cactus: " << foo.count[81] << endl;
//  values << endl;
//  values << "Sand: " << foo.count[12] << endl;
//  values << "Gravel: " << foo.count[13] << endl;
//  values << "Clay: " << foo.count[82] << endl;
//  values << endl;
//  values << "Gold Ore: " << foo.count[14] << endl;
//  values << "Iron Ore: " << foo.count[15] << endl;
//  values << "Coal Ore: " << foo.count[16] << endl;
//  values << "Diamond Ore: " << foo.count[56] << endl;
//  values << "Redstone: " << foo.count[73] + foo.count[74] << endl;
//  values << endl;
//  values << "Cobble: " << foo.count[4] << endl;
//  values << "Glass: " << foo.count[20] << endl;
//  values << "Cloth: " << foo.count[35] << endl;
//  values << "Gold: " << foo.count[41] << endl;
//  values << "Iron: " << foo.count[42] << endl;
//  values << "Diamond: " << foo.count[57] << endl;
//  values << endl;
//  values << "Farmland: " << foo.count[60] << endl;
//  values << "Crops: " << foo.count[59] << endl;
//  values << "Reed: " << foo.count[83] << endl;
//  values << "Torch: " << foo.count[50] << endl;
//  values << "CraftTable: " << foo.count[58] << endl;
//  values << "Chest: " << foo.count[54] << endl;
//  values << "Furnace: " << foo.count[61] + foo.count[62] << endl;
//  values << "Wooden Doors: " << foo.count[64] / 2 << endl;
//  values << "Iron Doors: " << foo.count[71] << endl;
//  values << "Signs: " << foo.count[63] + foo.count[68] << endl;
//  values << "Ladder: " << foo.count[65] << endl;
//  values << "Railtracks: " << foo.count[66] << endl;
//  values << "Wooden Stairs: " << foo.count[53] << endl;
//  values << "Rock Stairs: " << foo.count[67] << endl;
//  values << "Lever: " << foo.count[69] << endl;
//  values << "Buttons: " << foo.count[77] << endl;
//  values << "Pressure Plates: " << foo.count[70] + foo.count[72] << endl;
//  values << "Redstone Powder: " << foo.count[55] << endl;
//  values << "Redstone Torches: " << foo.count[75] + foo.count[76] << endl;
//  values << endl;
//    values.close();
    return 0;
}

void do_work(settings_t *s, string path, string out) {
  string txtname = out + ".txt";
  string pngname = out + ".png";
  
  Image image;
  
  cout << "world: " << path << " " << endl;
  cout << "png: " << pngname << " " << endl;
  cout << "txt: " << txtname << " " << endl;
  
  int cc = 0;
  
  cout << "Unpacking and drawing... " << flush;

  dirlist listing(path);

  Image all(4096, 4096);
  
  int i = 0;
  
  while (listing.hasnext()) {
    string p = listing.next();
    Level level(p.c_str());
    Image partial = level.get_image();
    all.composite(2048 + level.xPos * 16, 2048 + level.zPos * 16, partial);
    
    if (i % 100 == 0) {
      cout << i << " " << flush;
    }
    
    ++i;
  }
  
  cout << "done!" << endl;
  
  cout << "Saving image " << pngname << "... " << flush;
  
  if (write_image(s, pngname.c_str(), all, "Title stuff") != 0) {
    cout << "failed! " << strerror(errno) << endl;
    exit(1);
  }
  
  cout << "done!" << endl;

  cout << "Saving txt " << txtname << "... " << flush;

  if (save_txt(txtname, cc) != 0) {
    cout << "failed!" << endl;
      exit(1);
    }

    cout << "done!" << endl;
}

void do_help() {
  cout << "Usage: cart5 <world-directory> <output> [options]" << endl;
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
      //--yeah funny
      // -- i lolled aswell
      settings->slide = 69;
    else if ( opt.compare("Oa") == 0)
      settings->slide = 70;
    else if ( opt.compare("E") == 0) {
      cout << "Write number to exclude [0-128]" << endl << ":" << flush;
      cin >> exclude;
    }
    else {
      cerr << "Unkown option: " << opt << endl;
      exit(1);
    }
  }
  
  do_work(settings, path, out);
  
  return 0;
};
