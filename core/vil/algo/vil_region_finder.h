#ifndef vil_region_finder_h_
#define vil_region_finder_h_
//:
// \file
// \verbatim
//  Modifications
//   March.2005 - Gehua Yang - template on predicate to make it generic
// \endverbatim

#include <vector>
#include <functional>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>

//: Type of connectivity to use in finding the regions
//
// \relatesalso vil_region_finder
//
enum vil_region_finder_connectivity
{
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
template <class pix_type, class predicate_type = std::equal_to<pix_type> >
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
  // around (i,j) where the predicate claims true compared with the intensity of
  // pixel (i,j).
  //
  // This is a simple flood fill algorithm.
  //
  void
  same_int_region( unsigned i, unsigned j,
                   std::vector<unsigned>& ri,
                   std::vector<unsigned>& rj );


  //: Extract the region containing (i,j)
  //
  // This will return the coordinates of all the pixels in the region
  // around (i,j) where the predicate claims true compared with the intensity p
  //
  // This is a simple flood fill algorithm.
  //
  void
  same_int_region( unsigned i, unsigned j, pix_type p,
                   std::vector<unsigned>& ri,
                   std::vector<unsigned>& rj );

  //: The image from which the regions are being extracted
  image_view const&
  image() const;

  //: boolean mask on the region
  vil_image_view<bool> const&  boolean_region_image() const;

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

  //: predicate
  predicate_type predi_;
};

// do the implicit template thing.
#include "vil_region_finder.hxx"

#endif // vil_region_finder_h_
