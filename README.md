c10t - a cartography tool for Minecraft
=======================================

I wrote c10t entirely because i liked the tool Cartograph by ZomBuster, but i wasn't too fond of a couple of aspects about how rendering was performed.

Features
--------
 * Not using the old corona library, instead purely libpng
 * Pipelined rendering process (using image compositioning) which allows for multithreaded rendering.
 * A lot less memory intensive - the biggest concern is having to keep the entire (sometimes huge!) image in memory at once.
 * Uses proper command line options.
 * Can flip the map properly (90 - 180 - 270 degrees CCW), to get 270 use both -f and -i
 * Clean code, for easing further development.
 * Multiplatform - yes, it does compile properly on winblows and probably also on mac.

I would not have done this were it not for the excellent inspiration by ZomBuster and Firemark (the linux port of cartograph). Thank you for the inspiration.
