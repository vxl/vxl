// This is brl/bseg/bmrf/bmrf_curve_3d_builder.cxx
#include "bmrf_curve_3d_builder.h"
//:
// \file

#include "bmrf_curvel_3d.h"
#include "bmrf_network.h"
#include "bmrf_node.h"
#include "bmrf_arc.h"
#include "bmrf_epipole.h"
#include "bmrf_gamma_func.h"

#include <vcl_algorithm.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_qr.h>

//: Constructor
bmrf_curve_3d_builder::bmrf_curve_3d_builder()
 : network_(NULL), min_alpha_(0.0), max_alpha_(0.0)
{
  init_intrinsic();
}

//: Constructor
bmrf_curve_3d_builder::bmrf_curve_3d_builder(bmrf_network_sptr network)
 : network_(network), min_alpha_(0.0), max_alpha_(0.0)
{
  init_intrinsic();
  init_cameras();
}


//: Initialize the intrinsic camera parameters
void
bmrf_curve_3d_builder::init_intrinsic()
{
  // For our camera images are 1024 x 768
  // The focal length is 12.5mm / (6.25 um/pixel) = 2000 pixels
  K_[0][0] = 2000;  K_[0][1] = 0;     K_[0][2] = 512;
  K_[1][0] = 0;     K_[1][1] = 2000;  K_[1][2] = 384;
  K_[2][0] = 0;     K_[2][1] = 0;     K_[2][2] = 1;
}


//: Initialize the camera matrices
void
bmrf_curve_3d_builder::init_cameras()
{
  C_.clear();
  if (!network_)
    return;
  unsigned int num_frames = network_->num_frames();
  const vgl_point_2d<double>& ep = network_->epipole(1).location();

  vnl_double_3x4 E;
  E[0][0] = 1;  E[0][1] = 0;  E[0][2] = 0;  E[0][3] = ep.x();
  E[1][0] = 0;  E[1][1] = 1;  E[1][2] = 0;  E[1][3] = ep.y();
  E[2][0] = 0;  E[2][1] = 0;  E[2][2] = 1;  E[2][3] = 1;

  for (unsigned int f=0; f<num_frames; ++f) {
    vnl_double_3x4 Ef = K_*E;
    double dt = double(f);
    Ef[0][3] = -dt*ep.x();
    Ef[1][3] = -dt*ep.y();
    Ef[2][3] = -dt;
    C_.push_back(Ef);
  }
}


//: Set the network
void
bmrf_curve_3d_builder::set_network(const bmrf_network_sptr& network)
{
  network_ = network;
  init_cameras();
}


//: Return the constructed curves
vcl_set<vcl_list<bmrf_curvel_3d_sptr> >
bmrf_curve_3d_builder::curves() const
{
  return curves_;
}


//: Build The curves
bool
bmrf_curve_3d_builder::build(int min_prj, int min_len, float sigma)
{
  if (!network_)
    return false;

  unsigned int num_frames = network_->num_frames();
  if (num_frames < 2)
    return false;

  find_alpha_bounds();

  //double a_init = (min_alpha_ + max_alpha_)*0.5;
  vcl_set<vcl_list<bmrf_curvel_3d_sptr>*> growing_curves;

  vcl_set<bmrf_curvel_3d_sptr> empty_set;
  vcl_set<bmrf_curvel_3d_sptr> init_curvels = build_curvels(empty_set, min_alpha_);
  for ( vcl_set<bmrf_curvel_3d_sptr>::iterator itr = init_curvels.begin();
        itr != init_curvels.end(); ++itr)
  {
    vcl_list<bmrf_curvel_3d_sptr> new_curve;
    new_curve.push_back(*itr);
    // This is a very sloppy way of keeping pointers to lists in curves_
    // I will rewrite this soon
    vcl_list<bmrf_curvel_3d_sptr> *list_ptr = const_cast<vcl_list<bmrf_curvel_3d_sptr>*>  (&(*curves_.insert(new_curve).first));
    growing_curves.insert(list_ptr);
  }
  // step size in alpha
  double da = 0.001;
  for ( double alpha = min_alpha_+da; alpha < max_alpha_; alpha += da ) {
    vcl_cout << "percent complete : " << (alpha - min_alpha_)/(max_alpha_-min_alpha_)*100.0<< vcl_endl;
    vcl_set<bmrf_curvel_3d_sptr> curvels = extend_curves(growing_curves, alpha);
    // find all curvels
    vcl_set<bmrf_curvel_3d_sptr> new_curvels = build_curvels(curvels, alpha);
    this->append_curvels(new_curvels, growing_curves, min_prj);
  }

  for ( vcl_set<vcl_list<bmrf_curvel_3d_sptr> >::iterator itr = curves_.begin();
        itr != curves_.end();)
  {
    vcl_set<vcl_list<bmrf_curvel_3d_sptr> >::iterator next_itr = itr;
    ++next_itr;

    this->trim_curve(const_cast<vcl_list<bmrf_curvel_3d_sptr> &> (*itr), min_prj);
    this->fill_gaps(const_cast<vcl_list<bmrf_curvel_3d_sptr> &> (*itr), da);
    this->interp_gaps(const_cast<vcl_list<bmrf_curvel_3d_sptr> &> (*itr));
    if ( itr->size() < (unsigned int)min_len )
      curves_.erase(itr);
    else
      this->reconstruct_curve(const_cast<vcl_list<bmrf_curvel_3d_sptr> &> (*itr), sigma);
    itr = next_itr;
  }

  return true;
}


