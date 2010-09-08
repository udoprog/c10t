Cartography for Minecraft
=========================

* Cartograph by ZomBuster
* Linux version by Firemark
* Additional work by Udoprog <johnjohn.tedro@gmail.com>

See INSTALL for installation instructions

See ChangeLog for changes

This version is hosted at http://github.com/udoprog/cartography

No features have been discarded, instead the code has been subtly refactored and improvements made where they dont effect functionality.

Future work
-----------

 * Making the cartographer more memory friendly by implementing incremental image rendering. Currently all renderblocks are kept in-memory prior to image generation, this can be improved upon.
 * Fixing proper commandline options and a gui wrapper similar to the windows version

Options
-------

    Normal : a normal top down map
    heightmap: a colorized heightmap
    Heightgray: a black and white heightmap
    Slice: render a slice of the map (specify which one in the edit box below)
    Oblique: Render the map in oblique projection mode
    Obliqueangle: Render the map in angled oblique projection mode

    Ignore water: removes water from normal mode
    Flip : flip the map vertically in oblique mode
    Rotate: rotate the map 90 degrees in oblique mode
    Day: Set lighting to day
    Day/Night: set lighting to sunrise/dawn
    Night: set lighting to night 
    Cave Mode: Renders caves (empty blocks not lit by sunlight)
    Only: Only render a selected block, 0 to render everything, 1-66 to only render a certain block
