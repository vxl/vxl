#ifndef vil_region_finder_h_
#define vil_region_finder_h_
//:
// \file
#include <vcl_vector.h>
#include <vil/vil_image_view.h>

//: Type of connectivity to use in finding the regions
//
// \relates vil_region_finder
//
enum vil_region_finder_connectivity {
  vil_region_finder_4_conn,
  vil_region_finder_8_conn
};

//: Extract regions from an image using a flood-fill.
//
// This class encapsulates a simple flood fill algorithm to extract a
// four or eight connected regions from an image. It uses an auxiliary
// bool image to mark pixels as processed. This mark is not reset
// between calls to the region extraction routine, so each region can
// be extracted only once.
//
template <class pix_type>
class vil_region_finder
{
 public:
  //:
  typedef vil_image_view<pix_type> image_view;

  //: Prepare to extract regions from \a image
  vil_region_finder( image_view const& image,
                     vil_region_finder_connectivity conn = vil_region_finder_4_conn );

  //: Extract the region containing (i,j)
  //
  // This will return the coordinates of all the pixels in the region
  // around (i,j) that has the same intensity as the intensity of
  // pixel (i,j).
  //
  // This is a simple flood fill algorithm.
  // 
  void
  same_int_region( unsigned i, unsigned j,
                   vcl_vector<unsigned>& ri,
                   vcl_vector<unsigned>& rj );

  //: The image from which the regions are being extracted
  image_view const&
  image() const;

 private:
  //:
  // Marks all pixels as unprocessed, and sets the neighbour deltas
  // based on the requested connectivity.
  //
  void
  init( vil_region_finder_connectivity );

  //: The image
  image_view const image_;

  //: The marks
  vil_image_view<bool> processed_;

  //: The size of the nbr_delta_ array
  unsigned num_nbrs_;

  //: The deltas to the neighbours.
  int const (*nbr_delta_)[2];
};

// do the implicit template thing.
#include "vil_region_finder.txx"

#endif // vil_region_finder_h_
