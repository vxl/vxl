#include<vsrl/vsrl_image_correlation.h>
#include<vil/vil_image.h>
#include<vsrl/vsrl_window_accumulator.h>
#include<vcl_iostream.h>
#include<vsrl/vsrl_parameters.h>


// constructor

vsrl_image_correlation::vsrl_image_correlation(const vil_image &im1, const vil_image &im2):
  _buffer1(im1),
  _buffer2(im2)
{
  // initialize the window width and height

  _window_width=vsrl_parameters::instance()->correlation_window_width; // probably 20
  _window_height=vsrl_parameters::instance()->correlation_window_height; // probably 20
  _correlation_range=vsrl_parameters::instance()->correlation_range; // probably 10

  _image_correlations=0;
  _mean_x=0;
  _mean_y=0;
  _std_x=0;
  _std_y=0;
}

// desctructor
vsrl_image_correlation::~vsrl_image_correlation()
{
  if (_image_correlations)
  {
    int i;
    for (i=0;i<_correlation_range*2+1;i++)
      delete _image_correlations[i];
    delete[] _image_correlations;
  }
  delete _mean_x;
  delete _mean_y;
  delete _std_x;
  delete _std_y;
}

// set the dimensions of the correlation window


void vsrl_image_correlation::set_window_width(int width)
{
  _window_width=width;
}

void vsrl_image_correlation::set_window_height(int height)
{
  _window_height=height;
}

void vsrl_image_correlation::set_correlation_range(int range)
{
  _correlation_range = range;
}


int vsrl_image_correlation::get_correlation_range()
{
  return _correlation_range;
}


// make sure that a point is in the range of the image

bool vsrl_image_correlation::check_range(vil_byte_buffer &buf, int x, int y)
{
  return x>=0 && x<buf.width() && y>=0 && y<buf.height();
}


//***************************************************************************
//** These routines will perform the image correlation in an efficient manner
//***************************************************************************

void vsrl_image_correlation::compute_local_stats(vnl_matrix<double> &im, vnl_matrix<double> &mean,
                                                 vnl_matrix<double> &std)
{
  // mean = 1/N * sum X
  // var = 1/N * sum (X-mean)(X-mean) = 1/N Sum X*X - mean*mean

  // we want to compute the mean and standard deviation for the im

  // step one get the mean in an efficient manner

  vsrl_window_accumulator ac_mean;

  ac_mean.set_in_matrix(&im);
  ac_mean.set_out_matrix(&mean);
  ac_mean.set_window_width(this->_window_width);
  ac_mean.set_window_height(this->_window_height);
  ac_mean.execute();

  // now compute the square values;

  vnl_matrix<double> squares(im.rows(),im.cols());

  shift_multiply_matrix(0,im,im,squares);

  vsrl_window_accumulator ac_std;

  ac_std.set_in_matrix(&squares);
  ac_std.set_out_matrix(&std);
  ac_std.set_window_width(this->_window_width);
  ac_std.set_window_height(this->_window_height);
  ac_std.execute();

  for (unsigned int r=0;r<std.rows();r++)
    for (unsigned int c=0;c<std.cols();c++)
    {
      double xx=std(r,c);
      double m=mean(r,c);
      std(r,c) = vcl_sqrt(xx-m*m);
    }
}

void vsrl_image_correlation::shift_multiply_matrix(int offset, vnl_matrix<double> &X, vnl_matrix<double> &Y,
                                                        vnl_matrix<double> &XY)
{
  // we want to multiply X*(Y shifted by offset);

  XY.fill(0.0);

  // find the start and end colums

  int start_col = 0-offset;
  if (start_col<0)
    start_col=0;

  int end_col = Y.cols()-offset; // 1 more than last column
  if (end_col>X.cols())
    end_col=X.cols();

  for (int r=0;r<X.rows();r++)
    for (int c=start_col;c<end_col;c++)
      XY(r,c) = X(r,c) * Y(r,c+offset);
}


