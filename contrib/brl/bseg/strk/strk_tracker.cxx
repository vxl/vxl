// This is brl/bseg/strk/strk_tracker.cxx
#include "strk_tracker.h"
//:
// \file
#include <vcl_cmath.h> // for vcl_fabs(double)
#include <vcl_algorithm.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vil1/vil1_memory_image_of.h>
#include <vtol/vtol_intensity_face.h>
#include <vtol/vtol_vertex_2d.h>
#include <brip/brip_float_ops.h>

strk_correlated_face::strk_correlated_face()
{
  Ix_ = 0;
  Iy_ = 0;
  c_ = 0;
  f_ = 0;
}

strk_correlated_face::~strk_correlated_face()
{
  delete[] Ix_;
  delete[] Iy_;
}

void strk_correlated_face::set_face(vtol_intensity_face_sptr const& f)
{
  f_ = f;
  if (Ix_)
    delete[] Ix_;
  if (Iy_)
    delete[] Iy_;
  int n = f->Npix();
  Ix_ = new float[n];
  Iy_ = new float[n];
}

//Gives a sort on correlation score (currently increasing values JLM)
static bool corr_compare(strk_correlated_face* const f1,
                         strk_correlated_face* const f2)
{
  return f1->correlation() < f2->correlation();
}

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
strk_tracker::strk_tracker(strk_tracker_params& tp)
  : strk_tracker_params(tp)
{
  hypothesized_samples_.clear();
}

//:Default Destructor
strk_tracker::~strk_tracker()
{
  for (vcl_vector<strk_correlated_face*>::iterator
       cit = hypothesized_samples_.begin();
       cit != hypothesized_samples_.end(); cit++)
    delete *cit;
  hypothesized_samples_.clear();

  for (vcl_vector<strk_correlated_face*>::iterator
       cit = current_samples_.begin();
       cit != current_samples_.end(); cit++)
    delete *cit;
  current_samples_.clear();
}

void strk_tracker::set_gradient(strk_correlated_face* cf,
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

//-------------------------------------------------------------------------
//: Set the previous frame image
//
void strk_tracker::set_image_0(vil1_image& image)
{
  if (!image)
  {
    vcl_cout <<"In strk_tracker::set_image_i(.) - null input\n";
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
void strk_tracker::set_image_i(vil1_image& image)
{
  if (!image)
  {
    vcl_cout <<"In strk_tracker::set_image_i(.) - null input\n";
    return;
  }

  vil1_memory_image_of<float> flt=brip_float_ops::convert_to_float(image);

  image_i_ = brip_float_ops::gaussian(flt, sigma_);
}

//--------------------------------------------------------------------------
//: Set the initial model position
void strk_tracker::set_initial_model(vtol_face_2d_sptr const& face)
{
  initial_model_ = face;
}

//--------------------------------------------------------------------------
//: fill the pixels in the input face from the input image
void strk_tracker::fill_face(vtol_intensity_face_sptr const& face,
                             vil1_memory_image_of<float> const& image)
{
  if (!face)
    return;
  int width = image.width(), height = image.height();
  face->ResetPixelData();
  vgl_polygon<float> p;
  p.new_sheet();
  vcl_vector<vtol_vertex_sptr> verts;
  face->vertices(verts);
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end(); vit++)
    p.push_back(float((*vit)->cast_to_vertex_2d()->x()),
                float((*vit)->cast_to_vertex_2d()->y()));
  vgl_polygon_scan_iterator<float> psi(p, true);

  //go throught the pixels once to gather statistics for the face Npix etc.
  for (psi.reset(); psi.next(); )
    for (int x = psi.startx(); x<=psi.endx(); x++)
    {
      int y = psi.scany();
      if (x<0||x>=width||y<0||y>=height)
        continue;

      unsigned short v = (unsigned short)image(x, y);
      face->IncrementMeans(float(x), float(y), v);
    }
  face->InitPixelArrays();
  //Got through the pixels again to actually set the face arrays X(), Y() etc
  for (psi.reset(); psi.next();)
    for (int x = psi.startx(); x<=psi.endx(); x++)
    {
      int y = psi.scany();
      if (x<0||x>=width||y<0||y>=height)
        continue;
      unsigned short v = (unsigned short)image(x, y);
      face->InsertInPixelArrays(float(x), float(y), v);
    }
}

//--------------------------------------------------------------------------
//: Initialize the tracker
void strk_tracker::init()
{
  if (!image_0_)
    return;
  if (!initial_model_)
    return;
  vtol_intensity_face_sptr intf = new vtol_intensity_face(initial_model_);
  this->fill_face(intf, image_0_);
  strk_correlated_face* cf = new strk_correlated_face();
  cf->set_face(intf);
  this->set_gradient(cf, Ix_0_, Iy_0_);
  current_samples_.push_back(cf);
}

//--------------------------------------------------------------------------
//: Construct a new face which is a translated version of the input face
vtol_intensity_face_sptr
strk_tracker::transform_face(vtol_intensity_face_sptr const& face,
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
    X[i] = float(xp*c - yp*s + xo + tx);
    Y[i] = float(xp*s + yp*c + yo + ty);
    I[i] = Ij[i];
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
           << new_int_face->Yo() << ")\n";
#endif // DEBUG

  delete[] X;
  delete[] Y;
  delete[] I;
  return new_int_face;
}

//------------------------------------------------------------------------
//: Compute the local motion of the face
//
bool strk_tracker::compute_motion(strk_correlated_face* cf,
                                  double& tx, double& ty, double& theta,
                                  double& scale)
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
  double bx = 0, by =0, Ith=0, Isc=0;
  double sn_th = 0, sd_th = 0, sn_sc = 0, sd_sc=0;
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
    Isc = (Ix*(x-xo)+Iy*(y-yo));
    IxIx += Ix*Ix;
    IxIy += Ix*Iy;
    IyIy += Iy*Iy;
    bx -= dI*Ix;
    by -= dI*Iy;
    sn_th += dI*Ith;
    sd_th += Ith*Ith;
    sn_sc += dI*Isc;
    sd_sc += Isc*Isc;
    i++;
  }

  //Solve for tx and ty
  //the determinant
  double det = IxIx*IyIy-IxIy*IxIy;
  if (vcl_fabs(det)<1e-06||sd_th<1e-06||sd_sc<1.e-06)
    return false;
  //           -            -
  // tx    1  | IyIy   -IxIy | bx
  //    = --- |              |
  // ty   det |-IxIy    IxIx | by
  //           -            -
  //
  tx = ( IyIy*bx - IxIy*by)/det;
  ty = (-IxIy*bx + IxIx*by)/det;
  theta = -sn_th/sd_th;
  scale = 1.0 - sn_sc/sd_sc;
  return true;
}

