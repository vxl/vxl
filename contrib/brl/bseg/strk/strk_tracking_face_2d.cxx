// This is brl/bseg/strk/strk_tracking_face_2d.cxx
#include "strk_tracking_face_2d.h"
//:
// \file
// See strk_tracking_face_2d.h
//
//-----------------------------------------------------------------------------
#include <vcl_cmath.h> // for log(), exp() ..
#include <vcl_cstdlib.h> // for rand()
#include <vnl/vnl_math.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <bsta/bsta_joint_histogram.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge.h>
#include <btol/btol_face_algs.h>


//#define DEBUG

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

static vnl_matrix_fixed<double,3,3> ident()
{
  vnl_matrix_fixed<double,3,3> M;
  M[0][0] = 1.0;   M[0][1] = 0.0;   M[0][2] = 0.0;
  M[1][0] = 0.0;   M[1][1] = 1.0;   M[1][2] = 0.0;
  M[2][0] = 0.0;   M[2][1] = 0.0;   M[2][2] = 1.0;
  return M;
}
void strk_tracking_face_2d::init_bins()
{
  intensity_hist_bins_=16;
  gradient_dir_hist_bins_=8;
  color_hist_bins_=8;
}

strk_tracking_face_2d::
strk_tracking_face_2d(vtol_face_2d_sptr const& face,
                      vil1_memory_image_of<float> const& image,
                      vil1_memory_image_of<float> const& Ix,
                      vil1_memory_image_of<float> const& Iy,
                      vil1_memory_image_of<float> const& hue,
                      vil1_memory_image_of<float> const& sat,
                      const float min_gradient,
                      const float parzen_sigma,
                      const unsigned int intensity_hist_bins,
                      const unsigned int gradient_dir_hist_bins,
                      const unsigned int color_hist_bins)
{
  intensity_hist_bins_ = intensity_hist_bins;
  gradient_dir_hist_bins_=gradient_dir_hist_bins;
  color_hist_bins_=color_hist_bins;
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
  model_intensity_joint_entropy_=0;
  gradient_joint_entropy_=0;
  color_joint_entropy_=0;
  intensity_info_diff_ = 0;
  color_info_diff_ = 0;
  gradient_info_ = Ix&&Iy;
  color_info_ = hue&&sat;
  renyi_joint_entropy_ = false;
  this->init_intensity_info(face, image);
  //cases
  if (gradient_info_)
    this->init_gradient_info(Ix, Iy);

  if (color_info_)
    this->init_color_info(hue, sat);
  trans_ = ident();
}

strk_tracking_face_2d::strk_tracking_face_2d(vtol_intensity_face_sptr const& intf,
											 const unsigned int intensity_hist_bins,
                        const unsigned int gradient_dir_hist_bins,
                        const unsigned int color_hist_bins
                        )
{
  intensity_hist_bins_ = intensity_hist_bins;
  gradient_dir_hist_bins_=gradient_dir_hist_bins;
  color_hist_bins_=color_hist_bins;
  if (!intf)
    return;
  intf_ = intf;
  Ix_ = 0;
  Iy_ = 0;
  gradient_info_ = false;
  color_info_ = false;
  renyi_joint_entropy_ = false;
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
  model_intensity_joint_entropy_=0;
  gradient_joint_entropy_=0;
  color_joint_entropy_=0;
  intensity_info_diff_ = 0;
  color_info_diff_ = 0;
  trans_ = ident();
}

strk_tracking_face_2d::strk_tracking_face_2d(strk_tracking_face_2d_sptr const& tf)
{
  intensity_hist_bins_ = tf->intensity_hist_bins_;
  gradient_dir_hist_bins_= tf->gradient_dir_hist_bins_;
  color_hist_bins_= tf->color_hist_bins_;
  vtol_intensity_face_sptr intf = tf->face();
  vtol_face_2d_sptr f2d = new vtol_face_2d(intf->cast_to_face_2d());
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
  renyi_joint_entropy_ = tf->renyi_joint_entropy_;
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
  model_intensity_joint_entropy_=tf->model_intensity_joint_entropy_;
  gradient_joint_entropy_=tf->gradient_joint_entropy_;
  color_joint_entropy_=tf->color_joint_entropy_;
  intensity_info_diff_ = tf->intensity_info_diff_;
  color_info_diff_ = tf->color_info_diff_;
  min_gradient_ = tf->min_gradient_;
  parzen_sigma_ = tf->parzen_sigma_;
  trans_ = tf->trans_;
}

