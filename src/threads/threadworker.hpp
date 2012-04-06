// Distributed under the BSD License, see accompanying LICENSE.txt
// (C) Copyright 2010 John-John Tedro et al.
#ifndef _THREADWORKER_H_
#define _THREADWORKER_H_
#include <assert.h>

#include <queue>
#include <list>

// I admit, this is pretty drastic, but I was desperate
// now I just keep it for the sake of having it available to new
// platforms.
#if !defined(C10T_DISABLE_THREADS)
#include "threads/threadworker_impl.hpp"
#else
#include "threads/threadworker_fake.hpp"
#endif

#endif /* _THREADWORKER_H_ */
