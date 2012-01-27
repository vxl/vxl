//-----------------------------------------------------------------------------
//:
// \file
// \brief Collection of image metadata values needed for radiometric calculations
// \author D. E. Crispell 
// \date January 27, 2012
//
//----------------------------------------------------------------------------
#ifndef brad_image_metadata_h_
#define brad_image_metadata_h_

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class brad_image_metadata : public vbl_ref_count
{
public:
   // position of sun relative to imaged location 
   double sun_elevation_; // degrees above horizon
   double sun_azimuth_;   // degrees east of north

   // position of image sensor relative to imaged location
   double view_elevation_; // degrees above horizon
   double view_azimuth_;   // degrees east of north

   // gain and offset required to convert digital number (DN) to band-averaged radiance value
   double gain_;           // units W m^-2 sr^-1 um^-1 DN^-1
   double offset_;         // units W m^-2 sr^-1 um^-1

   // band-averaged sun irradiance (includes term accounting for Earth-Sun distance)
   double sun_irradiance_; // units W m^-2 
};

typedef vbl_smart_ptr<brad_image_metadata> brad_image_metadata_sptr;

#endif