strk_tracking_face_2d::strk_tracking_face_2d(strk_tracking_face_2d const& tf)
  : vbl_ref_count()
{
  vtol_intensity_face_sptr intf = tf.face();
  vtol_face_2d_sptr f2d = new vtol_face_2d(intf->cast_to_face_2d());
  intf_= new vtol_intensity_face(f2d, intf->Npix(),
                                 intf->Xj(), intf->Yj(),
                                 intf->Ij());

  Ix_ = 0;
  Iy_ = 0;
  gradient_info_ = tf.gradient_info_;
  if (gradient_info_)
  {
    int n = intf_->Npix();
    Ix_ = new float[n];
    Iy_ = new float[n];
    for (int i =0; i<n; i++)
    {
      Ix_[i]=tf.Ix(i);
      Iy_[i]=tf.Iy(i);
    }
  }
  hue_ = 0;
  sat_ = 0;
  color_info_ = tf.color_info_;
  if (color_info_)
  {
    int n = intf_->Npix();
    hue_ = new float[n];
    sat_ = new float[n];
    for (int i =0; i<n; i++)
    {
      hue_[i]=tf.hue(i);
      sat_[i]=tf.sat(i);
    }
  }
  renyi_joint_entropy_ = tf.renyi_joint_entropy_;
  intensity_mi_ = tf.int_mutual_info();
  gradient_dir_mi_ = tf.grad_mutual_info();
  color_mi_ = tf.color_mutual_info();
  model_intensity_entropy_=tf.model_intensity_entropy_;
  model_gradient_dir_entropy_=tf.model_gradient_dir_entropy_;
  model_color_entropy_=tf.model_color_entropy_;
  intensity_entropy_=tf.intensity_entropy_;
  gradient_dir_entropy_=tf.gradient_dir_entropy_;
  color_entropy_=tf.color_entropy_;
  intensity_joint_entropy_=tf.intensity_joint_entropy_;
  model_intensity_joint_entropy_=tf.model_intensity_joint_entropy_;
  gradient_joint_entropy_=tf.gradient_joint_entropy_;
  color_joint_entropy_=tf.color_joint_entropy_;
  intensity_info_diff_ = tf.intensity_info_diff_;
  color_info_diff_ = tf.color_info_diff_;
  min_gradient_ = tf.min_gradient_;
  parzen_sigma_ = tf.parzen_sigma_;
  trans_ = tf.trans_;
  intensity_hist_bins_=tf.intensity_hist_bins_;
  gradient_dir_hist_bins_=tf.gradient_dir_hist_bins_;
  color_hist_bins_=tf.color_hist_bins_;
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
  vgl_polygon<double> p;
  if (!btol_face_algs::vtol_to_vgl(intf_->cast_to_face_2d(), p))
    return;
  vgl_polygon_scan_iterator<double> psi(p, true);

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
  if(!intf_->Npix())
    {
      vcl_cout << "In strk_tracking_face_2d::strk_tracking_face_2d(..) -"
               << " no pixels\n";
      return;
    }
    intf_->InitPixelArrays();

  bsta_histogram<float> model_intensity_hist(255, intensity_hist_bins_);
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
#ifdef DEBUG
  vcl_cout << "\n\n Before Parzen(1d) - npix ="
           << model_intensity_hist.area() << vcl_endl;

  model_intensity_hist.print();
#endif //DEBUG

  model_intensity_hist.parzen(parzen_sigma_);

#ifdef DEBUG
  vcl_cout << "After Parzen(1d)\n";
  model_intensity_hist.print();
#endif //DEBUG
  //compute the model entropy
  model_intensity_entropy_ = model_intensity_hist.entropy();
}

//: fill the gradient values in the face
//  Assumes that the intensity face parent has already been initialized.
void strk_tracking_face_2d::
init_gradient_info(vil1_memory_image_of<float> const& Ix,
                   vil1_memory_image_of<float> const& Iy)
{
  if (!intf_||!Ix||!Iy)
    return;
  int n = intf_->Npix();
  Ix_ = new float[n];
  Iy_ = new float[n];
  bsta_histogram<float> model_gradient_dir_hist(360, gradient_dir_hist_bins_);
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
  model_gradient_dir_entropy_= model_gradient_dir_hist.entropy();
}

