// This is brl/bseg/sdet/sdet_tracker.cxx
#include "sdet_tracker.h"
//:
// \file
#include <vcl_cmath.h>   // for vcl_fabs(double)
#include <vcl_algorithm.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vil1/vil1_memory_image_of.h>
#include <vtol/vtol_intensity_face.h>
#include <vtol/vtol_vertex_2d.h>


//Gives a sort on correlation score (currently increasing values JLM)
static bool corr_compare(sdet_correlated_face* const& f1,
                         sdet_correlated_face* const& f2)
{
  return f1->correlation() < f2->correlation();//JLM Switched
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
}

//:Default Destructor
sdet_tracker::~sdet_tracker()
{
  for (vcl_vector<sdet_correlated_face*>::iterator
       cit = hypothesized_samples_.begin();
       cit != hypothesized_samples_.end(); cit++)
    delete *cit;
  hypothesized_samples_.clear();
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
  image_0_ = image;
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
  image_i_ = image;
}

//--------------------------------------------------------------------------
//: Set the initial model position
void sdet_tracker::set_initial_model(vtol_face_2d_sptr const& face)
{
  initial_model_ = face;
}

//--------------------------------------------------------------------------
//: fill the pixels in the input face from the input image
void sdet_tracker::fill_face(vtol_intensity_face_sptr const& face,
                             vil1_image image)
{
  if (!face)
    return;
  if (!image)
    return;
  vil1_memory_image_of<unsigned char> img(image);
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
      face->IncrementMeans(float(x), float(y), img(x, y));
    }
  face->InitPixelArrays();
  //Got through the pixels again to actually set the face arrays X(), Y() etc
  for (psi.reset(); psi.next();)
    for (int x = psi.startx(); x<=psi.endx(); x++)
    {
      int y = psi.scany();
      if (x<0||x>=width||y<0||y>=height)
        continue;
      face->InsertInPixelArrays(float(x), float(y), img(x, y));
    }
  vcl_cout << "Model Io " << face->Io() << "/n" << vcl_flush;
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
//   vcl_cout << "Initial Face Centroid (" << intf->Xo()<< ' '
//            <<  intf->Yo() << ")\n";

  current_samples_.push_back(intf);
}

//--------------------------------------------------------------------------
//: Construct a new face which is a translated version of the input face
vtol_intensity_face_sptr
sdet_tracker::transform_face(vtol_intensity_face_sptr const& face,
                             double tx, double ty)
{
  if (!face)
    return 0;
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
    X[i]=Xj[i]+float(tx);
    Y[i]=Yj[i]+float(ty);
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
    vtol_vertex_sptr new_v = new vtol_vertex_2d(v->x()+tx, v->y()+ty);
    new_verts.push_back(new_v);
  }
  vtol_face_2d_sptr f2d = new vtol_face_2d(new_verts);
  vtol_intensity_face_sptr new_int_face =
    new vtol_intensity_face(f2d, npix, X, Y, I);
//   vcl_cout << "Transformed Face Centroid (" << new_int_face->Xo()<< ' '
//            <<  new_int_face->Yo() << ")\n";

  delete [] X;
  delete [] Y;
  delete [] I;
  return new_int_face;
}

