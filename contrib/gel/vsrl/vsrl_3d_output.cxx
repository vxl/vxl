// This is gel/vsrl/vsrl_3d_output.cxx
#include "vsrl_3d_output.h"
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <vsrl/vsrl_step_diffusion.h>
#include <vsrl/vsrl_token_saliency.h>
#include <vsrl/vsrl_saliency_diffusion.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_scale_intensities.h>
#include <vsrl/vsrl_parameters.h>

// the constructor
vsrl_3d_output::vsrl_3d_output(const vil1_image &im1, const vil1_image &im2):
buffer1_(im1),
buffer2_(im2),
H_(4,4),
image_correlation_(im1,im2)
{
  matcher_=0;
  H_.fill(0.0);
  H_(0,0)=1;
  H_(1,1)=1;
  H_(2,2)=1;
  H_(3,3)=1;
  image1_=im1;
  image2_=im2;
}

// the destructor
vsrl_3d_output::~vsrl_3d_output()
{
}

// set the matcher

void vsrl_3d_output::set_matcher(vsrl_dense_matcher *matcher)
{
  matcher_ = matcher;
}


// set the projective transform

void vsrl_3d_output::set_projective_transform(vnl_matrix<double> &H)
{
  H_=H;
  return;
}


void vsrl_3d_output::write_output(const char *filename)
{
  // OK we are going to do several things
  // first we want a list of 3D points in space
  // this is easily done by taking (x,y)-> (x+d,y)
  // implies an interpretation of (x,y,d,1)
  // is a valid reconstruction which should look pretty decent

  if (!matcher_)
    return;

  // make a step_diffusion object to make better disparities

  vsrl_step_diffusion step_diffusion(matcher_);
  step_diffusion.execute();
#if 0
  this->write_disparity_image("test0_disp.ppm",&step_diffusion);

  // determine the saliency of each point in the image
  vcl_cout << "Perform the image correlation routines\n";
  image_correlation_.initial_calculations();

  vsrl_token_saliency ts(&image_correlation_);
  ts.create_saliency_image("test_sal.ppm");

  // use the token saliency to initialize a vsrl_saliency_diffusion object
  vsrl_saliency_diffusion sal_diffusion(matcher_);

  sal_diffusion.set_initial_disparity(&step_diffusion);

  sal_diffusion.set_saliency(&ts);

  sal_diffusion.execute(200);

  // write the output
  this->write_disparity_image("test1_disp.ppm",&sal_diffusion);
#endif // 0

  // these are the outputs of the data

  vcl_vector<double> X_out;
  vcl_vector<double> Y_out;
  vcl_vector<double> Z_out;

  // the texture coordinates

  vcl_vector<double> tx_out;
  vcl_vector<double> ty_out;


  // step 1 compute all the X for the interpretation

  vnl_matrix<double> input(4,1);
  vnl_matrix<double> output(4,1);

  int width = matcher_->get_width();
  int height = matcher_->get_height();

  // keep track of the indices

  vnl_matrix<int> point_index(width,height);
  point_index.fill(-1);
  int index=0;

  for (int x=0;x<width;x++)
  {
    for (int y=0;y<height;y++)
    {
      // get the disparity

      double diffuse_d = step_diffusion.get_disparity(x,y);
      // diffuse_d = sal_diffusion.get_disparity(x,y);

      if (non_valid_point(x,y))
        diffuse_d=0.0;

      // d = matcher_->get_disparity(x,y);
      if (diffuse_d > 0-999)
      {
        input(0,0)=x;
        input(1,0)=y;
        input(2,0)=diffuse_d;
        input(3,0)=1.0; // change me based on image scale

        // comput the output = H_ * input
        output = H_ * input;

        // normalize to X,Y,Z,1
        double W=output(3,0);

        if (W)
        {
          // the normalized coordinates

          double X=output(0,0)/W;
          double Y=output(1,0)/W;
          double Z=output(2,0)/W;

          // regardless of the calculations,
          // -correlation_range <= Z <= +correlation_range_
          // make sure of it.
          int c_range = matcher_->get_correlation_range();
          if (Z <= -c_range) Z = -c_range;
          if (Z >= c_range) Z = c_range;

          // the texture coordinates
          double tx=(float)x/(float)width;
          double ty=((float)height-(float)y)/(float)height;
          // ty=y/height;

          X_out.push_back(X);
          Y_out.push_back(height-Y);
          // Y_out.push_back(Y);

          Z_out.push_back(Z);

          tx_out.push_back(tx);
          ty_out.push_back(ty);

          // keep track of the point indices
          point_index(x,y)=index;
          index++;
        }
      }
    }
  }

  // O.K we can now write out the data;

  vcl_ofstream file(filename);
  int length = X_out.size();
  file << length << vcl_endl;

  // Create a range image to dump the data into for further use.
  // Set initial image to zero.
  range_image_.resize(width,height+1);
  for (int x=0;x<width;x++)
    for (int y=0;y<height+1;y++)
      range_image_(x,y) = 0.0;

  vcl_vector<double>::iterator iX=X_out.begin();
  vcl_vector<double>::iterator iY=Y_out.begin();
  vcl_vector<double>::iterator iZ=Z_out.begin();
  vcl_vector<double>::iterator itx=tx_out.begin();
  vcl_vector<double>::iterator ity=ty_out.begin();
  double maxval=0.0; // assume that not all values in Z_out are negative - TODO

  for (; iX!=X_out.end(); ++iX, ++iY, ++iZ, ++itx, ++ity)
  {
    file << (*iX) << ' ' << (*iY) << ' ' << (*iZ) << ' ' << *itx << ' ' << *ity << vcl_endl;
    // populate the range image
    int ix = int(*iX+0.5), iy = int(*iY+0.5); // round to nearest integer
    range_image_(ix,height-iy) = *iZ;
    if (*iZ > maxval) maxval = *iZ; // find the maximum for later scaling
  }

  // Save the range image - scaled to 0->255
  double scale = 255.0/maxval; double shift=0;
  vil1_image scaled_image = vil1_scale_intensities(range_image_,scale,shift);
  rimage_.resize(range_image_.cols(),range_image_.rows());
  double d_tmp;
  for (int r=0;r<range_image_.rows();r++) {
    for (int c=0;c<range_image_.cols();c++) {
      scaled_image.get_section(&d_tmp,c,r,1,1);
      rimage_(c,r) = (unsigned char)(d_tmp+0.5); // round to nearest byte-value
    }
  }
  if (!vil1_save(rimage_,"range_image.tif"))
    vcl_cerr << "vsrl_3d_output::write_output: Error saving range image!\n";

  // OK we can now compute the connectivity between points

  vcl_cout << "computing the triangles\n";

  // these are the vertex lists

  vcl_vector<int> vert1;
  vcl_vector<int> vert2;
  vcl_vector<int> vert3;

  // make all possible triangles

  for (int y=0;y<height-1;y++)
  {
    int y2=y+1;
    for (int x=0;x<width-1;x++)
    {
      int x2=x+1;

      int in1=point_index(x,y);
      int in2=point_index(x2,y);
      int in3=point_index(x2,y2);
      int in4=point_index(x,y2);

      // the first triangle

      if (in1>=0 && in2 >=0 && in3>=0) {
        vert1.push_back(in1);
        vert2.push_back(in2);
        vert3.push_back(in3);
      }

      // the second triangle

      if (in1>=0 && in3 >=0 && in4>=0) {
        vert1.push_back(in1);
        vert2.push_back(in3);
        vert3.push_back(in4);
      }
    }
  }

  vcl_cout << "writing triangles\n";

  // write the number of triangles
  length = vert1.size();
  file << length << vcl_endl;

  vcl_vector<int>::iterator v1,v2,v3;

  for (v1=vert1.begin(), v2=vert2.begin(), v3=vert3.begin(); v1<vert1.end();
       v1++,v2++,v3++)
    file << *v1 << ' ' << *v2 << ' ' << *v3 << vcl_endl;
}