//: fill the color values in the face
//  Assumes that the intensity face parent has already been initialized.
void strk_tracking_face_2d::
init_color_info(vil1_memory_image_of<float> const& hue,
                vil1_memory_image_of<float> const& sat)
{
  if (!intf_||!hue||!sat)
    return;
  int n = intf_->Npix();
  hue_ = new float[n];
  sat_ = new float[n];
  bsta_histogram<float> model_color_hist(360, color_hist_bins_);
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
  model_color_entropy_= model_color_hist.entropy();
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

#if 0
// rotate gradients and recompute gradient mutual information
void strk_tracking_face_2d::transform_gradients(double theta)
{
  double deg_rad = 180.0/vnl_math::pi;
  double c = vcl_cos(theta), s = vcl_sin(theta);

  bsta_histogram<float> model_gradient_dir_hist(360, gradient_dir_hist_bins_);
  gradient_dir_hist_bins_ = model_gradient_dir_hist.nbins();

  // step through points in face
  int i=0;
  for (this->reset(); this->next(); ++i)
  {
    float Ix0 = Ix_[i];
    float Iy0 = Iy_[i];
    Ix_[i] = float(Ix0*c - Iy0*s);
    Iy_[i] = float(Ix0*s + Iy0*c);
    float ang = float(deg_rad*vcl_atan2(Iy_[i],Ix_[i]))+180.f;
    float mag = vcl_abs(Ix_[i])+vcl_abs(Iy_[i]);
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
#endif // 0

// rotate gradients
void strk_tracking_face_2d::transform_gradients(double theta)
{
  double c = vcl_cos(theta), s = vcl_sin(theta);

  // step through gradient values in face
  int i=0;
  for (this->reset(); this->next(); ++i)
  {
    float Ix0 = Ix_[i];
    float Iy0 = Iy_[i];
    Ix_[i] = float(Ix0*c - Iy0*s);
    Iy_[i] = float(Ix0*s + Iy0*c);
  }
}

void strk_tracking_face_2d::transform(double tx, double ty, double theta, double scale)
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
  //update global transform
  vnl_matrix_fixed<double,3,3> Mt, Mt_inv, M, Mf;
  Mt[0][0] = 1.0;   Mt[0][1] = 0.0;   Mt[0][2] = -xo;
  Mt[1][0] = 0.0;   Mt[1][1] = 1.0;   Mt[1][2] = -yo;
  Mt[2][0] = 0.0;   Mt[2][1] = 0.0;   Mt[2][2] = 1.0;

  Mt_inv[0][0] = 1.0;   Mt_inv[0][1] = 0.0;   Mt_inv[0][2] = xo+tx;
  Mt_inv[1][0] = 0.0;   Mt_inv[1][1] = 1.0;   Mt_inv[1][2] = yo+ty;
  Mt_inv[2][0] = 0.0;   Mt_inv[2][1] = 0.0;   Mt_inv[2][2] = 1.0;

  double scc = c*scale, scs = s*scale;
  M[0][0] =scc ;   M[0][1] = -scs;   M[0][2] = 0;
  M[1][0] = scs;   M[1][1] = scc;    M[1][2] = 0;
  M[2][0] = 0.0;   M[2][1] = 0.0;    M[2][2] = 1.0;
  trans_ = Mt_inv*M*Mt*trans_;

  if (gradient_info_)
    this->transform_gradients(theta);
}

//:  Note this transform call does not currently transform gradient directions
void strk_tracking_face_2d::transform(vnl_matrix_fixed<double,3,3> const& T)
{
  vcl_vector<vtol_vertex_sptr> verts;
  this->face()->vertices(verts);
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end(); vit++)
  {
    vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
    if (!v)
      continue;
    double x = v->x(), y = v->y();
    vnl_vector_fixed<double, 3> X(x, y, 1.0), Xp;
    Xp = T*X;
    v->set_x(Xp[0]);
    v->set_y(Xp[1]);
  }
  for (this->reset(); this->next();)
  {
    double x = this->X(), y = this->Y();
    vnl_vector_fixed<double, 3> X(x, y, 1.0), Xp;
    Xp = T*X;
    this->set_X(float(Xp[0]));
    this->set_Y(float(Xp[1]));
  }
  trans_ = T*trans_;
}

float strk_tracking_face_2d::
compute_intensity_mutual_information(vil1_memory_image_of<float> const& image)
{
  if (!intf_)
    return 0;
  int width = image.width(), height = image.height();
  bsta_histogram<float> image_hist(255, intensity_hist_bins_);
  bsta_joint_histogram<float> joint_hist(255, intensity_hist_bins_);
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
    image_hist.upcount(Ii, 1.0f);
    joint_hist.upcount(Im, 1.0f, Ii, 1.0f);
#ifdef DEBUG
    //    vcl_cout << '(' << x << ' ' << y << "):[" << Im << ' ' << Ii << ']' << vcl_endl;
#endif
    n++;
  }
  if (n<0.9*npix)
    return 0;
//   vcl_cout << "Itensity Hist\n";
//   image_hist.print();
#ifdef DEBUG
  vcl_cout << "Itensity Hist\n";
  image_hist.print();
  vcl_cout << "Joint Intensity Hist\n";
  joint_hist.print();
#endif
  //apply parzen windows
  image_hist.parzen(parzen_sigma_);
  joint_hist.parzen(parzen_sigma_);

  //compute the mutual information
  intensity_entropy_= image_hist.entropy();
  intensity_joint_entropy_ = renyi_joint_entropy_ ? joint_hist.renyi_entropy() : joint_hist.entropy();

  float mi = float(model_intensity_entropy_ + intensity_entropy_ - intensity_joint_entropy_);

#ifdef DEBUG
  vcl_cout << "Entropies:(M,I,J, MI)=(" << model_intensity_entropy_ << ' '
           << intensity_entropy_ << ' ' << intensity_joint_entropy_ << ' ' << mi <<")\n";
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
  bsta_histogram<float> image_dir_hist(360, gradient_dir_hist_bins_);
  bsta_joint_histogram<float> joint_dir_hist(360, gradient_dir_hist_bins_);

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

  gradient_dir_entropy_ = image_dir_hist.entropy();
  gradient_joint_entropy_ = renyi_joint_entropy_ ? joint_dir_hist.renyi_entropy() : joint_dir_hist.entropy();

  float mi = float(model_gradient_dir_entropy_ + gradient_dir_entropy_ - gradient_joint_entropy_);
#ifdef DEBUG
  vcl_cout << "Dir Entropies:(M,I,J, MI)=(" << model_gradient_dir_entropy_ << ' '
           << gradient_dir_entropy_ << ' ' << gradient_joint_entropy_ << ' ' << mi <<")\n";
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
  bsta_histogram<float> color_hist(360, color_hist_bins_);
  bsta_joint_histogram<float> joint_color_hist(360, color_hist_bins_);

  int npix = intf_->Npix();
  if (!npix)
    return 0;

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

  //compute entropies
  color_entropy_ = color_hist.entropy();
  color_joint_entropy_ = renyi_joint_entropy_? joint_color_hist.renyi_entropy() : joint_color_hist.entropy();

  float mi = float(model_color_entropy_ + color_entropy_ - color_joint_entropy_);

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
bool strk_tracking_face_2d::
compute_only_gradient_mi(vil1_memory_image_of<float> const& Ix,
                         vil1_memory_image_of<float> const& Iy)
{
  if ((!Ix || !Iy) && gradient_info_)
    return false;
  
  if (gradient_info_)
    this->set_grad_mutual_info(this->compute_gradient_mutual_information(Ix,Iy));
  return true;
}
                                

void strk_tracking_face_2d::print_pixels(vil1_memory_image_of<float> const& image)
{
  if (!image)
    return;
  int width = image.width(), height = image.height();
  for (intf_->reset(); intf_->next();)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image(x,y);
    float Im = intf_->I();
    vcl_cout << '(' << x << ' ' << y << "):[" << Im << ' '
             << Ii << ']' << vcl_endl;
  }
}

