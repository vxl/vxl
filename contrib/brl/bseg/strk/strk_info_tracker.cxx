// This is brl/bseg/strk/strk_info_tracker.cxx
#include "strk_info_tracker.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h> // for rand()
#include <vul/vul_timer.h>
#include <vil1/vil1_memory_image_of.h>
#include <vgl/vgl_polygon.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_vertex_2d.h>
#include <btol/btol_face_algs.h>
#include <brip/brip_vil1_float_ops.h>
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
  return f1->total_info_diff() > f2->total_info_diff();//JLM Switched
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
  background_model_ = (strk_tracking_face_2d*)0;
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
//: Set the original background face. In most cases this face 
//  surrounds the tracking face (model) so we need to construct 
//  a new face that is multiply connected in order to exclude the model pixels.
void strk_info_tracker::set_background(vtol_face_2d_sptr const& face)
{
  orig_background_face_ = face;
}
//: construct the potentially multiply connected face
bool strk_info_tracker::construct_background_face(vtol_face_2d_sptr& face)
{
//convert to vgl_polygons  
  vgl_polygon<double> orig_background_poly, model_poly, new_poly;
  if(!btol_face_algs::vtol_to_vgl(orig_background_face_, orig_background_poly))
    return false;
  if(!btol_face_algs::vtol_to_vgl(initial_model_, model_poly))
    return false;
  //need to check for proper containment but later for now just see if the
  //vertices are all inside or all outside
  bool all_in = true, all_out = true;
  vcl_vector<vgl_point_2d<double> > const & model_verts = model_poly[0];
  for(vcl_vector<vgl_point_2d<double> >::const_iterator vit = model_verts.begin();
      vit != model_verts.end(); ++vit)
    if(orig_background_poly.contains(*vit))
      all_out=false;
    else
      all_in = false;
  if(!all_in && !all_out)
    return false;//can't handle intersecting model and background faces
  if(all_in)
    {
      new_poly.push_back(orig_background_poly[0]);//outside sheet
      new_poly.push_back(model_poly[0]);//inside sheet
    }
  if(all_out)
    new_poly.push_back(orig_background_poly[0]);

  if(!btol_face_algs::vgl_to_vtol(new_poly, face))
    return false;
  return true;
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
  if (!initial_model_||use_background_&&!orig_background_face_)
    {
      vcl_cout << "In strk_info_tracker::init() - not all faces set\n";
      return false;
    }

  strk_tracking_face_2d_sptr tf;
  tf = new strk_tracking_face_2d(initial_model_, image_0_,
                                 Ix_0_, Iy_0_, hue_0_, sat_0_,
                                 min_gradient_,
                                 parzen_sigma_);
  if(renyi_joint_entropy_)
    tf->set_renyi_joint_entropy();
  
  //  tf->print_pixels(image_0_);
  current_samples_.push_back(tf);
  if(!use_background_)
    return true;
  //construct the background tracking face
  if(!construct_background_face(background_face_))
    {
      vcl_cout << "In strk_info_tracker::init() -"
               << " background face construction failed\n";
      return false;
    }
  //debug JLM
#if 0
  vcl_vector<vtol_one_chain_sptr> chains;
  vcl_vector<vtol_edge_sptr> outer_edges, hole_edges, all_edges;
  background_face_->one_chains(chains);
  int n = chains.size();
  vcl_cout << "Number of one chains " << n << "\n";
  if(n==2)
    {
      background_face_->edges(all_edges);
      vtol_one_chain_sptr outer_chain = chains[0];
      vtol_one_chain_sptr hole_chain = chains[1];
      outer_chain->edges(outer_edges);
      hole_chain->edges(hole_edges);
      vcl_cout << "Outer Edges \n";
      for(vcl_vector<vtol_edge_sptr>::iterator eit = outer_edges.begin();
          eit != outer_edges.end(); ++eit)
        vcl_cout << "[(" << (*eit)->v1()->cast_to_vertex_2d()->x() 
                 << " " <<  (*eit)->v1()->cast_to_vertex_2d()->y() 
                 << ")(" << (*eit)->v2()->cast_to_vertex_2d()->x() 
                 << " " <<  (*eit)->v2()->cast_to_vertex_2d()->y() << ")]\n";
      vcl_cout << "Chain Hole Edges \n";
      for(vcl_vector<vtol_edge_sptr>::iterator eit = hole_edges.begin();
          eit != hole_edges.end(); ++eit)
        vcl_cout << "[(" << (*eit)->v1()->cast_to_vertex_2d()->x() 
                 << " " <<  (*eit)->v1()->cast_to_vertex_2d()->y() 
                 << ")(" << (*eit)->v2()->cast_to_vertex_2d()->x() 
                 << " " <<  (*eit)->v2()->cast_to_vertex_2d()->y() << ")]\n";

      vcl_vector<vtol_one_chain_sptr>* hole_cycles = background_face_->get_hole_cycles();
      vcl_vector<vtol_edge_sptr> h_edges;
      if(hole_cycles->size()>=1)
        (*hole_cycles)[0]->edges(h_edges);
      vcl_cout << "Get Hole Cycles Edges \n";
      for(vcl_vector<vtol_edge_sptr>::iterator eit = h_edges.begin();
          eit != h_edges.end(); ++eit)
        vcl_cout << "[" << (int)((*hole_cycles)[0]->direction(*(*eit))) << "]:[(" << (*eit)->v1()->cast_to_vertex_2d()->x() 
                 << " " <<  (*eit)->v1()->cast_to_vertex_2d()->y() 
                 << ")(" << (*eit)->v2()->cast_to_vertex_2d()->x() 
                 << " " <<  (*eit)->v2()->cast_to_vertex_2d()->y() << ")]\n";

      vcl_cout << "All Edges \n";
      for(vcl_vector<vtol_edge_sptr>::iterator eit = all_edges.begin();
          eit != all_edges.end(); ++eit)
        vcl_cout << "[(" << (*eit)->v1()->cast_to_vertex_2d()->x() 
                 << " " <<  (*eit)->v1()->cast_to_vertex_2d()->y() 
                 << ")(" << (*eit)->v2()->cast_to_vertex_2d()->x() 
                 << " " <<  (*eit)->v2()->cast_to_vertex_2d()->y() << ")]\n";

    }
#endif
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
      if(use_background_)
        {
          vnl_matrix_fixed<double, 3, 3>& T = tf->trans();
          vtol_face_2d_sptr  temp = btol_face_algs::transform(background_face_, T);
          back= new strk_tracking_face_2d(temp, image_i_,
                                          Ix_i_, Iy_i_,
                                          hue_i_, sat_i_,
                                          min_gradient_,
                                          parzen_sigma_);
          if(renyi_joint_entropy_)
            back->set_renyi_joint_entropy();

          tf->intensity_mutual_info_diff(back, image_i_);
          if(color_info_)
            tf->color_mutual_info_diff(back, hue_i_, sat_i_);
        }
      else
        tf->compute_mutual_information(image_i_, Ix_i_, Iy_i_, hue_i_, sat_i_);

      hypothesized_samples_.push_back(tf);
    }
  //sort the hypotheses
  if(use_background_)
    vcl_sort(hypothesized_samples_.begin(),
             hypothesized_samples_.end(), info_diff_compare);
  else
    vcl_sort(hypothesized_samples_.begin(),
             hypothesized_samples_.end(), info_compare);
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
                                 min_gradient_, parzen_sigma_);
  if(renyi_joint_entropy_)
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
  if (verbose_)
    vcl_cout << "Total Inf = " << tf->total_info()
             << " = IntInfo(" <<  tf->int_mutual_info()
             << ") + GradInfo(" <<  tf->grad_mutual_info()
             << ") + ColorInfo(" <<  tf->color_mutual_info()
             << ")\n" << vcl_flush;
  bool hist_print = true;
  if(verbose_&&hist_print)
    {
      tf->print_intensity_histograms(image_i_);
      if(gradient_info_)
        tf->print_gradient_histograms(Ix_i_, Iy_i_);
      if(color_info_)
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
             << "\n\n\n" << vcl_flush;
//   if(debug_)
//     tf->print_pixels(image_i_);

  hypothesized_samples_.clear();
  //save track history
  strk_tracking_face_2d_sptr refreshed_best =
    clone_and_refresh_data(current_samples_[0]);
  track_history_.push_back(refreshed_best);


  //print background characteristics
  if(use_background_)//JLM
    {
      //transform background face
      vnl_matrix_fixed<double, 3, 3>& T = tf->trans();
      vtol_face_2d_sptr  temp = btol_face_algs::transform(background_face_, T);
      background_model_ = new strk_tracking_face_2d(temp, image_i_,
                                                    Ix_i_, Iy_i_,
                                                    hue_i_, sat_i_,
                                                    min_gradient_,
                                                    parzen_sigma_);
      if(renyi_joint_entropy_)
        background_model_->set_renyi_joint_entropy();

      vcl_cout << "Printing Background Information\n";
      float MIdiff = 
        tf->intensity_mutual_info_diff(background_model_, image_i_, true);
      vcl_cout << "Intensity Inf Diff = " << MIdiff << "\n" << vcl_flush;
      if(color_info_)
        {
          float color_MIdiff = 
            tf->color_mutual_info_diff(background_model_, hue_i_, sat_i_, true);
          vcl_cout << "Color Inf Diff = " << color_MIdiff << "\n" << vcl_flush;
        }
      vcl_cout << "Total Inf Diff = " << tf->total_info_diff() << "\n" << vcl_flush;
    }

}

