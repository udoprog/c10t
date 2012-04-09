# - Try to find JSON spirit library based on boost
# - On Ubuntu it's part of libjson-spirit-dev, feel free to adjust this file to cover
#   a wider range of operating systems
# Once done, this will define
#
#  JSON_SPIRIT_FOUND - system has JSON spirit library
#  JSON_SPIRIT_INCLUDE_DIRS - the JSON spirit include directories
#  JSON_SPIRIT_LIBRARIES - link these to use the JSON spirit library 

include(LibFindMacros)

# Include dir
find_path(JSON_SPIRIT_INCLUDE_DIR
  NAMES json_spirit.h
  PATHS ${JSON_SPIRIT_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(JSON_SPIRIT_LIBRARY
  NAMES json_spirit
  PATHS ${JSON_SPIRIT_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(JSON_SPIRIT_PROCESS_INCLUDES JSON_SPIRIT_INCLUDE_DIR JSON_SPIRIT_INCLUDE_DIRS)
set(JSON_SPIRIT_PROCESS_LIBS JSON_SPIRIT_LIBRARY JSON_SPIRIT_LIBRARIES)

libfind_process(JSON_SPIRIT)

