#ifndef vsrl_diffusion_h
#define vsrl_diffusion_h

#include <vnl/vnl_matrix.h>
#include <vsrl/vsrl_dense_matcher.h>


//: this class is an abstract class that
// will define various methods used to difuse
// disparity measures across a disparity map

class vsrl_diffusion
{
public:

  // constructor
  vsrl_diffusion(vsrl_dense_matcher *matcher);

  // destructor
  ~vsrl_diffusion();

  // get the disparity of pixel x,y
  double get_disparity(int x, int y);

  // get the width and height of image1
  int get_width();
  int get_height();

  void write_disparity_image(char *filename);

 protected:

  // the width and height of the image
  int _width;
  int _height;

  // the matrix of disparities.
  vnl_matrix<double> *_disparity_matrix;

  // a matcher used to define the dimensions of the data
  vsrl_dense_matcher *_matcher;

  // print an image of the disparity_matrix
  void write_image(char *file_name,vnl_matrix<double> *mat);
  void write_image(char *file_name,int it_num, vnl_matrix<double> *mat);
};

#endif