#if 0
//--------------------------------------------------------------------------
//: Use parabolic interpolation to refine the transform for the best sample
//
void strk_info_tracker::refine_best_sample()
{
  strk_tracking_face_2d_sptr btf = current_samples_[0];
  //translation first
  double idtx = 0, idty = 0, idth = 0, ids = 0;
  double idtx_max=0, idty_max=0, inf_max= btf->total_info();
  double inf = 0;
  double xrange = 0, yrange = 0;
  vcl_cout << "Initial Info " << inf_max << vcl_endl;
  double range_fraction = 0.1;
  if (false) //  if (initial_model_)
  {
    vtol_face* f = (vtol_face_2d*)initial_model_->cast_to_face();
    vtol_topology_object* to = (vtol_topology_object*)f;
    vsol_spatial_object_2d* so = (vsol_spatial_object_2d*)to;
    vsol_box_2d_sptr bb = so->get_bounding_box();
    if (bb)
    {
      xrange = range_fraction*bb->width();
      yrange = range_fraction*bb->height();
    }
  }
  double dx = (2*xrange)/n_samples_, dy = (2*yrange)/n_samples_;
  vcl_cout << "X,Y range(" << xrange << ' ' << yrange << ")\n";
  strk_tracking_face_2d_sptr tf = new strk_tracking_face_2d(btf);
  for (double ty = -yrange; ty<=yrange; ty+=dy)
    for (double tx = -xrange; tx<=xrange;tx+=dx)
    {
      if (tx||ty)
      {
        tf->transform(tx, ty, 0, 1.0);
        this->mutual_info_face(tf);
        inf = tf->total_info();
        if (debug_)
          vcl_cout << '(' << tx << ' ' << ty  << ")=" << inf << vcl_endl;
        tf->transform(-tx, -ty, 0, 1.0);
        if (inf>inf_max)
        {
          inf_max = inf;
          idtx_max = tx;
          idty_max = ty;
        }
      }
    }
  idtx = idtx_max;
  idty = idty_max;
#if 0 // 34 lines commented out
  float dtx = 0, dty = 0, dth = 0, ds = 0;
  double bgtx=0, bgty=0, bgth=0, bgsc=1.0;
  double gtx=0, gty=0, gth=0, gsc=1.0;
  baf->global_transform(bgtx, bgty, bgth, bgsc);
  strk_quadratic_interpolator qtrans;face_points(points);
  strk_parabolic_interpolator pith;
  strk_parabolic_interpolator pisc;
  for (int i = 0; i<search_pattern.size(); i++)
  {
    strk_tracking_face_2d_sptr af = search_pattern[i];
    if (!af)
      continue;
    af->global_transform(gtx, gty, gth, gsc);
    dtx = gtx-bgtx; dty = gty-bgty;
    dth = gth-bgth;
    ds = gsc/bgsc-1.0;
    double inf = af->total_info();
    qtrans.add_data_point(dtx,dty,inf);
    pith.add_data_point(dth, inf);
    pisc.add_data_point(ds, inf);
  }

  if (search_radius_&&qtrans.solve())
    qtrans.extremum(idtx, idty);
  // limit the interpolated value
  if (vcl_fabs(idtx)>search_radius_)
    idtx = 0;
  if (vcl_fabs(idty)>search_radius_)
    idty = 0;                   min_gradient_,
                              
  if (vcl_fabs(idth)>angle_range_)
    idth = 0;
  if (vcl_fabs(ids)>scale_range_)
    ids = 0;
#endif // 0
  btf->transform(idtx, idty, idth, 1.0+ids);
  this->mutual_info_face(btf);
  double info = btf->total_info();
  vcl_cout << "Final dtrans (" << idtx << ' ' << idty
           << ' ' << idth << ' ' << ids << ") = " << info
           << vcl_endl;
}
#endif // 0

