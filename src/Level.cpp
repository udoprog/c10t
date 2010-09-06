#include "Level.h"

extern int flip;
extern int Rotate;
extern int daynight;
extern int cave;
extern int exclude;
extern int slide;


Color::Color(){
r = 255;
g=255;
b=255;
a=0;

}

Color::Color(unsigned char rr ,unsigned char gg,unsigned char bb,unsigned char aa){
r=rr;g=gg;b=bb;a=aa;

}

Color::~Color(){


}

void IMG::SetPixel(int x,int y,Color q){
if(x >= 0 && x < w && y >= 0 && y < h)
d[x+y*w] = q;
}

Color IMG::GetPixel(int x,int y){
if(x >= 0 && x < w && y >= 0 && y < h)
return d[x+y*w];
else
return Color(0,0,0,0);
}

Color * IMG::GetPPointer(int x,int y){
if(x >= 0 && x < w && y >= 0 && y < h)
return &d[x+y*w];
else
return 0;
}

IMG::IMG(){
w = 16;h = 16;
d = new Color[w*h];
}

IMG::IMG(int width,int height){
w = width; h = height;
d = new Color[w*h];
}

IMG::~IMG(){
delete [] d;

}

render::render(){
Q = new IMG(16,16);
}

render::~render(){
delete Q;
//MessageBoxW(NULL,L"Done", L" ", MB_OK);

}

render::render(int bb){
	if(bb == 69){
	Q = new IMG(16,144);

	}else if(bb == 70){
	Q = new IMG(33,160);

	}else{
	Q = new IMG(16,16);
	}

x = 0;y= 0;

}


Level::~Level(){

}

Color Blend(Color A,Color B,int h){
Color C;
float Aa = (float)A.a/255;
float Ba = (float)B.a/255;
float Alpha = Aa + Ba*(1-Aa);
C.r = (A.r*Aa + (B.r*((float)h/128))*Ba*(1-Aa))/Alpha;
C.g = (A.g*Aa + (B.g*((float)h/128))*Ba*(1-Aa))/Alpha;
C.b = (A.b*Aa + (B.b*((float)h/128))*Ba*(1-Aa))/Alpha;
C.a = Alpha*255;
return C;
}