void strk_tracking_face_2d::
face_points(vcl_vector<vtol_topology_object_sptr>& points)
{
  points.clear();
  for (intf_->reset(); intf_->next();)
  {
    double x = intf_->X(), y = intf_->Y();
    vtol_topology_object* to = new vtol_vertex_2d(x, y);
    points.push_back(to);
  }
}

//: randomly select a set of pixels from the interior
vcl_vector<float> strk_tracking_face_2d::random_intensities(int& n_pix)
{
  vcl_vector<float> rand_pix;
  if (!intf_)
    return rand_pix;
  int Np = this->Npix();
  if (n_pix > Np)
    n_pix = Np;
  //generate a random pixel index on [0, npix-1]
  unsigned short const* pix = intf_->Ij();
  for (int i = 0; i<n_pix; i++)
  {
    float x = (n_pix-1)*float(vcl_rand()/(RAND_MAX+1.0));
    int ni = (int)x;
    if (ni>Np-1)
      ni = Np-1;
    float v = (float)pix[ni];
    rand_pix.push_back(v);
  }
  return rand_pix;
}

//: compute the intensity joint entropy between the pixels in the image
//  under *this face and the "model" pixels of the other face.
//
float strk_tracking_face_2d::
compute_intensity_joint_entropy(strk_tracking_face_2d_sptr const& other,
                                vil1_memory_image_of<float> const& image)
{
  if (!intf_||!other||!image)
    return 0;
  int width = image.width(), height = image.height();
  bsta_joint_histogram<float> joint_hist(255, intensity_hist_bins_);
  int npix = intf_->Npix();
  int mpix = other->face()->Npix();
  if (!npix)
    return 0;
  int n = 0;
  //iterate through the pixels of the target model face
  for (intf_->reset(); intf_->next(); n++)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image(x,y);//the pixels under the target
    if (n<mpix)
      joint_hist.upcount((other->face()->Ij())[n], 1.0f, Ii, 1.0f);
  }
  if (n<0.9*npix)
    return 0;
