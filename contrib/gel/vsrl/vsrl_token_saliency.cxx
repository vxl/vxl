// This is gel/vsrl/vsrl_token_saliency.cxx
#include "vsrl_token_saliency.h"
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vil/vil_save.h>

vsrl_token_saliency::vsrl_token_saliency(vsrl_image_correlation *image_correlation)
{
  // constructor

  image_correlation_ = image_correlation;

  // maybe we will put this in vsrl_parameters ...
  std_thresh_ = 5;
  mean_thresh_ = 0.2;
  saliency_matrix_=0;
}

vsrl_token_saliency::~vsrl_token_saliency()
{
  delete saliency_matrix_;
}

// the threshold used to decide if a pixel is salient based on
// the std of the local correlation function.

void vsrl_token_saliency::set_std_threshold(double std_thresh)
{
  std_thresh_ = std_thresh;
}


double vsrl_token_saliency::get_std_threshold()
{
  return std_thresh_;
}

// the threshold used to decide if a pixel is salient based on
// the mean of the local correlation function.

void vsrl_token_saliency::set_mean_threshold(double mean_thresh)
{
  mean_thresh_ = mean_thresh;
}

double vsrl_token_saliency::get_mean_threshold()
{
  return mean_thresh_;
}


int vsrl_token_saliency::get_saliency(int x, int y)
{
  // we want to report the saliency of x and y

  if (!saliency_matrix_){
    this->compute_saliency_matrix();
  }

  if (x>=0 && (unsigned)x <saliency_matrix_->rows() &&
      y>=0 && (unsigned)y <saliency_matrix_->cols()){
    return (*saliency_matrix_)(x,y);
  }
  else
    return 0;
}

// determine the first salient point on the left
int vsrl_token_saliency::get_left_salient_pixel(int x, int y)
{
  assert(x>=0);
  for (int x1=x;x1>=0;--x1)
    if (get_saliency(x1,y)) // this is the left most salient point
      return x1;

  return 0-1;
}

// determine the first salient point on the right
int vsrl_token_saliency::get_right_salient_pixel(int x, int y)
{
  assert(x>=0);
  for (unsigned int x1=x; x1<saliency_matrix_->rows(); ++x1)
    if (get_saliency(x1,y)) // this is the left most salient point
      return x1;

  return 0-1;
}

// determine if the point is saliently bounded
bool vsrl_token_saliency::is_pixel_bounded_saliently(int x, int y)
{
  return get_left_salient_pixel(x,y) >= 0 && get_right_salient_pixel(x,y) >= 0;
}


void vsrl_token_saliency::get_saliency_stats(int x, int y, double &mean, double &std)
{
  // we want to get the saliency stats;

  image_correlation_->get_correlation_stats(x,y,mean,std);

  std=image_correlation_->get_std_1(x,y);
}


void vsrl_token_saliency::print_saliency_stats(int x, int y)
{
  double mean,std;
  get_saliency_stats(x,y,mean,std);

  vcl_cout << "Pixel " << x << " " << y << " has a local corelation mean and std " << mean << " " << std << vcl_endl;
}

void vsrl_token_saliency::print_saliency(int x, int y)
{
  vcl_cout << "Pixel " << x << " " << y << " has saliency " << get_saliency(x,y)  << vcl_endl;
}

void vsrl_token_saliency::print_saliency_stats(int y)
{
  for (unsigned int x=0;x<saliency_matrix_->rows();x++)
    print_saliency_stats(x,y);
}

void vsrl_token_saliency::print_saliency(int y)
{
  for (unsigned int x=0;x<saliency_matrix_->rows();x++)
    print_saliency(x,y);
}

void vsrl_token_saliency::create_saliency_image(char *filename)
{
  // we want to create a saliency image

  compute_saliency_matrix();

  unsigned int width=saliency_matrix_->rows();
  unsigned int height= saliency_matrix_->cols();

  // make an image buffer

  vil_byte_buffer buffer(width,height);

  // copy the matrix into the byte buffer

  for (unsigned int x=0;x<width;x++)
    for (unsigned int y=0;y<height;y++)
      buffer(x,y)= (*saliency_matrix_)(x,y);

  // save the file

  vil_save(buffer,filename);
}

void vsrl_token_saliency::compute_saliency_matrix()
{
  // we want to create a saliency matrix

  // allocate the saliency matrix

  int width = image_correlation_->get_image1_width();
  int height = image_correlation_->get_image1_height();

  if (saliency_matrix_)
    return;

  saliency_matrix_ = new vnl_matrix<int>(width,height);

  saliency_matrix_->fill(0-1);

  // ok we define saliency based on image correlation statistics

  for (int x=0;x<width;x++)
    for (int y=0;y<height;y++)
    {
      // get the local correlation statistics

      double mean, std;
      get_saliency_stats(x,y,mean,std);

      // if (mean > mean_thresh_ && std > std_thresh_)
      if (mean < mean_thresh_ || std < std_thresh_)
      {
        // we have a pixel in a relatively homogeneous region
        // so we will assume that this is not a salient point
        (*saliency_matrix_)(x,y)=0;
      }
      else
        (*saliency_matrix_)(x,y)=1;
    }
}
