//This is brl/bbas/volm/volm_tile.h
#ifndef volm_tile_h_
#define volm_tile_h_
//:
// \file
// \brief A class to hold volumetric matchers
// units are in meters
//
// \author Ozge C. Ozcanli
// \date September 18, 2012

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>

class volm_tile
{
 public:
   float lat_, lon_;  // lower left corner of the tile
   char hemisphere_;  // 'N' or 'S'
   char direction_;   // 'E' or 'W'
   float scale_i_;      // the total extent of the tile (E W direction)
   float scale_j_;      // the total extent of the tile (N S direction)
   unsigned ni_;
   unsigned nj_;
   vpgl_geo_camera cam_; // the output of this camera cannot be directly used, if 'W' use -lon
   
   volm_tile(float lat, float lon, char hemisphere, char direction, float scale_i, float scale_j, unsigned ni, unsigned nj);
   vcl_string get_string();
   
   void img_to_global(unsigned i, unsigned j, double& lon, double& lat);
   
   //: create a kml file of the tile as a box and with circular marks throughout at every n pixels in each direction
   void write_kml(vcl_string name, int n);
   
   //: mark the uncertainty region around a given location using a gaussian mask, the center has the designated score, the rest diminishes with respect to a cutoff degree
   //  lambda is the radius of the gaussian mask in pixels
   //  cut_off is the percentage that shows how quickly the kernel dies off, typically 50
   static void get_uncertainty_region(float lambda_i, float lambda_j, float cut_off, vbl_array_2d<bool>& mask, vbl_array_2d<float>& kernel);
   static void mark_uncertainty_region(int i, int j, float score, vbl_array_2d<bool>& mask, vbl_array_2d<float>& kernel, vil_image_view<unsigned int>& img);
   
   //static volm_tile parse_string(vcl_string& filename);
   
   static vcl_vector<volm_tile> generate_p1_tiles();
   static vcl_vector<volm_tile> generate_p1_wr1_tiles();
};


#endif // volm_tile_h_
