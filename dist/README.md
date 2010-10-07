This little setup came out of me wanting to setup my server to build SNAPSHOTS about once very day.
These scripts generate static binaries, and they require you to have all required libraries pre-installed.

This is not meant to be used by a broad set of users, therefore it is very specialized to my system setup, but still might be useful if you want to setup something similar yourself.

This is what I do for building static binary for windows:
  i686-mingw32-g++ -DBOOST_THREAD_USE_LIB -I./contrib/include \
       -static-libgcc $(find src -name "*.cpp") \
      ./contrib/boost_thread/src/win32/thread.cpp \
      ./contrib/boost_thread/src/win32/tss_pe.cpp \
      ./contrib/tss_dummy.cpp \
      -static -lpng14 -lz -lpthreadGC2 \
      -o c10t.exe

Now, the only thing I havent gotten down yet is mac. I have no experience and no means to test it, so if someone would like to inform me on how it's done, I'm all ears.

If you want to run these manually, you have to be in the root directory, and issue the following command:

  make -f dist/Makefile.<target-os> clean all

Please not that the 'clean' is basically compulsary, since it will probably otherwise break when doing multiple targets due to output format.

This also assumes that you've already generated the src/config.h, you do this by typing:

  cmake .

My cross development setup is base of gentoo crossdev, this allows me to have multiple comilers and library setups without conflicts:

  http://en.gentoo-wiki.com/wiki/Crossdev

Good luck, this is me sharing my experiences

-- Udoprog
