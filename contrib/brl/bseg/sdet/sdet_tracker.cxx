// This is brl/bseg/sdet/sdet_tracker.cxx
#include "sdet_tracker.h"
//:
// \file
#include <vcl_cmath.h>   // for vcl_fabs(double)
#include <vcl_algorithm.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vil1/vil1_memory_image_of.h>
#include <vtol/vtol_intensity_face.h>
#include <vtol/vtol_vertex_2d.h>
#include <brip/brip_float_ops.h>

sdet_correlated_face::sdet_correlated_face()
{
  a_ = 0;
  c_ = 0;
  Ix_ = 0;
  Iy_ = 0;
  f_ = 0;
  intensity_mi_=0;
  gradient_dir_mi_=0;
}

sdet_correlated_face::~sdet_correlated_face()
{
  delete [] Ix_;
  delete [] Iy_;
}

void sdet_correlated_face::set_face(vtol_intensity_face_sptr const& f)
{
  f_ = f;
  if (Ix_)
    delete [] Ix_;
  if (Iy_)
    delete [] Iy_;
  int n = f->Npix();
  Ix_ = new float[n];
  Iy_ = new float[n];
}

void sdet_correlated_face::set_int_mutual_info(const float mi)
{
  intensity_mi_ = mi;
  total_info_= mi + gradient_dir_mi_;
}

void sdet_correlated_face::set_grad_mutual_info(const float mi)
{
  gradient_dir_mi_ = mi;
  total_info_= mi + intensity_mi_;
}

sdet_byte_hist::sdet_byte_hist(int nbins)
{
  if (!nbins||nbins<0)
  {
    nbins_=0;
    delta_=0;
    area_valid_ = false;
    area_=0;
    return;
  }
  nbins_ = nbins;
  delta_ = 256/nbins;
  counts_.resize(nbins, 0.0);
  area_valid_ = false;
  area_ = 0;
}

void sdet_byte_hist::upcount(const double x)
{
  if (x<0||x>255)
    return;
  for (int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=x)
    {
      counts_[i] += 1.0;
      break;
    }
  area_valid_ = false;
}

void sdet_byte_hist::compute_area()
{
  area_ =0;
  for (int i = 0; i<nbins_; i++)
    area_ += counts_[i];
  area_valid_ = true;
}

double sdet_byte_hist::p(const int bin)
{
  if (bin<0||bin>=nbins_)
    return 0;
  if (!area_valid_)
    compute_area();
  if (!area_)
    return 0;
  return counts_[bin]/area_;
}

void sdet_byte_hist::print()
{
  for (int i=0; i<nbins_; i++)
    if (p(i))
      vcl_cout << "p[" << i << "]=" << p(i) << '\n';
}

sdet_byte_joint_hist::sdet_byte_joint_hist(int nbins)
{
  if (!nbins||nbins<0)
  {
    nbins_=0;
    delta_=0;
    volume_valid_ = false;
    volume_=0;
    return;
  }
  nbins_ = nbins;
  delta_ = 256/nbins;
  counts_.resize(nbins, nbins);
  counts_.fill(0.0);
  volume_valid_ = false;
  volume_ = 0;
}

void sdet_byte_joint_hist::upcount(const double a, const double b)
{
  if (a<0||a>255)
    return;
  if (b<0||b>255)
    return;
  int bin_a =0, bin_b = 0;

  for (int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=a)
    {
      bin_a = i;
      break;
    }
  for (int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=b)
    {
      bin_b = i;
      break;
    }
  double v = counts_[bin_a][bin_b]+1.0;
  counts_.put(bin_a, bin_b, v);
  volume_valid_ = false;
}

void sdet_byte_joint_hist::compute_volume()
{
  volume_=0;
  for (int a = 0; a<nbins_; a++)
    for (int b =0; b<nbins_; b++)
      volume_ += counts_[a][b];
  volume_valid_ = true;
}

double sdet_byte_joint_hist::p(const int a, const int b)
{
  if (a<0||a>=nbins_)
    return 0;
  if (b<0||b>=nbins_)
    return 0;
  if (!volume_valid_)
    compute_volume();
  if (!volume_)
    return 0;
  return  counts_[a][b]/volume_;
}

void sdet_byte_joint_hist::print()
{
  for (int a = 0; a<nbins_; a++)
    for (int b = 0; b<nbins_; b++)
      if (p(a,b))
        vcl_cout << "p[" << a << "][" << b << "]=" << p(a,b) << '\n';
}

sdet_gradient_dir_hist::sdet_gradient_dir_hist(int nbins)
{
  if (!nbins||nbins<0)
  {
    nbins_=0;
    delta_=0;
    area_valid_ = false;
    area_=0;
    return;
  }
  nbins_ = nbins;
  delta_ = 360/nbins;
  counts_.resize(nbins, 0.0);
  area_valid_ = false;
  area_ = 0;
}

