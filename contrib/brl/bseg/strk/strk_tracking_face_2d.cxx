// This is brl/bseg/strk/strk_tracking_face_2d.cxx
#include "strk_tracking_face_2d.h"
//:
// \file
// See strk_tracking_face_2d.h
//
//-----------------------------------------------------------------------------
#include <btol/btol_face_algs.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vcl_cmath.h> // for log(), exp() ..

static double strk_gaussian(const double x, const double sigma)
{
  double x_on_sigma = x / sigma;
  return (double)vcl_exp(- x_on_sigma * x_on_sigma / 2);
}

//:generate a 1-d Gaussian kernel  fuzz=0.02 is a good value
static void strk_1d_gaussian_kernel(const double sigma,
                                    const double fuzz,
                                    int& radius,
                                    vcl_vector<double>& kernel)
{
  for (radius = 0; strk_gaussian(double(radius), sigma) > fuzz; radius++)
  {;}                                         // find radius

  kernel.resize(2*radius + 1);
  if (!radius)
  {
    kernel[0]=1;
    return;
  }
  for (int i=0; i<=radius; ++i)
    kernel[radius+i] = kernel[radius-i] = strk_gaussian(double(i), sigma);
  double sum = 0;
  for (int i= 0; i <= 2*radius; ++i)
    sum += kernel[i];                           // find integral of weights
  for (int i= 0; i <= 2*radius; ++i)
    kernel[i] /= sum;                           // normalize by integral
}
// convolve a 1-d array with a Gaussian kernel.  Handle the borders by
// setting the kernel to zero outside the data array.  Adjust the output
// to obtain unit norm, i.e, normalize by the sum of the weights.
//
static void strk_1d_gaussian(const double sigma,
                             vcl_vector<double> const& in_buf,
                             vcl_vector<double>& out_buf)
{
  int n = in_buf.size(), r = 0;
  if (!n)
    return;
  out_buf.resize(n);
  if (n==1)
  {
    out_buf[0]=in_buf[0];
    return;
  }
  //the general case
  vcl_vector<double> ker;
  strk_1d_gaussian_kernel(sigma, 0.02, r, ker);
  for (int i = 0; i<n; i++)
  {
    double sum = 0;
    //case a)
    //the full kernel is applied
    if (i>r&&((n-1)-i>=r))
    {
      for (int k = -r; k<=r; k++)
        sum += ker[k+r]*in_buf[i+k];
      out_buf[i]=sum;
      continue;
    }
    //case b)
    // full kernel can't be used
    int r_minus = i;
    if (r_minus>r)
      r_minus=r;
    int r_plus = (n-1)-i;
    if (r_plus>r)
      r_plus=r;
    double ker_sum =0;
    for (int k = -r_minus; k<=r_plus; k++)
    {
      ker_sum += ker[k+r];
      sum += ker[k+r]*in_buf[i+k];
    }
    out_buf[i]=sum/ker_sum;
  }
}
//convolve a 2-d array with a Gaussian kernel.  Since the Gaussian is
//separable, first convolve along cols and then along rows
static void strk_2d_gaussian(const double sigma,
                             vbl_array_2d<double> const& in_buf,
                             vbl_array_2d<double>& out_buf)
{
  int n = in_buf.cols(), m = in_buf.rows();
  out_buf.resize(m, n);

  //convolve columns
  for (int row = 0; row<m; row++)
  {
    vcl_vector<double> row_buf(n), temp;
    for (int col = 0; col<n; col++)
      row_buf[col]=in_buf[row][col];
    strk_1d_gaussian(sigma, row_buf, temp);
    for (int col = 0; col<n; col++)
      out_buf[row][col]=temp[col];
  }
  //convolve rows
  for (int col = 0; col<n; col++)
  {
    vcl_vector<double> col_buf(m), temp;
    for (int row = 0; row<m; row++)
      col_buf[row]=out_buf[row][col];
    strk_1d_gaussian(sigma, col_buf, temp);
    for (int row = 0; row<m; row++)
      out_buf[row][col]=temp[row];
  }
}

