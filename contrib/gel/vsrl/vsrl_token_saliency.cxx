


#include <vsrl/vsrl_token_saliency.h>

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_cstddef.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vil/vil_save.h>



vsrl_token_saliency::vsrl_token_saliency(vsrl_image_correlation *image_correlation)
{
  // constructor

  _image_correlation = image_correlation;

  // maybe we will put this in vsrl_parameters ...
  _std_thresh = 5;
  _mean_thresh = 0.2;
  _saliency_matrix=0;
}

vsrl_token_saliency::~vsrl_token_saliency()
{
  delete _saliency_matrix;
}

// the threshold used to decide if a pixel is salient based on
// the std of the local correlation function.

void vsrl_token_saliency::set_std_threshold(double std_thresh)
{
  _std_thresh = std_thresh;
}


double vsrl_token_saliency::get_std_threshold()
{
  return _std_thresh;
}

// the threshold used to decide if a pixel is salient based on
// the mean of the local correlation function.

void vsrl_token_saliency::set_mean_threshold(double mean_thresh)
{
  _mean_thresh = mean_thresh;
}

double vsrl_token_saliency::get_mean_threshold()
{
  return _mean_thresh;
}


int vsrl_token_saliency::get_saliency(int x, int y)
{
  // we want to report the saliency of x and y

  if (!_saliency_matrix){
    this->compute_saliency_matrix();
  }

  if (x>=0 && (unsigned)x <_saliency_matrix->rows() &&
      y>=0 && (unsigned)y <_saliency_matrix->cols()){
    return (*_saliency_matrix)(x,y);
  }
  else
    return 0;
}

// determine the first salient point on the left
int vsrl_token_saliency::get_left_salient_pixel(int x, int y)
{

  int x1;

  for(x1=x;x1>=0 ;x1=x1-1){

    if(get_saliency(x1,y)){
      // this is the left most salient point
      return x1;
    }
  }

  return 0-1;
}

// determine the first salient point on the right
int vsrl_token_saliency::get_right_salient_pixel(int x, int y)
{

  int x1;

  for(x1=x;x1<_saliency_matrix->rows();x1++){

    if(get_saliency(x1,y)){
      // this is the left most salient point
      return x1;
    }
  }

  return 0-1;
}

// determine if the point is saliently bounded
bool vsrl_token_saliency::is_pixel_bounded_saliently(int x, int y)
{
  if(get_left_salient_pixel(x,y)>=0 && get_right_salient_pixel(x,y) >= 0){
    // this pixel is salient
    return true;
  }
  return false;
}


void vsrl_token_saliency::get_saliency_stats(int x, int y, double &mean, double &std)
{
  // we want to get the saliency stats;

  _image_correlation->get_correlation_stats(x,y,mean,std);

  std=_image_correlation->get_std_1(x,y);
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
  int x;

  for(x=0;x<_saliency_matrix->rows();x++){
    print_saliency_stats(x,y);
  }
}

void vsrl_token_saliency::print_saliency(int y)
{
  int x;

  for(x=0;x<_saliency_matrix->rows();x++){
    print_saliency(x,y);
  }
}

void vsrl_token_saliency::create_saliency_image(char *filename)
{
  // we want to create a saliency image

  compute_saliency_matrix();

  int width=_saliency_matrix->rows();
  int height= _saliency_matrix->cols();

  // make an image buffer

  vil_byte_buffer buffer(width,height);

  int x,y;

  // copy the matrix into the byte buffer

  for(x=0;x<width;x++){
    for(y=0;y<height;y++){
      buffer(x,y)= (*_saliency_matrix)(x,y);
    }
  }

  // save the file

  vil_save(buffer,filename);
}

void vsrl_token_saliency::compute_saliency_matrix()
{
  // we want to create a saliency matrix

  // allocate the saliency matrix

  int width = _image_correlation->get_image1_width();
  int height = _image_correlation->get_image1_height();

  if(_saliency_matrix){
    return;
  }



  _saliency_matrix = new vnl_matrix<int>(width,height);

  _saliency_matrix->fill(0-1);

  // ok we define saliency based on image correlation statistics

  int x,y;


  double mean,std;

  for(x=0;x<width;x++){
    for(y=0;y<height;y++){

      // get the local correlation statistics

      get_saliency_stats(x,y,mean,std);

      // if(mean > _mean_thresh && std > _std_thresh)
      if(mean < _mean_thresh || std < _std_thresh)
        {
          // we have a pixel in a relatively homogeneouse region
          // so we will assume that this is not a salient point
          (*_saliency_matrix)(x,y)=0;
        }
      else{
        (*_saliency_matrix)(x,y)=1;
      }
    }
  }
}
