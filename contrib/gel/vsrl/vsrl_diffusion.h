#ifndef vsrl_diffusion_h
#define vsrl_diffusion_h
//:
// \file

#include <vnl/vnl_matrix.h>
#include <vsrl/vsrl_dense_matcher.h>

//: this class is an abstract class that
// will define various methods used to diffuse
// disparity measures across a disparity map

class vsrl_diffusion
{
 protected:

  // the width and height of the image
  int width_;
  int height_;

  // the matrix of disparities.
  vnl_matrix<double> *disparity_matrix_;

  // a matcher used to define the dimensions of the data
  vsrl_dense_matcher *matcher_;

 public:

  // constructor
  vsrl_diffusion(vsrl_dense_matcher *matcher);

  // destructor
  ~vsrl_diffusion();

  // get the disparity of pixel x,y
  double get_disparity(int x, int y);

  // get the width and height of image1
  int get_width() { return width_; }
  int get_height() { return height_; }

  void write_disparity_image(const char *filename);

 protected:

  // print an image of the disparity_matrix
  void write_image(const char *file_name,vnl_matrix<double> *mat);
  void write_image(const char *file_name,int it_num, vnl_matrix<double> *mat);
};

#endif