void  vsrl_image_correlation::compute_correlation(int x_offset,
                                                  vnl_matrix<double> &X,
                                                  vnl_matrix<double> &Y,
                                                  vnl_matrix<double> &mean_x,
                                                  vnl_matrix<double> &mean_y,
                                                  vnl_matrix<double> &std_x,
                                                  vnl_matrix<double> &std_y,
                                                  vnl_matrix<double> &corr_matrix)
{
  // the idea is to compute the correlation between X and Y given mean_x and mean_y
  // cor = 1/N sum (xi - mean_x)*(yi -mean_y)/(std_x * std_y)
  // cor = 1/(stdx * stdy) * (1/N(sum xiyi) - mean_x * mean_y)


  // step 1 compute the matrix XY;

  vnl_matrix<double> XY(X.rows(),X.cols());

  shift_multiply_matrix(x_offset,X,Y,XY);

  // step 2 compute the accumulation of XY ;

  vnl_matrix<double> ac_XY(X.rows(),X.cols());

  vsrl_window_accumulator win_ac;

  win_ac.set_in_matrix(&XY);
  win_ac.set_out_matrix(&ac_XY);
  win_ac.set_window_width(this->_window_width);
  win_ac.set_window_height(this->_window_height);
  win_ac.execute();

  // step 3 compute the corr_matrix

  corr_matrix.fill(0.0);


  // find the start and end colums

  int start_col = 0-x_offset;
  if (start_col <0)
    start_col=0;

  int end_col = Y.cols()-x_offset;
  if (end_col>X.cols())
    end_col=X.cols();

  for (int r=0;r<X.rows();r++)
    for (int c=start_col;c<end_col;c++)
    {
      double sx = std_x(r,c);
      double sy = std_y(r,c+x_offset);

      double mx = mean_x(r,c);
      double my = mean_y(r,c+x_offset);

      double xy = ac_XY(r,c);

      if (sx!=0.0 && sy!=0.0)
        corr_matrix(r,c) = (xy-mx*my)/(sx*sy);
    }
}


void vsrl_image_correlation::initial_calculations()
{
  // we want to be able to compute all the possible correlations
  // between the two images;

  if (_mean_x)
    return; // the initial calculations have already been done


  // step 1 make a vnl_matrix from the two buffers

  vnl_matrix<double> X(_buffer1.height(),_buffer1.width());

  for (int r=0;r<X.rows();r++)
    for (int c=0;c<X.cols();c++)
      X(r,c)= _buffer1(c,r);

  vnl_matrix<double> Y(_buffer2.height(),_buffer2.width());


  for (int r=0;r<Y.rows();r++)
    for (int c=0;c<Y.cols();c++)
      Y(r,c)= _buffer2(c,r);

  // Now compute the mean and std for X and Y


  _mean_x = new vnl_matrix<double>(X.rows(),X.cols());
  _mean_y = new vnl_matrix<double>(Y.rows(),Y.cols());
  _std_x = new vnl_matrix<double>(X.rows(),X.cols());
  _std_y = new vnl_matrix<double>(Y.rows(),Y.cols());


  compute_local_stats(X,*_mean_x,*_std_x);
  compute_local_stats(Y,*_mean_y,*_std_y);


  // now allocate memory for the various corelation matrix values

  _image_correlations = (vnl_matrix<double>**)(malloc(sizeof(*_image_correlations) * (_correlation_range*2 +1)));

  // now compute each correlation

  for (int i=0;i<_correlation_range*2 + 1;i++)
  {
    vnl_matrix<double> *corr = new vnl_matrix<double>(X.rows(),X.cols());
    _image_correlations[i]=corr;

    int x_offset = i-_correlation_range;

    compute_correlation(x_offset,X,Y,*_mean_x,*_mean_y,*_std_x,*_std_y,*corr);
  }
}

// get local image stats
double vsrl_image_correlation::get_mean_1(int x, int y)
{
  return (*_mean_x)(y,x);
}