//: Determine the alpha bounds from the network
void
bmrf_curve_3d_builder::find_alpha_bounds()
{
  min_alpha_ = 4.0;  // greater than pi
  max_alpha_ = -4.0; // less than -pi
  for ( bmrf_network::seg_node_map::const_iterator itr = network_->begin();
        itr != network_->end();  ++itr )
  {
    min_alpha_ = vcl_min(min_alpha_, itr->first->min_alpha());
    max_alpha_ = vcl_max(max_alpha_, itr->first->max_alpha());
  }
}



//: For sorting matches by arc probability
static bool
bmrf_arc_prob_cmp( const bmrf_arc_sptr& left,
                   const bmrf_arc_sptr& right )
{
  return left->probability() < right->probability();
}


//: Build curvels by linking across time through probable arcs
vcl_set<bmrf_curvel_3d_sptr>
bmrf_curve_3d_builder::build_curvels(vcl_set<bmrf_curvel_3d_sptr>& all_curvels, double alpha) const
{
  // find all arcs at alpha
  vcl_vector<bmrf_arc_sptr> all_arcs = this->find_arcs_at(alpha);
  // sort by probability
  vcl_sort(all_arcs.begin(), all_arcs.end(), bmrf_arc_prob_cmp);
  
  typedef vcl_map<bmrf_node_sptr, bmrf_curvel_3d_sptr> node_curvel_map;
  node_curvel_map node_map;

  for ( vcl_set<bmrf_curvel_3d_sptr>::const_iterator c_itr = all_curvels.begin();
        c_itr != all_curvels.end();  ++c_itr )
  {
    if( (*c_itr)->num_projections() == 0 )
      continue;
    for( unsigned int f=0; f<network_->num_frames(); ++f){
      bmrf_node_sptr update_node = (*c_itr)->node_at_frame(f);
      if( update_node )
        node_map[update_node] = *c_itr;
    }
  }

  vcl_set<bmrf_curvel_3d_sptr> new_curvels;

  while( !all_arcs.empty() )
  {
    bmrf_arc_sptr curr_arc = all_arcs.back();
    all_arcs.pop_back();

    bmrf_node_sptr from_node = curr_arc->from();
    bmrf_node_sptr to_node = curr_arc->to();
    
    node_curvel_map::iterator itr1 = node_map.find(from_node);
    node_curvel_map::iterator itr2 = node_map.find(to_node);
    if( itr1 == node_map.end() ){
      if( itr2 == node_map.end() ){
        bmrf_curvel_3d_sptr curr_curvel = new bmrf_curvel_3d;
        curr_curvel->set_proj_in_frame(from_node->frame_num(), alpha, from_node);
        curr_curvel->set_proj_in_frame(to_node->frame_num(), alpha, to_node);
        node_map[from_node] = curr_curvel;
        node_map[to_node] = curr_curvel;
        new_curvels.insert(curr_curvel);
      }
      else{
        bmrf_curvel_3d_sptr curr_curvel = itr2->second;
        bmrf_node_sptr other_node = curr_curvel->node_at_frame(from_node->frame_num());
        if( !other_node ){
          curr_curvel->set_proj_in_frame(from_node->frame_num(), alpha, from_node);
          node_map[from_node] = curr_curvel;
        }
      }
    }
    else{  
      if( itr2 == node_map.end() ){
        bmrf_curvel_3d_sptr curr_curvel = itr1->second;
        if( !curr_curvel->node_at_frame(to_node->frame_num()) ){
          curr_curvel->set_proj_in_frame(to_node->frame_num(), alpha, to_node);
          node_map[to_node] = curr_curvel;
        }
      }
      else{
        bmrf_curvel_3d_sptr curvel_from = itr1->second;
        bmrf_curvel_3d_sptr curvel_to = itr2->second;
        if( curvel_from->merge(curvel_to) ){
          if(new_curvels.erase(curvel_to) == 0)
            all_curvels.erase(curvel_to);
          for( unsigned int f=0; f<network_->num_frames(); ++f){
            bmrf_node_sptr update_node = curvel_to->node_at_frame(f);
            if( update_node )
              node_map[update_node] = curvel_from;
          }
        }
      }
    }
  }
  return new_curvels;
}


