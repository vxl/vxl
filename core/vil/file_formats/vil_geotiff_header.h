// This is core/vil/file_formats/vil_geotiff_header.h
#ifndef vil_geotiff_header_h_
#define vil_geotiff_header_h_
//:
// \file
// \author    Gamze Tunali
// \date      Jan 31, 2007
// \brief A header structure for geotiff files
//
// This class is responsible for extracting (putting) information
// from (into) the geotiff header that is required to specify a vil_image_resource
// There are bool flags that indicate that the item has been successfully
// read (written) to the open geotiff file.
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <tiffio.h>
#include <geotiffio.h>

class vil_geotiff_header
{
 public:

  typedef enum {UNDEF=-1, NORTH=0, SOUTH=1} GTIF_HEMISPH;
  vil_geotiff_header(TIFF* tif);

  // destructor frees up gtif
  virtual ~vil_geotiff_header() { GTIFFree(gtif_); }

  int gtif_number_of_keys() const { return number_of_geokeys_; }

  bool gtif_tiepoints(std::vector<std::vector<double> > &tiepoints);

  bool gtif_pixelscale(double &scale_x, double &scale_y, double &scale_z);

  //:returns the matrix in the argument
  bool gtif_trans_matrix (double* &trans_matrix);

  //: returns the Zone and the Hemisphere (0 for N, 1 for S);
  bool PCS_WGS84_UTM_zone(int &zone, GTIF_HEMISPH &hemisph);

  //: returns the Zone and the Hemisphere (0 for N, 1 for S);
  bool PCS_NAD83_UTM_zone(int &zone, GTIF_HEMISPH &hemisph);

  //: returns true if in geographic coords, linear units are in meters and angular units are in degrees
  bool GCS_WGS84_MET_DEG();

  //: <key> : key id
  // <value>: a single value or an array of values
  // <size>:  the size of individual key values
  // <length> : the number of values in the value array
  // <type>: the type of the key
  bool get_key_value(geokey_t key, void** value,
                     int& size, int& length, tagtype_t& type);

  void print_gtif(){ if (gtif_) GTIFPrint(gtif_, nullptr, nullptr); }

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