double vsrl_image_correlation::get_mean_2(int x, int y)
{
  return (*_mean_y)(y,x);
}

double vsrl_image_correlation::get_std_1(int x, int y)
{
  return (*_std_x)(y,x);
}

double vsrl_image_correlation::get_std_2(int x, int y)
{
  return (*_std_y)(y,x);
}

double vsrl_image_correlation::get_image_value1(int x, int y)
{
  return _buffer1(x,y);
}


double vsrl_image_correlation::get_image_value2(int x, int y)
{
  return _buffer2(x,y);
}

double vsrl_image_correlation::get_correlation(int x1, int y1, int delta_x)
{
  if (!_image_correlations)
    return 0.0;

  int index = delta_x + _correlation_range;

  if (index < 0 || index >=(2*_correlation_range +1))
    return 0.0;

  return (*(_image_correlations[index]))(y1,x1);
}


int vsrl_image_correlation::get_image1_width()
{
  return _buffer1.width();
}

int vsrl_image_correlation::get_image2_width()
{
  return _buffer2.width();
}

int vsrl_image_correlation::get_image1_height()
{
  return _buffer1.height();
}

int vsrl_image_correlation::get_image2_height()
{
  return _buffer2.height();
}


// **************************************************
// *** These routines will perform the local corelation in a slow but correct manner
// **************************************************


// compute the local stats of a window
// slow
void vsrl_image_correlation::compute_local_stats(vil_byte_buffer &buf, int x, int y, double &mean, double &std)
{
  // find the dimensions of the window;

  int low_x= x- _window_width/2;
  int hi_x= x+ _window_width/2;

  if (low_x <0)
    low_x=0;

  if (hi_x>buf.width()-1)
    hi_x=buf.width()-1;

  int low_y= y- _window_height/2;
  int hi_y= y+ _window_height/2;

  if (low_y <0)
    low_y=0;

  if (hi_y>buf.height()-1)
    hi_y=buf.height()-1;


  // we want to compute the local stats of the data;

  double sum_x=0.0;
  double sum_xx=0.0;
  double N=0;

  for (int ix=low_x;ix<= hi_x;ix++)
    for (int iy=low_y;iy<=hi_y;iy++)
    {
      double val = buf(ix,iy);
      sum_x=sum_x+val;
      sum_xx=sum_xx + val*val;
      N++;
    }

  // compute the mean

  if (N)
  {
    mean=sum_x/N;

    double var=sum_xx/N  - mean*mean;

    std= vcl_sqrt(var);
  }
  else
  {
    mean=0;
    std=0;
  }
}


// slow
double vsrl_image_correlation::get_correlation(int x1, int y1, int x2, int y2)
{
  // we want to compute the correlation

  if (!check_range(_buffer1,x1,y1) || !check_range(_buffer2,x2,y2))
    return 0.0;

  // compute the window ranges for the data;

  // first the x component

  int dx = _window_width/2;
  if (x1 < dx)
    dx=x1;

  if (x2 < dx)
    dx=x2;

  int low_x1 = x1-dx;
  int low_x2 = x2-dx;

  dx=_window_width/2;
  if ((_buffer1.width()-1 -x1) < dx)
    dx=(_buffer1.width()-1 -x1);

  if ((_buffer2.width()-1 -x2) < dx)
    dx=(_buffer2.width()-1 -x2);

  int hi_x1= x1+dx;

  // now the y component

  int dy = _window_height/2;
  if (y1 < dy)
    dy=y1;

  if (y2 < dy)
    dy=y2;

  int low_y1 = y1-dy;
  int low_y2 = y2-dy;

  dy=_window_height/2;
  if ((_buffer1.height()-1 -y1) < dy)
    dy=(_buffer1.height()-1 -y1);

  if ((_buffer2.height()-1 -y2) < dy)
    dy=(_buffer2.height()-1 -y2);

  int hi_y1= y1+dy;


  // ****** now find the stats of the two windows

  double mean1,std1,mean2,std2;

  compute_local_stats(_buffer1,x1,y1,mean1,std1);
  compute_local_stats(_buffer2,x2,y2,mean2,std2);

  // now compute the correlation

  if (std1 && std2)
  {
    double val1, val2;
    double N=0;
    double corr=0;

    for (int i1=low_x1, i2=low_x2; i1<= hi_x1;i1++, i2++)
      for (int j1=low_y1, j2=low_y2; j1 <= hi_y1;j1++, j2++)
      {
        val1=_buffer1(i1,j1);
        val2=_buffer2(i2,j2);

        corr=corr+((val1-mean1)/std1)*((val2-mean2)/std2);
        N++;
      }

    if (N != 0)
      corr=corr/N;

    // return the correlation value
    return corr;
  }
  else
    return 0.0;
}