//: extend all curves to the next alpha 
vcl_set<bmrf_curvel_3d_sptr>
bmrf_curve_3d_builder::extend_curves( vcl_set<vcl_list<bmrf_curvel_3d_sptr>*>& growing_curves, 
                                      double alpha )
{
  unsigned int num_frames = network_->num_frames();
  vcl_set<bmrf_curvel_3d_sptr> new_curvels;

  for ( vcl_set<vcl_list<bmrf_curvel_3d_sptr>*>::const_iterator itr = growing_curves.begin();
        itr != growing_curves.end();  ++itr )
  {
    vcl_list<bmrf_curvel_3d_sptr>::reverse_iterator c_itr = (*itr)->rbegin();

    bmrf_curvel_3d_sptr curvel = new bmrf_curvel_3d;
    new_curvels.insert(curvel);
    (*itr)->push_back(curvel);

    for ( unsigned int frame = 1; frame < num_frames; ++frame )
    {
      bmrf_node_sptr node = (*c_itr)->node_at_frame(frame);
      if( !node )
        continue;
      bool prev_valid = true;
      for(unsigned int c=0; c<2; ++c){
        if( --c_itr == (*itr)->rend() || node != (*c_itr)->node_at_frame(frame)){
          prev_valid = false;
          break;
        }
      }
      if( !prev_valid )
        continue;

      if ((node->epi_seg()->min_alpha() <= alpha) &&
          (node->epi_seg()->max_alpha() >= alpha))
        curvel->set_proj_in_frame(frame, alpha, node);

    }
  }
  return new_curvels;
}


//: Find all curves that intersect \p alpha in \p frame
vcl_vector<bmrf_arc_sptr>
bmrf_curve_3d_builder::find_arcs_at(double alpha) const
{
  vcl_vector<bmrf_arc_sptr> matches;
  for ( bmrf_network::seg_node_map::const_iterator itr = network_->begin();
        itr != network_->end();  ++itr )
  {
    if ((itr->first->min_alpha() <= alpha) &&
        (itr->first->max_alpha() >= alpha))
    {
      for ( bmrf_node::arc_iterator a_itr = itr->second->begin(bmrf_node::TIME);
            a_itr != itr->second->end(bmrf_node::TIME); ++a_itr )
      {
        bmrf_epi_seg_sptr other_seg = (*a_itr)->to()->epi_seg();
        if ((other_seg->min_alpha() <= alpha) &&
            (other_seg->max_alpha() >= alpha))
        {
          matches.push_back(*a_itr);
        }
      }
    }
  }
  return matches;
}


//: Reconstruct the 3d location of a curvel from its projections
void
bmrf_curve_3d_builder::reconstruct_point(bmrf_curvel_3d_sptr curvel) const
{
  unsigned int num_frames = network_->num_frames();
  unsigned int nviews = curvel->num_projections(true);

  vcl_cout << "reconstructing from " << nviews << " views" << vcl_endl;

  vnl_matrix<double> A(2*nviews, 4, 0.0);

  unsigned int v = 0;
  for (unsigned int f = 0; f<num_frames; ++f) {
    vnl_double_2 pos;
    if ( curvel->pos_in_frame(f,pos) ) {
      for (unsigned int i=0; i<4; i++) {
        A[2*v  ][i] = pos[0]*C_[f][2][i] - C_[f][0][i];
        A[2*v+1][i] = pos[1]*C_[f][2][i] - C_[f][1][i];
      }
      ++v;
    }
  }
  vnl_svd<double> svd_solver(A);
  vnl_double_4 p = svd_solver.nullvector();

  curvel->set(p[0]/p[3], p[1]/p[3], p[2]/p[3]);
}


