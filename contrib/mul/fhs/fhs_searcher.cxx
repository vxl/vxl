//:
// \file
// \author Tim Cootes
// \brief Use F&H's DP style algorithm to search for global solutions

#include <fhs/fhs_searcher.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for std::abort()
#include <vnl/vnl_math.h>
#include <vil/vil_bilin_interp.h>
#include <vil/algo/vil_quad_distance_function.h>
#include <vimt/vimt_bilin_interp.h>

//: Default constructor
fhs_searcher::fhs_searcher()
{
}

//: Set tree defining relationships between features
//  Input arcs define neighbour relationships in any order.
//  root_node defines which feature to be used as the root
void fhs_searcher::set_tree(const vcl_vector<fhs_arc>& arcs,
                            unsigned root_node)
{
  if (!fhs_order_tree_from_root(arcs,arc_,children_,root_node))
  {
    vcl_cerr<<"fhs_searcher::set_tree() Failed to set up the tree.\n";
    return;
  }

  // Fill in elements of arc_to_j_
  arc_to_j_.resize(arc_.size()+1);
  for (unsigned i=0;i<arc_.size();++i)
    arc_to_j_[arc_[i].j()]=i;
}

//: Index of root node (set by last call to set_tree()
unsigned fhs_searcher::root_node() const
{
  assert(arc_.size()>0);
  return arc_[0].i();
}

//: Combine responses for image im_index
void fhs_searcher::combine_responses(unsigned im_index,
                                     const vimt_image_2d_of<float>& feature_response)
{
  sum_im_[im_index].deep_copy(feature_response);

  for (unsigned ci = 0; ci<children_[im_index].size();++ci)
  {
    unsigned child_node = children_[im_index][ci];
    const fhs_arc& arc_to_c = arc_[arc_to_j_[child_node]];

    // Add offset version of child_node information to sum_im_

    // An inefficient initial implementation, assuming a
    // large cost for points sampled outside any response image
    vimt_transform_2d i2w = sum_im_[im_index].world2im().inverse();
    vimt_transform_2d c_w2i =  sum_im_[child_node].world2im();

    vgl_vector_2d<double> dp(arc_to_c.dx(),arc_to_c.dy());

    // Check if transformation between images is an integer multiple
    vgl_point_2d<double> p0 = c_w2i(i2w(0,0)+dp);
    vgl_point_2d<double> p1 = c_w2i(i2w(1,0)+dp);
    vgl_point_2d<double> p2 = c_w2i(i2w(0,1)+dp);
    vgl_vector_2d<double> dx = p1-p0;
    vgl_vector_2d<double> dy = p2-p0;
    // Check that dx=(0,1) and dy=(1,0)
    bool is_int_transform = (vcl_fabs(dx.x()-1)<1e-4) &&
                            (vcl_fabs(dx.y()  )<1e-4) &&
                            (vcl_fabs(dy.x()  )<1e-4) &&
                            (vcl_fabs(dy.y()-1)<1e-4);

    vil_image_view<float>& sum_image = sum_im_[im_index].image();
    unsigned ni = sum_image.ni();
    unsigned nj = sum_image.nj();
    float* s_data = sum_image.top_left_ptr();
    vcl_ptrdiff_t s_istep = sum_image.istep();
    vcl_ptrdiff_t s_jstep = sum_image.jstep();
    unsigned cni = dist_im_[child_node].image().ni();
    unsigned cnj = dist_im_[child_node].image().nj();
    const float* c_data = dist_im_[child_node].image().top_left_ptr();
    vcl_ptrdiff_t c_istep = dist_im_[child_node].image().istep();
    vcl_ptrdiff_t c_jstep = dist_im_[child_node].image().jstep();

    if (is_int_transform)
    {
      // Use fast pointer based approach to performing the addition
      int ci0 = vnl_math_rnd(p0.x());
      int cj  = vnl_math_rnd(p0.y());
      const float * c_row = c_data + cj*c_jstep + ci0*c_istep;
      float *s_row = s_data;
      for (unsigned j=0;j<nj;++j,++cj,c_row+=c_jstep,s_row+=s_jstep)
      {
        int ci = ci0;
        const float *c_pix = c_row;
        float * s_pix = s_row;

        if (cj<0 || cj>=(cnj-1))  // Whole row is illegal
        {
          for (unsigned i=0;i<ni;++i, s_pix+=s_istep)
            *s_pix += 9999;  // Illegal if off the image
        }
        else
        {
          for (unsigned i=0;i<ni;++i,++ci,c_pix+=c_istep, s_pix+=s_istep)
          {
            if (ci<0 ||ci>=(cni-1))
              *s_pix += 9999;  // Illegal if off the image
            else
              *s_pix += *c_pix;
          }
        }
      }
    }
    else
    {
      for (unsigned j=0;j<nj;++j)
        for (unsigned i=0;i<ni;++i)
        {
          // Compute point in target image (in image co-ords)
          vgl_point_2d<double> p = c_w2i(i2w(i,j)+dp);

          if (p.x()<0 || p.y()<0 || p.x()>=(cni-1) || p.y()>=(cnj-1))
            sum_image(i,j) += 9999;  // Illegal if off the image
          else
            sum_image(i,j) += vil_bilin_interp_unsafe(p.x(),p.y(),
                                                      c_data,c_istep,c_jstep);
        }
    }
  }
}

