//-*-c++-*-
// This is brl/bseg/brip/brip_watershed.h
#ifndef brip_watershed_h_
#define brip_watershed_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Computes the seeded watershed algorithm 
//
//  The watershed algorithm proceeds from a set of seeds that are
//  defined by some other process.  A typical seed generator is to 
//  find local minima of gradient magnitude.  A cost function is defined that
//  provides a measure on pixels in the image.  A typical cost is based on
//  the gradient magnitude. The initial seed is extended to a region by adding
//  the lowest cost pixel.  The region continues to grow by adding lowest cost
//  unlabeled pixels to each pixel in the region.  
//
//  Each seed defines a unique label which is propagated to each pixel in the
//  associated region. Regions are isolated by a boundary with a unique (0)
//  label. 
//
//  Region growth is managed by a priority queue. Neigborhing pixels to each
//  seed are inserted in the queue to get the process started. When a new pixel
//  is added to the region, its unlabeled neighbors are added to the queue.
//
// \verbatim
//  Modifications
//   Initial version June 18, 2004
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_queue.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_2d.h>
#include <vbl/vbl_array_2d.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_region_pixel.h>
#include <brip/brip_watershed_params.h>
class brip_watershed : public brip_watershed_params
{
public:
	enum label {UNLABELED = 0, BOUNDARY=1};
  //:Constructors/destructor
  brip_watershed(brip_watershed_params const& bwp);
  ~brip_watershed();
  //: Accessors/Mutators
  void set_image(vil1_memory_image_of<float> const& image);
  unsigned int min_region_label(){return BOUNDARY + 1;}
  unsigned int max_region_label(){return max_region_label_;}
  vil1_image overlay_image();
  vbl_array_2d<unsigned int>& region_label_array(){return region_label_array_;}
  bool adjacent_regions(const unsigned int region,
                        vcl_vector<unsigned int>& adj_regs);
  //: Main process method 
  bool compute_regions();
  //: Debug methods
  void print_region_array();
  void print_adjacency_map();
protected:
  //: internal methods
  void print_neighborhood(int col, int row, unsigned int lab);
  bool add_adjacency(const unsigned int reg, const unsigned int adj_reg);
  bool compute_seeds();
  bool initialize_queue();
  bool grow_regions();
  //: members
  brip_watershed();//don't use this constructor
  int width_;
  int height_;
  unsigned int max_region_label_;
  vbl_array_2d<unsigned int> region_label_array_;
  vil1_memory_image_of<float> image_;
  vil1_memory_image_of<float> gradient_mag_image_;
  vcl_priority_queue<brip_region_pixel_sptr, vcl_vector<brip_region_pixel_sptr>,    brip_region_pixel::compare> priority_queue_;
  vcl_map<unsigned int, vcl_vector<unsigned int>* > region_adjacency_;

};

#endif // brip_watershed_h_