//: Simultaneously reconstruct all points in a 3d curve
void
bmrf_curve_3d_builder::reconstruct_curve(vcl_list<bmrf_curvel_3d_sptr>& curve, float sigma) const
{
  unsigned int num_frames = network_->num_frames();
  unsigned int num_pts = curve.size();

  float kernel[2];
  kernel[0] = 0.0f;
  kernel[1] = 1.0f;
  if (sigma > 0.0f) {
    kernel[0] = float(vcl_exp(-1.0/(2*sigma*sigma)));
    float kernel_sum = 2.0f*kernel[0] + kernel[1];
    kernel[0] /= kernel_sum;
    kernel[1] /= kernel_sum;
  }

#ifdef DEBUG
  vcl_cout << "reconstructing curve of size " << curve.size() << vcl_endl;
#endif
  vnl_matrix<double> A(3*num_pts, 3*num_pts, 0.0);
  vnl_vector<double> b(3*num_pts, 0.0);

  vcl_list<bmrf_curvel_3d_sptr>::iterator itr = curve.begin();
  for (unsigned int cnt=0; itr != curve.end(); ++itr, ++cnt)
  {
    unsigned int num_views = (*itr)->num_projections(true);
    vnl_matrix<double> C(2*num_views, 3, 0.0);
    vnl_vector<double> d(2*num_views, 0.0);
    unsigned int v=0;
    for (unsigned int f = 0; f<num_frames; ++f)
    {
      vnl_double_2 pos;
      if ( (*itr)->pos_in_frame(f,pos) ) {
        for (unsigned int i=0; i<3; i++) {
          C[2*v  ][i] = (pos[0]*C_[f][2][i] - C_[f][0][i])/2000;
          C[2*v+1][i] = (pos[1]*C_[f][2][i] - C_[f][1][i])/2000;
        }
        d[2*v  ] = -(pos[0]*C_[f][2][3] - C_[f][0][3])/2000;
        d[2*v+1] = -(pos[1]*C_[f][2][3] - C_[f][1][3])/2000;
        ++v;
      }
    }
    vnl_matrix<double> Ct = C.transpose();
    C = Ct * C;
    d = Ct * d;
    for (int i=0; i<3; ++i)
    {
      for (int j=0; j<3; ++j) {
        A[3*cnt+i][3*cnt+j] = C[i][j];
      }
      b[3*cnt+i] = d[i];
      if (cnt > 0 && cnt < num_pts-1) {
        A[3*cnt+i][3*(cnt-1)+i] -= double(kernel[0]);
        A[3*cnt+i][3* cnt   +i] += 1.0-double(kernel[1]);
        A[3*cnt+i][3*(cnt+1)+i] -= double(kernel[0]);
      }
    }
  }

  vnl_qr<double> qr_solver(A);
  vnl_vector<double> p = qr_solver.solve(b);

  vnl_vector<double> error = A*p - b;
  error.normalize();

  vnl_vector<double> error2(error.size()/3,0.0);
  for (unsigned int cnt=0; cnt < error2.size(); ++cnt) {
    double e1 = error[3*cnt];
    double e2 = error[3*cnt+1];
    double e3 = error[3*cnt+2];
    error2[cnt] = vcl_sqrt(e1*e1+e2*e2+e3*e3);
  }

  double max = error2.max_value();
  double min = error2.min_value();
  error2 -= min;
  error2 *= 1.0/(max-min);

  itr = curve.begin();
  for (unsigned int cnt=0; itr != curve.end(); ++itr, ++cnt)
  {
    (*itr)->set(p[cnt*3], p[cnt*3+1], p[cnt*3+2]);
    (*itr)->set_proj_error(error2[cnt]);
  }
}


