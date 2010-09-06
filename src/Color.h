#ifndef _COLOR_H_
#define _COLOR_H_

struct Color{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Color() : r(255), g(255), b(255), a(0) { }

	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
		r(r), g(g), b(b), a(a) {
	}

	~Color(){
	}
};

#endif /* _COLOR_H_ */
