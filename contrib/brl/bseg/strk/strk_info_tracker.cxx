// This is brl/bseg/strk/strk_info_tracker.cxx
#include "strk_info_tracker.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h> // for rand()
#include <vul/vul_timer.h>
#include <vil1/vil1_memory_image_of.h>
#include <vgl/vgl_polygon.h>
#include <vsol/vsol_box_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_vertex_2d.h>
#include <btol/btol_face_algs.h>
#include <brip/brip_vil1_float_ops.h>
#include <bsta/bsta_histogram.h>
#include <strk/strk_tracking_face_2d.h>

//Gives a sort on mutual information decreasing order
static bool info_compare(strk_tracking_face_2d_sptr const f1,
                         strk_tracking_face_2d_sptr const f2)
{
  return f1->total_info() > f2->total_info();//JLM Switched
}

//Gives a sort on mutual information decreasing order
static bool info_diff_compare(strk_tracking_face_2d_sptr const f1,
                              strk_tracking_face_2d_sptr const f2)
{
  double d1 = f1->total_info_diff(), d2 = f2->total_info_diff();
  return d1 > d2;//JLM Switched
}


//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
strk_info_tracker::strk_info_tracker(strk_info_tracker_params& tp)
  : strk_info_tracker_params(tp)
{
//  background_model_ = (strk_tracking_face_2d*)0;
}

//:Default Destructor
strk_info_tracker::~strk_info_tracker()
{
}

//-------------------------------------------------------------------------
//: Set the previous frame image
//
void strk_info_tracker::set_image_0(vil1_image& image)
{
  if (!image)
  {
    vcl_cout <<"In strk_info_tracker::set_image_i(.) - null input\n";
    return;
  }

  vil1_memory_image_of<float> in, hue, sat;
  int w = image.width(), h = image.height();

  if (!color_info_||image.components()==1)
  {
    vil1_memory_image_of<float> in=brip_vil1_float_ops::convert_to_float(image);
    image_0_= brip_vil1_float_ops::gaussian(in, sigma_);
  }

  if (color_info_&&image.components()==3)
  {
    vil1_memory_image_of<vil1_rgb<unsigned char> > temp(image);
    brip_vil1_float_ops::convert_to_IHS(temp, in, hue, sat);
    image_0_= brip_vil1_float_ops::gaussian(in, sigma_);
    hue_0_ = brip_vil1_float_ops::gaussian(hue, sigma_);
    sat_0_ = brip_vil1_float_ops::gaussian(sat, sigma_);
  }

  if (gradient_info_)
  {
    Ix_0_.resize(w,h);
    Iy_0_.resize(w,h);
    brip_vil1_float_ops::gradient_3x3(image_0_, Ix_0_, Iy_0_);
  }
}

//-------------------------------------------------------------------------
//: Set the next frame image
//
void strk_info_tracker::set_image_i(vil1_image& image)
{
  if (!image)
  {
    vcl_cout <<"In strk_info_tracker::set_image_i(.) - null input\n";
    return;
  }

  vil1_memory_image_of<float> in, hue, sat;
  int w = image.width(), h = image.height();

  if (!color_info_||image.components()==1)
  {
    vil1_memory_image_of<float> in=brip_vil1_float_ops::convert_to_float(image);
    image_i_= brip_vil1_float_ops::gaussian(in, sigma_);
  }

  if (color_info_&&image.components()==3)
  {
    vil1_memory_image_of<vil1_rgb<unsigned char> > temp(image);
    brip_vil1_float_ops::convert_to_IHS(temp, in, hue, sat);
    image_i_= brip_vil1_float_ops::gaussian(in, sigma_);
    hue_i_ = brip_vil1_float_ops::gaussian(hue, sigma_);
    sat_i_ = brip_vil1_float_ops::gaussian(sat, sigma_);
  }

  if (gradient_info_)
  {
    Ix_i_.resize(w,h);
    Iy_i_.resize(w,h);
    brip_vil1_float_ops::gradient_3x3(image_i_, Ix_i_, Iy_i_);
  }
}