//
//======================== HISTOGRAM IMPLEMENTATION =================
//
template <class T>
strk_hist<T>::strk_hist(const T range, const unsigned int nbins)
  : area_valid_(false), area_(0), nbins_(nbins), range_(range), delta_(0)
{
  if (nbins>0)
  {
    delta_ = range_/nbins;
    counts_.resize(nbins, T(0));
  }
}

template <class T>
void strk_hist<T>::upcount(T x, T mag)
{
  if (x<0||x>360)
    return;
  for (unsigned int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=x)
    {
      counts_[i] += mag;
      break;
    }
  area_valid_ = false;
}

template <class T>
void strk_hist<T>::compute_area() const
{
  area_ =0;
  for (unsigned int i = 0; i<nbins_; i++)
    area_ += counts_[i];
  area_valid_ = true;
}

template <class T>
T strk_hist<T>::p(unsigned int bin) const
{
  if (bin>=nbins_)
    return 0;
  if (!area_valid_)
    compute_area();
  if (area_ == T(0))
    return 0;
  else
    return counts_[bin]/area_;
}
template <class T>
T strk_hist<T>::area() const
{
  if (!area_valid_)
    compute_area();
  return area_;
}

template <class T>
void strk_hist<T>::parzen(const T sigma)
{
  if (sigma<=0)
    return;
  double sd = (double)sigma;
  vcl_vector<double> in(nbins_), out(nbins_);
  for (unsigned int i=0; i<nbins_; i++)
    in[i]=counts_[i];
  strk_1d_gaussian(sd, in, out);
  for (unsigned int i=0; i<nbins_; i++)
    counts_[i]=(T)out[i];
}

template <class T>
void strk_hist<T>::print() const
{
  for (unsigned int i=0; i<nbins_; i++)
    if (p(i) > 0)
      vcl_cout << "p[" << i << "]=" << p(i) << '\n';
}

template <class T>
strk_joint_hist<T>::strk_joint_hist(const T range,
                                    const unsigned int nbins)
  : volume_valid_(false), volume_(0), nbins_(nbins), range_(range), delta_(0)
{
  if (nbins>0)
  {
    delta_ = range_/nbins;
    counts_.resize(nbins, nbins);
    counts_.fill(T(0));
  }
}

template <class T>
void strk_joint_hist<T>::upcount(T a, T mag_a,
                                 T b, T mag_b)
{
  if (a<0||a>360)
    return;
  if (b<0||b>360)
    return;
  int bin_a =0, bin_b = 0;
  for (unsigned int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=a)
    {
      bin_a = i;
      break;
    }
  for (unsigned int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=b)
    {
      bin_b = i;
      break;
    }
  T v = counts_[bin_a][bin_b]+ mag_a + mag_b;
  counts_.put(bin_a, bin_b, v);
  volume_valid_ = false;
}

template <class T>
void strk_joint_hist<T>::compute_volume() const
{
  volume_=0;
  for (unsigned int a = 0; a<nbins_; a++)
    for (unsigned int b =0; b<nbins_; b++)
      volume_ += counts_[a][b];
  volume_valid_ = true;
}

template <class T>
T strk_joint_hist<T>::p(unsigned int a, unsigned int b) const
{
  if (a>=nbins_)
    return 0;
  if (b>=nbins_)
    return 0;
  if (!volume_valid_)
    compute_volume();
  if (volume_ == T(0))
    return 0;
  else
    return counts_[a][b]/volume_;
}

template <class T>
T strk_joint_hist<T>::volume() const
{
  if (!volume_valid_)
    compute_volume();
  return volume_;
}

template <class T>
void strk_joint_hist<T>::parzen(const T sigma)
{
  if (sigma<=0)
    return;
  double sd = (double)sigma;
  vbl_array_2d<double> in(nbins_, nbins_), out;
  for (unsigned int row = 0; row<nbins_; row++)
    for (unsigned int col = 0; col<nbins_; col++)
      in[row][col] = (double)counts_[row][col];

  strk_2d_gaussian(sd, in, out);

  for (unsigned int row = 0; row<nbins_; row++)
    for (unsigned int col = 0; col<nbins_; col++)
      counts_[row][col] = (T)out[row][col];
}