#ifdef DEBUG
  vcl_cout << "Joint Observation-Background  Hist\n";
  joint_hist.print();
#endif
  //apply parzen windows
  joint_hist.parzen(parzen_sigma_);

  //compute the joint entropy
  return renyi_joint_entropy_ ? joint_hist.renyi_entropy() : joint_hist.entropy();
}

//: compute the intensity joint entropy between the model pixels of this face and the model pixels of the "other" face.
//  the pixels from the "other" face are randomly selected and equal
//  in number to the pixels inside *this face.  It is assumed that
//  there is no spatial intersection of the two pixel sets.
//
float strk_tracking_face_2d::
compute_model_intensity_joint_entropy(strk_tracking_face_2d_sptr const& other)
{
  if (!intf_||!other)
    return 0;

  bsta_joint_histogram<float> joint_hist(255, intensity_hist_bins_);
  int npix = intf_->Npix();
  if (!npix)
    return 0;
  int n = 0;
  vcl_vector<float> rand_int = other->random_intensities(npix);
  //iterate through the pixels of the target model face
  for (intf_->reset(); intf_->next(); n++)
  {
    if (n<npix)
      joint_hist.upcount(rand_int[n], 1.0f, intf_->I(), 1.0f);
  }
  if (n<0.9*npix)
    return 0;
#ifdef DEBUG
  vcl_cout << "Joint Model-Background  Hist\n";
  joint_hist.print();
#endif
  //apply parzen windows
  joint_hist.parzen(parzen_sigma_);

  //compute the joint entropy
  return renyi_joint_entropy_ ? joint_hist.renyi_entropy() : joint_hist.entropy();
}

//: randomly select a set of pixels from the interior
bool strk_tracking_face_2d::random_colors(int& n_pix,
                                          vcl_vector<float>& hue,
                                          vcl_vector<float>& sat)
{
  hue.clear();
  sat.clear();
  if (!intf_||!hue_||!sat_)
    return false;
  int Np = this->Npix();
  if (!Np)
    return false;
  if (n_pix > Np)
    n_pix = Np;
  //generate a random pixel index on [0, npix-1]
  for (int i = 0; i<n_pix; i++)
  {
    float x = (n_pix-1)*float(vcl_rand()/(RAND_MAX+1.0));
    int ni = (int)x;
    if (ni>Np-1)
      ni = Np-1;
    float hv = (float)hue_[ni];
    float sv = (float)sat_[ni];
    hue.push_back(hv);
    sat.push_back(sv);
  }
  return true;
}