//--------------------------------------------------------------------------
//: Set the initial model region and position
void strk_info_tracker::set_initial_model(vtol_face_2d_sptr const& face)
{
  initial_model_ = face;
}


//--------------------------------------------------------------------------
//: Initialize the info_tracker
bool strk_info_tracker::init()
{
  if (!image_0_)
  {
    vcl_cout << "In strk_info_tracker::init() - no initial video frame\n";
    return false;
  }

  if (!initial_model_)
  {
    vcl_cout << "In strk_info_tracker::init() - not all faces set\n";
    return false;
  }

  strk_tracking_face_2d_sptr tf;
  tf = new strk_tracking_face_2d(initial_model_, image_0_,
                                 Ix_0_, Iy_0_, hue_0_, sat_0_,
                                 min_gradient_,
                                 parzen_sigma_,
                                 intensity_hist_bins_,
                                 gradient_dir_hist_bins_,
                                 color_hist_bins_
                                 );

  initial_tf_ = tf;
  if (renyi_joint_entropy_)
    tf->set_renyi_joint_entropy();

  current_samples_.push_back(tf);
  return true;
}

//--------------------------------------------------------------------------
//: generate a randomly positioned augmented face
strk_tracking_face_2d_sptr strk_info_tracker::
generate_randomly_positioned_sample(strk_tracking_face_2d_sptr const& seed)
{
  if (!seed)
    return 0;
  //random sample of uniform distribution
  float tx = (2.f*search_radius_)*float(vcl_rand()/(RAND_MAX+1.0)) - search_radius_;
  float ty = (2.f*search_radius_)*float(vcl_rand()/(RAND_MAX+1.0)) - search_radius_;
  float theta = (2.f*angle_range_)*float(vcl_rand()/(RAND_MAX+1.0)) - angle_range_;
  float s = (2.f*scale_range_)*float(vcl_rand()/(RAND_MAX+1.0)) - scale_range_;
  float scale = 1+s;
  strk_tracking_face_2d* tf = new strk_tracking_face_2d(seed);

  tf->transform(tx, ty, theta, scale);
  return tf;
}

static void
print_tracking_bounds(vcl_vector<strk_tracking_face_2d_sptr> const& faces)
{
  if (faces.size() == 0)
    return;
#ifdef DEBUG
  float xmin = faces.front()->face()->Xo(), xmax = xmin;
  float ymin = faces.front()->face()->Yo(), ymax = ymin;
  for (vcl_vector<strk_tracking_face_2d_sptr>::const_iterator fit =
       faces.begin(); fit != faces.end(); fit++)
  {
    float x = (*fit)->face()->Xo(), y = (*fit)->face()->Yo();
    if (x < xmin) xmin = x;
    if (x > xmax) xmax = x;
    if (y < ymin) ymin = y;
    if (y > ymax) ymax = y;
  }
  strk_tracking_face_2d_sptr f = faces[0];
  float search_area = (xmax-xmin)*(ymax-ymin);
  float R = f->Npix()/search_area;
   vcl_cout << "S[(" << xmin << ' ' << xmax << ")(" << ymin << ' ' << ymax
            << ")]= " << search_area << ", area ratio = " << R << '\n';
#endif
}

//--------------------------------------------------------------------------
//: generate a random set of new faces from the existing samples
void strk_info_tracker::generate_samples()
{
  vul_timer t;
  for (vcl_vector<strk_tracking_face_2d_sptr>::iterator fit =
       current_samples_.begin(); fit != current_samples_.end(); fit++)
    for (int i = 0; i<n_samples_; i++)
    {
      strk_tracking_face_2d_sptr tf, back;
      tf = this->generate_randomly_positioned_sample(*fit);
      if (!tf)
        continue;
      if (use_background_)
      {
        tf->intensity_mutual_info_diff(background_faces_, image_i_);
        if (color_info_)
          tf->color_mutual_info_diff(background_faces_, hue_i_, sat_i_);
        if (gradient_info_)
          tf->compute_only_gradient_mi(Ix_i_, Iy_i_);
      }
      else
        tf->compute_mutual_information(image_i_, Ix_i_, Iy_i_, hue_i_, sat_i_);

      hypothesized_samples_.push_back(tf);
    }
  //sort the hypotheses
  if (use_background_)
    vcl_sort(hypothesized_samples_.begin(),
             hypothesized_samples_.end(), info_diff_compare);
  else
    vcl_sort(hypothesized_samples_.begin(),
             hypothesized_samples_.end(), info_compare);
  print_tracking_bounds(hypothesized_samples_);//jlm
}

