//:
// \file
#include <vul/vul_timer.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <brip/brip_vil1_float_ops.h>
#include <brip/brip_para_cvrg.h>


//-----------------------------------------------------------------------
//: Variable Initialization
void brip_para_cvrg::init_variables()
{
  width_ = int(3*sigma_);
  proj_n_ = 2*proj_width_ + 1;
  sup_proj_ = new float[proj_n_];
  proj_0_ = new float[proj_n_];
  proj_45_ = new float[proj_n_];
  proj_90_ = new float[proj_n_];
  proj_135_ = new float[proj_n_];
}


//-----------------------------------------------------------------------
//:  Image Initialization
void brip_para_cvrg::init(vil1_image const & image)
{
  vul_timer t;
  //we don't have roi capability so just use the whole image for now
  int w = image.width(), h = image.height();
  xstart_ = 0;
  ystart_ = 0;
  xsize_ = w;
  ysize_ = h;

  vcl_cout << "xstart = " << xstart_ << " ystart_ = " << ystart_ << '\n'
           << "xsize = " << xsize_ << " ysize_ = " << ysize_ << '\n';

  image_ = brip_vil1_float_ops::convert_to_float(image);
  avg_ = make_float_image(w, h);
  grad0_ = make_float_image(w, h);
  grad45_ = make_float_image(w, h);
  grad90_ = make_float_image(w, h);
  grad135_ = make_float_image(w, h);
  det_ = make_float_image(w, h);
  dir_ = make_float_image(w, h);
  this->init_variables();
  vcl_cout << "Do Initialization in " << t.real() << " msecs\n";
}

//-----------------------------------------------------------------------------
//
//: Constructor s
//
brip_para_cvrg::brip_para_cvrg(float sigma, float thresh,
                               float gauss_tail, int proj_width,
                               int proj_height, int sup_radius,
                               bool v) :
  brip_para_cvrg_params(sigma, thresh, gauss_tail, proj_width, proj_height,
                        sup_radius, v)
{
  this->init_variables();
}

brip_para_cvrg::brip_para_cvrg(brip_para_cvrg_params& pdp) :
  brip_para_cvrg_params(pdp)
{
  this->init_variables();
}

//-----------------------------------------------------------------------------
//
//: Destructor.
//
brip_para_cvrg::~brip_para_cvrg()
{
  vul_timer t;
  free_float_image(avg_);
  free_float_image(grad0_);
  free_float_image(grad45_);
  free_float_image(grad90_);
  free_float_image(grad135_);
  free_float_image(dir_);
  free_float_image(det_);
  delete [] proj_0_;
  delete [] proj_45_;
  delete [] proj_90_;
  delete [] proj_135_;
  delete [] sup_proj_;

  vcl_cout << "Destruct in " << t.real() << " msecs\n";
}


//-----------------------------------------------------------------------------
//
//: Returns an m*n array of floats.  Private.
//
float **brip_para_cvrg::make_float_image(int m, int n)
{
  float *array = new float[m*n];
  float **data = new float* [m];
  for (int i =0; i < m; i++)
  {
    data[i] = &array[i*n];
  }
  return data;
}


//-----------------------------------------------------------------------------
//
//: Frees an m*n array of floats.  Private.
//
void brip_para_cvrg::free_float_image(float **image)
{
 float* array = image[0];
  delete [] array;
  delete [] image;
}


//-----------------------------------------------------------------------------
//
//: Returns a vector of floats length m.  Private.
//
float *brip_para_cvrg::make_float_vector(int m)
{
  float *tmp;

  tmp = new float[m];
  if (tmp == NULL)
  {
    vcl_cout << "Can't allocate a " << m << " vector\n";
    vcl_exit(1);
  }

  return tmp;
}


//-----------------------------------------------------------------------------
//
//: Sets a floating point image to val.  Private.
//
void brip_para_cvrg::set_float_image(float **image, float val)
{
  int x, y;
  float* ptr = image[0];
  // copy first col
  for (y=0;y<image_.height();y++)
    ptr[y] = val;

  for (x=1; x < image_.width();x++)
  {
    ptr = image[x];
    vcl_memcpy((char*)ptr, (char*)image[x-1], image_.height()*sizeof(float));
  }
}


//-----------------------------------------------------------------------------
//
//: Sets a floating point image to val.  Private.
//
void brip_para_cvrg::set_float_vector(float *vector, int n, float val)
{
  int x;
  for (x = 0; x<n; x++)
    vector[x]=val;
}