Level::Level(){

mapx = 16;
mapy = 16;
mapz = 128;

	

	for(int m = 0;m < 128;m++){
	count[m] = 0;
	}


	
	for(int b = 0;b < 255;b++){
		BlockC[b] = Color(0,0,0,0);
	}			
				BlockC[0] = Color(255,255,255,0);
				BlockC[1] = Color(120,120,120,255);
				BlockC[2] = Color(117,176,73,255);
				BlockC[3] = Color(134,96,67,255);
				BlockC[4] = Color(115,115,115,255);
				BlockC[48] = Color(115,115,115,255);
				BlockC[5] = Color(157,128,79,255);
				BlockC[6] = Color(120,120,120,0);
				BlockC[7] = Color(84,84,84,255);
				BlockC[8] = Color(38,92,255,51);
				BlockC[9] = Color(38,92,255,51);
				BlockC[10] = Color(255,90,0,255);
				BlockC[11] = Color(255,90,0,255);
				BlockC[12] = Color(218,210,158,255);
				BlockC[13] = Color(136,126,126,255);
				BlockC[14] = Color(143,140,125,255);
				BlockC[15] = Color(136,130,127,255);
				BlockC[16] = Color(115,115,115,255);
				BlockC[17] = Color(102,81,51,255);
				BlockC[18] = Color(60,192,41,100);
				BlockC[20] = Color(255,255,255,64); //glass
				//BlockC[21] = Color(222,50,50,255);
				//BlockC[22] = Color(222,136,50,255);
				//BlockC[23] = Color(222,222,50,255);
				//BlockC[24] = Color(136,222,50,255);
				//BlockC[25] = Color(50,222,50,255);
				//BlockC[26] = Color(50,222,136,255);
				//BlockC[27] = Color(50,222,222,255);
				//BlockC[28] = Color(104,163,222,255);
				//BlockC[29] = Color(120,120,222,255);
				//BlockC[30] = Color(136,50,222,255);
				//BlockC[31] = Color(174,74,222,255);
				//BlockC[32] = Color(222,50,222,255);
				//BlockC[33] = Color(222,50,136,255);
				//BlockC[34] = Color(77,77,77,255);
				BlockC[35] = Color(222,222,222,255); //Color(143,143,143,255); 
				//BlockC[36] = Color(222,222,222,255);
				BlockC[37] = Color(255,0,0,255);
				BlockC[38] = Color(255,255,0,0);
				BlockC[41] = Color(232,245,46,255);
				BlockC[42] = Color(191,191,191,255);
				BlockC[43] = Color(200,200,200,255);
				BlockC[44] = Color(200,200,200,255);
				BlockC[45] = Color(170,86,62,255);
				BlockC[46] = Color(160,83,65,255);
				BlockC[49] = Color(26,11,43,255);
				BlockC[50] = Color(245,220,50,200);
				BlockC[51] = Color(255,170,30,200);
				//BlockC[52] = Color(245,220,50,255); unnecessary afaik
				BlockC[53] = Color(157,128,79,255);
				BlockC[54] = Color(125,91,38,255);
				//BlockC[55] = Color(245,220,50,255); unnecessary afaik
				BlockC[56] = Color(129,140,143,255);
				BlockC[57] = Color(45,166,152,255);
				BlockC[58] = Color(114,88,56,255);
				BlockC[59] = Color(146,192,0,255);
				BlockC[60] = Color(95,58,30,255);
				BlockC[61] = Color(96,96,96,255);
				BlockC[62] = Color(96,96,96,255);
				BlockC[63] = Color(111,91,54,255);
				BlockC[64] = Color(136,109,67,255);
				BlockC[65] = Color(181,140,64,32);
				BlockC[66] = Color(150,134,102,180);
				BlockC[67] = Color(115,115,115,255);
				BlockC[71] = Color(191,191,191,255);
				BlockC[73] = Color(131,107,107,255);
				BlockC[74] = Color(131,107,107,255);
				BlockC[75] = Color(181,140,64,32);
				BlockC[76] = Color(255,0,0,200);
				BlockC[78] = Color(255,255,255,255);
				BlockC[79] = Color(83,113,163,51);
				BlockC[80] = Color(250,250,250,255);
				BlockC[81] = Color(25,120,25,255);
				BlockC[82] = Color(151,157,169,255);
				BlockC[83] = Color(193,234,150,255);




}

Color Level::GetColor(int blockid){
return BlockC[blockid];

}

void Level::Edit(int x,int y,int z,int block,unsigned char * &d){
	if ((x >= mapx) || (x < 0)) return;
	if ((y >= mapy) || (y < 0)) return;
	if ((z >= mapz) || (z < 0)) return;
	d[x*mapz + y*mapz*mapx + z] = block;

}

int Level::Read(int x,int y,int z,unsigned char * &d,int ret){
	if ((x >= mapx) || (x < 0)) return ret;
	if ((y >= mapy) || (y < 0)) return ret;
	if ((z >= mapz) || (z < 0)) return ret;
	return d[x*mapz + y*mapz*mapx + z];

}

int Level::GetHeight(int x,int y){
		if ( x >= mapx ) return 0;
		if ( y >= mapy ) return 0;
		if ( x < 0 ) return 0;
		if ( y < 0 ) return 0;
		for (int z = mapz;z > 0;z--){
			if ((Read(x,y,z,blocks,0) != 0)){
				return z;
			}

		}
	return 0;

}



