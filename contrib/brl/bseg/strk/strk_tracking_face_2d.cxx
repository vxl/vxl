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
#include <vcl_cmath.h> // for log()
//
//======================== HISTOGRAM IMPLEMENTATION =================
//

strk_histf::strk_histf(double range, int nbins)
{
  if (!nbins||nbins<0||range<=0)
  {
    nbins_=0;
    range_ = 0;
    delta_=0;
    area_valid_ = false;
    area_=0;
    return;
  }
  nbins_ = nbins;
  range_ = range;
  delta_ = range/nbins;
  counts_.resize(nbins, 0.0);
  area_valid_ = false;
  area_ = 0;
}

void strk_histf::upcount(const double x)
{
  if (x<0||x>range_)
    return;
  for (int i = 0; i<nbins_;i++)
    if ((i+1)*delta_>=x)
    {
      counts_[i] += 1.0;
      break;
    }
  area_valid_ = false;
}

void strk_histf::compute_area()
{
  area_ =0;
  for (int i = 0; i<nbins_; i++)
    area_ += counts_[i];
  area_valid_ = true;
}

double strk_histf::p(const int bin)
{
  if (bin<0||bin>=nbins_)
    return 0;
  if (!area_valid_)
    compute_area();
  if (!area_)
    return 0;
  return counts_[bin]/area_;
}

void strk_histf::print()
{
  for (int i=0; i<nbins_; i++)
    if (p(i))
      vcl_cout << "p[" << i << "]=" << p(i) << "\n";
}

strk_joint_histf::strk_joint_histf(int nbins)
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

void strk_joint_histf::upcount(const double a, const double b)
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

void strk_joint_histf::compute_volume()
{
  volume_=0;
  for (int a = 0; a<nbins_; a++)
    for (int b =0; b<nbins_; b++)
      volume_ += counts_[a][b];
  volume_valid_ = true;
}

double strk_joint_histf::p(const int a, const int b)
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

void strk_joint_histf::print()
{
  for (int a = 0; a<nbins_; a++)
    for (int b = 0; b<nbins_; b++)
      if (p(a,b))
        vcl_cout << "p[" << a << "][" << b << "]=" << p(a,b) << "\n";
}

strk_double_histf::strk_double_histf(int nbins)
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

void strk_double_histf::upcount(const double x, const double mag)
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

void strk_double_histf::compute_area()
{
  area_ =0;
  for (int i = 0; i<nbins_; i++)
    area_ += counts_[i];
  area_valid_ = true;
}

double strk_double_histf::p(const int bin)
{
  if (bin<0||bin>=nbins_)
    return 0;
  if (!area_valid_)
    compute_area();
  if (!area_)
    return 0;
  return counts_[bin]/area_;
}

void strk_double_histf::print()
{
  for (int i=0; i<nbins_; i++)
    if (p(i))
      vcl_cout << "p[" << i << "]=" << p(i) << "\n";
}

strk_double_joint_histf::strk_double_joint_histf(int nbins)
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

void strk_double_joint_histf::upcount(const double a, const double mag_a,
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

void strk_double_joint_histf::compute_volume()
{
  volume_=0;
  for (int a = 0; a<nbins_; a++)
    for (int b =0; b<nbins_; b++)
      volume_ += counts_[a][b];
  volume_valid_ = true;
}

double strk_double_joint_histf::p(const int a, const int b)
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

void strk_double_joint_histf::print()
{
  for (int a = 0; a<nbins_; a++)
    for (int b = 0; b<nbins_; b++)
      if (p(a,b))
        vcl_cout << "p[" << a << "][" << b << "]=" << p(a,b) << "\n";
}

//
//======================== TRACKING FACE IMPLEMENTATION =============
//
void strk_tracking_face_2d::centroid(double& x, double& y)
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
                      vil1_memory_image_of<float> const& image)
{
  intf_ = 0;
  Ix_ = 0;
  Iy_ = 0;
  gradient_info_ = false;
  intensity_mi_=0;
  gradient_dir_mi_=0;
  vil1_memory_image_of<float> t1, t2;
  this->init(face, image, t1, t2);
}