//-----------------------------------------------------------------------------
//
//: Copies float image1 to image2.  Private.
//
void brip_para_cvrg::copy_image(float **image1, float **image2)
{
  vcl_memcpy((char*)image2[0], (char*)image1[0],
             image_.width()* image_.height() * sizeof(float));
#if 0 // was:
  for (int x=0;x<image_.width();++x)
    for (int y=0;y<image_.height();++y)
      image2[x][y] = image1[x][y];
#endif // 0
}


//-----------------------------------------------------------------------------
//
//: Convolves the image with the smoothing kernel.  Private.
//
void brip_para_cvrg::smooth_image()
{
  vul_timer t;
  smooth_ = brip_vil1_float_ops::gaussian(image_, sigma_);
  vcl_cout << "Smooth image in " << t.real() << " msecs\n";
}


//----------------------------------------------------------
//: Compute the average value in the 7x7 window
void brip_para_cvrg::avg(int x, int y, vil1_memory_image_of<float> const& smooth, float** avg)
{
  float sum =0;
  for (int i = -3; i<=3; i++)
    for (int j = -3; j<=3; j++)
    {
      sum += smooth(x+i, y+j);
    }

  avg[x][y] = sum/49.0;
}


//----------------------------------------------------------
//: Compute a gradient operator at x,y along the x axis
void brip_para_cvrg::grad0(int x, int y, vil1_memory_image_of<float> const& smooth, float** grad0)
{
  float plus = 0.5*smooth(x+1,y+3) + smooth(x+1,y+2) + smooth(x+1,y+1) +
    smooth(x+1,y) + smooth(x+1,y-1) + smooth(x+1,y-2)
    + 0.5*smooth(x+1,y-3);
  float minus = 0.5*smooth(x-1,y+3) + smooth(x-1,y+2) + smooth(x-1,y+1) +
    smooth(x-1,y) + smooth(x-1,y-1) + smooth(x-1,y-2) + 0.5*smooth(x-1,y-3);
  grad0[x][y] = (plus-minus);
}


//----------------------------------------------------------
//: Compute a gradient operator at x,y at 45 degrees
void brip_para_cvrg::grad45(int x, int y, vil1_memory_image_of<float> const& smooth, float** grad45)
{
  float plus = smooth(x-2,y+3) + smooth(x-1,y+2) + smooth(x,y+1)
    + smooth(x+1,y) + smooth(x+2,y-1) + smooth(x+3,y-2);
  float minus = smooth(x-3,y+2) + smooth(x-2,y+1) + smooth(x-1,y)
    + smooth(x,y-1) + smooth(x+1,y-2) + smooth(x+2,y-3);

  grad45[x][y] = 1.30*(plus-minus);
}


//----------------------------------------------------------
//: Compute a gradient operator at x,y at 90 degrees
void brip_para_cvrg::grad90(int x, int y, vil1_memory_image_of<float> const& smooth, float** grad90)
{
  float plus = 0.5*smooth(x+3,y+1) + smooth(x+2,y+1) + + smooth(x+1,y+1) +
    smooth(x,y+1) + smooth(x-1,y+1) + smooth(x-2,y+1) + 0.5*smooth(x-3,y+1);
  float minus =0.5*smooth(x+3,y-1) + smooth(x+2,y-1)+ smooth(x+1,y-1) +
    smooth(x,y-1) + smooth(x-1,y-1) + smooth(x-2,y-1) + 0.5*smooth(x-3,y-1);
  grad90[x][y] = (plus-minus);
}


//----------------------------------------------------------
//: Compute a gradient operator at x,y at 135 degrees
void brip_para_cvrg::grad135(int x, int y, vil1_memory_image_of<float> const& smooth, float** grad135)
{
  float plus = smooth(x+3,y+2) + smooth(x+2,y+1) + smooth(x+1,y)
    + smooth(x,y-1) + smooth(x-1,y-2) + smooth(x-2,y-3);
  float minus = smooth(x+2,y+3) + smooth(x+1,y+2) + smooth(x,y+1)
    + smooth(x-1,y) + smooth(x-2,y-1) + smooth(x-3,y-2);

  grad135[x][y] = 1.3*(plus-minus);
}


//-----------------------------------------------------------------------------
//
//: Convolves with the kernel in the x direction, to compute the local derivative in that direction.  Private.
//
void brip_para_cvrg::compute_gradients()
{
  vul_timer t;
  int x,y;
  int radius = width_+3;
  set_float_image(grad0_,0.0);
  set_float_image(grad45_,0.0);
  set_float_image(grad90_,0.0);
  set_float_image(grad135_,0.0);
  for (y=radius;y<image_.height()-radius;y++)
    for (x=radius;x<image_.width()-radius;x++)
    {
      this->avg(x, y, smooth_, avg_);
      this->grad0(x, y, smooth_, grad0_);
      this->grad45(x, y, smooth_, grad45_);
      this->grad90(x, y, smooth_, grad90_);
      this->grad135(x, y, smooth_, grad135_);
    }
  vcl_cout << "Compute gradients in " << t.real() << " msecs\n";
}