void sdet_gradient_dir_hist::upcount(const double x, const double mag)
{
  if (x<0||x>360)
    return;
  for (int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=x)
    {
      counts_[i] += mag;
      break;
    }
  area_valid_ = false;
}

void sdet_gradient_dir_hist::compute_area()
{
  area_ =0;
  for (int i = 0; i<nbins_; i++)
    area_ += counts_[i];
  area_valid_ = true;
}

double sdet_gradient_dir_hist::p(const int bin)
{
  if (bin<0||bin>=nbins_)
    return 0;
  if (!area_valid_)
    compute_area();
  if (!area_)
    return 0;
  return counts_[bin]/area_;
}

void sdet_gradient_dir_hist::print()
{
  for (int i=0; i<nbins_; i++)
    if (p(i))
      vcl_cout << "p[" << i << "]=" << p(i) << '\n';
}

sdet_gradient_dir_joint_hist::sdet_gradient_dir_joint_hist(int nbins)
{
  if (!nbins||nbins<0)
  {
    nbins_=0;
    delta_=0;
    volume_valid_ = false;
    volume_=0;
    return;
  }
  nbins_ = nbins;
  delta_ = 360/nbins;
  counts_.resize(nbins, nbins);
  counts_.fill(0.0);
  volume_valid_ = false;
  volume_ = 0;
}

void sdet_gradient_dir_joint_hist::upcount(const double a, const double mag_a,
                                           const double b, const double mag_b)
{
  if (a<0||a>360)
    return;
  if (b<0||b>360)
    return;
  int bin_a =0, bin_b = 0;
  for (int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=a)
    {
      bin_a = i;
      break;
    }
  for (int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=b)
    {
      bin_b = i;
      break;
    }
  double v = counts_[bin_a][bin_b]+ mag_a + mag_b;
  counts_.put(bin_a, bin_b, v);
  volume_valid_ = false;
}

void sdet_gradient_dir_joint_hist::compute_volume()
{
  volume_=0;
  for (int a = 0; a<nbins_; a++)
    for (int b =0; b<nbins_; b++)
      volume_ += counts_[a][b];
  volume_valid_ = true;
}

double sdet_gradient_dir_joint_hist::p(const int a, const int b)
{
  if (a<0||a>=nbins_)
    return 0;
  if (b<0||b>=nbins_)
    return 0;
  if (!volume_valid_)
    compute_volume();
  if (!volume_)
    return 0;
  return  counts_[a][b]/volume_;
}

void sdet_gradient_dir_joint_hist::print()
{
  for (int a = 0; a<nbins_; a++)
    for (int b = 0; b<nbins_; b++)
      if (p(a,b))
      vcl_cout << "p[" << a << "][" << b << "]=" << p(a,b) << '\n';
}

//Gives a sort on correlation score (currently decreasing values JLM)
static bool corr_compare(sdet_correlated_face* const f1,
                         sdet_correlated_face* const f2)
{
  return f1->total_info() > f2->total_info();//JLM Switched
}


//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
sdet_tracker::sdet_tracker(sdet_tracker_params& tp)
  : sdet_tracker_params(tp)
{
  hypothesized_samples_.clear();
  model_intensity_hist_=0;
  model_intensity_entropy_=0;
  model_gradient_dir_hist_=0;
  model_gradient_dir_entropy_=0;
}

//:Default Destructor
sdet_tracker::~sdet_tracker()
{
  for (vcl_vector<sdet_correlated_face*>::iterator
       cit = hypothesized_samples_.begin();
       cit != hypothesized_samples_.end(); cit++)
    delete *cit;
  hypothesized_samples_.clear();

  for (vcl_vector<sdet_correlated_face*>::iterator
       cit = current_samples_.begin();
       cit != current_samples_.end(); cit++)
    delete *cit;
  current_samples_.clear();
  if (model_intensity_hist_)
    delete model_intensity_hist_;
  if (model_gradient_dir_hist_)
    delete model_gradient_dir_hist_;
}

//-------------------------------------------------------------------------
//: Set the previous frame image
//
void sdet_tracker::set_image_0(vil1_image& image)
{
  if (!image)
  {
    vcl_cout <<"In sdet_tracker::set_image_i(.) - null input\n";
    return;
  }

  vil1_memory_image_of<float> flt=brip_float_ops::convert_to_float(image);

  image_0_= brip_float_ops::gaussian(flt, sigma_);

  int w = image_0_.width(), h = image_0_.height();
  Ix_0_.resize(w,h);
  Iy_0_.resize(w,h);
  brip_float_ops::gradient_3x3(image_0_, Ix_0_, Iy_0_);
}

