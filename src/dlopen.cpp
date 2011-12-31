#include "dlopen.hpp"

/**
 * Concept shamelessly stolen from;
 * http://www.codeproject.com/KB/architecture/plat_ind_coding.aspx
 */

#include <string>

#if defined(_MSC_VER)
#   include <windows.h>
#elif defined(__GNUC__)
#   include <dlfcn.h>
#else
#error Unknown compiler
#endif

struct dl_t {
#if defined(_MSC_VER)
  HINSTANCE impl;
#elif defined(__GNUC__)
  void* impl;
#endif
};

# if defined(_MSC_VER)
#define EXT ".dll"
#define EXTL 4
# elif defined(__GNUC__)
#define EXT ".so"
#define EXTL 3
# endif

dl_t* dl_open(const char *name)
{
  std::string fullname = std::string(name);
  void* impl;

  if (fullname.substr(fullname.size() - EXTL).compare(EXT) != 0) {
    fullname += EXT;
  }

# if defined(_MSC_VER)
  impl = LoadLibrary(fullname);
# elif defined(__GNUC__)
  impl = dlopen(fullname.c_str(), RTLD_LAZY);
# endif

  if (impl == NULL) {
    return NULL;
  }

  dl_t* dl = new dl_t;
  dl->impl = impl;
  return dl;
}

void* dl_sym(dl_t* dl, const char *sym)
{
#if defined(_MSC_VER)
  return (void*)GetProcAddress(dl->impl, sym);
#elif defined(__GNUC__)
  return dlsym(dl->impl, sym);
#endif
}

bool dl_close(dl_t* dl)
{
  bool ok;
#if defined(_MSC_VER)
  ok = FreeLibrary(dl->impl);
#elif defined(__GNUC__)
  ok = dlclose(dl->impl);
#endif
  delete dl;
  return ok;
}