//: Perform global search
//  Images of feature response supplied.  The transformation
//  (world2im()) for each image can be used to indicate regions
//  which don't necessarily overlap.  However, effective displacements
//  are assumed to be in pixel sized steps.
//
//  After calling search(), results can be obtained using
//  points() and best_points() etc
void fhs_searcher::search(const vcl_vector<vimt_image_2d_of<float> >& feature_response)
{
  assert(feature_response.size()==n_points());

  sum_im_.resize(n_points());
  dist_im_.resize(n_points());
  pos_im_.resize(n_points());

  // Work from leaves back to root
  for (int a=int(arc_.size())-1; a>=0; --a)
  {
    // arc_[a].j() is unique, and is only visited after its children
    unsigned node_a = arc_[a].j();

    // Set sum_im_[node_a] to be the feature response image
    // combined with the suitably offset summed responses from
    // the children of node_a
    combine_responses(node_a,feature_response[node_a]);

    // Compute scaling applied to x and y axes
    const vimt_transform_2d& w2im = feature_response[node_a].world2im();
    double sx = w2im.delta(vgl_point_2d<double>(0,0),
                       vgl_vector_2d<double>(1,0)).length();
    double sy = w2im.delta(vgl_point_2d<double>(0,0),
                       vgl_vector_2d<double>(0,1)).length();


    vil_quad_distance_function(sum_im_[node_a].image(),
                               1.0/(sx*sx*arc_[a].var_x()),
                               1.0/(sy*sy*arc_[a].var_y()),
                               dist_im_[node_a].image(),
                               pos_im_[node_a].image());
    dist_im_[node_a].set_world2im(sum_im_[node_a].world2im());
    pos_im_[node_a].set_world2im(sum_im_[node_a].world2im());
  }

  // Now combine children of root node
  unsigned root_node = arc_[0].i();
  combine_responses(root_node,feature_response[root_node]);

  // sum_im_[root_node] now contains quality of fit result for solution
}

//: Compute optimal position of all points given position of root
//  Assumes search() has been called first
void fhs_searcher::points_from_root(const vgl_point_2d<double>& root_pt,
                        vcl_vector<vgl_point_2d<double> >& pts) const
{
  if (n_points()<2)
  {
    vcl_cerr<<"fhs_searcher::points_from_root() Not initialised.\n";
    vcl_abort();
  }

  pts.resize(n_points());
  pts[root_node()]=root_pt;

  // Propogate solution through the tree
  // arc_ ordered so that parents always precede leaves
  for (vcl_vector<fhs_arc>::const_iterator a=arc_.begin();a!=arc_.end();++a)
  {
    // Compute mean predicted position for leaf point
    vgl_point_2d<double> p_j0 = pts[a->i()]
                                 +vgl_vector_2d<double>(a->dx(),a->dy());

    // Look up position allowing for offset (in image coords)
    double px = vimt_bilin_interp_safe(pos_im_[a->j()],p_j0,0);
    double py = vimt_bilin_interp_safe(pos_im_[a->j()],p_j0,1);
    // Project back to world co-ords
    pts[a->j()] = pos_im_[a->j()].world2im().inverse()(px,py);
  }
}

static vgl_point_2d<int> min_image_point(const vil_image_view<float>& image)
{
  const unsigned ni=image.ni(),nj=image.nj();
  const vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const float* row = image.top_left_ptr();
  unsigned best_i=0,best_j=0;
  float min_val = *row;
  for (unsigned j=0;j<nj;++j,row+=jstep)
  {
    const float* pixel = row;
    for (unsigned i=0;i<ni;++i,pixel+=istep)
      if (*pixel<min_val) {min_val=*pixel, best_i=i; best_j=j; }
  }

  return vgl_point_2d<int>(best_i,best_j);
}

//: Compute optimal position of all points
//  Assumes search() has been called first
double fhs_searcher::best_points(vcl_vector<vgl_point_2d<double> >& pts) const
{
  // Find position of global minima in root quality image
  vgl_point_2d<int> p_im = min_image_point(sum_im_[root_node()].image());
  vgl_point_2d<double> root_pt = sum_im_[root_node()].world2im().inverse()(p_im.x(),p_im.y());
  points_from_root(root_pt,pts);

  return sum_im_[root_node()].image()(p_im.x(),p_im.y());
}

//: Return final total cost image for root
const vimt_image_2d_of<float>& fhs_searcher::root_cost_image() const
{
  return sum_im_[root_node()];
}