const render * Level::LoadLevelFromFile(const char * name, const int slide, const bool CWATER,const int cut){

	render * R;
	R = new render(slide);
	R->isgood = false;
	long length;
	

	blocklight = new unsigned char[mapx*mapy*mapz];
	skylight = new unsigned char[mapx*mapy*mapz];
	blocks = new unsigned char[mapx*mapy*mapz];
	
	

	gzFile filein = gzopen(name, "rb");
	while(!filein){filein = gzopen(name,"rb");}
//	filein.seekg (0,std::ios::end);
	length = 131072;
//	filein.seekg (0,std::ios::beg);

	databuffer = new unsigned char[length];
	//filein.seekg (0, std::ios::beg);
	int q = gzread(filein,databuffer,length);
	while(q == -1){q = gzread(filein,databuffer,length);
	};
	gzclose(filein);

	if( databuffer[0] == 10 && 
		databuffer[1] == 0 && 
		databuffer[2] == 0 && 
		databuffer[3] == 10 && 
		databuffer[4] == 0 && 
		databuffer[5] == 5 &&
		databuffer[6] ==76 &&
		databuffer[7] == 101 &&
		databuffer[8] == 118 &&
		databuffer[9] == 101 &&
		databuffer[10] == 108){
			R->isgood = true;
	}else{
	return R;
	}
		
		
		int startlight = 0;
		int bstartlight = 0;
		int startterrain = 0;
		//std::cout << "\nprocessing " << length << " bytes\n";
		
		for(int z = 0;z < length;z++){

			if(z > 10 && databuffer[z] == 115 && databuffer[z-1] == 111 && databuffer[z-2] == 80 && databuffer[z-3] == 120 ){	
				
				
				long int Int32 = 0;
				bool neg = false;
				if(databuffer[z+1] == 255)neg = true;
				//Int32 += (int)databuffer[z+3]*256;
				if(neg){
				Int32 += -(255-(int)databuffer[z+3])*256;
				Int32 += -(255-(int)databuffer[z+4]);
				}else{
				Int32 += (int)databuffer[z+3]*256;
				Int32 += (int)databuffer[z+4]+1;
				}
				R->x = Int32;



			}
			if(z > 10 && databuffer[z] == 115 && databuffer[z-1] == 111 && databuffer[z-2] == 80 && databuffer[z-3] == 122){
				
				
				long int Int32 = 0;
				bool neg = false;
				if(databuffer[z+1] == 255)neg = true;
				if(neg){
				Int32 += -(255-(int)databuffer[z+3])*256;
				Int32 += -(255-(int)databuffer[z+4]);
				}else{
				Int32 += (int)databuffer[z+3]*256;
				Int32 += (int)databuffer[z+4]+1;
				}
				

			
				R->y = Int32;

			}


			
			if(databuffer[z] == 115 && databuffer[z-1] == 107 && databuffer[z-2] == 99 && databuffer[z-3] == 111 && databuffer[z-4] == 108 && databuffer[z-5] == 66 && databuffer[z-6] == 6 && databuffer[z-7] == 0){
			startterrain = z+5;

			}
			if(z > startterrain + 10 && startterrain != 0) break;

			if(databuffer[z] == 116 && databuffer[z-1] == 104 && databuffer[z-2] == 103 && databuffer[z-3] == 105 && databuffer[z-4] == 76 && databuffer[z-5] == 121){
			startlight = z+5;

			}
			if(databuffer[z] == 116 && databuffer[z-1] == 104 && databuffer[z-2] == 103 && databuffer[z-3] == 105 && databuffer[z-4] == 76 && databuffer[z-5] == 107){
			bstartlight = z+5;

			}

		}
		
		
		/*for(int x = 0;x< 16;x++){
		for(int y = 0;y< 16;y++){
		for(int z = 0;z< 128;z++){
		Edit(x,y,z,databuffer[startterrain + x*128 + y*128*16 + z],blocks);
		//count[databuffer[startterrain + x*128 + y*128*16 + z]]++;
		}}}*/

		for(int pss = 0;pss < 32768;pss++){

		blocks[pss] = databuffer[startterrain + pss];
		count[databuffer[startterrain + pss]]++;
		}

	
		for(int x = 0;x< 16;x++){
		for(int y = 0;y< 16;y++){
		for(int z = 0;z< 128;z++){
		
		int half = x*64 + y*64*16 + z*0.5; 
		int l;
		if(z % 2 == 0)
		l = (int)databuffer[startlight + half] % 16;
		else
		l = (int)databuffer[startlight + half]/16;
		Edit(x,y,z,l,skylight);
		
		}}}

		Color waste;
		
		for(int x = 0;x< 16;x++){
		for(int y = 0;y< 16;y++){
		for(int z = 0;z< 128;z++){
		
		int half = x*64 + y*64*16 + z*0.5; 
		int l;
		if(z % 2 == 0)
		l = (int)databuffer[bstartlight + half] % 16;
		else
		l = (int)databuffer[bstartlight + half]/16;
		Edit(x,y,z,l,blocklight);
		
		}}}

		
		
		if(slide < 69){
		for(int x = 0;x< 16;x++){
		for(int y = 0;y< 16;y++){

		int l;
		
			Color *t = &R->Q->d[y+x*16];
			if(cave){ 
			for(int z = 127;z >= 0;z--){
			if(IsBlock(x,y,z) && Read(x,y,z,skylight,0) == 0){
				Color v;
			int zzz = 128-z;
			v.r = (zzz > 0)*(zzz <= 32)*255 + (zzz > 32)*(zzz < 64)*(32-(zzz-32))*8;
			v.g = (zzz > 96)*(zzz < 128)*(32-(zzz-96))*8 + (zzz > 0)*(zzz < 32)*(zzz)*8 + (zzz >= 32)*(zzz <= 96)*255;
			v.b = (zzz >= 96)*(zzz < 128)*255 + (zzz > 64)*(zzz < 96)*(zzz-64)*8;
			v.a = (Read(x,y,z,blocklight,0)+1)*10;
			
			*t = Blend(*t,v,128);

			}
			}


			}else if(slide == -1){
			for(int z = 127;z >= 0;z--){
				int rr = Read(x,y,z,blocks,0);
				if((rr != 0 && (exclude == 0 || rr == exclude)) || (!CWATER && (rr != 8 && rr != 9))){
				switch(daynight){
				case(0):
				l = getlight(x,y,z+1,1,1,CWATER,slide)*4;
				break;
				case(1):
				l = getlight(x,y,z+1,0.5,1,CWATER,slide)*4;
				break;
				case(2):
				l = getlight(x,y,z+1,0,1,CWATER,slide)*4;
				break;
				}
				if(exclude != 0) l = 128;
				int br = 0.8*l + 0.5*z;if(br > 128) br = 128;
				
				if(rr == 10 || rr == 11 || rr == 50 || rr == 51 || rr == 76){
					*t = Blend(*t,GetColor(rr),128);
				}else if((rr == 8 || rr == 9 || rr == 79) && CWATER){
					Color P = GetColor(rr);
					if(Read(x,y,z+1,blocks,0) == 0){
					P.a = 128; *t = Blend(*t,P,br);
					}
					}else{
					*t = Blend(*t,GetColor(rr),br);
					}}
				//if(z == 0)
					//t = BlockC[10];
				if(t->a >= 255){z = -10;t->a = 255;}
			}}else if(slide == -3){
				for(int z = 127;z >= 0;z--){
				int rr = Read(x,y,z,blocks,0);
				if((rr != 0 && ((rr != 8 && rr != 9) || !CWATER) && (exclude == 0 || rr == exclude))){
				
				int zzz = 128-z;
				t->r = (zzz > 0)*(zzz <= 32)*255 + (zzz > 32)*(zzz < 64)*(32-(zzz-32))*8;
				t->g = (zzz > 96)*(zzz < 128)*(32-(zzz-96))*8 + (zzz > 0)*(zzz < 32)*(zzz)*8 + (zzz >= 32)*(zzz <= 96)*255;
				t->b = (zzz >= 96)*(zzz < 128)*255 + (zzz > 64)*(zzz < 96)*(zzz-64)*8;
				t->a = 255;
				z = -10;
				}
				}
			}else if(slide == -4){
				for(int z = 127;z >= 0;z--){
				int rr = Read(x,y,z,blocks,0);
				if((rr != 0 && ((rr != 8 && rr != 9) || !CWATER) && (exclude == 0 || rr == exclude))){

				t->r = z*2;
				t->g = z*2;
				t->b = z*2;
				t->a = 255;
				z = -10;
				}
				}
			}else if(slide == -5){
				for(int z = 127;z >= 0;z--){
				int rr = Read(x,y,z,blocks,0);

					if(rr == 14 )*t = Blend(*t,Color(0,255,0,32),128);
					if(rr == 15 )*t = Blend(*t,Color(255,0,0,32),128);
					if(rr == 16 )*t = Blend(*t,Color(0,0,0,32),128);
					if(rr == 56 )*t = Blend(*t,Color(0,0,255,32),128);
				
				if(t->a > 255){z = -10;t->a = 255;}
				}}
		else{
			int rr = Read(x,y,cut,blocks,0);
			*t = GetColor(rr);
			
			}
		}}}else{
			if(slide == 70){
	for(int z = 128;z > -1;z--){
			for(int xr = 0;xr < 16;xr++){
				for(int yr = !(Rotate && !flip)*14;yr != 0+(Rotate && !flip)*16;yr += 1-!(Rotate && !flip)*2){
				
				

				int xo,yo;
				
				int ff = -1;int  ff2 = 1;

				if(Rotate){
				
				if(!flip){
				yo =  yr;
				xo =  15-xr;
				ff = -1;
				ff2 = -1;
				}else{
				ff = 1;
				xo = xr;
				}
				
				}else{
				xo = 15 - xr;
				
				}

				

				if(flip){
				
				if(!Rotate){
				xo = xr;
				yo = 15 - yr;
				ff = 1;
				ff2 = 1;
				}else{
					yo =  yr;
					ff2 = -1;
				}
				
				}else{
				yo = 15 - yr;
				
				}

				int x = xr;
				int y = yr;
				if(Rotate && !flip)
				y =  15 - yr;
				

				int rr = Read(xo,yo,z,blocks,0);
				bool f,t,r;
				f = IsBlock(xo-ff,yo,z);
				t = IsBlock(xo,yo,z+1);
				r = IsBlock(xo,yo-ff2,z);

				


				int rrx = 16+(y)-(x);
				int rry = 0+(y)+(x);		
				Color *t2;Color *t3;Color *t4;
				Color *t1 = &R->Q->d[rry+((128-z)+rrx)*33];
				
				if(rrx+1+(128-z) < 160)
				t2 = &R->Q->d[rry+((128-z)+rrx+1)*33];
				else
				t2 = &waste;


				
				if(rry+1 < 34 && rrx+1+(128-z) < 160)
				t3 = &R->Q->d[rry+1+((128-z)+rrx+1)*33];
				else
				t2 = &waste;
				
				t4 = &R->Q->d[rry+1+((128-z)+rrx)*33];
				
				

				


				int here,top,left,right;
				if(exclude == 0 && !cave){
				switch(daynight){
				case(0):
				here = getlight(xo,yo,z,1,1,CWATER,slide)*4;
				top = getlight(xo,yo,z+1,1,1,CWATER,slide)*4;
				left = getlight(xo-ff,yo,z,1,1,CWATER,slide)*4;
				right = getlight(xo,yo-ff2,z,1,1,CWATER,slide)*4;
				break;
				case(1):
				here = getlight(xo,yo,z,0.5,1,CWATER,slide)*4;
				top = getlight(xo,yo,z+1,0.5,1,CWATER,slide)*4;
				left = getlight(xo-ff,yo,z,0.5,1,CWATER,slide)*4;
				right = getlight(xo,yo-ff2,z,0.5,1,CWATER,slide)*4;
				break;
				case(2):
				here = getlight(xo,yo,z,0,1,CWATER,slide)*4;
				top = getlight(xo,yo,z+1,0,1,CWATER,slide)*4;
				left = getlight(xo-ff,yo,z,0,1,CWATER,slide)*4;
				right = getlight(xo,yo-ff2,z,0,1,CWATER,slide)*4;
				break;
				}}

				if(exclude != 0){top = 128;here = 128; right = 128;left = 128;}
				if(cave){
				Color B;
				
				int blight = Read(xo,yo,z,blocklight,0);
				if(blight > 0) blight++;
				
				if(Read(xo,yo,z,skylight,15) == 0 && IsBlock(xo,yo,z))
					blight = 1;

				if(Read(xo,yo,z,skylight,15) != 0 || rr == 8 || rr == 9 || rr == 79)
					blight = 0;

				int zzz = 128-z;
				B.r = (zzz > 0)*(zzz <= 32)*255 + (zzz > 32)*(zzz < 64)*(32-(zzz-32))*8;
				B.g = (zzz > 96)*(zzz < 128)*(32-(zzz-96))*8 + (zzz > 0)*(zzz < 32)*(zzz)*8 + (zzz >= 32)*(zzz <= 96)*255;
				B.b = (zzz >= 96)*(zzz < 128)*255 + (zzz > 64)*(zzz < 96)*(zzz-64)*8;
				B.a = (blight > 0)*60+((blight)*12);

				
				*t1 = Blend(*t1,B,128);
				*t2 = Blend(*t2,B,100);
				*t3 = Blend(*t3,B,90);
				*t4 = Blend(*t4,B,118);

				}

				if(!cave){
				if(rr != 0 && rr != 78 && (f || t ||  r || exclude != 0) && (exclude == 0 || rr == exclude)){
				if (rr == 76 || rr == 75 || rr == 6 || rr == 37 || rr == 38 || rr == 39 || rr == 40 || rr == 51 || rr == 50 || rr == 59 || rr == 63 || rr == 65 || rr == 66){
					if(rr == 50 || rr == 51 || rr == 76){
					*t2 = Blend(*t2,GetColor(rr),128);
					}else{
					*t2 = Blend(*t2,GetColor(rr),here*0.8+z*0.4);
					}
				}else{
					if(rr == 10 || rr == 11){
					*t1 = Blend(*t1,GetColor(rr),(128*0.8+z*0.4));
					*t2 = Blend(*t2,GetColor(rr),(128*0.8+z*0.4)*0.65);
					*t3 = Blend(*t3,GetColor(rr),(128*0.8+z*0.4)*0.5);
					*t4 = Blend(*t3,GetColor(rr),(128*0.8+z*0.4)*0.8);

					}else{
						if(rr == 8 || rr == 9 || rr == 79){
						Color P = GetColor(rr);
						if(Read(xo,yo,z+1,blocks,0) == 0 || Read(xo+1,yo,z,blocks,-1) == 0 || Read(xo,yo+1,z,blocks,-1) == 0){
							P.a = 128;
						*t1 = Blend(*t1,P,(here*0.8+z*0.4));				
						*t2 = Blend(*t2,P,(here*0.8+z*0.4)*0.65);
						*t3 = Blend(*t3,P,(here*0.8+z*0.4)*0.5);
						*t4 = Blend(*t4,P,(here*0.8+z*0.4)*0.8);}


						}else if(rr == 2){
				if(Read(xo,yo,z+1,blocks,0) == 78)
				*t1 = Blend(*t1,Color(255,255,255,255),(top*0.8+z*0.4));
				else
				*t1 = Blend(*t1,GetColor(2),(top*0.8+z*0.4));				
				*t2 = Blend(*t2,GetColor(3),(left*0.8+z*0.4)*0.65);
				*t3 = Blend(*t3,GetColor(3),(right*0.8+z*0.4)*0.5);
				if(Read(xo,yo,z+1,blocks,0) == 78)
				*t4 = Blend(*t4,Color(255,255,255,255),(top*0.8+z*0.4)*0.8);
				else
				*t4 = Blend(*t4,GetColor(2),(top*0.8+z*0.4)*0.8);
						}else{
				if(Read(xo,yo,z+1,blocks,0) == 78)
				*t1 = Blend(*t1,Color(255,255,255,255),(top*0.8+z*0.4));
				else
				*t1 = Blend(*t1,GetColor(rr),(top*0.8+z*0.4));				
				*t2 = Blend(*t2,GetColor(rr),(left*0.8+z*0.4)*0.75);
				*t3 = Blend(*t3,GetColor(rr),(right*0.8+z*0.4)*0.5);
				if(Read(xo,yo,z+1,blocks,0) == 78)
				*t4 = Blend(*t4,Color(255,255,255,255),(top*0.8+z*0.4)*0.8);
				else
				*t4 = Blend(*t4,GetColor(rr),(top*0.8+z*0.4)*0.8);

						}
					
					
					}
				}
				}
				}

				}}}

				}else if(slide == 69){
		for(int z = 128;z > -1;z--){
			for(int xr = 15-(flip == 1)*15;xr != -1+(flip == 1)*17;xr += -1+(flip == 1)*2){
				for(int yr = 0;yr < 16;yr++){
				
				int xo,yo;

				if(Rotate == 1){
				xo = yr;
				yo = xr;
				}else{
				xo = xr;
				yo = yr;
				}

				int x = xr;
				int y = yr;
				int ff = 1;
				if(flip == 1){
				x = 15-xr;
				//y = 15-yo;
				ff = -1;
				}

				

				int rr = Read(xo,yo,z,blocks,0);
				bool f,t;
				t = IsBlock(xo,yo,z+1);
				if(Rotate == 1){
					if(flip == 1){
					f = IsBlock(xo,yo-1,z);
					}else{
					f = IsBlock(xo,yo+1,z);
					}
				}else{
					if(flip == 1){
					f = IsBlock(xo-1,yo,z);

					}else{
					f = IsBlock(xo+1,yo,z);
					}}

				Color *t1 = &R->Q->d[y+((128-z)+x)*16];			
				Color *t2;
				if(z > 1)
					t2 = &R->Q->d[y+((128-z)+x+1)*16];
				else
				t2 = new Color();

				int here,top,front;
				if(exclude == 0 && !cave){
				switch(daynight){
				case(0):
				here = getlight(xo,yo,z,1,1,CWATER,slide)*4;
				top = getlight(xo,yo,z+1,1,1,CWATER,slide)*4;
				front = getlight(xo+ff*(Rotate != 1),yo+ff*(Rotate == 1),z,1,1,CWATER,slide)*4;
				break;
				case(1):
				here = getlight(xo,yo,z,0.5,1,CWATER,slide)*4;
				top = getlight(xo,yo,z+1,0.5,1,CWATER,slide)*4;
				front = getlight(xo+ff*(Rotate != 1),yo+ff*(Rotate == 1),z,0.5,1,CWATER,slide)*4;
				break;
				case(2):
				here = getlight(xo,yo,z,0,1,CWATER,slide)*4;
				top = getlight(xo,yo,z+1,0,1,CWATER,slide)*4;
				front = getlight(xo+ff*(Rotate != 1),yo+ff*(Rotate == 1),z,0,1,CWATER,slide)*4;
				break;
				}}

				if(exclude != 0){top = 128;here = 128; front = 128;}
				if(cave){
				Color B;
				
				int blight = Read(xo,yo,z,blocklight,0);
				if(blight > 0) blight++;
				
				if(Read(xo,yo,z,skylight,15) == 0 && IsBlock(xo,yo,z))
					blight = 1;

				if(Read(xo,yo,z,skylight,15) != 0 || rr == 8 || rr == 9)
					blight = 0;

				int zzz = 128-z;
				B.r = (zzz > 0)*(zzz <= 32)*255 + (zzz > 32)*(zzz < 64)*(32-(zzz-32))*8;
				B.g = (zzz > 96)*(zzz < 128)*(32-(zzz-96))*8 + (zzz > 0)*(zzz < 32)*(zzz)*8 + (zzz >= 32)*(zzz <= 96)*255;
				B.b = (zzz >= 96)*(zzz < 128)*255 + (zzz > 64)*(zzz < 96)*(zzz-64)*8;
				B.a = (blight > 0)*60+((blight)*12);

				
				*t1 = Blend(*t1,B,128);
				*t2 = Blend(*t2,B,100);

				}

				if(!cave){
				if(rr != 0  && rr != 78 && (t || f || exclude != 0) && (exclude == 0 || rr == exclude)){
				if(rr == 8 || rr == 9 || rr == 79){
				Color P = GetColor(rr);
				if(Read(xo,yo,z+1,blocks,0) == 0){
				P.a = 128;
				*t1 = Blend(*t1,P,top);}

				
				}else if (rr == 6 || rr == 75 || rr == 76 || rr == 37 || rr == 38 || rr == 39 || rr == 40 || rr == 51 || rr == 50 || rr == 59 || rr == 63 || rr == 65 || rr == 66){
					if(rr == 50 || rr == 51 || rr == 76){
					*t2 = Blend(*t2,GetColor(rr),128*0.8+z*0.4);
					}else{
					*t2 = Blend(*t2,GetColor(rr),here*0.8+z*0.4);
					}
				}else{
					if(rr == 10 || rr == 11){
					*t1 = Blend(*t1,GetColor(rr),128*0.8+z*0.4);
					*t2 = Blend(*t2,GetColor(rr),128*0.8+z*0.4);

					}else{
					if(rr == 2){
				if(Read(xo,yo,z+1,blocks,0) == 78)
				*t1 = Blend(*t1,Color(255,255,255,255),top*0.8+z*0.4);
				else
				*t1 = Blend(*t1,GetColor(2),(top*0.8+z*0.4));				
				*t2 = Blend(*t2,GetColor(3),(front*0.8+z*0.4)*0.75);
						}else{
				if(Read(xo,yo,z+1,blocks,0) == 78)
				*t1 = Blend(*t1,Color(255,255,255,255),(top*0.8+z*0.4));
				else
				*t1 = Blend(*t1,GetColor(rr),(top*0.8+z*0.4));				
				*t2 = Blend(*t2,GetColor(rr),(front*0.8+z*0.4)*0.75);

						}
						
					
					}
				}
				}
				}

				}}}}
		
	}
	delete[]	skylight;
	delete[]	blocks;	
	delete[] blocklight;
	delete[] databuffer;
	return R;
}


bool Level::IsBlock(int x,int y,int z){
	int qq = Read(x,y,z,blocks,0);
	return (qq > 62 || qq == 10 || qq == 20 || qq == 0 || qq == 37 || qq == 38 || qq == 39 || qq == 40 || qq == 50 || qq == 18 || qq == 8 || qq == 9);
}


const double Level::getlight(int x,int y,int z,double sky,double block,bool CWATER,int slide){
double b = (double)Read(x,y,z,skylight,15*sky)*sky;


double s = (double)Read(x,y,z,blocklight,0)*block;
if(!CWATER && slide == -1) s = 15*sky;
if(s > b)
return s+1;
else
return b+1;
}