//-------------------------------------------------------------------------
//: Set the next frame image
//
void sdet_tracker::set_image_i(vil1_image& image)
{
  if (!image)
  {
    vcl_cout <<"In sdet_tracker::set_image_i(.) - null input\n";
    return;
  }

  vil1_memory_image_of<float> flt=brip_float_ops::convert_to_float(image);

  image_i_ = brip_float_ops::gaussian(flt, sigma_);
  int w = image_i_.width(), h = image_i_.height();
  Ix_i_.resize(w,h);
  Iy_i_.resize(w,h);
  brip_float_ops::gradient_3x3(image_i_, Ix_i_, Iy_i_);
}

//--------------------------------------------------------------------------
//: Set the initial model position
void sdet_tracker::set_initial_model(vtol_face_2d_sptr const& face)
{
  initial_model_ = face;
  if (model_intensity_hist_)
    delete model_intensity_hist_;
  model_intensity_hist_ =0;

  if (model_gradient_dir_hist_)
    delete model_gradient_dir_hist_;
  model_gradient_dir_hist_=0;
}

//--------------------------------------------------------------------------
//: fill the pixels in the input face from the input image
void sdet_tracker::fill_face(vtol_intensity_face_sptr const& face,
                             vil1_memory_image_of<float> const& image)
{
  if (!face)
    return;
  int width = image.width(), height = image.height();
  face->ResetPixelData();
  vgl_polygon p;
  p.new_sheet();
  vcl_vector<vtol_vertex_sptr> verts;
  face->vertices(verts);
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end(); vit++)
    p.push_back(float((*vit)->cast_to_vertex_2d()->x()),
                float((*vit)->cast_to_vertex_2d()->y()));
  vgl_polygon_scan_iterator psi(p, true);

  //go throught the pixels once to gather statistics for the face Npix etc.
  for (psi.reset(); psi.next();)
    for (int x = psi.startx(); x<=psi.endx(); x++)
    {
      int y = psi.scany();
      if (x<0||x>=width||y<0||y>=height)
        continue;

      unsigned short v = (unsigned short)image(x, y);
      face->IncrementMeans(x, y, v);
    }
  face->InitPixelArrays();

  if (model_intensity_hist_)
    delete model_intensity_hist_;
  model_intensity_hist_ = new sdet_byte_hist();

  if (model_gradient_dir_hist_)
    delete model_gradient_dir_hist_;
  model_gradient_dir_hist_ = new sdet_gradient_dir_hist;
  double deg_rad = 180.0/vnl_math::pi;
  //Got through the pixels again to actually set the face arrays X(), Y() etc
  for (psi.reset(); psi.next();)
    for (int x = psi.startx(); x<=psi.endx(); x++)
    {
      int y = psi.scany();
      if (x<0||x>=width||y<0||y>=height)
        continue;
      unsigned short v = (unsigned short)image(x, y);
      model_intensity_hist_->upcount(v);
      face->InsertInPixelArrays(x, y, v);
      float Ix = Ix_0_(x,y), Iy = Iy_0_(x,y);
      double ang = (deg_rad*vcl_atan2(Iy, Ix))+180.0;
      //      double mag = vcl_sqrt(Ix*Ix + Iy*Iy);
      double mag = vcl_fabs(Ix)+vcl_fabs(Iy);
      model_gradient_dir_hist_->upcount(ang, mag);
    }
  //compute the model entropy
  int nbins = model_intensity_hist_->nbins();
  double ent = 0;
  for (int m = 0; m<nbins; m++)
  {
    double pm = model_intensity_hist_->p(m);
    if (!pm)
      continue;
    ent -= pm*vcl_log(pm);
  }
  model_intensity_entropy_ = ent/vcl_log(2.0);

  //compute the gradient direction entropy
  nbins = model_gradient_dir_hist_->nbins();
  ent = 0;
  for (int m = 0; m<nbins; m++)
  {
    double pm = model_gradient_dir_hist_->p(m);
    if (!pm)
      continue;
    ent -= pm*vcl_log(pm);
  }
  model_gradient_dir_entropy_= ent/vcl_log(2.0);
}

void sdet_tracker::set_gradient(sdet_correlated_face* cf,
                                vil1_memory_image_of<float> const& Ix,
                                vil1_memory_image_of<float> const& Iy)
{
  if (!cf)
    return;
  vtol_intensity_face_sptr f = cf->face();
  int i = 0;
  for (f->reset(); f->next();i++)
  {
    int x = int(f->X()), y = int(f->Y());
    cf->set_Ix(i, Ix(x,y));
    cf->set_Iy(i, Iy(x,y));
  }
}