template <class T>
void strk_joint_hist<T>::print() const
{
  for (unsigned int a = 0; a<nbins_; a++)
    for (unsigned int b = 0; b<nbins_; b++)
      if (p(a,b) > 0)
        vcl_cout << "p[" << a << "][" << b << "]=" << p(a,b) << '\n';
}

//
//======================== TRACKING FACE IMPLEMENTATION =============
//
void strk_tracking_face_2d::centroid(double& x, double& y) const
{
  if (!intf_)
  {
    x = 0;
    y = 0;
  }
  vtol_face_2d_sptr f = intf_->cast_to_face_2d();
  vsol_point_2d_sptr p = btol_face_algs::centroid(f);
  x = p->x();
  y = p->y();
}

strk_tracking_face_2d::
strk_tracking_face_2d(vtol_face_2d_sptr const& face,
                      vil1_memory_image_of<float> const& image,
                      vil1_memory_image_of<float> const& Ix,
                      vil1_memory_image_of<float> const& Iy,
                      vil1_memory_image_of<float> const& hue,
                      vil1_memory_image_of<float> const& sat,
                      const float min_gradient,
                      const float parzen_sigma)
{
  min_gradient_ = min_gradient;
  parzen_sigma_ = parzen_sigma;
  intf_ = 0;
  Ix_ = 0;
  Iy_ = 0;
  hue_ = 0;
  sat_ = 0;
  intensity_mi_=0;
  gradient_dir_mi_=0;
  color_mi_=0;
  model_intensity_entropy_=0;
  model_gradient_dir_entropy_=0;
  model_color_entropy_=0;
  intensity_entropy_=0;
  gradient_dir_entropy_=0;
  color_entropy_=0;
  intensity_joint_entropy_=0;
  gradient_joint_entropy_=0;
  color_joint_entropy_=0;
  gradient_info_ = Ix&&Iy;
  color_info_ = hue&&sat;
  this->init_intensity_info(face, image);
  //cases
  if (gradient_info_)
    this->init_gradient_info(Ix, Iy);

  if (color_info_)
    this->init_color_info(hue, sat);
}

strk_tracking_face_2d::strk_tracking_face_2d (vtol_intensity_face_sptr const& intf)
{
  if (!intf)
    return;
  intf_ = intf;
  Ix_ = 0;
  Iy_ = 0;
  gradient_info_ = false;
  color_info_ = false;
  intensity_mi_=0;
  gradient_dir_mi_=0;
  color_mi_=0;
  model_intensity_entropy_=0;
  model_gradient_dir_entropy_=0;
  model_color_entropy_=0;
  intensity_entropy_=0;
  gradient_dir_entropy_=0;
  color_entropy_=0;
  intensity_joint_entropy_=0;
  gradient_joint_entropy_=0;
  color_joint_entropy_=0;
}

strk_tracking_face_2d::strk_tracking_face_2d(strk_tracking_face_2d_sptr const& tf)
{
  vcl_vector<vtol_vertex_sptr> verts, new_verts;
  vtol_intensity_face_sptr intf = tf->face();
  intf->vertices(verts);
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit!= verts.end(); vit++)
    new_verts.push_back(new vtol_vertex_2d((*vit)->cast_to_vertex_2d()->x(),
                                           (*vit)->cast_to_vertex_2d()->y()));

  vtol_face_2d_sptr f2d = new vtol_face_2d(new_verts);
  intf_= new vtol_intensity_face(f2d, intf->Npix(),
                                 intf->Xj(), intf->Yj(),
                                 intf->Ij());

  Ix_ = 0;
  Iy_ = 0;
  gradient_info_ = tf->gradient_info_;
  if (gradient_info_)
  {
    int n = intf_->Npix();
    Ix_ = new float[n];
    Iy_ = new float[n];
    for (int i =0; i<n; i++)
    {
      Ix_[i]=tf->Ix(i);
      Iy_[i]=tf->Iy(i);
    }
  }
  hue_ = 0;
  sat_ = 0;
  color_info_ = tf->color_info_;
  if (color_info_)
  {
    int n = intf_->Npix();
    hue_ = new float[n];
    sat_ = new float[n];
    for (int i =0; i<n; i++)
    {
      hue_[i]=tf->hue(i);
      sat_[i]=tf->sat(i);
    }
  }
  intensity_mi_ = tf->int_mutual_info();
  gradient_dir_mi_ = tf->grad_mutual_info();
  color_mi_ = tf->color_mutual_info();
  model_intensity_entropy_=tf->model_intensity_entropy_;
  model_gradient_dir_entropy_=tf->model_gradient_dir_entropy_;
  model_color_entropy_=tf->model_color_entropy_;
  intensity_entropy_=tf->intensity_entropy_;
  gradient_dir_entropy_=tf->gradient_dir_entropy_;
  color_entropy_=tf->color_entropy_;
  intensity_joint_entropy_=tf->intensity_joint_entropy_;
  gradient_joint_entropy_=tf->gradient_joint_entropy_;
  color_joint_entropy_=tf->color_joint_entropy_;
  min_gradient_ = tf->min_gradient_;
  parzen_sigma_ = tf->parzen_sigma_;
}