//--------------------------------------------------------------------------
//: perform a normalized cross-correlation between the face and image
void sdet_tracker::correlate_face(vtol_intensity_face_sptr const& face,
                                  vil1_image image, double& corr, double& avg_int)
{
  vil1_memory_image_of<unsigned char> img(image);
  int width = img.width(), height = img.height();
  if (!face)
    return ;
  if (!image)
    return ;
#if 0
  double face_avg = 0, face_sum=0, image_sum=0;
#endif
  double image_avg = 0, cross_sum=0;
  int n = face->Npix();
  for (face->reset(); face->next();)
  {
    int x = int(face->X()), y = int(face->Y());
    if (x>=width||y>=height)
      continue;
    double face_int = face->I();
    double image_int = img(x,y);
#if 0
    face_avg += face_int;
#endif
    image_avg += image_int;
    cross_sum += vcl_fabs(face_int-image_int);
  }
  image_avg /= n;
  cross_sum /= n;
  avg_int = image_avg;
#if 0
  face_avg /= n;
  vcl_cout << "Face Avg " << face_avg << "  Img Avg " << image_avg << '\n';
  for (face->reset(); face->next();)
  {
    int x = face->X(), y = face->Y();
    if (x>=width||y>=height)
      continue;
    double face_int = face->I()-face_avg;
    double image_int = img(x,y)-image_avg;
    face_sum += face_int*face_int;
    image_sum += image_int*image_int;
    //Switched to diff
    cross_sum += (face->I()-img(x,y))*(face->I()-img(x,y));
  }
  vcl_cout << "Face avg " << face_avg << " Image Avg " << image_avg << '\n';

  if (face_sum<=0 || image_sum<=0) return;
#endif // 0
  if (avg_int!=0)
    corr = cross_sum;
  else
    corr = 1.0e6;
#if 0
  vcl_cout << "Face Avg " << face_avg
           << "  Img Avg " << image_avg
           << "  Corr " << corr << '\n';
#endif
}

//--------------------------------------------------------------------------
//: generate a randomly positioned face within a given radius
vtol_intensity_face_sptr
sdet_tracker::generate_sample(vtol_intensity_face_sptr const& seed)
{
  float x = (2.f*search_radius_)*(rand()/(RAND_MAX+1.f)) - search_radius_;
  float y = (2.f*search_radius_)*(rand()/(RAND_MAX+1.f)) - search_radius_;
  return this->transform_face(seed, x, y);
}

//--------------------------------------------------------------------------
//: generate a random set of new faces from the existing samples
void sdet_tracker::generate_samples()
{
  for (vcl_vector<sdet_correlated_face*>::iterator
       cit = hypothesized_samples_.begin();
       cit != hypothesized_samples_.end(); cit++)
    delete *cit;
  hypothesized_samples_.clear();
  for (vcl_vector<vtol_intensity_face_sptr>::iterator fit =
       current_samples_.begin(); fit != current_samples_.end(); fit++)
    for (int i = 0; i<n_samples_; i++)
    {
      vtol_intensity_face_sptr f = this->generate_sample(*fit);
      double c=0, avg_int=0;
      this->correlate_face(f, image_i_, c, avg_int);
      sdet_correlated_face* cf = new sdet_correlated_face();
      cf->set_face(f);
      cf->set_correlation(c);
      cf->set_avg_intensity(avg_int);

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
  current_samples_.clear();
  for (int i =0; i<n_samples_; i++)
  {
    vcl_cout << "Corr = " << hypothesized_samples_[i]->correlation()
             << " Avg Int = "<< hypothesized_samples_[i]->avg_intensity()
             << '\n';
    current_samples_.push_back(hypothesized_samples_[i]->face());
    vcl_cout << vcl_flush;
  }
}

//--------------------------------------------------------------------------
//: because of sorting, the best sample will be the first current sample
vtol_face_2d_sptr sdet_tracker::get_best_sample()
{
  if (!current_samples_.size())
    return 0;
  return current_samples_[0]->cast_to_face_2d();
}

//--------------------------------------------------------------------------
//: because of sorting the samples will be in descending order of correlation
void sdet_tracker::get_samples(vcl_vector<vtol_face_2d_sptr>& samples)
{
  samples.clear();
  for (vcl_vector<vtol_intensity_face_sptr>::iterator
       fit = current_samples_.begin(); fit != current_samples_.end(); fit++)
    samples.push_back((*fit)->cast_to_face_2d());
}

//--------------------------------------------------------------------------
//: Main tracking method
void sdet_tracker::track()
{
  this->generate_samples();
  this->cull_samples();
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
