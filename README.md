c10t - a cartography tool for Minecraft
=======================================

I wrote c10t entirely because i liked the tool Cartograph by ZomBuster, but I wasn't too fond of a couple of aspects about how rendering was performed.

-- Udoprog <johnjohn.tedro@gmail.com>

More Info: http://toolchain.eu/minecraft/c10t/

Donations
---------
People asked where they could submit donations, you can do it at the "More Info" page (look at the bottom).

Requirements
------------

  * libz (?)
  * libpng (>= 1.2)
  * libfreetype (>= 2)
  * libboost (thread, filesystem, system and test) (>= 1.40)

Note: People have reported problem with boost 1.45, I personally have not been able to confirm this, if you ever encounter it, please report the bug as thorough as possible.

Features
--------

  * Pipelined rendering process (using image compositioning) which allows for
    multithreaded rendering.
  * Very memory friendly, you can specify a memory limit and it will switch caching to file.
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

    $ sudo apt-get install cmake libpng-dev zlib1g-dev libboost-all-dev libboost-all libfreetype6-dev

  2. Run:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make c10t

 * CMake should generate a file called src/config.h from the input file src/config.h.cmake
  
 * There are several targets you can make.  c10t, c10t-lib, c10t-debug, and c10t-test.
   * If you wish to build all of these simply run `make` which defaults to target `all`.
   * If you wish to build any individual one run `make <target>` e.g. `make c10t-test`.

  3. The executable (`c10t`) will be in the current directory.

There are a couple of available targets

 * ___c10t-debug___ - debug build with symbols, nice for debugging
 * ___c10t-lib___ - library that contains all c10t functions not in main or nbt_inspect
 * ___nbt-inspect___ - dumps the content of an nbt data file (basically anything in the world directory). Useful for writing tools.

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
    Dim-Dul - for providing a nice map to render
    vostok4 - for implementing google API support
    rmmh - for figuring out png compression and encouraging people to build properly.
    Athemis - for fixing beta 1.2 compatibility.
    sn4kebite - for implementing Wool colors.
