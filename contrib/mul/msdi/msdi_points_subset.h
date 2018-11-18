#ifndef msdi_points_subset_h_
#define msdi_points_subset_h_

//:
// \file
// \author Tim Cootes
// \brief Adaptor which steps through images and returns a subset of original points

#include <msdi/msdi_marked_images.h>
#include <msm/msm_points.h>

//: Adaptor which steps through images and returns a subset of original points
//  Given a source msdi_marked_images, generates a subset of each set of points.
class msdi_points_subset : public msdi_marked_images {
private:
  //: Original data
  msdi_marked_images& marked_images_;

  //: Indicate which points to return in subset
  std::vector<unsigned> index_;

  //: Current points
  msm_points points_;

  //: True if points are current
  bool points_ok_;

  // Private copy operator to prevent copying
  msdi_points_subset& operator=(const msdi_points_subset&);
public:
    //: Default constructor
    // /params index indicates subset of points to return
  msdi_points_subset(msdi_marked_images& raw_data,
                             const std::vector<unsigned>& subset_index);

    //: Construct with external vectors of images and points
    //  Pointers retained to both - they must stay in scope.
  msdi_points_subset();

    //: Destructor
  ~msdi_points_subset() override;

  //: Move to start of data
  void reset() override;

  //: Move to next item.  Return true until reach end of items
  bool next() override;

  //: Return number of examples this will provide
  unsigned size() const override;

  //: Return current image
  const vimt_image_2d& image() override;

  //: Return current image pyramid
  const vimt_image_pyramid& image_pyr() override;

  //: points for the current image
  const msm_points& points() override;

    //: Return current image file name
  std::string image_name() const override;

    //: Return current points file name
  std::string points_name() const override;
};

#endif // msdi_points_subset_h_