//--------------------------------------------------------------------------
//: Initialize the tracker
void sdet_tracker::init()
{
  if (!image_0_)
    return;
  if (!initial_model_)
    return;
  vtol_intensity_face_sptr intf = new vtol_intensity_face(initial_model_);
  this->fill_face(intf, image_0_);
  sdet_correlated_face* cf = new sdet_correlated_face();
  cf->set_face(intf);
  this->set_gradient(cf, Ix_0_, Iy_0_);
  current_samples_.push_back(cf);
}

//--------------------------------------------------------------------------
//: Construct a new face which is a translated version of the input face
vtol_intensity_face_sptr
sdet_tracker::transform_face(vtol_intensity_face_sptr const& face,
                             double tx, double ty, double theta, double scale)
{
  if (!face)
    return 0;
  double xo = face->Xo(), yo = face->Yo();
  double c = vcl_cos(theta), s = vcl_sin(theta);
  //Get the digital region information
  int npix = face->Npix();
  float* X = new float[npix];
  float* Y = new float[npix];
  unsigned short* I = new unsigned short[npix];
  float const* Xj = face->Xj();
  float const* Yj = face->Yj();
  unsigned short const* Ij = face->Ij();
  for (int i=0; i<npix; i++)
  {
    double x = Xj[i], y = Yj[i];
    double xp =(x-xo)*scale, yp =(y-yo)*scale;
    X[i] = xp*c - yp*s + xo + tx;
    Y[i] = xp*s + yp*c + yo + ty;
    I[i]=Ij[i];
  }
  vcl_vector<vtol_vertex_sptr> verts, new_verts;
  face->vertices(verts);
  if (verts.size()<3)
    return 0;
  //translate the vertices
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end(); vit++)
  {
    vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
    if (!v)
      continue;
    double x = v->x(), y = v->y();
    double xp =(x-xo)*scale, yp =(y-yo)*scale;
    double new_x = xp*c - yp*s + xo + tx;
    double new_y = xp*s + yp*c + yo + ty;
    vtol_vertex_sptr new_v = new vtol_vertex_2d(new_x, new_y);
    new_verts.push_back(new_v);
  }
  vtol_face_2d_sptr f2d = new vtol_face_2d(new_verts);
  vtol_intensity_face_sptr new_int_face =
    new vtol_intensity_face(f2d, npix, X, Y, I);
#ifdef DEBUG
  vcl_cout << "Transformed Face Centroid (" << new_int_face->Xo()<< ' '
           <<  new_int_face->Yo() << ")\n";
#endif // DEBUG

  delete [] X;
  delete [] Y;
  delete [] I;
  return new_int_face;
}

