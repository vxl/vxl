#include "vsrl/vsrl_stereo_dense_matcher.h"
#include <vsrl/vsrl_parameters.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>

vsrl_stereo_dense_matcher::vsrl_stereo_dense_matcher(const vil_image &im1, const vil_image &im2):
  vsrl_dense_matcher(im1),
  _image_correlation(im1,im2)
{
  _raster_array=0;
  _num_raster=0;

  _correlation_range= vsrl_parameters::instance()->correlation_range; // probaly 10
}

vsrl_stereo_dense_matcher::~vsrl_stereo_dense_matcher()
{
  if (_raster_array)
  {
    for (int i=0;i<_num_raster;i++)
      delete _raster_array[i];
    delete [] _raster_array;
  }
}


void vsrl_stereo_dense_matcher::execute()
{
  // we want to perform the dense matching between the two images

  if (!_raster_array)
    // we must perform some initial calculations
    this->initial_calculations();

  // start the dynamic program for each raster

  vcl_cout << "Performing dynamic programs " << vcl_endl;

  for (int i=0;i<_num_raster;i++)
    evaluate_raster(i);
}

void vsrl_stereo_dense_matcher::initial_calculations()
{
  // we want to perform the dense matching between the two images

  // step 1 - compute the correlations between the two images
  //          so that the dynamic programs can perform their calculations efficiently

  vcl_cout << "Performing image correlations " << vcl_endl;

  _image_correlation.set_correlation_range(_correlation_range);

  _image_correlation.initial_calculations();


  // step 2 - create an array of dynamic programs that process each raster
  // and initialize them to zero

  _num_raster = _image_correlation.get_image1_height();

  typedef vsrl_raster_dp_setup* raster_ptr;
  _raster_array = new raster_ptr[_num_raster];

  for (int i=0;i<_num_raster;i++)
    _raster_array[i]=0;
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

  if (y<0 || y >=_num_raster)
    return 0-1;
  else
  {
    if (!(_raster_array[y]))
      // we need to perform the dynamic program on the raster
      this->evaluate_raster(y);

    return _raster_array[y]->get_assignment(x);
  }
}


void vsrl_stereo_dense_matcher::evaluate_raster(int i)
{
  if (i<0 || i>= _num_raster)
    vcl_cout << "Warning tried to evaluate inapropriate raster " << vcl_endl;

  // we want to evaulate the raster i

  vcl_cout << "evaluating raster " << i << vcl_endl;

  // set up the i'th raster array
  vsrl_raster_dp_setup *raster = new vsrl_raster_dp_setup(i, &_image_correlation);

  // if the previous or the next raster has been computed,
  // we wish to use this information to bias the new raster

  if (i>0)
    if (_raster_array[i-1])
      raster->set_prior_raster(_raster_array[i-1]);

  if (i<_num_raster-1)
    if (_raster_array[i+1])
      raster->set_prior_raster(_raster_array[i+1]);

  // set the correlation range for the raster
  raster->set_search_range(_correlation_range);

  // performing the dynamic program
  raster->execute();

  // keep track of the raster
  _raster_array[i]=raster;
}


void vsrl_stereo_dense_matcher::write_disparity_image(char *filename)
{
  // we want to write a disparity image

  // make a buffer which has the size of image1

  vil_byte_buffer buffer(_image1);

  for (int x=0;x<buffer.width();x++)
    for (int y=0;y<buffer.height();y++)
      buffer(x,y)=0;

  // go through each point, get the disparity and save it into the buffer

  for (int y=0;y<buffer.height();y++)
     for (int x=0;x<buffer.width();x++)
     {
       int disparity = this->get_disparity(x,y);
       int value = disparity + _correlation_range+1;
       if (value < 0)
         value = 0;
       if (value>2*_correlation_range+1)
         value=0;
       buffer(x,y)=value;
     }

  // save the file, using file name extension to determine type
  // vil_save(buffer, filename, _image1.file_format());
  vil_save(buffer, filename);
}

// print out the correlation costs for point x,y

void vsrl_stereo_dense_matcher::print_correlation_cost(int x, int y)
{
  vcl_cout << "Correlation costs for pixel " << x << " " << y << vcl_endl;

  for (int disp = 0-_correlation_range;disp < _correlation_range;disp++)
    vcl_cout << disp << " -> " << _image_correlation.get_correlation(x,y,disp) << vcl_endl;
}
