#ifndef vxl_version_h_
#define vxl_version_h_

//:
// \file
// \brief The VXL version.
// This version number applies to the whole vxl tree, not just the
// core libraries.

//: Major version number
// This will only increase after major changes, or an large accumulation of 
// significant smaller ones.
#define VXL_VERSION_MAJOR 1

//: Minor version number
// This increments between versions. There is no 
// "even = release, odd = developement" pattern, or anything like that.
#define VXL_VERSION_MINOR 0

//: This patch number only applies to Packaged vxl releases.
// If you have downloaded the latest CVS version, you should use VXL_VERSION_DATE.
#define VXL_VERSION_PATCH 0


#define VXL_VERSION_STRING "1.0.0"

//: This is the date of the release.
// If you have downloaded the latest version from the CVS repository,
// you may wish to locally modify this number to reflect the download date.
#define VXL_VERSION_DATE "2003-10-09"

#endif // vxl_version_h_