float strk_tracking_face_2d::
compute_color_joint_entropy(strk_tracking_face_2d_sptr const& other,
                            vil1_memory_image_of<float> const& hue,
                            vil1_memory_image_of<float> const& sat)
{
  if (!intf_||!other||!hue||!sat||!hue_||!sat_)
    return 0;
  int npix = intf_->Npix();
  int mpix = other->face()->Npix();
  if (!npix)
    return 0;
  bsta_joint_histogram<float> joint_color_hist;
  int width = hue.width(), height = hue.height();
  unsigned int i = 0;
  for (intf_->reset(); intf_->next(); i++)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float hue_i = hue(x,y), sat_i = sat(x,y);
    //    float hue_other = hvals[i], sat_other = svals[i];
    if(i<mpix)
	{
	float hue_other = other->hue(i), sat_other = other->sat(i);
    if (sat_other>0&&sat_i>0)
		joint_color_hist.upcount(hue_other, sat_other, hue_i, sat_i);}
  }
#ifdef DEBUG
  vcl_cout << "Joint Color Background  Hist\n";
  joint_color_hist.print();
#endif
  //apply parzen windows
  joint_color_hist.parzen(parzen_sigma_);

  return renyi_joint_entropy_ ? joint_color_hist.renyi_entropy() : joint_color_hist.entropy();
}

//: Difference in mutual information due to intensity
//
//     MI(this:obs, this:model)-MI(this:obs, other:model)
//
//  the stored model intensity information from the other face
//  is compared with the observation intensites of *this face.  It is
//  assumed that there is no intersection of the two pixel sets.
//
//  The other face has been constructed on the current image
//  and so its model data reflects the current intensity statistics outside
//  *this observation region
//
float strk_tracking_face_2d::
intensity_mutual_info_diff(strk_tracking_face_2d_sptr const& other,
                           vil1_memory_image_of<float> const& image,
                           bool verbose)
{
  if (!other||!image)
    return 0;
  //sets up the mutual information between the fa model and the current
  //image observation
  this->compute_intensity_mutual_information(image);

  float Hb =  other->model_intensity_entropy();
  float Hm =  this->model_intensity_entropy();
  float Ho =  this->intensity_entropy();
  float Hob = this->compute_intensity_joint_entropy(other, image);
  float Hmb = this->compute_model_intensity_joint_entropy(other);
  float Hom = intensity_joint_entropy_;
  float mi_diff = Hmb-Hom;
  if (verbose)
  {
    vcl_cout << "background entropy = " << Hb
             << "\nmodel entropy = " << Hm
             << "\nentropy = " << Ho
             << "\nobs-background joint entropy = " << Hob
             << "\nmodel-background joint entropy = " << Hmb
             << "\nobs-model joint entropy = " << Hom
             << "\nmutual info diff = " << mi_diff
             << vcl_endl << vcl_endl;
  }
  intensity_info_diff_ = mi_diff;
  return mi_diff;
}