//------------------------------------------------------------
//: Project the gradient magnitude along a given direction.
//  The result is a 1-d projection plot.
//  \verbatim
//                     .
//                    *  .
//                   ^  *  .
//                 /   ^  *  .
//                  \    ^  *  .
//                         ^  *  \ .
//   2*proj_width_+1   x     ^  x-----2*proj_height_+1
//                        \ / \ .
// \endverbatim
float brip_para_cvrg::project(int x, int y, int dir, float* projection)
{
  int w,h;
  int w0 = proj_width_;
//float energy = 0.0;
  for (h=-proj_height_; h<=proj_height_; h++)
    for (w=-w0; w<=w0; w++)
      switch (dir)
      {
       case 0:
        projection[w+w0] += grad0_[x+w][y+h];
        break;
       case 45:
        projection[w+w0] += grad45_[x+h+w][y+w-h];
        break;
       case 90:
        projection[w+w0] += grad90_[x+h][y+w];
        break;
       case 135:
        projection[w+w0] += grad135_[x+h-w][y+w+h];
        break;
       default:
        projection[w+w0] += 0;
        break;
      }
  float max_energy = 0;
  for (int i =0; i<proj_n_; i++)
  {
    float val = vcl_fabs(projection[i]);
    if (val>max_energy)
      max_energy = val;
  }
  return max_energy;
}


//: Prune any sequences of more than one maximum value.
//That is, it is possible to have a "flat" top peak with an arbitrarily
//long sequence of equal, but maximum values.
//
void brip_para_cvrg::remove_flat_peaks(int n, float* array)
{
  int nbm = n-1;

  //Here we define a small state machine - parsing for runs of peaks
  //init is the state corresponding to an initial run (starting at i ==0)
  bool init= array[0]==0;
  int init_end =0;

  //start is the state corresponding to any other run of peaks
  bool start=false;
  int start_index=0;

  //The scan of the state machine
  for (int i = 0; i < n; i++)
  {
    float v = array[i];

    //State init: a string of non-zeroes at the beginning.
    if (init&&v!=0)
      continue;

    if (init&&v==0)
    {
      init_end = i;
      init = false;
      continue;
    }

    //State !init&&!start: a string of "0s"
    if (!start&&v==0)
      continue;

    //State !init&&start: the first non-zero value
    if (!start&&v!=0)
    {
      start_index = i;
      start = true;
      continue;
    }
    //State ending flat peak: encountered a subsequent zero after starting
    if (start&&v==0)
    {
      int peak_location = (start_index+i-1)/2;//The middle of the run
      for (int k = start_index; k<=(i-1); k++)
        if (k!=peak_location)
          array[k] = 0;
      start = false;
    }
  }
  //Now handle the boundary conditions
  if (init_end!=0)  //Was there an initial run of peaks?
  {
    int init_location = (init_end-1)/2;
    for (int k = 0; k<init_end; k++)
      if (k!=init_location)
        array[k] = 0;
  }
  if (start)       // Did we reach the end of the array in a run of pks?
  {
    int end_location = (start_index + nbm)/2;
    for (int k = start_index; k<n; k++)
      if (k!=end_location)
        array[k] = 0;
  }
}


//------------------------------------------------------------------
//: Find locally maximum peaks in the input array
void brip_para_cvrg::non_maximum_supress(float* input_array, float* sup_array)
{
  if ((2*sup_radius_ +1)> proj_width_)
  {
    vcl_cout << "In brip_para_cvrg::NonMaximumSupress(..) the kernel is too large\n";
  }
  float* tmp = this->make_float_vector(proj_n_);
  for (int i=0; i<proj_n_; i++)
    tmp[i]=vcl_fabs(input_array[i]);
  //Get the counts array of "this"
  //Make a new Histogram for the suppressed

  for (int i = sup_radius_; i < (proj_n_-sup_radius_); i++)
  {
    //find the maximum value in the current kernel
    float max_val = tmp[0];
    for (int k = -sup_radius_; k <= sup_radius_ ;k++)
    {
      int index = i+k;
      if (tmp[index] > max_val)
        max_val = tmp[index];
    }
    //Is position i a local maximum?
    if (vcl_fabs(max_val-tmp[i])<1e-03)
     sup_array[i] = max_val; //Yes. So set the counts to the max value
  }
  this->remove_flat_peaks(proj_n_, sup_array);
}


