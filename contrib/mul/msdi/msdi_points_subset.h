#ifndef msdi_points_subset_h_
#define msdi_points_subset_h_

// :
// \file
// \author Tim Cootes
// \brief Adaptor which steps through images and returns a subset of original points

#include <msdi/msdi_marked_images.h>
#include <msm/msm_points.h>

// : Adaptor which steps through images and returns a subset of original points
//  Given a source msdi_marked_images, generates a subset of each set of points.
class msdi_points_subset : public msdi_marked_images
{
private:
  // : Original data
  msdi_marked_images& marked_images_;

  // : Indicate which points to return in subset
  vcl_vector<unsigned> index_;

  // : Current points
  msm_points points_;

  // : True if points are current
  bool points_ok_;

  // Private copy operator to prevent copying
  msdi_points_subset & operator=(const msdi_points_subset &);

public:
  // : Default constructor
  // /params index indicates subset of points to return
  msdi_points_subset(msdi_marked_images& raw_data, const vcl_vector<unsigned>& subset_index);

  // : Construct with external vectors of images and points
  //  Pointers retained to both - they must stay in scope.
  msdi_points_subset();

  // : Destructor
  virtual ~msdi_points_subset();

  // : Move to start of data
  virtual void reset();

  // : Move to next item.  Return true until reach end of items
  virtual bool next();

  // : Return number of examples this will provide
  virtual unsigned size() const;

  // : Return current image
  virtual const vimt_image_2d & image();

  // : Return current image pyramid
  virtual const vimt_image_pyramid & image_pyr();

  // : points for the current image
  virtual const msm_points & points();

  // : Return current image file name
  virtual vcl_string image_name() const;

  // : Return current points file name
  virtual vcl_string points_name() const;

};

#endif // msdi_points_subset_h_
