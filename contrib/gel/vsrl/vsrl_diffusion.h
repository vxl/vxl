#ifndef vsrl_diffusion_h
#define vsrl_diffusion_h

#include <vnl/vnl_matrix.h>
#include <vsrl/vsrl_dense_matcher.h>


// this class is an abstract class that
// will define various methods used to difuse
// dispararty measures across a disparaty map


class vsrl_diffusion
{
 public:

  // constructor

  vsrl_diffusion(vsrl_dense_matcher *matcher);

  // destructor

  ~vsrl_diffusion();

  // get the disparity of pixel x,y

  double get_disparaty(int x, int y);

  // get the width and height of image1

  int get_width();
  int get_height();

  void write_disparaty_image(char *filename);

 protected:

  // the width and height of the image
  int _width;
  int _height;

  // the matrix of disparaties.
  vnl_matrix<double> *_disparaty_matrix;

  // a matcher used to define the dimensions of the data
  vsrl_dense_matcher *_matcher;

  // print an image of the disparaty_matrix

  void write_image(char *file_name,vnl_matrix<double> *mat);
  void write_image(char *file_name,int it_num, vnl_matrix<double> *mat);
};

#endif
