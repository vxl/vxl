#include <vcl_iostream.h>
#include <vil1/vil1_save.h>
#include <vsol/vsol_point_2d.h>
#include <vsrl/vsrl_parameters.h>
#include <brip/brip_vil1_float_ops.h>
#include <brct/brct_dense_reconstructor.h>

brct_dense_reconstructor::brct_dense_reconstructor(const vil1_image &im1,
                                                   const vil1_image &im2): 
  vsrl_dense_matcher(brip_vil1_float_ops::convert_to_grey(im1)),
  image_correlation_(image1_, brip_vil1_float_ops::convert_to_grey(im2))
{
  raster_array_=0;
  num_raster_=0;
  correlation_range_= vsrl_parameters::instance()->correlation_range; // probaly 10
}

brct_dense_reconstructor::~brct_dense_reconstructor()
{
  if (raster_array_)
  {
    for (int i=0;i<num_raster_;i++)
      delete raster_array_[i];
    delete [] raster_array_;
  }
}

//: set search range
void brct_dense_reconstructor::set_search_range(const int range)
{
  vsrl_parameters::instance()->correlation_range = range;
}

//: set correlation window radius
void brct_dense_reconstructor::set_correlation_window_radius(const int radius)
{
  int w = 2*radius + 1;
  vsrl_parameters::instance()->correlation_window_width = w;
  vsrl_parameters::instance()->correlation_window_height = w;
}
//: set inner null cost
void brct_dense_reconstructor::set_inner_cost(const double inner_cost)
{
  vsrl_parameters::instance()->inner_cost = inner_cost;
}
//: set outer null cost
void brct_dense_reconstructor::set_outer_cost(const double outer_cost)
{
  vsrl_parameters::instance()->outer_cost = outer_cost;
}
  //: set continuity cost
void brct_dense_reconstructor::set_continuity_cost(const double continuity_cost)
{
  vsrl_parameters::instance()->continuity_cost = continuity_cost;
}

void brct_dense_reconstructor::print_params()
{
  //  vsrl_parameters::instance()->print();
}

void brct_dense_reconstructor::execute()
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

void brct_dense_reconstructor::initial_calculations()
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

int brct_dense_reconstructor::get_disparity(int x, int y)
{
  int new_x = get_assignment(x,y);

  if (new_x >=0)
    return get_assignment(x,y)-x;
  else
    return 0-1000;
}

int brct_dense_reconstructor::get_assignment(int x, int y)
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


void brct_dense_reconstructor::evaluate_raster(const int i)
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


void brct_dense_reconstructor::write_disparity_image(char *filename)
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

void brct_dense_reconstructor::print_correlation_cost(const int x, const int y)
{
  vcl_cout << "Correlation costs for pixel " << x << " " << y << vcl_endl;

  for (int disp = 0-correlation_range_;disp < correlation_range_;disp++)
    vcl_cout << disp << " -> " << image_correlation_.get_correlation(x,y,disp) << vcl_endl;
}
//: get vsol points corresponding to a line from image 0
vcl_vector<vsol_point_2d_sptr> brct_dense_reconstructor::points0(const int i,
                                                                 const int del)
{
  vcl_vector<vsol_point_2d_sptr> points;
  int w = image_correlation_.get_image1_width();
  int h = image_correlation_.get_image1_height();
  if(i<0||i>=h)
    return points;
  for(int x = 0; x<w; x+=del)
    points.push_back(new vsol_point_2d(x, i));
  return points;
}

//: get vsol points corresponding to a line from image 1
vcl_vector<vsol_point_2d_sptr> brct_dense_reconstructor::points1(const int i,
                                                                 const int del)
{
  vcl_vector<vsol_point_2d_sptr> points;
  int w = image_correlation_.get_image1_width();
  int h = image_correlation_.get_image1_height();
  if(i<0||i>=h)
    return points;
  for(int x = 0; x<w; x+=del)
    {
      int xa = this->get_assignment(x, i);
      vcl_cout << "(" << x << " " << xa << ")\n";
      points.push_back(new vsol_point_2d(xa, i));
    }
  vcl_cout << vcl_flush;
  return points;
}
void  brct_dense_reconstructor::get_correlation(const int x0, const int y0,
                                                vcl_vector<int>& xpos,
                                                vcl_vector<double>& corr)
{
  xpos.clear();
  corr.clear();
  int range = image_correlation_.get_correlation_range();
  int w = image_correlation_.get_image2_width();
  for(int x=x0-range; x<=x0+range; ++x)
    if(x>=0&&x<w)
      {
        xpos.push_back(x);
        corr.push_back(image_correlation_.get_correlation(x0, y0, x, y0));
      }
}