bool strk_info_tracker::refresh_sample()
{
#if 0
  double t = vcl_rand()/(RAND_MAX+1.0);
  return t<=frac_time_samples_;
#else
  vcl_rand();
  return false;
#endif // 0
}

strk_tracking_face_2d_sptr strk_info_tracker::
clone_and_refresh_data(strk_tracking_face_2d_sptr const& sample)
{
  if (!sample)
    return (strk_tracking_face_2d*)0;
  vtol_face_2d_sptr f =
    sample->face()->cast_to_face_2d();
  strk_tracking_face_2d_sptr tf;
  tf = new strk_tracking_face_2d(f, image_i_, Ix_i_, Iy_i_, hue_i_, sat_i_,
                                 min_gradient_, parzen_sigma_,
                                 intensity_hist_bins_,
                                 gradient_dir_hist_bins_,
                                 color_hist_bins_
                                 );
  if (renyi_joint_entropy_)
    tf->set_renyi_joint_entropy();
  return tf;
}

//--------------------------------------------------------------------------
//: cull out the best N hypothesized samples to become the current samples
void strk_info_tracker::cull_samples()
{
  current_samples_.clear();
  for (int i=0; i<n_samples_; i++)
  {
    current_samples_.push_back(hypothesized_samples_[i]);
    if (debug_)
      vcl_cout << "I[" << i << "]= " << hypothesized_samples_[i]->total_info() << vcl_endl;
  }

  strk_tracking_face_2d_sptr tf = hypothesized_samples_[0];
  if (verbose_&&!use_background_)
    vcl_cout << "Total Inf = " << tf->total_info()
             << " = IntInfo(" <<  tf->int_mutual_info()
             << ") + GradInfo(" <<  tf->grad_mutual_info()
             << ") + ColorInfo(" <<  tf->color_mutual_info()
             << ')' << vcl_endl;
  bool hist_print = false;
  if (verbose_&&hist_print)
  {
    tf->print_intensity_histograms(image_i_);
    if (gradient_info_)
      tf->print_gradient_histograms(Ix_i_, Iy_i_);
    if (color_info_)
      tf->print_color_histograms(hue_i_, sat_i_);
  }

  if (debug_)
    vcl_cout << "model_intensity_entropy = " << tf->model_intensity_entropy()
             << "\nintensity_entropy = " << tf->intensity_entropy()
             << "\nintensity_joint_entropy = " <<tf->intensity_joint_entropy()
             << "\n\nmodel_gradient_entropy = " << tf->model_gradient_entropy()
             << "\ngradient_entropy = " << tf->gradient_entropy()
             << "\ngradient_joint_entropy = " << tf->gradient_joint_entropy()
             << "\n\nmodel_color_entropy = " << tf->model_color_entropy()
             << "\ncolor_entropy = " << tf->color_entropy()
             << "\ncolor_joint_entropy = " << tf->color_joint_entropy()
             << vcl_endl << vcl_endl;
#ifdef DEBUG
  tf->print_pixels(image_i_);
#endif

  hypothesized_samples_.clear();
  //save track history
  strk_tracking_face_2d_sptr refreshed_best =
    clone_and_refresh_data(current_samples_[0]);
  track_history_.push_back(refreshed_best);


  //print background characteristics
  if (use_background_)
  {
   vcl_cout << "\n====Background Information Diffs====\n";
    float MIdiff =
      tf->intensity_info_diff();
    vcl_cout << "Intensity Inf Diff = " << MIdiff << vcl_endl;
    if (color_info_)
    {
      float color_MIdiff = tf->color_info_diff();
      vcl_cout << "Color Inf Diff = " << color_MIdiff << vcl_endl;
    }
    if (gradient_info_)
    {
      float grad_MI = tf->grad_mutual_info();
      vcl_cout << "Grad Mutual Info = " << grad_MI << vcl_endl;
    }
    vcl_cout << "Total Inf Diff = " << tf->total_info_diff() << vcl_endl;

    this->construct_background_faces(tf->face()->cast_to_face_2d());
  }
}