strk_tracking_face_2d::~strk_tracking_face_2d()
{
  delete [] Ix_;
  delete [] Iy_;
  delete [] hue_;
  delete [] sat_;
}

void strk_tracking_face_2d::set_gradient(vil1_memory_image_of<float> const& Ix,
                                         vil1_memory_image_of<float> const& Iy)
{
    int i = 0;
    if (!intf_||!Ix_||!Iy_)
      return;
    for (intf_->reset(); intf_->next();i++)
    {
      int x = int(intf_->X()), y = int(intf_->Y());
      this->set_Ix(i, Ix(x,y));
      this->set_Iy(i, Iy(x,y));
    }
}

void strk_tracking_face_2d::set_color(vil1_memory_image_of<float> const& hue,
                                      vil1_memory_image_of<float> const& sat)
{
    int i = 0;
    if (!intf_||!hue_||!sat_)
      return;
    for (intf_->reset(); intf_->next();i++)
    {
      int x = int(intf_->X()), y = int(intf_->Y());
      this->set_hue(i, hue(x,y));
      this->set_sat(i, sat(x,y));
    }
}

void strk_tracking_face_2d::
init_intensity_info(vtol_face_2d_sptr const& face,
                    vil1_memory_image_of<float> const& image)

{
  if (!face||!image)
    return;
  intf_ = new vtol_intensity_face(face);
  int width = image.width(), height = image.height();
  intf_->ResetPixelData();
  vgl_polygon<float> p;
  p.new_sheet();
  vcl_vector<vtol_vertex_sptr> verts;
  intf_->vertices(verts);
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end(); vit++)
    p.push_back(float((*vit)->cast_to_vertex_2d()->x()),
                float((*vit)->cast_to_vertex_2d()->y()));
  vgl_polygon_scan_iterator<float> psi(p, true);

  //go throught the pixels once to gather statistics for the face Npix etc.
  for (psi.reset(); psi.next();)
    for (int x = psi.startx(); x<=psi.endx(); x++)
    {
      int y = psi.scany();
      if (x<0||x>=width||y<0||y>=height)
        continue;

      unsigned short v = (unsigned short)image(x, y);
      intf_->IncrementMeans(float(x), float(y), v);
    }
  intf_->InitPixelArrays();

  strk_hist<float> model_intensity_hist(255, 16);
  intensity_hist_bins_ = model_intensity_hist.nbins();

  //Got through the pixels again to actually set the face arrays X(), Y() etc
  for (psi.reset(); psi.next();)
    for (int x = psi.startx(); x<=psi.endx(); x++)
    {
      int y = psi.scany();
      if (x<0||x>=width||y<0||y>=height)
        continue;
      unsigned short v = (unsigned short)image(x, y);
      model_intensity_hist.upcount(v, 1.0f);
      intf_->InsertInPixelArrays(float(x), float(y), v);
    }
  //apply parzen window to histogram
  vcl_cout << "\n\n Before Parzen(1d) - npix ="
           << model_intensity_hist.area() << vcl_endl;

  model_intensity_hist.print();
  model_intensity_hist.parzen(parzen_sigma_);
  vcl_cout << "After Parzen(1d)\n";
  model_intensity_hist.print();

  //compute the model entropy
  double ent = 0;
  for (unsigned int m = 0; m<intensity_hist_bins_; m++)
  {
    float pm = model_intensity_hist.p(m);
    if (!pm)
      continue;
    ent -= pm*vcl_log(pm);
  }
  model_intensity_entropy_ = float(ent/vcl_log(2.0));
}

