#ifndef vsrl_3d_output_h
#define vsrl_3d_output_h

// this program will take the disparity between two images and
// compute a 3d output for the data. If (u,v) -> (u + d, v) then
// one possible set of cameras could be P1 =(1000,0100,0010) and P2
// would equal P2=(1010,0100,0010). In this way the point in space
// would be X=(u,v,d,1) it turns out that (u,v,1) = P1 X and
// (u,v+d,1) = P2 X. This means that X is within a projective
// transformation of the true X.

#include <vsrl/vsrl_dense_matcher.h>
#include <vsrl/vsrl_image_correlation.h>
#include <vnl/vnl_matrix.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vsrl/vsrl_diffusion.h>

class vsrl_3d_output
{
  // the matcher used to compute the data

  vsrl_dense_matcher *matcher_;

  // the image buffers

  vil1_memory_image_of<int> buffer1_;
  vil1_memory_image_of<int> buffer2_;

  // the projective transform used to convert the initial values
  // of X into the true values of X

  vnl_matrix<double> H_;

  vsrl_image_correlation image_correlation_; // the image correlation object

  vil1_image image1_; // the first image
  vil1_image image2_; // the second image

 public:

  // the constructor

  vsrl_3d_output(const vil1_image &im1, const vil1_image &im2);

  // destructor
  ~vsrl_3d_output();

  // set the matcher of the data
  void set_matcher(vsrl_dense_matcher *matcher);

  // set the projective transform

  void set_projective_transform(vnl_matrix<double> &H);

  // read in the transform

  void read_projective_transform(char *filename);

  // write the data

  void write_output(char *filename);

  vil1_memory_image_of<double> get_double_range_image() { return range_image_;}
  vil1_memory_image_of<unsigned char> get_unsigned_range_image() { return rimage_;}

  vil1_memory_image_of<double> range_image_;
  vil1_memory_image_of<unsigned char> rimage_;

 private:

  bool non_valid_point(int x, int y); // identifies non valid points

  void  write_disparity_image(char *filename,vsrl_diffusion *diff);
};

#endif // vsrl_3d_output_h