//: Difference in mutual information due to color
//
//     MI(this:obs, this:model)-MI(this:obs, other:model)
//
//  the stored model color information from the other face
//  is compared with the observation colors of *this face.  It is
//  assumed that there is no intersection of the two pixel sets.
//
//  The other face has been constructed on the current image
//  and so its model data reflects the current intensity statistics outside
//  *this observation region
//
float strk_tracking_face_2d::
color_mutual_info_diff(strk_tracking_face_2d_sptr const& other,
                       vil1_memory_image_of<float> const& hue,
                       vil1_memory_image_of<float> const& sat,
                       bool verbose)
{
  if (!other||!hue||!sat)
    return 0;
  //sets up the mutual information between the fa model and the current
  //image observation
  this->compute_color_mutual_information(hue, sat);

  float Hb =  other->model_color_entropy();
  float Hm =  this->model_color_entropy();
  float Hob = this->compute_color_joint_entropy(other, hue, sat);
  float Hom = color_joint_entropy_;
  float mi_diff = Hm - Hb - Hom + Hob;
  if (verbose)
  {
    vcl_cout << "background color entropy = " << Hb
             << "\nmodel color entropy = " << Hm
             << "\nobs-background color joint entropy = " << Hob
             << "\nobs-model color joint entropy = " << Hom
             << "\ncolor mutual info diff = " << mi_diff
             << vcl_endl << vcl_endl;
  }
  color_info_diff_ = mi_diff;
  return mi_diff;
}
float strk_tracking_face_2d::
intensity_mutual_info_diff(vcl_vector<strk_tracking_face_2d_sptr> const& others,
                                   vil1_memory_image_of<float> const& image,
                                   bool verbose)
{
  if (!others.size()||!image)
    return 0;
  //sets up the mutual information between the fa model and the current
  //image observation
  this->compute_intensity_mutual_information(image);
  float Hm =  this->model_intensity_entropy();
  float Ho =  this->intensity_entropy();
  float Hom = intensity_joint_entropy_;
  int n_others = 0;
  float mi_diff_avg = 0, Hob_avg = 0, Hb_avg = 0;
  for(vcl_vector<strk_tracking_face_2d_sptr>::const_iterator fit = others.begin(); fit != others.end(); fit++, n_others++)
  {
    float Hb = (*fit)->model_intensity_entropy();
    float Hob = this->compute_intensity_joint_entropy(*fit, image);
    float mi_diff = Hm - Hb - Hom + Hob;
    Hb_avg += Hb;
    Hob_avg += Hob;
    mi_diff_avg += mi_diff;
  }
  if(n_others)
    {
      Hb_avg/=n_others;
      Hob_avg/=n_others;
      mi_diff_avg/=n_others;
    }
  //  if (verbose)
  if (true)
  {
    vcl_cout << "background entropy = " << Hb_avg
             << "\nmodel entropy = " << Hm
             << "\nobs entropy = " << Ho
             << "\nobs-background joint entropy = " << Hob_avg
             << "\nobs-model joint entropy = " << Hom
             << "\nmutual info diff = " << mi_diff_avg
             << vcl_endl << vcl_endl;
  }
  intensity_info_diff_ = mi_diff_avg;
  return mi_diff_avg;
}
float strk_tracking_face_2d::
color_mutual_info_diff(vcl_vector<strk_tracking_face_2d_sptr> const& others,
                       vil1_memory_image_of<float> const& hue,
                       vil1_memory_image_of<float> const& sat,
                       bool verbose)
{
  if (!others.size()||!hue||!sat)
    return 0;
  //sets up the mutual information between the fa model and the current
  //image observation
  this->compute_color_mutual_information(hue, sat);

  float Hm =  this->model_color_entropy();
  float Hom = color_joint_entropy_;
  int n_others = 0;
  float mi_diff_avg = 0, Hob_avg = 0, Hb_avg = 0;
  for(vcl_vector<strk_tracking_face_2d_sptr>::const_iterator fit = others.begin(); fit != others.end(); fit++, n_others++)
  {
    float Hb = (*fit)->model_color_entropy();
    float Hob = this->compute_color_joint_entropy(*fit, hue, sat);
    float mi_diff = Hm - Hb - Hom + Hob;
    Hb_avg += Hb;
    Hob_avg += Hob;
    mi_diff_avg += mi_diff;
  }
  if(n_others)
    {
      Hb_avg/=n_others;
      Hob_avg/=n_others;
      mi_diff_avg/=n_others;
    }
  if (verbose)
  {
    vcl_cout << "background color entropy = " << Hb_avg
             << "\nmodel color entropy = " << Hm
             << "\nobs-background color joint entropy = " << Hob_avg
             << "\nobs-model color joint entropy = " << Hom
             << "\ncolor mutual info diff = " << mi_diff_avg
             << vcl_endl << vcl_endl;
  }
  color_info_diff_ = mi_diff_avg;
  return mi_diff_avg;
}
float strk_tracking_face_2d::total_info_diff()
{
  float temp = intensity_info_diff_+color_info_diff_;
  if(gradient_info_)
    return temp+gradient_dir_mi_;
  return temp;
}
void strk_tracking_face_2d::
print_intensity_histograms(vil1_memory_image_of<float> const& image)
{
  if (!intf_||!image)
    return;
  int width = image.width(), height = image.height();
  bsta_histogram<float> model_image_hist(255, intensity_hist_bins_);
  bsta_histogram<float> obs_image_hist(255, intensity_hist_bins_);
  int npix = intf_->Npix();
  if (!npix)
    return;
  for (intf_->reset(); intf_->next();)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image(x,y);
    float Im = intf_->I();
    model_image_hist.upcount(Im, 1.0f);
    obs_image_hist.upcount(Ii, 1.0f);
  }
  vcl_cout << "model intensity histogram\n";
  model_image_hist.print();
  vcl_cout << "obs intensity histogram\n";
  obs_image_hist.print();
  vcl_cout << vcl_endl;
}

