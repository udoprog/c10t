There has been a couple of variants of this script popping out recently, I took the liberty of consolidating them into the master branch.
-- Udoprog - Nov 17 2010

=== google-api.ps1 ===
Originally authored by Kochu

"""
  Usage is the same as with the .sh: google-api.ps1 "world" "target"
  You may need to change the path to the c10t executable in the script, add its location to the PATH variable, or install it into system32.

  To run it, you will need to enable script execution by powershell - it is off by default.
  To do this, you will need to run powershell as an admin and type "set-executionpolicy unrestricted", then "y" to confirm
  You'll have to do it only once.

  Hope it works.
""" -- Kochu @ http://www.minecraftforum.net/viewtopic.php?f=25&t=33803&start=570

=== google-api.php ===
Originally authored by Rendrik

"""
  I've ported the google-api script to PHP, mainly for use in a windows environment. I haven't tested this in Linux.
  It uses GD to resize the images.
  The script must be placed in the same directory as c10t.exe

  Usage is:
  php google-api.php [world path]* [image output directory]* [c10t args in quotes]

  For example:
  php google-api.php world\ gmap_images\ "--isometric -r 270"
  or 
  php google-api.php world\ tiles\ "--oblique-angle --show-signs"

  Make sure you have the trailing slash for directorys.
  This will create the following files:
  map.html
  options.js
  (image dir)\(lots of images).png

  Just open map.html to test it out.

  If there is any issue, you can open the PHP file and change the VERBOSE constant to true. This will output the c10t commands and responses.
""" -- Rendrik @ http://www.minecraftforum.net/viewtopic.php?f=25&t=33803&start=570
