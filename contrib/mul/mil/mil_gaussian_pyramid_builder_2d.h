#ifndef mil_gaussian_pyramid_builder_2d_h_
#define mil_gaussian_pyramid_builder_2d_h_
#ifdef __GNUC__
#pragma interface
#endif

//: \file
//  \brief Build gaussian pyramids of mil_image_2d_of<T>
//  \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_image_pyramid_builder.h>

//: Build gaussian pyramids of mil_image_2d_of<T>
//  Smooth with a gaussian filter (1-5-8-5-1 by default)
//  and subsample so that image at level i-1 is half the
//  size of that at level i
template <class T>
class mil_gaussian_pyramid_builder_2d : public mil_image_pyramid_builder {
private:
  int max_levels_;

  mutable mil_image_2d_of<T> work_im_;

  //: Checks pyramid has at least n levels of correct type
  void checkPyr(mil_image_pyramid& im_pyr,  int n_levels);

  //: Deletes all data in im_pyr
  void emptyPyr(mil_image_pyramid& im_pyr);

public:

		//: Dflt ctor
	mil_gaussian_pyramid_builder_2d();

		//: Destructor
	virtual ~mil_gaussian_pyramid_builder_2d();
	
		//: Create new (empty) pyramid on heap
		//  Caller responsible for its deletion
	virtual mil_image_pyramid* newImagePyramid() const;

		//: Define maximum number of levels to build
		//  Limits levels built in subsequent calls to build()
		//  Useful efficiency measure.  As build() only takes
		//  a shallow copy of the original image, using
		//  max_l=1 avoids any copying or smoothing.
	virtual void setMaxLevels(int max_l);

		//: Get the current maximum number levels allowed
	virtual int maxLevels();

	
		//: Build pyramid
	virtual void build(mil_image_pyramid&, const mil_image&);

		//: Smooth and subsample src_im to produce dest_im
		//  Applies 1-5-8-5-1 filter in x and y, then samples
		//  every other pixel.
	void gaussReduce(mil_image_2d_of<T>& dest_im,
					 const mil_image_2d_of<T>& src_im);

		//: Scale step between levels
	virtual double scaleStep() const;
	
		//: Version number for I/O 
	short version_no() const;

		//: Name of the class
	virtual vcl_string is_a() const;

		//: Create a copy on the heap and return base class pointer
	virtual mil_image_pyramid_builder* clone() const;

		//: Print class to os
	virtual void print_summary(vcl_ostream& os) const;
		
		//: Save class to binary file stream
		//!in: bfs: Target binary file stream
	virtual void b_write(vsl_b_ostream& bfs) const;

		//: Load class from binary file stream
		//!out: bfs: Target binary file stream
	virtual void b_read(vsl_b_istream& bfs);
};

#endif