void vsrl_3d_output::read_projective_transform(const char *filename)
{
  // since this is a projective transform from the RH point
  // of view. I am assuming that the data will be of the form
  // (X,Y,Z,W)^T = H (v, u, du, 1)
  // so if want this to be consisten with our world
  // were v and u are transposed, the first and second
  // column of H must be swapped


  vcl_cout << "opening file " << filename << vcl_endl;
  vcl_ifstream file(filename);

  // get rid of the header

  char hold[512];

  file >> hold;
  file >> hold;
  file >> hold;
  file >> hold;

  vnl_matrix<double> H(4,4);

  for (int row=0;row<4;row++){
    for (int col=0;col<4;col++){
      double value;
      file >> value;
      vcl_cout << "Point r c " << value << ' ' << row << ' ' << col << vcl_endl;

      if (col>1) H(row,col)=value;
      else       H(row,1-col)=value;
    }
  }

  vcl_cout << "Setting transform to\n" << H << vcl_endl;
  this->set_projective_transform(H);
}


// identify points which are not part of the rectified image

bool vsrl_3d_output::non_valid_point(int x, int y)
{
  if (x>=0 && x < buffer1_.width() && y>=0 && y < buffer1_.height())
    if (buffer1_(x,y)==3)
      return true;

  if (x>=0 && x < buffer2_.width() && y>=0 && y < buffer2_.height())
    if (buffer2_(x,y)==3)
      return true;

  // it looks like this is a valid point
  return false;
}

void vsrl_3d_output::write_disparity_image(const char *filename,vsrl_diffusion *diff)
{
  // we want to write a disparity image

  // make a buffer which has the size of image1

  vil1_memory_image_of<int> buffer(image1_);

  for (int x=0;x<buffer.width();x++)
    for (int y=0;y<buffer.height();y++)
      buffer(x,y)=0;

  // go through each point, get the disparity and save it into the buffer

  int correlation_range = vsrl_parameters::instance()->correlation_range;

  for (int y=0;y<buffer.height();y++)
    for (int x=0;x<buffer.width();x++)
    {
      int disparity = (int)(diff->get_disparity(x,y));
      int value = disparity + correlation_range+1;
      if (value < 0)
        value = 0;

      if (value>2*correlation_range+1)
        value=0;

      buffer(x,y)=value;
    }

  // save the file
  // vil1_save(buffer, filename, image1_.file_format());
  vil1_save(buffer, filename);
}