void strk_tracking_face_2d::
init_gradient_info(vil1_memory_image_of<float> const& Ix,
                   vil1_memory_image_of<float> const& Iy)
{
  if (!intf_||!Ix||!Iy)
    return;
  int n = intf_->Npix();
  Ix_ = new float[n];
  Iy_ = new float[n];
  strk_hist<float> model_gradient_dir_hist;
  gradient_dir_hist_bins_ = model_gradient_dir_hist.nbins();
  int i = 0;
  double deg_rad = 180.0/vnl_math::pi;
  for (intf_->reset(); intf_->next(); i++)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    float Ixi = Ix(x,y), Iyi = Iy(x,y);
    // set the gradient values
    Ix_[i] = Ixi;  Iy_[i] = Iyi;
    float ang = float(deg_rad*vcl_atan2(Iyi, Ixi))+180.f;
    float mag = vcl_abs(Ixi)+vcl_abs(Iyi);
    if (mag>min_gradient_)
      model_gradient_dir_hist.upcount(ang, mag);
  }

  //apply parzen window to histogram
  model_gradient_dir_hist.parzen(parzen_sigma_);

  //compute the gradient direction entropy
  float ent = 0;
  for (unsigned int m = 0; m<gradient_dir_hist_bins_; m++)
  {
    float pm = model_gradient_dir_hist.p(m);
    if (!pm)
      continue;
    ent -= pm*vcl_log(pm);
  }
  model_gradient_dir_entropy_= float(ent/vcl_log(2.0));
}

void strk_tracking_face_2d::
init_color_info(vil1_memory_image_of<float> const& hue,
                vil1_memory_image_of<float> const& sat)
{
  if (!intf_||!hue||!sat)
    return;
  int n = intf_->Npix();
  hue_ = new float[n];
  sat_ = new float[n];
  strk_hist<float> model_color_hist;
  color_hist_bins_ = model_color_hist.nbins();
  int i = 0;
  for (intf_->reset(); intf_->next(); i++)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    float hue_i = hue(x,y), sat_i = sat(x,y);
    // set the gradient values
    hue_[i] = hue_i;  sat_[i] = sat_i;
    if (sat_i>0)
      model_color_hist.upcount(hue_i, sat_i);
  }

  //apply parzen window to histogram
  model_color_hist.parzen(parzen_sigma_);

  //compute the color entropy
  float ent = 0;
  for (unsigned int m = 0; m<color_hist_bins_; m++)
  {
    float pm = model_color_hist.p(m);
    if (!pm)
      continue;
    ent -= pm*vcl_log(pm);
  }
  model_color_entropy_= float(ent/vcl_log(2.0));
}

void strk_tracking_face_2d::set_int_mutual_info(float mi)
{
  intensity_mi_ = mi;
  total_info_= mi + gradient_dir_mi_+color_mi_;
}

void strk_tracking_face_2d::set_grad_mutual_info(float mi)
{
  gradient_dir_mi_ = mi;
  total_info_= mi + intensity_mi_ + color_mi_;
}

void strk_tracking_face_2d::set_color_mutual_info(float mi)
{
  color_mi_ = mi;
  total_info_= mi + intensity_mi_+ gradient_dir_mi_;
}

void strk_tracking_face_2d::transform(double tx, double ty,
                                      double theta, double scale)
{
  double xo = 0, yo =0;
  this->centroid(xo, yo);
  double c = vcl_cos(theta), s = vcl_sin(theta);
  vcl_vector<vtol_vertex_sptr> verts;
  this->face()->vertices(verts);
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end(); vit++)
  {
    vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
    if (!v)
      continue;
    double x = v->x(), y = v->y();
    double xp =(x-xo)*scale, yp =(y-yo)*scale;
    v->set_x(xp*c - yp*s + xo + tx);
    v->set_y(xp*s + yp*c + yo + ty);
  }
  for (this->reset(); this->next();)
  {
    double x = this->X(), y = this->Y();
    double xp =(x-xo)*scale, yp =(y-yo)*scale;
    this->set_X(float(xp*c - yp*s + xo + tx));
    this->set_Y(float(xp*s + yp*c + yo + ty));
  }
}