strk_tracking_face_2d::
strk_tracking_face_2d(vtol_face_2d_sptr const& face,
                      vil1_memory_image_of<float> const& image,
                      vil1_memory_image_of<float> const& Ix,
                      vil1_memory_image_of<float> const& Iy)
{
  intf_ = 0;
  Ix_ = 0;
  Iy_ = 0;
  gradient_info_ = true;
  intensity_mi_=0;
  gradient_dir_mi_=0;
  this->init(face, image, Ix, Iy);
}

strk_tracking_face_2d::strk_tracking_face_2d (vtol_intensity_face_sptr const& intf)
{
  if (!intf)
    return;
  intf_ = intf;
  Ix_ = 0;
  Iy_ = 0;
  gradient_info_ = false;
  intensity_mi_=0;
  gradient_dir_mi_=0;
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
  intensity_mi_ = tf->int_mutual_info();
  gradient_dir_mi_ = tf->grad_mutual_info();
  model_intensity_entropy_=tf->model_intensity_entropy_;
  model_gradient_dir_entropy_=tf->model_gradient_dir_entropy_;
}

strk_tracking_face_2d::~strk_tracking_face_2d()
{
  delete [] Ix_;
  delete [] Iy_;
}

void
strk_tracking_face_2d::init_face_info(vil1_memory_image_of<float> const& image,
                                      vil1_memory_image_of<float> const& Ix,
                                      vil1_memory_image_of<float> const& Iy)
{
  if (!intf_||!image)
    return;
  bool use_grad = (Ix && Iy);//this is ugly - fix me !!
  int width = image.width(), height = image.height();
  intf_->ResetPixelData();
  vgl_polygon p;
  p.new_sheet();
  vcl_vector<vtol_vertex_sptr> verts;
  intf_->vertices(verts);
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
      intf_->IncrementMeans(x, y, v);
    }
  intf_->InitPixelArrays();

  strk_histf model_intensity_hist;
  intensity_hist_bins_ = model_intensity_hist.nbins();

  strk_double_histf model_gradient_dir_hist;
  gradient_dir_hist_bins_ = model_gradient_dir_hist.nbins();


  double deg_rad = 180.0/vnl_math::pi;
  //Got through the pixels again to actually set the face arrays X(), Y() etc
  for (psi.reset(); psi.next();)
    for (int x = psi.startx(); x<=psi.endx(); x++)
    {
      int y = psi.scany();
      if (x<0||x>=width||y<0||y>=height)
        continue;
      unsigned short v = (unsigned short)image(x, y);
      model_intensity_hist.upcount(v);
      intf_->InsertInPixelArrays(x, y, v);
      if (use_grad)
      {
        float Ixi = Ix(x,y), Iyi = Iy(x,y);
        double ang = (deg_rad*vcl_atan2(Iyi, Ixi))+180.0;
        double mag = vcl_fabs(Ixi)+vcl_fabs(Iyi);
        model_gradient_dir_hist.upcount(ang, mag);
      }
    }
  //compute the model entropy
  double ent = 0;
  for (int m = 0; m<intensity_hist_bins_; m++)
  {
    double pm = model_intensity_hist.p(m);
    if (!pm)
      continue;
    ent -= pm*vcl_log(pm);
  }
  model_intensity_entropy_ = ent/vcl_log(2.0);

  //compute the gradient direction entropy
  ent = 0;
  if (use_grad)
    for (int m = 0; m<gradient_dir_hist_bins_; m++)
    {
      double pm = model_gradient_dir_hist.p(m);
      if (!pm)
        continue;
      ent -= pm*vcl_log(pm);
    }
  model_gradient_dir_entropy_= ent/vcl_log(2.0);
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

void strk_tracking_face_2d::
init(vtol_face_2d_sptr const& face, vil1_memory_image_of<float> const& image,
     vil1_memory_image_of<float> const& Ix,
     vil1_memory_image_of<float> const& Iy)
{
  if (!face)
    return;
  intf_ = new vtol_intensity_face(face);

  if (!image)
    return;
  this->init_face_info(image, Ix, Iy);//also sets model entropies
  if (!gradient_info_)
    return;
  int n = intf_->Npix();
  Ix_ = new float[n];
  Iy_ = new float[n];
  this->set_gradient(Ix, Iy);
}

