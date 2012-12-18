c10t - a cartography tool for Minecraft
=======================================

I wrote c10t entirely because i liked the tool Cartograph by ZomBuster, but I wasn't too fond of a couple of aspects about how rendering was performed.

-- Udoprog <johnjohn.tedro@gmail.com>

More Info: http://toolchain.eu/minecraft/c10t/

Master SNAPSHOT builds
----------------------
note: I don't support Windows and Mac builds since they are a major
hurdle, but the build scripts are present under the dist/ folder.

  * [Linux x86](http://toolchain.eu/jenkins/job/c10t-snapshot/target=x86-linux/lastStableBuild/)
  * [Linux x86\_64](http://toolchain.eu/jenkins/job/c10t-snapshot/target=x86_64-linux/lastBuild/)

Donations
---------
People asked where they could submit donations, you can do it at the "More Info" page (look at the bottom).

Requirements
------------

  * libz (?)
  * libpng (>= 1.2)
  * libfreetype (>= 2)
  * libboost (thread, filesystem, system and test) (>= 1.46)

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

###Ubuntu###

 1. Install dependencies:

        $ sudo apt-get install build-essential cmake libpng-dev zlib1g-dev libboost-all-dev libfreetype6-dev

    If libboost >= 1.46 is not available by using your package manager, you need to install it from source. To do so download and untar it from www.boost.org and perform the following operations as root:

        $ ./bootstrap --prefix=/usr
        $ ./b2 install

    If you have troubles installing boost, consult the Getting Started Guide on the website of the boost library.

 2. Clone repository

        $ git clone git://github.com/udoprog/c10t.git
        $ cd c10t

    You need to clone the repository in order to successfully execute the commands in step three. You can get support for git on the following web page:
    http://schacon.github.com/git/gittutorial.html
    Just scroll to the section "Using git for collaboration".

 3. Update submodules which contains some required dependencies (./libs):

        $ git submodule init
        $ git submodule update

 4. Run:

        $ mkdir build
        $ cd build
        $ cmake ..
        $ make c10t

 * CMake should generate a file called src/config.h from the input file src/config.h.cmake
  
 * There are several targets you can make.  c10t, c10t-lib, c10t-debug, and c10t-test.
   * If you wish to build all of these simply run `make` which defaults to target `all`.
   * If you wish to build any individual one run `make <target>` e.g. `make c10t-test`.

4. The executable `c10t` should be in the current directory.

There are a couple of available targets

 * ___c10t-debug___ - debug build with symbols, nice for debugging
 * ___c10t-lib___ - library that contains all c10t functions not in main or nbt_inspect
 * ___nbt-inspect___ - dumps the content of an nbt data file (basically anything in the world directory). Useful for writing tools.
 * ___region-inspect___ - dumps the content of a mcr region file.

Issues
------

 * Issues should be posted on http://github.com/udoprog/c10t/issues
 * Run the program in debug mode (c10t --debug), this will print useful information, but be much slower.
 * ALWAYS include `c10t --version` information when posting issues, that way it can be determined weither the issue already has been resolved or not, if this information is missing, the issue probably cannot be resolved.
 * ALWAYS include the following information: Platform (e.g. Windows, Linux, Mac) and Architecture (x86, x86_64)
 * IF POSSIBLE include information which can help us reproduce the problem.

Contributors
------------
    UniversE - for epic center calculation, and saving the api!

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
    ekryyn - for implementing the altitude graph!