float strk_tracking_face_2d::
compute_intensity_mutual_information(vil1_memory_image_of<float> const& image)
{
  if (!intf_)
    return 0;

  int width = image.width(), height = image.height();
  //  strk_histf<float> image_hist;//JLM
  strk_hist<float> image_hist(255, 16);
  // strk_joint_histf<float> joint_hist;//JLM
  strk_joint_hist<float> joint_hist(255, 16);
  int npix = intf_->Npix();
  if (!npix)
    return 0;
  int n = 0;
  for (intf_->reset(); intf_->next();)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image(x,y);
    float Im = intf_->I();
    image_hist.upcount(Ii, 1.0f);//JLM
    joint_hist.upcount(Im, 1.0f, Ii, 1.0f); //JLM
    n++;
  }
  if (n<0.9*npix)
    return 0;
#ifdef DEBUG
  vcl_cout << "Image Hist\n";
  image_hist.print();
  vcl_cout << "Joint Hist\n";
  joint_hist.print();
#endif
  //apply parzen windows
  image_hist.parzen(parzen_sigma_);
  joint_hist.parzen(parzen_sigma_);

  //compute the mutual information
  int nbins = image_hist.nbins();
  float enti = 0, jent=0;
  for (int i = 0; i<nbins; i++)
  {
    float pi = image_hist.p(i);
    if (pi)
      enti -= pi*(float)vcl_log(pi);
    for (int m = 0; m<nbins; m++)
    {
      float jp = joint_hist.p(m,i);
      if (jp)
        jent -= jp*(float)vcl_log(jp);
    }
  }
  enti /= (float)vcl_log(2.0);
  intensity_entropy_=enti;
  jent /= (float)vcl_log(2.0);
  intensity_joint_entropy_ = jent;
  float mi = float(model_intensity_entropy_) + enti - jent;
#ifdef DEBUG
  vcl_cout << "Entropies:(M,I,J, MI)=(" << model_intensity_entropy_ << ' '
           << enti << ' ' << jent << ' ' << mi <<")\n";
#endif
  return mi;
}

float strk_tracking_face_2d::
compute_gradient_mutual_information(vil1_memory_image_of<float> const& Ix,
                                    vil1_memory_image_of<float> const& Iy)
{
  if (!intf_||!Ix||!Iy)
    return 0;
  int width = Ix.width(), height = Iy.height();
  strk_hist<float> image_dir_hist;
  strk_joint_hist<float> joint_dir_hist;

  int npix = intf_->Npix();
  if (!npix)
    return 0;
  double deg_rad = 180.0/vnl_math::pi;
  int i = 0, n = 0;
  for (intf_->reset(); intf_->next(); ++i, ++n)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ix0 = this->Ix(i), Iy0 = this->Iy(i);
    float ang0 = float(deg_rad*vcl_atan2(Iy0, Ix0))+180.f;
    float mag0 = vcl_abs(Ix0)+vcl_abs(Iy0); // was: vcl_sqrt(Ix0*Ix0 + Iy0*Iy0);
    float Ixi = Ix(x,y), Iyi = Iy(x,y);
    float angi = float(deg_rad*vcl_atan2(Iyi, Ixi))+180.f;
    float magi = vcl_abs(Ixi)+vcl_abs(Iyi); // was: vcl_sqrt(Ixi*Ixi + Iyi*Iyi);
#ifdef DEBUG
    vcl_cout << "ang0, mag0 " << ang0 << ' ' << mag0 << '\n'
             << "Ixi, Iyi " << Ixi << ' ' << Iyi << '\n'
             << "angi, magi " << angi << ' ' << magi << '\n';
#endif
    if (mag0>min_gradient_&&magi>min_gradient_)
    {
      image_dir_hist.upcount(angi, magi);
      joint_dir_hist.upcount(ang0,mag0,angi,magi);
    }
  }
  if (n<0.9*npix)
    return 0;