//: Attempt to interpolate artificial values for missing correspondences
void
bmrf_curve_3d_builder::interp_gaps(vcl_list<bmrf_curvel_3d_sptr>& curve)
{
  unsigned int num_frames = network_->num_frames();
  for (unsigned int f=0; f<num_frames; ++f)
  {
    vnl_double_2 last_point;
    vcl_list<bmrf_curvel_3d_sptr>::iterator last_itr = curve.end();
    int gap_size = 0;
    for (vcl_list<bmrf_curvel_3d_sptr>::iterator itr = curve.begin();
         itr != curve.end(); ++itr )
    {
      vnl_double_2 temp_pt;
      if ( (*itr)->pos_in_frame(f, temp_pt) )
      {
        if ( gap_size > 0 && gap_size < 4)
        {
          int gap = 1;
          vnl_double_2 step = temp_pt - last_point;
          for (vcl_list<bmrf_curvel_3d_sptr>::iterator fill_itr = ++last_itr;
               fill_itr != itr;  ++fill_itr, ++gap)
          {
            double ratio = double(gap)/double(gap_size+1);
            vnl_double_2 new_pt = last_point + step*ratio;
            (*fill_itr)->set_psuedo_point(f, new_pt );
          }
        }
        last_point = temp_pt;
        last_itr = itr;
        gap_size = 0;
      }
      else if (last_itr != curve.end()) {
        ++gap_size;
      }
    }
  }
}


//: Attempt to fill in missing correspondences
void
bmrf_curve_3d_builder::fill_gaps(vcl_list<bmrf_curvel_3d_sptr>& curve, double da)
{
  unsigned int num_frames = network_->num_frames();
  for (unsigned int f=0; f<num_frames; ++f)
  {
    bool filled = true;
    while( filled )
    {
      filled = false;
 
      for( vcl_list<bmrf_curvel_3d_sptr>::iterator itr = curve.begin(); itr != curve.end(); )
      {
        bmrf_curvel_3d_sptr last_curvel = NULL;
        // move to the next valid node
        while(itr != curve.end() && !(*itr)->node_at_frame(f))
          ++itr;
        // move to the next NULL node
        while(itr != curve.end() && (*itr)->node_at_frame(f))
          last_curvel = *(itr++);
        
        if ( itr == curve.end() )
          break;
        
        bmrf_node_sptr node = last_curvel->node_at_frame(f);
        double alpha = last_curvel->alpha_at_frame(f) + da;
        if ( node->epi_seg()->max_alpha() > alpha ){
          (*itr)->set_proj_in_frame(f, alpha, node);
          filled = true;
        }
      }
      
      
      for(vcl_list<bmrf_curvel_3d_sptr>::reverse_iterator itr = curve.rbegin(); itr != curve.rend(); )
      {
        bmrf_curvel_3d_sptr last_curvel = NULL;
        // move to the next valid node
        while(itr != curve.rend() && !(*itr)->node_at_frame(f))
          ++itr;
        // move to the next NULL node
        while(itr != curve.rend() && (*itr)->node_at_frame(f))
          last_curvel = *(itr++);
        
        if ( itr == curve.rend() )
          break;
        
        bmrf_node_sptr node = last_curvel->node_at_frame(f);
        double alpha = last_curvel->alpha_at_frame(f) - da;
        if ( node->epi_seg()->min_alpha() < alpha ){
          (*itr)->set_proj_in_frame(f, alpha, node);
          filled = true;
        }
      }
    }
  }
}


//: Trim the ends of the curve with few correspondences
void
bmrf_curve_3d_builder::trim_curve(vcl_list<bmrf_curvel_3d_sptr>& curve, int min_prj)
{
  unsigned int num_frames = network_->num_frames();
  for (unsigned int f=0; f<num_frames; ++f)
  {
    bool last_valid = false;
    vnl_double_2 last_point;
    for (vcl_list<bmrf_curvel_3d_sptr>::iterator itr = curve.begin();
         itr != curve.end(); ++itr)
    {
      vnl_double_2 curr_point;
      if ( !(*itr)->pos_in_frame(f, curr_point) ){
        last_valid = false;
        continue;
      }

      if (!last_valid){
        last_valid = true;
        last_point = curr_point;
        continue;
      }
      
      double dist = vnl_vector_ssd(curr_point, last_point);
      if (dist > 2.0){
        (*itr)->set_proj_in_frame(f,0.0, NULL);
      }
       
      last_point = curr_point;
    }
  }
}


