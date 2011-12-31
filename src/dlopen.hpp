#ifndef _DLOPEN_HPP
#define _DLOPEN_HPP

struct dl_t;

dl_t* dl_open(const char*);
void* dl_sym(dl_t*, const char*);
bool dl_close(dl_t*);

#endif /* _DLOPEN_HPP */
