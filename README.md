c10t - a cartography tool for Minecraft
=======================================

I wrote c10t entirely because i liked the tool Cartograph by ZomBuster, but i
wasn't too fond of a couple of aspects about how rendering was performed.

-- Udoprog <johnjohn.tedro@gmail.com>

Requirements
------------

  * libz (?)
  * libpng (=> 1.2)
  * libboost_thread (>= 1.40)
  * libboost_test
  * pthread (through boost)

Features
--------

  * Not using the old corona library, instead purely libpng
  * Pipelined rendering process (using image compositioning) which allows for
    multithreaded rendering.
  * A lot less memory intensive - the biggest concern is having to keep the
    entire (sometimes huge!) image in memory at once.
  * Uses proper command line options.
  * Clean code, for easing further development.
  * Multiplatform - yes, it does compile properly on windows and mac.
  * A gui wrapper, see: http://github.com/udoprog/c10t-swt (native gui with java bindings)

I would not have done this were it not for the excellent inspiration by
ZomBuster and Firemark (the linux port of cartograph). Thank you for the
inspiration.

Building from Source (using cmake)
----------------------------------

### Ubuntu ###

  1. Install dependencies:

    $ sudo apt-get install cmake libpng12-dev zlib1g-dev libboost-thread1.40-dev libboost-thread1.40.0 \
    libboost-test1.40-dev libboost-test1.40.0

  2. Run:

    $ cmake .
    $ make c10t

  note: CMake should generate a file called src/config.h from the input file src/config.h.cmake
  note: There are several targets you can make.  c10t, c10t-lib, c10t-debug, and c10t-test.
        If you wish to build all of these simply run `make` which defaults to target `all`.
        If you wish to build any individual one run `make <target>` e.g. `make c10t-test`.

  3. The executable (`c10t`) will be in the current directory.

Issues
------

 * Issues should be posted on http://github.com/udoprog/c10t/issues
 * Run the program in debug mode (c10t --debug), this will print useful information, but be much slower.
 * ALWAYS include `c10t --version` information when posting issues, that way it can be determined weither the issue already has been resolved or not, if this information is missing, the issue probably cannot be resolved.
 * ALWAYS include the following information: Platform (e.g. Windows, Linux, Mac) and Architecture (x86, x86_64)
 * IF POSSIBLE include information which can help us reproduce the problem.

Contributions
-------------
    ZomBuster and Firemark - for their original work
    Guardian9979 - for his continual nagging for improvements
    acleone - for his work with cmake [85e980a]
    j005u - for explaining how cocoa works
    jnnnnn - for limit options which are helpful when debugging (and quite cool)
    mudaltsov - for mapping out the Mac OS X building process and creating a splendid package!
    frozencow - for fixing boost_thread configuration for cmake
    reportingjsr - for pointing out build issues