double vsrl_image_correlation::get_sub_pixel_delta(int x1,int y1, int delta_x)
{
  // we wish to determine the correct delta_x using sub-pixel accuracy.
  // the idea is that we can compute the correlations for delta_x-1, delta_x
  // and delta_x +1. let Y= aX^2 + b X + c.
  // let X1 = -1, Y1 = corr (delta_x -1),
  // let X2 = 0, Y2 = corr (delta_x ),
  // let X3 = -1, Y3 = corr (delta_x +1)
  // then |X1^2  X1 1| a     |Y1|
  // then |X2^2  X2 1| b  =  |Y2|
  // then |X3^2  X3 1| c     |Y3|
  //           A       S      B
  // A^-1 = |.5  -1 .5|
  //        |-.5  0 .5|
  //        | 0   1 0 |
  // there for a = .5 Y1 - Y2 + 0.5 Y3
  //           b = -.5 Y1 +0.5 Y3
  //           c = Y2
  // the derivative  dY/dX = 2aX + b
  //  there for maxima/minima occurs at X_hat = -b/2a
  // the second derivative is a so if 2a so if
  // 2a is poisitive then h_hat is a local minima
  // if 2a is negative then X_hat ins a local maxima
  // if 2a = 0, we are dealing with  a straigt line

  // first check to see if the correlation range is within
  // spec.

  int index = delta_x + _correlation_range;

  if (index <1 || index >= (2*_correlation_range))
    return delta_x; // cannot three measurements sending back the original

  // get Y1 , Y2 , Y3

  double Y1 = get_correlation(x1,y1,delta_x -1);
  double Y2 = get_correlation(x1,y1,delta_x);
  double Y3 = get_correlation(x1,y1,delta_x +1);

  double a = 0.5*Y1 -Y2 + 0.5 *Y3;
  double b = 0.5*Y3 - 0.5*Y1;

  double x_hat=0;
  if (a>=0)
  {
    // this is a straight line or a minumum has
    // occured in the interval

    x_hat =0;

    if (Y1 > Y3)
      x_hat = 0.0-0.5;

    if (Y3 > Y1)
      x_hat = 0.5;
  }
  else
  {
    // we have trapped a local maxima
    x_hat = 0.0-b/ (2.0*a);
    if (x_hat >0.5)
      x_hat = 0.5;

    if (x_hat < (0.0-0.5))
      x_hat = (0.0-0.5);
  }

  return delta_x + x_hat;
}


void vsrl_image_correlation::get_correlation_stats(int x, int y, double &mean, double &std)
{
  // we want to determine how the correltaion behaves for the point x, y
  // we do this by finding the correlation for every possible offset and
  // then recording the mean and standard deviation for this correlation function

  double N=0;
  double sum_z=0;
  double sum_zz=0;

  // cover all posible correlations

  for (int delta=0-_correlation_range;delta<_correlation_range+1;delta++)
  {
    double z = this->get_correlation(x,y,delta);
    sum_z += z;
    sum_zz += z*z;
    ++N;
  }

  if (N!=0)
  {
    mean = sum_z/N;
    double var = sum_zz/N - mean*mean;
    std = vcl_sqrt(var);
  }
}