double strk_tracker::compute_gradient_angle(strk_correlated_face* cf)
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


double strk_tracker::compute_angle_motion(strk_correlated_face* cf)
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

bool strk_tracker::
compute_scale_motion(strk_correlated_face* cf, double& sx, double& sy)
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
    float dx = float(x-xo), dy = float(y-yo); // local x,y
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

double strk_tracker::compute_correlation(strk_correlated_face* cf)
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

//--------------------------------------------------------------------------
//: perform a normalized cross-correlation between the face and image
void strk_tracker::correlate_face(strk_correlated_face* cf)
{
  float corr_thresh = 100.f;
  double tx=0, ty=0, theta = 0, scale=1.0;
  double sx=0, sy=0;//total translations
  double sth =0, psc = 1.0;
  double c = vcl_sqrt(this->compute_correlation(cf));
  if (c>corr_thresh)
  {
    cf->set_correlation(float(c));
    return;
  }

  if (!this->compute_motion(cf, tx, ty, theta, scale))
  {
    cf->set_correlation(1e6f);
    return;
  }

  sx+=tx; sy+=ty;
#ifdef DEBUG
  vcl_cout << "Initial corr("<< tx << ' ' << ty << ")= " << this->compute_correlation(cf, tx, ty)<< '\n';
#endif // DEBUG
  //refine the position of the sample so that translation falls below a threshold
  bool done = false;
  int max_iter = 3;
  double thresh = 0.1;

  while ((!done)&&max_iter>0)
  {
    this->transform_sample_in_place(cf, tx, ty, theta, scale);
    if (!this->compute_motion(cf, tx, ty, theta, scale))
    {
      cf->set_correlation(1e6f);
      return;
    }
    if ((vcl_fabs(tx)<thresh)&&vcl_fabs(ty)<thresh&&vcl_fabs(theta)<0.01
        &&vcl_fabs(scale-1.0)<0.01)
      done=true;
    sx+=tx; sy+=ty;
    sth += theta;
    psc *=scale;
    max_iter--;
  }
  c = this->compute_correlation(cf);
#ifdef DEBUG
  vcl_cout << "Final corr(" << sx << " " << sy << " " << sth << " " << psc
           << ")= " << vcl_sqrt(c) << '\n';
#endif //DEBUG
  cf->set_correlation((float)vcl_sqrt(c));
}

//--------------------------------------------------------------------------
//: generate a randomly positioned face within a given radius
vtol_intensity_face_sptr
strk_tracker::generate_sample(vtol_intensity_face_sptr const& seed)
{
  float x = (2.f*search_radius_)*float(rand()/(RAND_MAX+1.f)) - search_radius_;
  float y = (2.f*search_radius_)*float(rand()/(RAND_MAX+1.f)) - search_radius_;
  float theta = (2.f*angle_range_)*float(rand()/(RAND_MAX+1.f)) - angle_range_;
  float s = (2.f*scale_range_)*float(rand()/(RAND_MAX+1.f)) - scale_range_;
  float scale = 1+s;
  return this->transform_face(seed, x, y, theta, scale);
}