//--------------------------------------------------------------------------
//: because of sorting, the best sample will be the first current sample
vtol_face_2d_sptr strk_info_tracker::get_best_sample()
{
  if (!current_samples_.size())
    return 0;
  //  this->refine_best_sample();
  return current_samples_[0]->face()->cast_to_face_2d();
}

//--------------------------------------------------------------------------
//: because of sorting the samples will be in descending order of mutual info
void strk_info_tracker::get_samples(vcl_vector<vtol_face_2d_sptr>& samples)
{
  samples.clear();
  for (vcl_vector<strk_tracking_face_2d_sptr>::iterator
       fit = current_samples_.begin(); fit != current_samples_.end(); fit++)
    samples.push_back((*fit)->face()->cast_to_face_2d());
}

//--------------------------------------------------------------------------
//: Main tracking method
void strk_info_tracker::track()
{
  vul_timer t;
  this->generate_samples();
  if (verbose_)
    vcl_cout << "Samples generated in " << t.real() << " msecs.\n";
  this->cull_samples();
}

void strk_info_tracker::clear()
{
  current_samples_.clear();
  hypothesized_samples_.clear();
}

//--------------------------------------------------------------------------
//: Evaluate information between I_0 and I_i at the initial region
//  Useful for debugging purposes.
void strk_info_tracker::evaluate_info()
{
  if (!initial_model_)
    return;
  strk_tracking_face_2d_sptr tf =
    new strk_tracking_face_2d(initial_model_, image_0_,
                              Ix_0_, Iy_0_, hue_0_, sat_0_,
                              min_gradient_,
                              parzen_sigma_,
                              intensity_hist_bins_,
                              gradient_dir_hist_bins_,
                              color_hist_bins_
                              );

  if (renyi_joint_entropy_)
    tf->set_renyi_joint_entropy();

  if (!tf->compute_mutual_information(image_i_, Ix_i_, Iy_i_, hue_i_, sat_i_))
    return;

  if (verbose_)
    vcl_cout << "Total Inf = " << tf->total_info()
             << " = IntInfo(" <<  tf->int_mutual_info()
             << ") + GradInfo(" <<  tf->grad_mutual_info()
             << ") + ColorInfo(" <<  tf->color_mutual_info()
             << ')' << vcl_endl;

  if (debug_)
    vcl_cout << "model_intensity_entropy = " << tf->model_intensity_entropy()
             << "\nintensity_entropy = " << tf->intensity_entropy()
             << "\nintensity_joint_entropy = " <<tf->intensity_joint_entropy()
             << "\n\nmodel_gradient_entropy = " << tf->model_gradient_entropy()
             << "\ngradient_entropy = " << tf->gradient_entropy()
             << "\ngradient_joint_entropy = " << tf->gradient_joint_entropy()
             << "\n\nmodel_color_entropy = " << tf->model_color_entropy()
             << "\ncolor_entropy = " << tf->color_entropy()
             << "\ncolor_joint_entropy = " << tf->color_joint_entropy()
             << vcl_endl << vcl_endl;
}

void strk_info_tracker::
get_best_face_points(vcl_vector<vtol_topology_object_sptr>& points)
{
  points.clear();
  if (!current_samples_.size())
    return;
  if (!use_background_)
    current_samples_[0]->face_points(points);
}