//---------------------------------------------------------------
//: Find the amount of overlapping parallel coverage
//
float brip_para_cvrg::parallel_coverage(float* input_array)
{
  this->set_float_vector(sup_proj_, proj_n_, 0.0);
  this->non_maximum_supress(input_array, sup_proj_);
  int n_peaks = 0;
  float proj_sum = 0;
  for (int i = 0; i<proj_n_; i++)
    if (sup_proj_[i]>0)
    {
      n_peaks++;
      proj_sum += sup_proj_[i];
    }
  if (n_peaks<2)
    return 0;
  return proj_sum/(n_peaks);
}


//----------------------------------------------------------
//: Find the direction with maximum parallel coverage.  Return the normalized coverage value.
void brip_para_cvrg::compute_parallel_coverage()
{
  vul_timer t;
//float min_sum = .01f;
  this->set_float_image(det_,0.0);
  this->set_float_image(dir_,0.0);
  int radius = proj_width_+proj_height_ + 3;
  for (int y=radius; y<(image_.height()-radius);y++)
    for (int x=radius ;x<(image_.width()-radius);x++)
    {
      this->set_float_vector(proj_0_, proj_n_, 0.0);
      this->set_float_vector(proj_45_, proj_n_, 0.0);
      this->set_float_vector(proj_90_, proj_n_, 0.0);
      this->set_float_vector(proj_135_, proj_n_, 0.0);

      float coverage[4];
      this->project(x, y, 0, proj_0_);
      coverage[0] = this->parallel_coverage(proj_0_);
   // vcl_cout << " O degrees = " << coverage[0] << '\n';

      this->project(x, y, 45, proj_45_);
      coverage[1] = this->parallel_coverage(proj_45_);
   // vcl_cout << " 45 degrees = " << coverage[1] << '\n';

      this->project(x, y, 90, proj_90_);
      coverage[2] = this->parallel_coverage(proj_90_);
   // vcl_cout << " 90 degrees = " << coverage[2] << '\n';

      this->project(x, y, 135, proj_135_);
      coverage[3] = this->parallel_coverage(proj_135_);
   // vcl_cout << " 135 degrees = " << coverage[3] << '\n';
      float max_coverage = 0;
      int max_dir = 0;
      for (int i =0; i<4; i++)
        if (coverage[i]>max_coverage)
        {
          max_coverage = coverage[i];
          max_dir = i*45;
        }

      det_[x][y] = max_coverage;
    }
  vcl_cout << "Do parallel coverage in " << t.real() << " msecs\n";
}


//------------------------------------------------------------------
//: Compute a 8-bit image from the projected gradients
//
void brip_para_cvrg::compute_image(float** data, vil1_memory_image_of<unsigned char>& image)
{
  image = vil1_memory_image_of<unsigned char>(xsize_, ysize_);
   //find the maximum value
  float max_val = 0;
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
      if (data[x][y]>max_val)
        max_val = data[x][y];
  if (max_val<1e-06)
    max_val = 1e-06f;
  //Normalize the data and load the image
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
    {
      float temp = 255*data[x][y]/max_val;
      unsigned char val;
      val = (unsigned char)temp;
      image(x, y)=val;
    }
}


void brip_para_cvrg::do_coverage(vil1_image const& image)
{
  this->init(image);
  this->smooth_image();
  this->compute_gradients();
  this->compute_parallel_coverage();
}


//------------------------------------------------------------
//: Get the float image of detections. Scale onto [0, max]
//
vil1_memory_image_of<float>
brip_para_cvrg::get_float_detection_image(const float max)
{
  vil1_memory_image_of<float> out(xsize_, ysize_);
  out.fill(0);
  float max_val = 0;
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
      if (det_[x][y]>max_val)
        max_val = det_[x][y];

  if (max_val==0)
    return out;
  float s = max/max_val;
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
      out(x,y) = s*det_[x][y];
  return out;
}


//------------------------------------------------------------
//: Get the unsigned char image of detections
//
vil1_memory_image_of<unsigned char> brip_para_cvrg::get_detection_image()
{
  if (!det_image_)
    this->compute_image(det_, det_image_);
  return det_image_;
}


//------------------------------------------------------------
//: Get the direction image (unsigned char)
//
vil1_memory_image_of<unsigned char>  brip_para_cvrg::get_dir_image()
{
  if (!dir_image_)
    this->compute_image(dir_, dir_image_);
  return dir_image_;
}
