#ifndef GUARD_H
#define GUARD_H

typedef struct _settings {
	int flip;
	int Rotate;
	int daynight;
	int cave;
	int exclude;
	int slide;
	int water;
} settings_t;

extern int Rotate;
extern int daynight;
extern int cave;
extern int exclude;
extern int slide;
extern int flip;
extern int water;

void f();

#endif