void strk_tracking_face_2d::set_int_mutual_info(const float mi)
{
  intensity_mi_ = mi;
  total_info_= mi + gradient_dir_mi_;
}

void strk_tracking_face_2d::set_grad_mutual_info(const float mi)
{
  gradient_dir_mi_ = mi;
  total_info_= mi + intensity_mi_;
}

void strk_tracking_face_2d::transform(const double tx, const double ty,
                                      const double theta, const double scale)
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
    this->set_X(xp*c - yp*s + xo + tx);
    this->set_Y(xp*s + yp*c + yo + ty);
  }
}

double strk_tracking_face_2d::
compute_intensity_mutual_information(vil1_memory_image_of<float> const& image)
{
  if (!intf_)
    return 0;

  int width = image.width(), height = image.height();
  double mi = 0;
  strk_histf image_hist;
  strk_joint_histf joint_hist;
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
    image_hist.upcount(Ii);
    joint_hist.upcount(Im, Ii);
    n++;
  }
  float npixf = (float)npix, nf = (float)n;
  float frac = nf/npixf;
  if (frac<0.9)
    return 0;
  // vcl_cout << "Model Hist\n";
  //    model_intensity_hist_->print();
  //    vcl_cout << "Image Hist\n";
  //    image_hist.print();
  //    vcl_cout << "Joint Hist\n";
  //    joint_hist.print();

  //compute the mutual information
  int nbins = image_hist.nbins();
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
  mi = model_intensity_entropy_ + enti - jent;
  //   vcl_cout << "Entropies:(M,I,J, MI)=(" << model_intensity_entropy_ << " "
  //            << enti << " " << jent << " " << mi <<")\n";
  return mi;
}

double strk_tracking_face_2d::
compute_gradient_mutual_information(vil1_memory_image_of<float> const& Ix,
                                    vil1_memory_image_of<float> const& Iy)
{
  if (!intf_||!Ix||!Iy)
    return 0;
  int width = Ix.width(), height = Iy.height();
  double mi = 0;
  strk_double_histf image_dir_hist;
  strk_double_joint_histf joint_dir_hist;

  int npix = intf_->Npix();
  if (!npix)
    return 0;
  double deg_rad = 180.0/vnl_math::pi;
  int i = 0, n = 0;
  for (intf_->reset(); intf_->next(); i++)
  {
    int x = int(intf_->X()), y = int(intf_->Y());
    if (x<0||x>=width||y<0||y>=height)
      continue;
    float Ix0 = this->Ix(i), Iy0 =  this->Iy(i);
    double ang0 = (deg_rad*vcl_atan2(Iy0, Ix0))+180.0;
    //    double mag0 = vcl_sqrt(Ix0*Ix0 + Iy0*Iy0);
    double mag0 = vcl_fabs(Ix0)+vcl_fabs(Iy0);
    //    vcl_cout << "ang0, mag0 " << ang0 << " " << mag0 << "\n";
    float Ixi = Ix(x,y), Iyi = Iy(x,y);
    //    vcl_cout << "Ixi, Iyi " << Ixi << " " << Iyi << "\n";
    double angi = (deg_rad*vcl_atan2(Iyi, Ixi))+180.0;
    //    double magi = vcl_sqrt(Ixi*Ixi + Iyi*Iyi);
    double magi = vcl_fabs(Ixi)+vcl_fabs(Iyi);
    //    vcl_cout << "angi, magi " << angi << " " << magi << "\n";
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
#endif

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
  //   vcl_cout << "Dir Entropies:(M,I,J, MI)=(" << model_intensity_entropy_ << " "
  //            << enti << " " << jent << " " << mi <<")\n";
  return mi;
}

bool strk_tracking_face_2d::
compute_mutual_information(vil1_memory_image_of<float> const& image,
                           vil1_memory_image_of<float> const& Ix,
                           vil1_memory_image_of<float> const& Iy)
{
  if (!image)
    return false;

  if (!Ix && this->gradient_needed())
    return false;

  if (!Iy && this->gradient_needed())
    return false;

  this->set_int_mutual_info(this->compute_intensity_mutual_information(image));
  if (gradient_info_)
    this->set_grad_mutual_info(this->
                               compute_gradient_mutual_information(Ix, Iy));
  return true;
}
