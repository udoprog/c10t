#include "Level.h"
#include "resource.h"

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>

#include <string>
#include <vector>
#include <stack>
#include <list>
#include <iostream>
#include <fstream>
#include "global.h"

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

void generate_png(string path, string out) {
	string txtname = out + ".txt";
	string pngname = out + ".png";

	cout << "world: " << path << " " << endl;
	cout << "png: " << pngname << " " << endl;
	cout << "txt: " << txtname << " " << endl;
	//return 0;
 
	// string C = L"C:\\Users\\Harm\\Documents\\Visual Studio 2008\\Projects\\WinCartograph\\test\\edit";
	
	//wcout << A.c_str();
	//return 0;
    	vector<string> files;
	list<render> renderblocks;

	int counter = 0;
	int cc = 0;
	//int lool = 0;
	
	if (!ListFiles(path, files,counter)) {
		exit(1);
	}
	
	//lool = files.size();
	Level foo;
	//std::ofstream myfile ("files.txt",std::ios::app);

	//decompress_one_file("C:\\Users\\Harm\\AppData\\Roaming\\.minecraft\\saves\\World3\\4\\1c\\c.4.-g.dat","level_in");
	//std::cout << "\nRendering chunks..";
	
	//if (ListFiles(C.c_str(), L"*", files,counter)){
	cout << "2) Unpacking and drawing... " << flush;
        for (vector<string>::iterator it = files.begin(); it != files.end(); it++) {
			 
			 const render * temp = foo.LoadLevelFromFile(it->c_str(),slide,water,cut);
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
	


	if(slide == 70){
		int cxx = ((((maxx-minx))+(maxy-miny)))*0.5;
		int cyy = (((maxx-minx))+(maxy-miny))*0.5;

	  imageheight = (cxx)*32 + 288;
	  imagewidth = (cyy)*32 + 288;
	  
		}else{
	imageheight = (maxx-minx)*16 + 144;
	imagewidth = (maxy-miny)*16 + 144;
	}
	IMG * MAP;
	if(slide == 70)
	MAP = new IMG(imageheight,imagewidth+128);
	else if(slide == 69)
	MAP = new IMG(imageheight,imagewidth);
	else
	MAP = new IMG(imageheight,imagewidth);

	//std::cout << "\nAssembling image...";
	//if(slide == 69){
	renderblocks.sort();
	renderblocks.reverse();
	//}

	//int * xytt;
	
	//xytt = new int[10000]; 
	//for(int b = 0;b < 10000;b++){
	//xytt[b] = 0;
	//}
	for (list<render>::iterator rit=renderblocks.begin();rit!=renderblocks.end();rit++){
	
		//total++;	
		int xb = rit->x;
		int yb = rit->y;

		int xx = (xb-minx)*16;
		int yy = (yb-miny)*16;

		if(slide == 70){
		if(flip == 1)
		yy = abs((yb-miny)-(maxy-miny))*16;
		if(Rotate == 0)
		xx = abs((xb-minx)-(maxx-minx))*16;
		
		

		int temx = xx;
		int temy = yy;
		xx = imageheight/2+(temx)-(temy);
		yy = (temx)+(temy)+128;

	}

	if(slide == 69){
	
		if(flip == 1){
			if(Rotate == 0){
			//xx = (xb-minx)*16;
			yy = abs((yb-miny)-(maxy-miny))*16;
			}else{
			xx = (yb-miny)*16;
			yy = abs((xb-minx)-(maxx-minx))*16;

			}
		}else{
		if(Rotate == 1){
		xx = (yb-miny)*16;
		yy = (xb-minx)*16;
		}
		}
		}
	//xytt[(xx/16)+(yy/16)*100]++;
	//if(xytt[(xx/16)+(yy/16)*100] > 1)
	//myfile << "X:" << xb << " Y:" << yb << " " << "xx:" << xx << " yy:" << yy << "  " << xytt[(xx/16)+(yy/16)*100] << endl;
	//std::cout << xx << "_" << yy << " ";
	if(xx >= 0 && yy >= 0 && xx < 40000 && yy < 40000){
		if(slide == 70){

		
	for(int xo = 0;xo < 33;xo++){
	for(int yo = 0;yo < 160;yo++){
		int x = xo;
		int y = yo;
	MAP->SetPixel(x+xx,y+yy-128,Blend(MAP->GetPixel(x+xx,y+yy-128),rit->Q->GetPixel(x,y),128));
	}}


		}else{
	for(int xo = 0;xo < 16;xo++){
	for(int yo = 0;yo < 16+(slide > 68)*128;yo++){					
		int x = xo;
		int y = yo;
		
		//MAP.SetPixel(x+xx,y+yy,sf::Color(125,255,0,255));

		if(slide == 69)
		MAP->SetPixel(x+xx,y+yy,Blend(MAP->GetPixel(x+xx,y+yy),rit->Q->GetPixel(x,y),128));
		else if(slide == 70)
		MAP->SetPixel(x+xx,y+yy-128,Blend(MAP->GetPixel(x+xx,y+yy-128),rit->Q->GetPixel(x,y),128));
		else
		MAP->SetPixel(x+xx,y+yy,rit->Q->GetPixel(x,y));
		
	}
	}}
	}}

	cout << "3) Saving image " << pngname << "... " << flush;
	corona::Image *Output = corona::CreateImage(imageheight, imagewidth, corona::PF_R8G8B8A8,MAP->d);
	corona::SaveImage(pngname.c_str(), corona::FF_PNG,Output);
	
	delete MAP;
	delete Output;
	//delete MAP;

	//remove("level_in");
	renderblocks.clear();
	cout << "Done!" << endl;

	cout << "4) Saving txt " << txtname << "... " << flush;
	
	ofstream values( txtname.c_str() );
	if (values.is_open())
	{
		values << "Map surface is: " << cc*16*16 << " square meters" << endl;
		values << "Block amounts:" << endl;
		values << endl;
		values << "Air: " << foo.count[0] << endl;
		values << "Stone: " << foo.count[1] << endl;
		values << "Grass: " << foo.count[2] << endl;
		values << "Dirt: " << foo.count[3] << endl;
		values << "Snow: " << foo.count[78] << endl;
		values << endl;
		values << "Water: " << foo.count[8]+foo.count[9] << endl;
		values << "Ice: " << foo.count[79] << endl;
		values << "Lava: " <<	foo.count[10]+foo.count[11] << endl;
		values << endl;
		values << "Obisidian: " <<	foo.count[49] << endl;
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
		values << "Redstone: " << foo.count[73]+foo.count[74] << endl;
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
		values << "Furnace: " << foo.count[61]+foo.count[62] << endl;
		values << "Wooden Doors: " << foo.count[64]/2 << endl;
		values << "Iron Doors: " << foo.count[71] << endl;
		values << "Signs: " << foo.count[63]+foo.count[68] << endl;
		values << "Ladder: " << foo.count[65] << endl;
		values << "Railtracks: " << foo.count[66] << endl;
		values << "Wooden Stairs: " << foo.count[53] << endl;
		values << "Rock Stairs: " << foo.count[67] << endl;
		values << "Lever: " << foo.count[69] << endl;
		values << "Buttons: " << foo.count[77] << endl;
		values << "Pressure Plates: " << foo.count[70]+foo.count[72] << endl;
		values << "Redstone Powder: " << foo.count[55] << endl;
		values << "Redstone Torches: " << foo.count[75]+foo.count[76] << endl;
		
		values << endl;
	
		values.close();
	} else {
		cout << "fatal write to txt" << endl;
	}
	
	cout << "Done!" << endl;
}

void do_help() {
	cout << "Usage: cart5 <world-directory> <output.png> [options]" << endl;
	cout << "Valid options:"		
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
	
	slide = -1;

	water = false;
	cave = false;
	Rotate = false;
	flip = false;
	daynight = 0;

	for (int i = 3; i < argc; i++) {
		string opt(argv[i]);
		
		if (opt.compare("W") == 0) {
			water = true;
		}
	
		else if ( opt.compare("C") == 0) {
			cave = true;
		}
		else if ( opt.compare("R") == 0) {
			Rotate = true;
		}
		else if ( opt.compare("F") == 0) {
			flip = true;
		}
		
		else if ( opt.compare("D") == 0)
			daynight = 0;
		else if ( opt.compare("d") == 0)
			daynight = 1;
		else if ( opt.find("n") == 0)
			daynight = 2;
		else if ( opt.compare("Hc") == 0)
			slide = -3;
		else if ( opt.compare("Hg") == 0)
			slide = -4;
		else if ( opt.compare("Or") == 0)
			slide = -5;
		else if ( opt.compare("Ob") == 0)
			slide = 69;//yeah funny
		else if ( opt.compare("Oa") == 0)
			slide = 70;
		else if ( flag.find("E") != string::npos ) {
			cout << "Write number to exclude [0-128]" << endl << ":" << flush;
			cin >> exclude;
		}
		else {
			cerr << "Unkown option: " << opt << endl;
			exit(1);
		}
	}
	
	generate_png(path, out);
	
	return 0;
};
	