//--------------------------------------------------------------------------
//: because of sorting, the best sample will be the first current sample
vtol_face_2d_sptr strk_info_tracker::get_best_sample()
{
  if (!current_samples_.size())
    return 0;
  //  this->refine_best_sample();
  return current_samples_[0]->face()->cast_to_face_2d();
}

vtol_face_2d_sptr strk_info_tracker::current_background()
{
  return background_model_->face()->cast_to_face_2d();
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
                              parzen_sigma_);
  if(renyi_joint_entropy_)
    tf->set_renyi_joint_entropy();

  if (!tf->compute_mutual_information(image_i_, Ix_i_, Iy_i_, hue_i_, sat_i_))
    return;

  if (verbose_)
    vcl_cout << "Total Inf = " << tf->total_info()
             << " = IntInfo(" <<  tf->int_mutual_info()
             << ") + GradInfo(" <<  tf->grad_mutual_info()
             << ") + ColorInfo(" <<  tf->color_mutual_info()
             << ")\n" << vcl_flush;

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
             << "\n\n\n"<< vcl_flush;
}
void strk_info_tracker::
get_best_face_points(vcl_vector<vtol_topology_object_sptr>& points)
{
  points.clear();
  if(!current_samples_.size())
    return;
  if(!use_background_)
    current_samples_[0]->face_points(points);
  else
    background_model_->face_points(points);
}