//: Match the \p curvels to the ends of the \p growing_curves
void
bmrf_curve_3d_builder::append_curvels(vcl_set<bmrf_curvel_3d_sptr>& curvels,
                                      vcl_set<vcl_list<bmrf_curvel_3d_sptr>*>& growing_curves,
                                      int min_prj)
{
  vcl_set<vcl_list<bmrf_curvel_3d_sptr>*> grown_curves;

  typedef vcl_pair<bmrf_curvel_3d_sptr, bmrf_curvel_3d_sptr> append_match;
  vcl_vector<vcl_pair<double, append_match> > matches;
  for ( vcl_set<vcl_list<bmrf_curvel_3d_sptr>*>::iterator g_itr = growing_curves.begin();
        g_itr != growing_curves.end();  ++g_itr )
  {
    typedef vcl_set<bmrf_curvel_3d_sptr>::iterator curvel_iterator;

    bmrf_curvel_3d_sptr end_curvel = (*g_itr)->back();
    bmrf_curvel_3d_sptr prev_curvel = *(++((*g_itr)->rbegin()));

    for ( curvel_iterator c_itr = curvels.begin();
          c_itr != curvels.end();  ++c_itr )
    {
      double align = this->append_correct((*c_itr), prev_curvel);
      if ( align > 0.5 ) {
        matches.push_back(vcl_pair<double, append_match>(align, append_match(end_curvel, *c_itr)));
      }
    }
  }

  vcl_sort(matches.begin(), matches.end());

  while( !matches.empty() )
  {
    bmrf_curvel_3d_sptr base_curvel = matches.back().second.first;
    bmrf_curvel_3d_sptr merge_curvel = matches.back().second.second;
    matches.pop_back();
    vcl_set<bmrf_curvel_3d_sptr>::iterator c_itr = curvels.find(merge_curvel);
    if( c_itr == curvels.end() )
      continue;
    if( base_curvel->merge(merge_curvel) )
      curvels.erase(c_itr);
  }

  for ( vcl_set<vcl_list<bmrf_curvel_3d_sptr>*>::iterator g_itr = growing_curves.begin();
        g_itr != growing_curves.end();  ++g_itr )
  {
    if( (*g_itr)->back()->num_projections() < min_prj )
      (*g_itr)->pop_back();
    else
      grown_curves.insert(*g_itr);
  }

  // make new curves for the unmatched curvels
  for ( vcl_set<bmrf_curvel_3d_sptr>::iterator c_itr = curvels.begin();
        c_itr != curvels.end();  ++c_itr )
  {
    if ( (*c_itr)->num_projections() < min_prj)
      continue;
    vcl_list<bmrf_curvel_3d_sptr> new_curve;
    new_curve.push_back(*c_itr);
    // This is a very sloppy way of keeping pointers to lists in curves_
    // I will rewrite this soon
    vcl_list<bmrf_curvel_3d_sptr> *list_ptr = const_cast<vcl_list<bmrf_curvel_3d_sptr> *>  (&(*curves_.insert(new_curve).first));
    grown_curves.insert(list_ptr);
  }

  growing_curves = grown_curves;
}


//: Return a measure (0.0 to 1.0) of how well \p new_c matches \p prev_c
double
bmrf_curve_3d_builder::append_correct( const bmrf_curvel_3d_sptr& new_c,
                                       const bmrf_curvel_3d_sptr& prev_c ) const
{
  unsigned int num_frames = network_->num_frames();
  unsigned int total_overlap = 0;
  unsigned int total_cover = 0;
  unsigned int total_equal = 0;
  for (unsigned int f = 0; f<num_frames; ++f) {
    bmrf_node_sptr p_node = prev_c->node_at_frame(f);
    bmrf_node_sptr n_node = new_c->node_at_frame(f);
    if ( p_node || n_node )
      ++total_cover;
    if ( p_node && n_node ) {
      ++total_overlap;
      if ( p_node == n_node )
        ++total_equal;
    }
  }

  if (total_overlap == 0)
    return 0.0;

  // Rate according to percentage of overlapping curvels that match
  // Use the percentage of curves that overlap as a tie-breaker
  double equal_ratio = double(total_equal)/double(total_overlap);
  double overlap_ratio = double(total_overlap)/double(total_cover);
  return equal_ratio - (1.0 - overlap_ratio)/double(num_frames);
}