void strk_tracking_face_2d::
print_gradient_histograms(vil1_memory_image_of<float> const& Ix,
                          vil1_memory_image_of<float> const& Iy)
{
  if (!intf_||!Ix||!Iy)
    return;
  int width = Ix.width(), height = Iy.height();
  bsta_histogram<float> model_image_dir_hist(360, gradient_dir_hist_bins_);
  bsta_histogram<float> obs_image_dir_hist(360, gradient_dir_hist_bins_);
  int npix = intf_->Npix();
  if (!npix)
    return;
  double deg_rad = 180.0/vnl_math::pi;
  int i = 0;
  for (intf_->reset(); intf_->next(); ++i)
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
    if (mag0>min_gradient_&&magi>min_gradient_)
    {
      model_image_dir_hist.upcount(ang0, mag0);
      obs_image_dir_hist.upcount(angi, magi);
    }
  }
  vcl_cout << "model gradient dir histogram\n";
  model_image_dir_hist.print();
  vcl_cout << "obs gradient dir histogram\n";
  obs_image_dir_hist.print();
  vcl_cout << '\n';
}

void strk_tracking_face_2d::
print_color_histograms(vil1_memory_image_of<float> const& hue,
                       vil1_memory_image_of<float> const& sat)
{
  if (!intf_||!hue||!sat)
    return;
  int width = hue.width(), height = hue.height();
  bsta_histogram<float> model_color_hist(360, color_hist_bins_);
  bsta_histogram<float> obs_color_hist(360, color_hist_bins_);

  int npix = intf_->Npix();
  if (!npix)
    return;
  int i = 0;
  for (intf_->reset(); intf_->next(); ++i)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float hue0 = this->hue(i), sat0 = this->sat(i);
    float hue_i = hue(x,y), sat_i = sat(x,y);
    if (sat0>0)
      model_color_hist.upcount(hue0, sat0);
    if (sat_i>0)
      obs_color_hist.upcount(hue_i, sat_i);
  }
  vcl_cout << "model color histogram\n";
  model_color_hist.print();
  vcl_cout << "obs color histogram\n";
  obs_color_hist.print();
  vcl_cout << '\n';
}

bsta_histogram<float> strk_tracking_face_2d::  
intensity_histogram(vil1_memory_image_of<float> const& image)
{
  bsta_histogram<float> image_hist(255, intensity_hist_bins_);
	if (!intf_||!image)
    return image_hist;
  int width = image.width(), height = image.height();
  int npix = intf_->Npix();
  if (!npix)
    return image_hist;
  for (intf_->reset(); intf_->next();)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ii = image(x,y);
    image_hist.upcount(Ii, 1.0f);
  }
  return image_hist;
}

bsta_histogram<float> strk_tracking_face_2d::
gradient_histogram(vil1_memory_image_of<float> const& Ix,
                   vil1_memory_image_of<float> const& Iy)
{
  bsta_histogram<float> grad_dir_hist(360, gradient_dir_hist_bins_);
  if (!intf_||!Ix||!Iy)
    return grad_dir_hist;
  int width = Ix.width(), height = Iy.height();
  int npix = intf_->Npix();
  if (!npix)
    return grad_dir_hist;
  double deg_rad = 180.0/vnl_math::pi;
  int i = 0;
  for (intf_->reset(); intf_->next(); ++i)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ixi = Ix(x,y), Iyi = Iy(x,y);
    float angi = float(deg_rad*vcl_atan2(Iyi, Ixi))+180.f;
    float magi = vcl_abs(Ixi)+vcl_abs(Iyi); // was: vcl_sqrt(Ixi*Ixi + Iyi*Iyi);
    if (magi>min_gradient_)
      grad_dir_hist.upcount(angi, magi);
  }
  return grad_dir_hist;
}
bsta_histogram<float> strk_tracking_face_2d::
color_histogram(vil1_memory_image_of<float> const& hue,
                   vil1_memory_image_of<float> const& sat)
{
  bsta_histogram<float> color_hist(360, color_hist_bins_);
	if(!intf_||!hue||!sat)
    return color_hist;
  int width = hue.width(), height = hue.height();
  int npix = intf_->Npix();
  if (!npix)
    return color_hist;
  int i = 0;
  for (intf_->reset(); intf_->next(); ++i)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float hue_i = hue(x,y), sat_i = sat(x,y);
    //    vcl_cout << "HS(" << hue_i << " " << sat_i << ")\n";
    if (sat_i>0)
      color_hist.upcount(hue_i, sat_i);
  }
  return color_hist;
}