#ifdef DEBUG
  vcl_cout << "Image Dir Hist\n";
  image_dir_hist.print();
  vcl_cout << "Joint Dir Hist\n";
  joint_dir_hist.print();
#endif

  //apply parzen windows
  image_dir_hist.parzen(parzen_sigma_);
  joint_dir_hist.parzen(parzen_sigma_);

  int nbins = image_dir_hist.nbins();
  float enti = 0, jent=0;
  for (int i = 0; i<nbins; i++)
  {
    float pi = image_dir_hist.p(i);
    if (pi)
      enti -= pi*(float)vcl_log(pi);
    for (int m = 0; m<nbins; m++)
    {
      float jp = joint_dir_hist.p(m,i);
      if (jp)
        jent -= jp*(float)vcl_log(jp);
    }
  }
  enti /= (float)vcl_log(2.0);
  gradient_dir_entropy_ = enti;
  jent /= (float)vcl_log(2.0);
  gradient_joint_entropy_ = jent;
  float mi = float(model_gradient_dir_entropy_) + enti - jent;
#ifdef DEBUG
  vcl_cout << "Dir Entropies:(M,I,J, MI)=(" << model_intensity_entropy_ << ' '
           << enti << ' ' << jent << ' ' << mi <<")\n";
#endif
  return mi;
}

float strk_tracking_face_2d::
compute_color_mutual_information(vil1_memory_image_of<float> const& hue,
                                 vil1_memory_image_of<float> const& sat)
{
  if (!intf_||!hue||!sat)
    return 0;
  int width = hue.width(), height = hue.height();
  strk_hist<float> color_hist;
  strk_joint_hist<float> joint_color_hist;

  int npix = intf_->Npix();
  if (!npix)
    return 0;
  // double deg_rad = 180.0/vnl_math::pi;
  int i = 0, n = 0;
  for (intf_->reset(); intf_->next(); ++i, ++n)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float hue0 = this->hue(i), sat0 = this->sat(i);
    float hue_i = hue(x,y), sat_i = sat(x,y);
    if (sat_i>0)
      color_hist.upcount(hue_i, sat_i);
    if (sat0>0&&sat_i>0)
      joint_color_hist.upcount(hue0, sat0, hue_i, sat_i);
  }
  if (n<0.9*npix)
    return 0;

  //apply parzen windows
  color_hist.parzen(parzen_sigma_);
  joint_color_hist.parzen(parzen_sigma_);

  int nbins = color_hist.nbins();
  float enti = 0, jent=0;
  for (int i = 0; i<nbins; i++)
  {
    float pi = color_hist.p(i);
    if (pi)
      enti -= pi*(float)vcl_log(pi);
    for (int m = 0; m<nbins; m++)
    {
      float jp = joint_color_hist.p(m,i);
      if (jp)
        jent -= jp*(float)vcl_log(jp);
    }
  }
  enti /= (float)vcl_log(2.0);
  color_entropy_ = enti;
  jent /= (float)vcl_log(2.0);
  color_joint_entropy_=jent;
  float mi = float(model_color_entropy_) + enti - jent;
  return mi;
}

bool strk_tracking_face_2d::
compute_mutual_information(vil1_memory_image_of<float> const& image,
                           vil1_memory_image_of<float> const& Ix,
                           vil1_memory_image_of<float> const& Iy,
                           vil1_memory_image_of<float> const& hue,
                           vil1_memory_image_of<float> const& sat)
{
  if (!image)
    return false;

  if ((!Ix || !Iy) && gradient_info_)
    return false;

  if ((!hue || !sat) && color_info_)
    return false;

  this->set_int_mutual_info(this->compute_intensity_mutual_information(image));

  if (gradient_info_)
    this->set_grad_mutual_info(this->compute_gradient_mutual_information(Ix,Iy));

  if (color_info_)
    this->set_color_mutual_info(this->compute_color_mutual_information(hue, sat));

  return true;
}
#if 0
template class strk_histf<float>;
template class strk_joint_histf<float>;
#endif
template class strk_hist<float>;
template class strk_joint_hist<float>;