//--------------------------------------------------------------------------
//: generate a randomly positioned correlation face
strk_correlated_face*
strk_tracker::generate_cf_sample(strk_correlated_face* seed)
{
  if (!seed)
    return 0;
  vtol_intensity_face_sptr f = this->generate_sample(seed->face());
  strk_correlated_face* cf = new strk_correlated_face();
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
strk_correlated_face*
strk_tracker::regenerate_cf_sample(strk_correlated_face* sample)
{
  if (!sample)
    return 0;
  vtol_face_2d_sptr f = sample->face()->cast_to_face_2d();
  vtol_intensity_face_sptr intf = new vtol_intensity_face(f);
  this->fill_face(intf, image_i_);
  strk_correlated_face* cf = new strk_correlated_face();
  cf->set_face(intf);
  this->set_gradient(cf, Ix_i_, Iy_i_);
  return cf;
}

//--------------------------------------------------------------------------
//: Transform a sample in place, that is, move the X and Y values to X+tx and Y+ty, and rotate about the face center.
void strk_tracker::transform_sample_in_place(strk_correlated_face* sample,
                                             double tx, double ty,
                                             double theta, double scale)
{
  if (!sample)
    return;
  vtol_intensity_face_sptr face = sample->face();
  float xo = face->Xo(), yo = face->Yo();
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
    double xp =(x-xo)*scale, yp =(y-yo)*scale;
    v->set_x(xp*c - yp*s + xo + tx);
    v->set_y(xp*s + yp*c + yo + ty);
  }
  for (face->reset(); face->next();)
  {
    float x = face->X(), y = face->Y();
    double xp =(x-xo)*scale, yp =(y-yo)*scale;
    face->set_X(float(xp*c - yp*s + xo + tx));
    face->set_Y(float(xp*s + yp*c + yo + ty));
  }
}

//--------------------------------------------------------------------------
//: generate a random set of new faces from the existing samples
void strk_tracker::generate_samples()
{
  vul_timer t;
  for (vcl_vector<strk_correlated_face*>::iterator fit =
       current_samples_.begin(); fit != current_samples_.end(); fit++)
    for (int i = 0; i<n_samples_; i++)
    {
      strk_correlated_face* cf = this->generate_cf_sample(*fit);
      this->correlate_face(cf);
      hypothesized_samples_.push_back(cf);
    }
  //sort the hypotheses
  vcl_sort(hypothesized_samples_.begin(),
           hypothesized_samples_.end(), corr_compare);
}

//--------------------------------------------------------------------------
//: cull out the best N hypothesized samples to become the current samples
void strk_tracker::cull_samples()
{
  for (vcl_vector<strk_correlated_face*>::iterator
       cit = current_samples_.begin();
       cit != current_samples_.end(); cit++)
    delete *cit;
  current_samples_.clear();

  for (int i =0; i<n_samples_; i++)
  {
#ifdef DEBUG
    vcl_cout << "Corr = " << hypothesized_samples_[i]->correlation() << '\n';
    vcl_cout << vcl_flush;
#endif //DEBUG
    current_samples_.push_back(hypothesized_samples_[i]);
  }

  for (unsigned int i=n_samples_; i<hypothesized_samples_.size(); ++i)
    delete hypothesized_samples_[i];
  hypothesized_samples_.clear();
}

//--------------------------------------------------------------------------
//: because of sorting, the best sample will be the first current sample
vtol_face_2d_sptr strk_tracker::get_best_sample()
{
  if (!current_samples_.size())
    return 0;
  return current_samples_[0]->face()->cast_to_face_2d();
}

//--------------------------------------------------------------------------
//: because of sorting the samples will be in descending order of correlation
void strk_tracker::get_samples(vcl_vector<vtol_face_2d_sptr>& samples)
{
  samples.clear();
  for (vcl_vector<strk_correlated_face*>::iterator
       fit = current_samples_.begin(); fit != current_samples_.end(); fit++)
    samples.push_back((*fit)->face()->cast_to_face_2d());
}

//--------------------------------------------------------------------------
//: Main tracking method
void strk_tracker::track()
{
  vul_timer t;
  this->generate_samples();
  this->cull_samples();

  //strk_correlated_face* best = current_samples_[0];
}

void strk_tracker::clear()
{
  current_samples_.clear();
  for (vcl_vector<strk_correlated_face*>::iterator
       cit = hypothesized_samples_.begin();
       cit != hypothesized_samples_.end(); cit++)
    delete *cit;
  hypothesized_samples_.clear();
}
