// This is core/vil/file_formats/vil_geotiff_header.h
#ifndef vil_geotiff_header_h_
#define vil_geotiff_header_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author    Gamze Tunali
// \date      Jan 31, 2007
// \brief A header structure for geotiff files
//
// This class is responsible for extracting (putting) information
// from (into) the geotiff header that is required to specify a vil_image_resource
// There are bool flags that indicate that the item has been successfuly
// read (written) to the open geotiff file.
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vxl_config.h>
#include <vcl_cmath.h>
#include <vil/vil_pixel_format.h>
#include <vil/file_formats/vil_tiff_header.h>
#include <vnl/vnl_vector.h>
#include <tiffio.h>
#include <geotiffio.h>


class vil_geotiff_header
{
 public:
   
   typedef enum {UNDEF=-1, NORTH=0, SOUTH=1} GTIF_HEMISPH; 
   vil_geotiff_header(TIFF* tif);

   // destructor frees up gtif
   virtual ~vil_geotiff_header() { GTIFFree(gtif_); }

   int gtif_number_of_keys() { return number_of_geokeys_; }

   bool gtif_tiepoints(vcl_vector<vcl_vector<double> > &tiepoints);

   bool gtif_pixelscale(double &scale_x, double &scale_y, double &scale_z);

   //:returns the matrix in an   
   bool gtif_trans_matrix (vcl_vector<double> &trans_matrix);
  
   //: returns the Zone and the Hemisphere (0 for N, 1 for S);
   bool PCS_WGS84_UTM_zone(int &zone, GTIF_HEMISPH &hemisph);

   //: <key> : key id
   // <value>: a single value or an array of values
   // <size>:  the size of individial key values
   // <length> : the number of values in the value array
   // <type>: the type of the key
   bool get_key_value(geokey_t key, void** value, 
                     int& size, int& length, tagtype_t& type);

  void print_gtif(){ if (gtif_) GTIFPrint(gtif_, 0, 0); }

 private:

   TIFF* tif_;
   GTIF* gtif_;

  // the current version is 1, changes only Tag's key structure is changed
  unsigned short key_directory_version_;   

  // the revision number is key_revision.minor_revision
  unsigned short key_revision_;
  unsigned short minor_revision_;

  // the number of keys defined in the rest of the tag
  int number_of_geokeys_;

  modeltype_t model_type_;
  rastertype_t raster_type_;
  geographic_t geographic_type_;
  geounits_t geounits_;

  bool gtif_modeltype (modeltype_t& type);
  bool gtif_rastertype (rastertype_t&);
  bool geounits (geounits_t&); 
  bool geographic_type(geographic_t&);

};

#endif //vil_geotiff_header_h_
