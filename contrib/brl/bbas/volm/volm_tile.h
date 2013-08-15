//This is brl/bbas/volm/volm_tile.h
#ifndef volm_tile_h_
#define volm_tile_h_
//:
// \file
// \brief A class to hold volumetric matchers
// Units are in meters
//
// \author Ozge C. Ozcanli
// \date September 18, 2012
// \verbatim
// Modifications
//  Yi Dong - May 08 2012 - add accessors
//  Yi Dong - July 31 2013 - modify the transfer matrix to handle southern hemisphere case
//  Yi Dong - Aug 04 2013 - add accessors
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_2d.h>

class volm_tile
{
 public:
   //: specify lat lon in positive numbers but specify hemisphere ('N' or 'S') and direction ('W' or 'E')
   volm_tile(float lat, float lon, char hemisphere, char direction, float scale_i, float scale_j, unsigned ni, unsigned nj);
   //: specify lat lon as regular, e.g. negative lon for 'W'
   volm_tile(float lat, float lon, float scale_i, float scale_j, unsigned ni, unsigned nj);

   //: parse the name string and construct tile, for now only parses for 'N' and 'W'
   volm_tile(vcl_string name, unsigned ni, unsigned nj);

   volm_tile() {}
   vcl_string get_string();

   //: assessors
   unsigned ni() { return ni_; }
   unsigned nj() { return nj_; }
   float scale_i() { return scale_i_; }
   float scale_j() { return scale_j_; }
   char hemisphere() const { return hemisphere_; }
   char direction() const { return direction_; }

   //: do the conversions in reporting lon,lat
   double lower_left_lon();
   double lower_left_lat();

   //: create a bounding box where lon is x and lat is y and lower left corner has geo coords
   vgl_box_2d<float> bbox();
   vgl_box_2d<double> bbox_double();

   void img_to_global(unsigned i, unsigned j, double& lon, double& lat);
   
   bool global_to_img(double lon, double lat, unsigned& i, unsigned& j);

   //: create a kml file of the tile as a box and with circular marks throughout at every n pixels in each direction
   void write_kml(vcl_string name, int n);

   //: mark the uncertainty region around a given location using a gaussian mask, the center has the designated score, the rest diminishes with respect to a cutoff degree
   //  \p lambda_i and \p lambda_j are the radii of the gaussian mask in pixels
   //  \p cut_off is the percentage that shows how quickly the kernel dies off, typically 50
   //  overwrites the value in the image, it assumes that current value of the pixel is compared to this score before this function is called
   static void get_uncertainty_region(float lambda_i, float lambda_j, float cut_off, vbl_array_2d<bool>& mask, vbl_array_2d<float>& kernel);
   static void mark_uncertainty_region(int i, int j, float score, vbl_array_2d<bool>& mask, vbl_array_2d<float>& kernel, vil_image_view<unsigned int>& img);
   static void mark_uncertainty_region(int i, int j, float score, vbl_array_2d<bool>& mask, vbl_array_2d<float>& kernel, vil_image_view<vxl_byte>& img);
#if 0
   static volm_tile parse_string(vcl_string& filename);
#endif
   static vcl_vector<volm_tile> generate_p1_tiles();
   static vcl_vector<volm_tile> generate_p1_wr1_tiles();
   static vcl_vector<volm_tile> generate_p1_wr2_tiles();
   static vcl_vector<volm_tile> generate_p1b_wr1_tiles();
   static vcl_vector<volm_tile> generate_p1b_wr2_tiles();
   static vcl_vector<volm_tile> generate_p1b_wr3_tiles();
   static vcl_vector<volm_tile> generate_p1b_wr4_tiles();
   static vcl_vector<volm_tile> generate_p1b_wr5_tiles();
   static vcl_vector<volm_tile> generate_p1b_wr_tiles(int world_id);
   //: calculate width of the tile
   double calculate_width();
   double calculate_height();
   
   //: Binary save self to stream.
   void b_write(vsl_b_ostream &os) const;

   //: Binary load self from stream.
   void b_read(vsl_b_istream &is);

   //: Return IO version number;
   short version() const { return 1; }  
  
 public: 
  float lat_, lon_;  // lower left corner of the tile
  char hemisphere_;  // 'N' or 'S'
  char direction_;   // 'E' or 'W'
  float scale_i_;      // the total extent of the tile (E W direction)
  float scale_j_;      // the total extent of the tile (N S direction)
  unsigned ni_;
  unsigned nj_;
  vpgl_geo_camera cam_; // the output of this camera cannot be directly used, if 'W' use -lon
};


#endif // volm_tile_h_
