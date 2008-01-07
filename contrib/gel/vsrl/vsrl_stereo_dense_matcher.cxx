#include "vsrl_stereo_dense_matcher.h"
//:
// \file

#include "vsrl_parameters.h"
#include <vil1/vil1_save.h>
#include <vcl_iostream.h>

vsrl_stereo_dense_matcher::vsrl_stereo_dense_matcher(const vil1_image &im1, const vil1_image &im2):
  vsrl_dense_matcher(im1),
  image_correlation_(im1,im2)
{
  raster_array_=0;
  num_raster_=0;

  correlation_range_= vsrl_parameters::instance()->correlation_range; // probaly 10
}

vsrl_stereo_dense_matcher::~vsrl_stereo_dense_matcher()
{
  if (raster_array_)
  {
    for (int i=0;i<num_raster_;i++)
      delete raster_array_[i];
    delete [] raster_array_;
  }
}


void vsrl_stereo_dense_matcher::execute()
{
  // we want to perform the dense matching between the two images

  if (!raster_array_)
    // we must perform some initial calculations
    this->initial_calculations();

  // start the dynamic program for each raster

  vcl_cout << "Performing dynamic programs\n";

  for (int i=0;i<num_raster_;i++)
    evaluate_raster(i);
}

void vsrl_stereo_dense_matcher::initial_calculations()
{
  // we want to perform the dense matching between the two images

  // step 1 - compute the correlations between the two images
  //          so that the dynamic programs can perform their calculations efficiently

  vcl_cout << "Performing image correlations\n";

  image_correlation_.set_correlation_range(correlation_range_);

  image_correlation_.initial_calculations();


  // step 2 - create an array of dynamic programs that process each raster
  // and initialize them to zero

  num_raster_ = image_correlation_.get_image1_height();

  typedef vsrl_raster_dp_setup* raster_ptr;
  raster_array_ = new raster_ptr[num_raster_];

  for (int i=0;i<num_raster_;i++)
    raster_array_[i]=0;
}

int vsrl_stereo_dense_matcher::get_disparity(int x,int y)
{
  int new_x = get_assignment(x,y);

  if (new_x >=0)
    return get_assignment(x,y)-x;
  else
    return 0-1000;
}

int vsrl_stereo_dense_matcher::get_assignment(int x, int y)
{
  // we want to get the assignment of pixel x from raster y

  if (y<0 || y >=num_raster_)
    return 0-1;
  else
  {
    if (!(raster_array_[y]))
      // we need to perform the dynamic program on the raster
      this->evaluate_raster(y);

    return raster_array_[y]->get_assignment(x);
  }
}


void vsrl_stereo_dense_matcher::evaluate_raster(int i)
{
  if (i<0 || i>= num_raster_)
    vcl_cout << "Warning tried to evaluate inapropriate raster\n";

  // we want to evaulate the raster i

  vcl_cout << "evaluating raster " << i << vcl_endl;

  // set up the i'th raster array
  vsrl_raster_dp_setup *raster = new vsrl_raster_dp_setup(i, &image_correlation_);

  // if the previous or the next raster has been computed,
  // we wish to use this information to bias the new raster

  if (i>0)
    if (raster_array_[i-1])
      raster->set_prior_raster(raster_array_[i-1]);

  if (i<num_raster_-1)
    if (raster_array_[i+1])
      raster->set_prior_raster(raster_array_[i+1]);

  // set the correlation range for the raster
  raster->set_search_range(correlation_range_);

  // performing the dynamic program
  raster->execute();

  // keep track of the raster
  raster_array_[i]=raster;
}


void vsrl_stereo_dense_matcher::write_disparity_image(char *filename)
{
  // we want to write a disparity image

  // make a buffer which has the size of image1

  vil1_byte_buffer buffer(image1_);

  for (int x=0;x<buffer.width();x++)
    for (int y=0;y<buffer.height();y++)
      buffer(x,y)=0;

  // go through each point, get the disparity and save it into the buffer

  for (int y=0;y<buffer.height();y++)
     for (int x=0;x<buffer.width();x++)
     {
       int disparity = this->get_disparity(x,y);
       int value = disparity + correlation_range_+1;
       if (value < 0)
         value = 0;
       if (value>2*correlation_range_+1)
         value=0;
       buffer(x,y)=value;
     }

  // save the file, using file name extension to determine type
  // vil1_save(buffer, filename, image1_.file_format());
  vil1_save(buffer, filename);
}

// print out the correlation costs for point x,y

void vsrl_stereo_dense_matcher::print_correlation_cost(int x, int y)
{
  vcl_cout << "Correlation costs for pixel " << x << ' ' << y << vcl_endl;

  for (int disp = 0-correlation_range_;disp < correlation_range_;disp++)
    vcl_cout << disp << " -> " << image_correlation_.get_correlation(x,y,disp) << vcl_endl;
}