//------------------------------------------------------------------------
//: Compute the local motion of the face
//
bool sdet_tracker::compute_motion(sdet_correlated_face* cf,
                                  double& tx, double& ty, double& theta)
{
  tx = 0; ty =0; theta =0;
  if (!cf)
    return false;
  int width = image_i_.width(), height = image_i_.height();
  vtol_intensity_face_sptr face = cf->face();
  if (!face)
    return false;
  int i = 0;
  double IxIx=0, IxIy = 0, IyIy =0;
  double bx = 0, by =0, Ith=0;
  double sn = 0, sd = 0;
  double xo = face->Xo(), yo = face->Yo();
  for (face->reset(); face->next();)
  {
    int x = int(face->X()), y = int(face->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image_i_(x,y);
    float dI = Ii-face->I();
    float Ix = cf->Ix(i), Iy = cf->Iy(i);
    Ith = (-Ix*(y-yo)+Iy*(x-xo));
    IxIx += Ix*Ix;
    IxIy += Ix*Iy;
    IyIy += Iy*Iy;
    bx -= dI*Ix;
    by -= dI*Iy;
    Ith = (-Ix*(y-yo)+Iy*(x-xo));
    sn += dI*Ith;
    sd += Ith*Ith;
    i++;
  }
  //Solve for tx and ty
  //the determinant
  double det = IxIx*IyIy-IxIy*IxIy;
  if (vcl_fabs(det)<1e-06||sd<1e-06)
    return false;
  //           -            -
  // tx    1  | IyIy   -IxIy | bx
  //    = --- |              |
  // ty   det |-IxIy    IxIx | by
  //           -            -
  //  tx = 1/det ( IyIy*bx - IxIy*by)
  //  ty = 1/det (-IxIy*bx + IyIy*by)
  //
  tx = ( IyIy*bx - IxIy*by)/det;
  ty = (-IxIy*bx + IxIx*by)/det;
  theta = sn/sd;
  return true;
}

double sdet_tracker::compute_gradient_angle(sdet_correlated_face* cf)
{
  if (!cf)
    return false;
  int width = image_i_.width(), height = image_i_.height();
  vtol_intensity_face_sptr face = cf->face();
  if (!face)
    return false;
  double IxIx=0, IxIy = 0, IyIy =0;
  for (face->reset(); face->next();)
  {
    int x = int(face->X()), y = int(face->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ix = Ix_i_(x,y), Iy = Iy_i_(x,y);
    IxIx += Ix*Ix;
    IxIy += Ix*Iy;
    IyIy += Iy*Iy;
  }
#ifdef DEBUG
  vcl_cout << " G = " << '\n' << IxIx << ' ' << IxIy << '\n'
           << IxIy << ' ' << IyIy << "\n\n";
#endif // DEBUG
  double theta_rad = 0.5*vcl_atan2(2*IxIy,(IyIy-IxIx));
  double theta_deg = (180*theta_rad)/vnl_math::pi;
  return theta_deg;
}

void sdet_tracker::compute_gradient_angle_hist(sdet_correlated_face* cf,
                                               vcl_vector<double>& ang_hist)
{
  if (!cf)
    return;
  int width = image_i_.width(), height = image_i_.height();
  vtol_intensity_face_sptr face = cf->face();
  if (!face)
    return;
  for (face->reset(); face->next();)
  {
    int x = int(face->X()), y = int(face->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ix = Ix_i_(x,y), Iy = Iy_i_(x,y);
    double ang = 180*vcl_atan2(Iy, Ix)/vnl_math::pi;
    double mag = vcl_sqrt(Ix*Ix + Iy*Iy);
    //convert to 0->360 range
    ang +=180;
    for (int i=0; i<8; ++i)
      if (ang<(i+1)*45)
      {
        ang_hist[i]+=mag;
        break;
      }
  }
}

double sdet_tracker::compute_angle_motion(sdet_correlated_face* cf)
{
  if (!cf)
    return 0;
  int width = image_i_.width(), height = image_i_.height();
  double sn = 0, sd=0;
  int i = 0;
  vtol_intensity_face_sptr face = cf->face();
  double xo = face->Xo(), yo = face->Yo();
  for (face->reset(); face->next();)
  {
    int x = int(face->X()), y = int(face->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image_i_(x,y);
    float dI = Ii-face->I();
    float Ix = cf->Ix(i), Iy = cf->Iy(i);
    double Ith = (-Ix*(y-yo)+Iy*(x-xo));
    sn += dI*Ith;
    sd += Ith*Ith;
  }
  double theta = 180*sn/(sd*vnl_math::pi);
  return theta;
}

bool sdet_tracker::
compute_scale_motion(sdet_correlated_face* cf, double& sx, double& sy)
{
  sx =1.0;
  sy =1.0;
  if (!cf)
    return false;
  int width = image_i_.width(), height = image_i_.height();
  double xxIxIx = 0, xyIxIy = 0, yyIyIy=0;
  double xdIIx = 0, ydIIy = 0;
  int i = 0;
  vtol_intensity_face_sptr face = cf->face();
  double xo = face->Xo(), yo = face->Yo();
  for (face->reset(); face->next();)
  {
    int x = int(face->X()), y = int(face->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image_i_(x,y);
    float dI = Ii-face->I();
    float Ix = cf->Ix(i), Iy = cf->Iy(i);
    float dx = x-xo, dy = y-yo;//local x,y
    xxIxIx += dx*dx*Ix*Ix;
    xyIxIy += dx*dy*Ix*Iy;
    yyIyIy += dy*dy*Iy*Iy;
    xdIIx -= dx*dI*Ix;
    ydIIy -= dy*dI*Iy;
  }
  double det = xxIxIx*yyIyIy-xyIxIy*xyIxIy;
  if (det<1e-06)
    return false;
  sx = 1+(yyIyIy*xdIIx - xyIxIy*ydIIy)/det;
  sy = 1+(xxIxIx*ydIIy - xyIxIy*xdIIx)/det;
  return true;
}

double sdet_tracker::compute_correlation(sdet_correlated_face* cf)
{
  if (!cf)
    return 0;
  int width = image_i_.width(), height = image_i_.height();
  double c = 0;
  int i = 0;
  vtol_intensity_face_sptr face = cf->face();
  for (face->reset(); face->next();)
  {
    int x = int(face->X()), y = int(face->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image_i_(x,y);
    float dI = Ii-face->I();
    c += dI*dI;
    i++;
  }
  if (i)
    return c/=i;
  else
    return 1e6;
}

double sdet_tracker::compute_intensity_mutual_information(sdet_correlated_face* cf)
{
  if (!cf)
    return 0;
  if (!model_intensity_hist_)
    return 0;
  int width = image_i_.width(), height = image_i_.height();
  sdet_byte_hist image_hist;
  sdet_byte_joint_hist joint_hist;

  vtol_intensity_face_sptr face = cf->face();
  int npix = face->Npix();
  if (!npix)
    return 0;
  int n = 0;
  for (face->reset(); face->next();)
  {
    int x = int(face->X()), y = int(face->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image_i_(x,y);
    float Im = face->I();
    image_hist.upcount(Ii);
    joint_hist.upcount(Im, Ii);
    n++;
  }
  float npixf = (float)npix, nf = (float)n;
    float frac = nf/npixf;
  if (frac<0.9)
    return 0;
#ifdef DEBUG
  vcl_cout << "Model Hist\n";
  model_intensity_hist_->print();
  vcl_cout << "Image Hist\n";
  image_hist.print();
  vcl_cout << "Joint Hist\n";
  joint_hist.print();
#endif

  //compute the mutual information
  int nbins = image_hist.nbins();
#if 0 // old implementation
  double mi = 0;
  for (int m = 0; m<nbins; m++)
  {
    double pm = model_intensity_hist_->p(m);
    if (!pm)
      continue;
    for (int i = 0; i<nbins; i++)
    {
      double pi = image_hist.p(i);
      if (!pi)
        continue;
      double jp = joint_hist.p(m,i);
      if (!jp)
        continue;
      double r = jp/(pm*pi);
      mi += jp*vcl_log(r);
    }
  }
  mi /= vcl_log(2.0);
#else // 0
  double enti = 0, jent=0;
  for (int i = 0; i<nbins; i++)
  {
    double pi = image_hist.p(i);
    if (pi)
      enti -= pi*vcl_log(pi);
    for (int m = 0; m<nbins; m++)
    {
      double jp = joint_hist.p(m,i);
      if (jp)
        jent -= jp*vcl_log(jp);
    }
  }
  enti /= vcl_log(2.0);
  jent /= vcl_log(2.0);
  double mi = model_intensity_entropy_ + enti - jent;
#ifdef DEBUG
  vcl_cout << "Entropies:(M,I,J, MI)=(" << model_intensity_entropy_ << ' '
           << enti << ' ' << jent << ' ' << mi <<")\n";
#endif
#endif // 0
  return mi;
}

double sdet_tracker::compute_gradient_mutual_information(sdet_correlated_face* cf)
{
  if (!cf)
    return 0;
  if (!model_gradient_dir_hist_)
    return 0;
  int width = image_i_.width(), height = image_i_.height();
  double mi = 0;
  sdet_gradient_dir_hist image_dir_hist;
  sdet_gradient_dir_joint_hist joint_dir_hist;

  vtol_intensity_face_sptr face = cf->face();
  int npix = face->Npix();
  if (!npix)
    return 0;
  double deg_rad = 180.0/vnl_math::pi;
  int i = 0, n = 0;
  for (face->reset(); face->next(); i++)
  {
    int x = int(face->X()), y = int(face->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ix0 = cf->Ix(i), Iy0 = cf->Iy(i);
    double ang0 = (deg_rad*vcl_atan2(Iy0, Ix0))+180.0;
    double mag0 = vcl_fabs(Ix0)+vcl_fabs(Iy0);// was: vcl_sqrt(Ix0*Ix0+Iy0*Iy0);
    float Ixi = Ix_i_(x,y), Iyi = Iy_i_(x,y);
    double angi = (deg_rad*vcl_atan2(Iyi, Ixi))+180.0;
    double magi = vcl_fabs(Ixi)+vcl_fabs(Iyi);// was: vcl_sqrt(Ixi*Ixi+Iyi*Iyi);
#ifdef DEBUG
    vcl_cout << "ang0, mag0 " << ang0 << ' ' << mag0 << '\n'
             << "Ixi, Iyi " << Ixi << ' ' << Iyi << '\n'
             << "angi, magi " << angi << ' ' << magi << '\n';
#endif // DEBUG
    image_dir_hist.upcount(angi, magi);
    joint_dir_hist.upcount(ang0,mag0,angi,magi);
    n++;
  }
  float npixf = (float)npix, nf = (float)n;
  float frac = nf/npixf;
  if (frac<0.9)
    return 0;
#ifdef DEBUG
  vcl_cout << "Model Dir Hist\n";
  model_gradient_dir_hist_->print();
  vcl_cout << "Image Dir Hist\n";
  image_dir_hist.print();
  vcl_cout << "Joint Dir Hist\n";
  joint_dir_hist.print();
#endif // DEBUG

  int nbins = image_dir_hist.nbins();
  double enti = 0, jent=0;
  for (int i = 0; i<nbins; i++)
  {
    double pi = image_dir_hist.p(i);
    if (pi)
      enti -= pi*vcl_log(pi);
    for (int m = 0; m<nbins; m++)
    {
      double jp = joint_dir_hist.p(m,i);
      if (jp)
        jent -= jp*vcl_log(jp);
    }
  }
  enti /= vcl_log(2.0);
  jent /= vcl_log(2.0);
  mi = model_gradient_dir_entropy_ + enti - jent;
#ifdef DEBUG
  vcl_cout << "Dir Entropies:(M,I,J, MI)=(" << model_intensity_entropy_ << ' '
           << enti << ' ' << jent << ' ' << mi <<")\n";
#endif // DEBUG
  return mi;
}

void sdet_tracker::mutual_info_face(sdet_correlated_face* cf)
{
  cf->set_int_mutual_info(this->compute_intensity_mutual_information(cf));
  if (gradient_info_)
    cf->set_grad_mutual_info(this->compute_gradient_mutual_information(cf));
  else
    cf->set_grad_mutual_info(0.0);
}

//--------------------------------------------------------------------------
//: perform a normalized cross-correlation between the face and image
void sdet_tracker::correlate_face(sdet_correlated_face* cf)
{
  double corr_thresh = 10;
  double tx=0, ty=0, theta = 0, c = 0;
  double sx=0, sy=0;//total translations
  c = vcl_sqrt(this->compute_correlation(cf));
  if (c>corr_thresh)
  {
    cf->set_correlation(c);
    return;
  }

  if (!this->compute_motion(cf, tx, ty, theta))
  {
    cf->set_correlation(1e6);
    return;
  }

  sx+=tx; sy+=ty;
#ifdef DEBUG
  vcl_cout << "Initial corr("<< tx << ' ' << ty <<  ")= " << this->compute_correlation(cf, tx, ty)<< '\n';
#endif // DEBUG
  //refine the position of the sample so that translation falls below a threshold
  bool done = false;
  int max_iter = 3;
  double thresh = 0.1;
  while ((!done)&&max_iter>0)
  {
    this->transform_sample_in_place(cf, tx, ty, theta);
    if (!this->compute_motion(cf, tx, ty, theta))
    {
      cf->set_correlation(1e6);
      return;
    }
       vcl_cout << "t["<< 3-max_iter << "] = (" << tx << ' ' << ty << ' '
                << 180*theta/vnl_math::pi << ") c = "
                << this->compute_correlation(cf) << '\n';
    if ((vcl_fabs(tx)<thresh)&&vcl_fabs(ty)<thresh&&vcl_fabs(theta)<0.01)
      done=true;
    sx+=tx; sy+=ty;
    max_iter--;
  }
  c = this->compute_correlation(cf);
#ifdef DEBUG
  vcl_cout << "Final corr(" << sx << ' ' << sy << ")= " << vcl_sqrt(c) << '\n';
#endif // DEBUG
  cf->set_correlation(vcl_sqrt(c));
}

//--------------------------------------------------------------------------
//: generate a randomly positioned face within a given radius
vtol_intensity_face_sptr
sdet_tracker::generate_sample(vtol_intensity_face_sptr const& seed)
{
  float x = (2.0*search_radius_)*(rand()/(RAND_MAX+1.0)) - search_radius_;
  float y = (2.0*search_radius_)*(rand()/(RAND_MAX+1.0)) - search_radius_;
  float theta = (2.0*angle_range_)*(rand()/(RAND_MAX+1.0)) - angle_range_;
  float s = (2.0*scale_range_)*(rand()/(RAND_MAX+1.0)) - scale_range_;
  float scale = 1+s;
  return this->transform_face(seed, x, y, theta, scale);
}

//--------------------------------------------------------------------------
//: generate a randomly positioned correlation face
sdet_correlated_face*
sdet_tracker::generate_cf_sample(sdet_correlated_face* seed)
{
  if (!seed)
    return 0;
  vtol_intensity_face_sptr f = this->generate_sample(seed->face());
  sdet_correlated_face* cf = new sdet_correlated_face();
  cf->set_face(f);
  int n = f->Npix();
  // copy the gradient values
  for (int i = 0; i<n; i++)
  {
    cf->set_Ix(i, seed->Ix(i));
    cf->set_Iy(i, seed->Iy(i));
  }
  return cf;
}

//--------------------------------------------------------------------------
//: Create a new sample with intensity and gradient information from its current location in the image.
sdet_correlated_face*
sdet_tracker::regenerate_cf_sample(sdet_correlated_face* sample)
{
  if (!sample)
    return 0;
  vtol_face_2d_sptr f = sample->face()->cast_to_face_2d();
  vtol_intensity_face_sptr intf = new vtol_intensity_face(f);
  this->fill_face(intf, image_i_);
  sdet_correlated_face* cf = new sdet_correlated_face();
  cf->set_face(intf);
  this->set_gradient(cf, Ix_i_, Iy_i_);
  return cf;
}

//--------------------------------------------------------------------------
//: Transform a sample in place, that is, move the X and Y values to X+tx and Y+ty, and rotate about the face center.
void sdet_tracker::transform_sample_in_place(sdet_correlated_face* sample,
                                             double tx, double ty,
                                             double theta)
{
  if (!sample)
    return;
  vtol_intensity_face_sptr face = sample->face();
  double xo = face->Xo(), yo = face->Yo();
  double c = vcl_cos(theta), s = vcl_sin(theta);
  vcl_vector<vtol_vertex_sptr> verts;
  face->vertices(verts);
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
      vit != verts.end(); vit++)
  {
    vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
    if (!v)
      continue;
    double x = v->x(), y = v->y();
    v->set_x((x-xo)*c -(y-yo)*s + xo + tx);
    v->set_y((x-xo)*s +(y-yo)*c + yo + ty);
  }
  for (face->reset(); face->next();)
  {
    double x = face->X(), y = face->Y();
    face->set_X((x-xo)*c -(y-yo)*s + xo + tx);
    face->set_Y((x-xo)*s +(y-yo)*c + yo + ty);
  }
}

//--------------------------------------------------------------------------
//: generate a random set of new faces from the existing samples
void sdet_tracker::generate_samples()
{
  vul_timer t;
  for (vcl_vector<sdet_correlated_face*>::iterator fit =
       current_samples_.begin(); fit != current_samples_.end(); fit++)
    for (int i = 0; i<n_samples_; i++)
    {
      sdet_correlated_face* cf = this->generate_cf_sample(*fit);
      //      this->correlate_face(cf);
      this->mutual_info_face(cf);
      hypothesized_samples_.push_back(cf);
    }
  //sort the hypotheses
  vcl_sort(hypothesized_samples_.begin(),
           hypothesized_samples_.end(), corr_compare);
}

//--------------------------------------------------------------------------
//: cull out the best N hypothesized samples to become the current samples
void sdet_tracker::cull_samples()
{
  vcl_cout << "Culling\n";
  for (vcl_vector<sdet_correlated_face*>::iterator
       cit = current_samples_.begin();
       cit != current_samples_.end(); cit++)
    delete *cit;
  current_samples_.clear();
  for (int i =0; i<n_samples_; i++)
  {
#ifdef DEBUG
   vcl_cout << "Corr = " << hypothesized_samples_[i]->correlation() << '\n';
#endif // DEBUG
    current_samples_.push_back(hypothesized_samples_[i]);
    vcl_cout << vcl_flush;
  }
  vcl_cout << "Total Inf = " << hypothesized_samples_[0]->total_info()
           << " = IntInfo(" <<  hypothesized_samples_[0]->int_mutual_info()
           << ") + GradInfo(" <<  hypothesized_samples_[0]->grad_mutual_info()
           << ")\n";
#ifdef DEBUG
  double sx =0, sy =0;
  this->compute_scale_motion(hypothesized_samples_[0], sx , sy);
  vcl_cout << " Motion Scale = (" << sx << ' ' << sy << ")\n"
           << compute_gradient_angle(hypothesized_samples_[0])<< '\n'
           << compute_angle_motion(hypothesized_samples_[0])<< '\n';
  vcl_vector<double> hist(8);
  this->compute_gradient_angle_hist(hypothesized_samples_[0], hist);
  for (int i = 0; i<8; i++)
    vcl_cout << "H["<< i << "] = " << hist[i] << '\n';
#endif // DEBUG
  for (unsigned int i=n_samples_; i<hypothesized_samples_.size(); ++i)
    delete hypothesized_samples_[i];
  hypothesized_samples_.clear();
}

//--------------------------------------------------------------------------
//: because of sorting, the best sample will be the first current sample
vtol_face_2d_sptr sdet_tracker::get_best_sample()
{
  if (!current_samples_.size())
    return 0;
  return current_samples_[0]->face()->cast_to_face_2d();
}

//--------------------------------------------------------------------------
//: because of sorting the samples will be in descending order of correlation
void sdet_tracker::get_samples(vcl_vector<vtol_face_2d_sptr>& samples)
{
  samples.clear();
  for (vcl_vector<sdet_correlated_face*>::iterator
       fit = current_samples_.begin(); fit != current_samples_.end(); fit++)
    samples.push_back((*fit)->face()->cast_to_face_2d());
}

//--------------------------------------------------------------------------
//: Main tracking method
void sdet_tracker::track()
{
  vul_timer t;
  this->generate_samples();
  vcl_cout << "Samples generated " << t.real() << " msecs.\n";
  this->cull_samples();

#if 0
  sdet_correlated_face* best = current_samples_[0];
  if (best)
  {
    sdet_correlated_face* regenerated_best=this->regenerate_cf_sample(best);
    current_samples_[0]=regenerated_best;
    delete best;
  }
#endif // 0
}

void sdet_tracker::clear()
{
  current_samples_.clear();
  for (vcl_vector<sdet_correlated_face*>::iterator
       cit = hypothesized_samples_.begin();
       cit != hypothesized_samples_.end(); cit++)
    delete *cit;
  hypothesized_samples_.clear();
}
