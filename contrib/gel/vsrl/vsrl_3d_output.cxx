#include <vsrl/vsrl_3d_output.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <vsrl/vsrl_step_diffusion.h>
#include <vsrl/vsrl_token_saliency.h>
#include <vsrl/vsrl_saliency_diffusion.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_save.h>
#include <vsrl/vsrl_parameters.h>

// the constructor
vsrl_3d_output::vsrl_3d_output(const vil_image &im1, const vil_image &im2):
_buffer1(im1),
_buffer2(im2),
_H(4,4),
_image_correlation(im1,im2)
{
  _matcher=0;
  _H.fill(0.0);
  _H(0,0)=1;
  _H(1,1)=1;
  _H(2,2)=1;
  _H(3,3)=1;
  _image1=im1;
  _image2=im2;
}

// the destructor
vsrl_3d_output::~vsrl_3d_output()
{
}

// set the matcher

void vsrl_3d_output::set_matcher(vsrl_dense_matcher *matcher)
{
  _matcher = matcher;
}


// set the projective tranform

void vsrl_3d_output::set_projective_transform(vnl_matrix<double> &H)
{
  _H=H;

  return;
}


void vsrl_3d_output::write_output(char *filename)
{
  // OK we are going to do several things
  // first we want a list of 3D points in space
  // this is easily done by taking (x,y)-> (x+d,y)
  // implies an interpretation of (x,y,d,1)
  // is a valid reconstruction which should look pretty decent

  if (!_matcher){
    return;
  }

  // make a step_diffusion object to make better disparities

  vsrl_step_diffusion step_diffusion(_matcher);
  step_diffusion.execute();

  // this->write_disparity_image("test0_disp.ppm",&step_diffusion);

  // determine the saliency of each point in the image

  // vcl_cout << "Perform the image correlation routines" << endl;
  // _image_correlation.initial_calculations();

  // vsrl_token_saliency ts(&_image_correlation);
  // ts.create_saliency_image("test_sal.ppm");

  // use the token saliency to initiliaze a vsrl_saliency_diffusion object

  // vsrl_saliency_diffusion sal_diffusion(_matcher);


  // sal_diffusion.set_initial_disparity(&step_diffusion);

  // sal_diffusion.set_saliency(&ts);

  // sal_diffusion.execute(200);

  // write the output

  // this->write_disparity_image("test1_disp.ppm",&sal_diffusion);


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

  double width = _matcher->get_width();
  double height = _matcher->get_height();

  double difuse_d;
  int x,y; // image coordinates and disparity
  double X,Y,Z,W; // 3d coordinates
  double tx,ty; // texture coordinates

  // keep track of the indices

  vnl_matrix<int> point_index(_matcher->get_width(),_matcher->get_height());
  point_index.fill(-1);
  int index=0;

  for (x=0;x<width;x++){
    for (y=0;y<height;y++){
      // get the disparity

      difuse_d = step_diffusion.get_disparity(x,y);
      // difuse_d = sal_diffusion.get_disparity(x,y);

      if (non_valid_point(x,y)){
        difuse_d=0.0;
      }

      // d = _matcher->get_disparity(x,y);
      if (difuse_d > 0-999){

        input(0,0)=x;
        input(1,0)=y;
        input(2,0)=difuse_d;
        input(3,0)=1.0; // change me based on image scale


        // comput the output = _H * input

        output = _H * input;


        // normalize to X,Y,Z,1

        W=output(3,0);

        if (W){
          // the normalized coordinates

          X=output(0,0)/W;
          Y=output(1,0)/W;
          Z=output(2,0)/W;


          // the texture coordinates
          tx=x/width;
          ty=(height-y)/height;
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

  vcl_vector<double>::iterator iX,iY,iZ,itx,ity;


  // O.K we can now write out the data;

  vcl_ofstream file(filename);
  int length = X_out.size();
  file << length << vcl_endl;


  iY=Y_out.begin();
  iZ=Z_out.begin();
  itx =tx_out.begin();
  ity = ty_out.begin();

  for (iX=X_out.begin();iX!=X_out.end();iX++, iY++, iZ++,itx++, ity++)
    {
      file << (*iX) << " " << (*iY) << " " << (*iZ) << " " << *itx << " " << *ity <<
        vcl_endl;
    }

  // OK we can now compute the conectivity between points

  vcl_cout << "computing the triangles" << vcl_endl;

  // these are the vertex lists

  vcl_vector<int> vert1;
  vcl_vector<int> vert2;
  vcl_vector<int> vert3;

  int x2,y2;
  int in1,in2,in3,in4;

  // make all posible triangles

  for (y=0;y<height-1;y++){
    y2=y+1;
    for (x=0;x<width-1;x++){
      x2=x+1;

      in1=point_index(x,y);
      in2=point_index(x2,y);
      in3=point_index(x2,y2);
      in4=point_index(x,y2);

      // the first triangle

      if (in1>=0 && in2 >=0 && in3>=0){
        vert1.push_back(in1);
        vert2.push_back(in2);
        vert3.push_back(in3);
      }

      // the second triangle

      if (in1>=0 && in3 >=0 && in4>=0){
        vert1.push_back(in1);
        vert2.push_back(in3);
        vert3.push_back(in4);
      }
    }
  }

  vcl_cout << "writing triangles" << vcl_endl;

  // write the number of triangles
  length = vert1.size();
  file << length << vcl_endl;

  vcl_vector<int>::iterator v1,v2,v3;

  for (v1=vert1.begin(), v2=vert2.begin(), v3=vert3.begin(); v1<vert1.end();
      v1++,v2++,v3++){

    file << *v1 << " " << *v2 << " " << *v3 << vcl_endl;
  }
}


void vsrl_3d_output::read_projective_transform(char *filename)
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

  int row,col;
  double value;

  for (row=0;row<4;row++){
    for (col=0;col<4;col++){
      file >> value;
      vcl_cout << "Point r c " << value << " " << row << " " << col << vcl_endl;

      if (col==2 || col==3){
        H(row,col)=value;
      }

      if (col==1){
        H(row,0)=value;
      }
      if (col==0){
        H(row,1)=value;
      }
    }
  }

  vcl_cout << "Seting transform to " << vcl_endl << H << vcl_endl;
  this->set_projective_transform(H);
}


// identify points which are not part of the recitfied image

bool vsrl_3d_output::non_valid_point(int x, int y)
{
  if (x>=0 && x < _buffer1.width() && y>=0 && y <_buffer1.height()){
    if (_buffer1(x,y)==3){
      return true;
    }
  }

  if (x>=0 && x < _buffer2.width() && y>=0 && y <_buffer2.height()){
    if (_buffer2(x,y)==3){
      return true;
    }
  }

  // it looks like this is a valid point
  return false;
}

void vsrl_3d_output::write_disparity_image(char *filename,vsrl_diffusion *diff)
{
  // we want to write a disparity image

  // make a buffer which has the size of image1

  vil_memory_image_of<int> buffer(_image1);

  int x,y;
  int disparity;
  int value;


  for (x=0;x<buffer.width();x++)
    for (y=0;y<buffer.height();y++)
      buffer(x,y)=0;

  // go through each point, get the disparity and save it into the buffer

  int correlation_range = vsrl_parameters::instance()->correlation_range;

  for (y=0;y<buffer.height();y++)
    for (x=0;x<buffer.width();x++){
      disparity = (int)(diff->get_disparity(x,y));
      value = disparity + correlation_range+1;
      if (value < 0)
        value = 0;

      if (value>2*correlation_range+1)
        value=0;

      buffer(x,y)=value;
    }

  // save the file
  // vil_save(buffer, filename, _image1.file_format());
  vil_save(buffer, filename);
}