//--------------------------------------------------------------------------
//: Construct tracking faces which are clones of the current tracking face
//  (but arrayed on each edge of the bounding box of the current tracking face)
bool strk_info_tracker::
construct_background_faces(vtol_face_2d_sptr const& current_model,
                           bool first_frame)
{
  if (!current_model)
    return false;
  background_faces_.clear();
  vsol_box_2d_sptr bb = current_model->get_bounding_box();
  if (!bb)
    return false;
  double min_x = bb->get_min_x();
  double max_x = bb->get_max_x();
  double min_y = bb->get_min_y();
  double max_y = bb->get_max_y();
  double width = max_x-min_x;
  double height = max_y-min_y;
  vcl_vector<vtol_vertex_sptr> verts;
  vcl_vector<vtol_vertex_2d_sptr> model_verts_2d;
  current_model->vertices(verts);
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end();vit++)
    model_verts_2d.push_back((*vit)->cast_to_vertex_2d());
  double tx[4], ty[4];
  tx[0]=width; ty[0]=0;
  tx[1]=-width; ty[1]=0;
  tx[2]=0; ty[2]=height;
  tx[3]=0; ty[3]=-height;
  for (int i = 0; i<4; i++)
  {
    vcl_vector<vtol_vertex_sptr> face_verts;
    for (vcl_vector<vtol_vertex_2d_sptr>::iterator vit =model_verts_2d.begin();
         vit != model_verts_2d.end(); vit++)
    {
      vtol_vertex_2d_sptr mv = (*vit);
      double x = mv->x()+tx[i], y = mv->y()+ty[i];
      vtol_vertex_2d_sptr tmv = new vtol_vertex_2d(x, y);
      face_verts.push_back(tmv->cast_to_vertex());
    }
    vtol_face_2d_sptr f = new vtol_face_2d(face_verts);
    strk_tracking_face_2d_sptr tf;
    if (first_frame)
      tf = new strk_tracking_face_2d(f, image_0_,
                                     Ix_0_, Iy_0_,
                                     hue_0_, sat_0_,
                                     min_gradient_,
                                     parzen_sigma_,
                                     intensity_hist_bins_,
                                     gradient_dir_hist_bins_,
                                     color_hist_bins_
                                     );
    else
      tf = new strk_tracking_face_2d(f, image_i_,
                                     Ix_i_, Iy_i_,
                                     hue_i_, sat_i_,
                                     min_gradient_,
                                     parzen_sigma_,
                                     intensity_hist_bins_,
                                     gradient_dir_hist_bins_,
                                     color_hist_bins_
                                     );
    if (!tf->Npix())
      continue;

    if (renyi_joint_entropy_)
      tf->set_renyi_joint_entropy();
    tf->compute_mutual_information(image_i_,
                                   Ix_i_, Iy_i_,
                                   hue_i_, sat_i_);
    background_faces_.push_back(tf);
  }
  return true;
}

bool
strk_info_tracker::get_background_faces(vcl_vector<vtol_face_2d_sptr>& faces)
{
  if (!background_faces_.size())
    return false;
  faces.clear();
  for (vcl_vector<strk_tracking_face_2d_sptr>::iterator fit = background_faces_.begin(); fit != background_faces_.end(); fit++)
    faces.push_back((*fit)->face()->cast_to_face_2d());
  return true;
}

//: Assemble the intensity, gradient and color histograms into a single vector
//  |intensity|gradient|color|

vcl_vector<float> strk_info_tracker::histograms()
{
  strk_tracking_face_2d_sptr f = current_samples_[0];
    //make the color index for display |intensity|gradient|color
  unsigned int ibins = intensity_hist_bins_;
  unsigned int gbins = gradient_dir_hist_bins_;
  unsigned int cbins = color_hist_bins_;
  unsigned int nbins = ibins + gbins + cbins;
  vcl_vector<float> out(nbins, 0.0);

  bsta_histogram<float> int_hist = f->intensity_histogram(image_i_);
  for (unsigned int i = 0; i<ibins; ++i)
    out[i]=int_hist.p(i);

  if (gradient_info_)
  {
    bsta_histogram<float> grad_hist = f->gradient_histogram(Ix_i_, Iy_i_);
    for (unsigned int i = 0; i<gbins; ++i)
      out[i+ibins]=grad_hist.p(i);
  }
  if (color_info_)
  {
    bsta_histogram<float> color_hist = f->gradient_histogram(hue_i_, sat_i_);
    for (unsigned int i = 0; i<cbins; ++i)
      out[i+ibins+gbins]=color_hist.p(i);
  }
  return out;
}
