// This is brl/bseg/bmrf/bmrf_curve_3d_builder.cxx
#include "bmrf_curve_3d_builder.h"
//:
// \file

#include "bmrf_curvel_3d.h"
#include "bmrf_network.h"
#include "bmrf_node.h"
#include "bmrf_epipole.h"
#include "bmrf_gamma_func.h"

#include <vcl_algorithm.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
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
  int num_frames = network_->num_frames();
  const vgl_point_2d<double>& ep = network_->epipole(1).location();

  vnl_double_3x4 E;
  E[0][0] = 1;  E[0][1] = 0;  E[0][2] = 0;  E[0][3] = ep.x();
  E[1][0] = 0;  E[1][1] = 1;  E[1][2] = 0;  E[1][3] = ep.y();
  E[2][0] = 0;  E[2][1] = 0;  E[2][2] = 1;  E[2][3] = 1;

  for (int f=0; f<num_frames; ++f){
    vnl_double_3x4 Ef = K_*E;
    Ef[0][3] = -f*ep.x();
    Ef[1][3] = -f*ep.y();
    Ef[2][3] = -f;
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

  int num_frames = network_->num_frames();
  if (num_frames < 2)
    return false;

  find_alpha_bounds();

  //double a_init = (min_alpha_ + max_alpha_)*0.5;
  vcl_list<vcl_list<bmrf_curvel_3d_sptr>*> growing_curves;

  vcl_list<bmrf_curvel_3d_sptr> init_curvels = build_curvels(min_alpha_, min_prj);
  for ( vcl_list<bmrf_curvel_3d_sptr>::iterator itr = init_curvels.begin();
        itr != init_curvels.end(); ++itr)
  {
    vcl_list<bmrf_curvel_3d_sptr> new_curve;
    new_curve.push_back(*itr);
    // This is a very sloppy way of keeping pointers to lists in curves_
    // I will rewrite this soon
    vcl_list<bmrf_curvel_3d_sptr> *list_ptr = const_cast<vcl_list<bmrf_curvel_3d_sptr> *>  (&(*curves_.insert(new_curve).first));
    growing_curves.push_back(list_ptr);
  }

  for( double alpha = min_alpha_+0.001; alpha < max_alpha_; alpha += 0.001 ) {
    // find all curvels
    vcl_list<bmrf_curvel_3d_sptr> curvels = build_curvels(alpha, min_prj);
    this->append_curvels(curvels, growing_curves);
  }

  for ( vcl_set<vcl_list<bmrf_curvel_3d_sptr> >::iterator itr = curves_.begin();
        itr != curves_.end();)
  {
    vcl_set<vcl_list<bmrf_curvel_3d_sptr> >::iterator next_itr = itr;
    ++next_itr;
    if ( itr->size() < min_len )
      curves_.erase(itr);
    else
      this->reconstruct_curve(const_cast<vcl_list<bmrf_curvel_3d_sptr> &> (*itr), sigma);
    itr = next_itr;
  }

#if 0
  int i = 0;
  for ( vcl_list<bmrf_curvel_3d_sptr>::iterator a_itr = init_curvels.begin();
        a_itr != init_curvels.end(); ++a_itr, ++i )
  {
    vcl_cout << "curve #" << i << vcl_endl
             << *(*a_itr) << vcl_endl;
    vnl_double_4 hpt3 = vnl_double_4((*a_itr)->x(),(*a_itr)->y(),(*a_itr)->z(),1);
    for ( int f = 0; f < num_frames; ++f){
      vnl_double_2 pos;
      if ( (*a_itr)->pos_in_frame(f, pos) ){
        vnl_double_3 hpt2 = C_[f]*hpt3;
        vcl_cout << "at frame " << f << " pos=" << pos
                 << " <--> "<< hpt2[0]/hpt2[2] << ' '<< hpt2[1]/hpt2[2] << vcl_endl;
      }
    }
  }
#endif

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


//: Build curvels by matching curves in all frames at \p alpha
vcl_list<bmrf_curvel_3d_sptr>
bmrf_curve_3d_builder::build_curvels(double alpha, int min)
{
  int num_frames = network_->num_frames();

  vcl_list<bmrf_curvel_3d_sptr> all_curvels;
  vcl_list<bmrf_curvel_3d_sptr> prev_curvels;

  // find all intersecting curves
  vcl_map<double, bmrf_node_sptr> matches = find_curves_at(alpha, 0);
  for ( vcl_map<double, bmrf_node_sptr>::iterator itr = matches.begin();
        itr != matches.end(); ++itr )
  {
    bmrf_curvel_3d_sptr curvel = new bmrf_curvel_3d();
    curvel->set_proj_in_frame(0, alpha, itr->second);
    prev_curvels.push_back(curvel);
    all_curvels.push_back(curvel);
  }

  for ( int frame = 1; frame < num_frames; ++frame ) {
    vcl_list<bmrf_curvel_3d_sptr> curr_curvels;
    // find all intersecting curves
    vcl_map<double, bmrf_node_sptr> matches = find_curves_at(alpha, frame);
//    for ( vcl_map<double, bmrf_node_sptr>::iterator itr = matches.begin();
//          itr != matches.end(); ++itr )
    for ( vcl_list<bmrf_curvel_3d_sptr>::iterator itr = prev_curvels.begin();
          itr != prev_curvels.end();  ++itr)
    {
      vcl_map<double, bmrf_node_sptr>::iterator c_itr = best_match(*itr, matches, alpha, frame);
      if( c_itr != matches.end() ){
        (*itr)->set_proj_in_frame(frame, alpha, c_itr->second);
        curr_curvels.push_back(*itr);
        matches.erase(c_itr);
      }
    }

    for ( vcl_map<double, bmrf_node_sptr>::iterator itr = matches.begin();
          itr != matches.end(); ++itr )
    {
      bmrf_curvel_3d_sptr curvel(new bmrf_curvel_3d);
      curvel->set_proj_in_frame(frame, alpha, itr->second);
      curr_curvels.push_back(curvel);
      all_curvels.push_back(curvel);
    }
    prev_curvels = curr_curvels;
  }

  // remove those with less than 'min' projections
  // triangulate 3D points for the rest
  for ( vcl_list<bmrf_curvel_3d_sptr>::iterator itr = all_curvels.begin();
        itr != all_curvels.end(); )
  {
    vcl_list<bmrf_curvel_3d_sptr>::iterator next_itr = itr;
    ++next_itr;

    if ((*itr)->num_projections() < min)
      all_curvels.erase(itr);
    else{
      //this->reconstruct_point(*itr);
    }

    itr = next_itr;
  }

  return all_curvels;
}


//: Find all curves that intersect \p alpha in \p frame
vcl_map<double, bmrf_node_sptr>
bmrf_curve_3d_builder::find_curves_at(double alpha, int frame)
{
  vcl_map<double, bmrf_node_sptr> matches;
  for ( bmrf_network::seg_node_map::const_iterator itr = network_->begin(frame);
        itr != network_->end(frame);  ++itr )
  {
    if ((itr->first->min_alpha() <= alpha) &&
        (itr->first->max_alpha() >= alpha))
    {
      matches[itr->first->s(alpha)] = (itr->second);
    }
  }
  return matches;
}


//: return the curvel in \p list that best matches \p node at \p alpha
vcl_list<bmrf_curvel_3d_sptr>::iterator 
bmrf_curve_3d_builder::best_match( const bmrf_node_sptr& node, 
                                   vcl_list<bmrf_curvel_3d_sptr>& list,
                                   double alpha ) const
{
  vcl_vector<vcl_list<bmrf_curvel_3d_sptr>::iterator> matches;
  vcl_vector<bmrf_node::bmrf_arc_sptr> arcs;
  node->probability();
  for ( bmrf_node::arc_iterator itr = node->begin(bmrf_node::TIME);
        itr != node->end(bmrf_node::TIME); ++itr )
  {
    bmrf_node_sptr neighbor = (*itr)->to();
    if ( (neighbor->epi_seg()->min_alpha() > alpha) ||
         (neighbor->epi_seg()->max_alpha() < alpha) )
      continue;

    for ( vcl_list<bmrf_curvel_3d_sptr>::iterator l_itr = list.begin();
          l_itr != list.end();  ++l_itr )
    {
      if ((*l_itr)->is_projection(neighbor)){
        matches.push_back(l_itr);
        arcs.push_back(*itr);
        break;
      }
    }
  }
  if(matches.empty())
    return list.end();
  if(matches.size() > 1){
    vcl_cout << "warning: "<< matches.size() <<" matches" << vcl_endl;
    for (int i=0; i<matches.size(); ++i){
      vcl_cout << "   prob: " << arcs[i]->probability()<< vcl_endl;
    }
  }
  return matches.front();
}


//: return the node iterator in \p choices that best matches \p curvel at \p alpha
vcl_map<double, bmrf_node_sptr>::iterator 
bmrf_curve_3d_builder::best_match( const bmrf_curvel_3d_sptr& curvel, 
                                   vcl_map<double, bmrf_node_sptr>& choices,
                                   double alpha, int frame ) const
{
  vcl_vector<vcl_map<double, bmrf_node_sptr>::iterator> matches;
  vcl_vector<bmrf_node::bmrf_arc_sptr> arcs;

  bmrf_node_sptr node = curvel->node_at_frame(frame-1);
  if(!node)
    return choices.end();
  node->probability();

  for ( bmrf_node::arc_iterator a_itr = node->begin(bmrf_node::TIME);
        a_itr != node->end(bmrf_node::TIME); ++a_itr )
  {
    bmrf_node_sptr neighbor = (*a_itr)->to();
    if(neighbor->frame_num() != frame)
      continue;

    for ( vcl_map<double, bmrf_node_sptr>::iterator c_itr = choices.begin();
          c_itr != choices.end(); ++c_itr )
    {
      if (c_itr->second == neighbor){
        matches.push_back(c_itr);
        arcs.push_back(*a_itr);
        break;
      }
    }
  }

        /*
  for ( vcl_map<double, bmrf_node_sptr>::iterator c_itr = choices.begin();
        c_itr != choices.end(); ++c_itr )
  {
    c_itr->second->probability();
    for ( bmrf_node::arc_iterator itr = c_itr->second->begin(bmrf_node::TIME);
          itr != c_itr->second->end(bmrf_node::TIME); ++itr )
    {
      bmrf_node_sptr neighbor = (*itr)->to();
      if ( (neighbor->epi_seg()->min_alpha() > alpha) ||
           (neighbor->epi_seg()->max_alpha() < alpha) )
        continue;

      if (curvel->is_projection(neighbor)){
        matches.push_back(c_itr);
        arcs.push_back(*itr);
        break;
      }
    }
  }
*/
  if(matches.empty())
    return choices.end();

  int best_index = 0;
  double max_prob = 0.0;
  if(matches.size() > 1){
    return choices.end(); // test to see if this stuff matters

    vcl_cout << "warning: "<< matches.size() <<" matches" << vcl_endl;
    for (int i=0; i<matches.size(); ++i){
      if(arcs[i]->probability() > max_prob){
        best_index = i;
        max_prob = arcs[i]->probability();
      }
      vcl_cout << "   prob: " << arcs[i]->probability()<< vcl_endl;
    }
  }
  return matches[best_index];
}


//: Reconstruct the 3d location of a curvel from its projections
void
bmrf_curve_3d_builder::reconstruct_point(bmrf_curvel_3d_sptr curvel) const
{
  unsigned int num_frames = network_->num_frames();
  unsigned int nviews = curvel->num_projections();

  vcl_cout << "reconstructing from " << nviews << " views" << vcl_endl;

  vnl_matrix<double> A(2*nviews, 4, 0.0);

  unsigned int v = 0;
  for (unsigned int f = 0; f<num_frames; ++f){
    vnl_double_2 pos;
    if ( curvel->pos_in_frame(f,pos) ){
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
  kernel[0] = 0.0;
  kernel[1] = 1.0;
  if(sigma > 0.0){   
    kernel[0] = vcl_exp(-1.0/(2.0*sigma*sigma));
    float kernel_sum = 2.0*kernel[0] + kernel[1];
    kernel[0] /= kernel_sum;
    kernel[1] /= kernel_sum;
  }

  //vcl_cout << "reconstructing curve of size " << curve.size() << vcl_endl;
  vnl_matrix<double> A(3*num_pts, 3*num_pts, 0.0);
  vnl_vector<double> b(3*num_pts, 0.0);

  vcl_list<bmrf_curvel_3d_sptr>::iterator itr = curve.begin();
  for (unsigned int cnt=0; itr != curve.end(); ++itr, ++cnt)
  {
    unsigned int num_views = (*itr)->num_projections();
    vnl_matrix<double> C(2*num_views, 3, 0.0);
    vnl_vector<double> d(2*num_views, 0.0);
    unsigned int v=0;
    for (unsigned int f = 0; f<num_frames; ++f){
      vnl_double_2 pos;
      if ( (*itr)->pos_in_frame(f,pos) ){
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
    for (int i=0; i<3; ++i){
      for (int j=0; j<3; ++j){
        A[3*cnt+i][3*cnt+j] = C[i][j];
      }
      b[3*cnt+i] = d[i];
      if (cnt > 0 && cnt < num_pts-1) {
        A[3*cnt+i][3*(cnt-1)+i] -= kernel[0];
        A[3*cnt+i][3*cnt+i]     += (1.0-kernel[1]);
        A[3*cnt+i][3*(cnt+1)+i] -= kernel[0];
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


//: Match the \p curvels to the ends of the \p growing_curves
void
bmrf_curve_3d_builder::append_curvels(vcl_list<bmrf_curvel_3d_sptr> curvels,
                                      vcl_list<vcl_list<bmrf_curvel_3d_sptr>*>& growing_curves)
{
  vcl_list<vcl_list<bmrf_curvel_3d_sptr>*> grown_curves;
  for ( vcl_list<vcl_list<bmrf_curvel_3d_sptr>*>::iterator g_itr = growing_curves.begin();
        g_itr != growing_curves.end();  ++g_itr )
  {
    bmrf_curvel_3d_sptr end_curvel = (*g_itr)->back();
//  bool found_match = false;
    for ( vcl_list<bmrf_curvel_3d_sptr>::iterator c_itr = curvels.begin();
          c_itr != curvels.end();  ++c_itr )
    {
      if ( this->append_correct((*c_itr), end_curvel) ){
        (*g_itr)->push_back((*c_itr));
        grown_curves.push_back(*g_itr);
        curvels.erase(c_itr);
//      found_match = true;
        break;
      }
    }
  }
  // make new curves for the unmatched curvels
  for ( vcl_list<bmrf_curvel_3d_sptr>::iterator c_itr = curvels.begin();
        c_itr != curvels.end();  ++c_itr )
  {
    vcl_list<bmrf_curvel_3d_sptr> new_curve;
    new_curve.push_back(*c_itr);
    // This is a very sloppy way of keeping pointers to lists in curves_
    // I will rewrite this soon
    vcl_list<bmrf_curvel_3d_sptr> *list_ptr = const_cast<vcl_list<bmrf_curvel_3d_sptr> *>  (&(*curves_.insert(new_curve).first));
    grown_curves.push_back(list_ptr);
  }

  growing_curves = grown_curves;
}


//: Return true if \p new_c is a reasonable match to \p prev_c
bool
bmrf_curve_3d_builder::append_correct( const bmrf_curvel_3d_sptr& new_c,
                                       const bmrf_curvel_3d_sptr& prev_c ) const
{
  unsigned int num_frames = network_->num_frames();
  unsigned int total_overlap = 0;
  unsigned int total_equal = 0;
  for (unsigned int f = 0; f<num_frames; ++f){
    bmrf_node_sptr p_node = prev_c->node_at_frame(f);
    bmrf_node_sptr n_node = new_c->node_at_frame(f);
    if ( p_node && n_node ){
      ++total_overlap;
      if ( p_node == n_node )
        ++total_equal;
    }
  }
  float ratio = 0.0;
  if ( total_overlap > 0 )
    ratio = float(total_equal)/float(total_overlap);
  return  ratio > 0.5;
}
