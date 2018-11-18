//:
// \file
// \author Tim Cootes
// \brief Compute a direction at each point, usually normal to curve

#include "msm_pose_maker.h"
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

//: Set up definitions of directions from the curves.
void msm_pose_maker::set_from_curves(unsigned n_points,
                                     const msm_curves& curves)
{
  end0_.resize(n_points,0);
  end1_.resize(n_points,0);

  for (unsigned j=0;j<curves.size();++j)
  {
    const msm_curve& curve=curves[j];
    unsigned nc=curve.size();
    // Tangent at point defined by previous and next points
    for (unsigned i=0;i<nc;++i)
    {
      if (defined(curve[i])) continue;
      end0_[curve[i]]=curve[(i+nc-1)%nc];
      end1_[curve[i]]=curve[(i+nc+1)%nc];
    }
    // For open curves, first point tangent is p0-p1,
    // last is p[nc-2],p[nc-1]
    if (curve.open())
    {
      if (defined(curve[0]))
        end0_[curve[0]]=curve[0];  // First point is p[0]-p[1]
      if (defined(curve[0]))
        end1_[curve[nc-1]]=curve[nc-1];
    }
  }
}

//: Compute a direction at each point, usually normal to curve.
void msm_pose_maker::create_vectors(const msm_points& points,
                                    std::vector<vgl_vector_2d<double> >& dir) const
{
  if (end0_.size()==0)
  {
    dir.resize(points.size());
    for (auto & i : dir)
      i=vgl_vector_2d<double>(1,0);
    return;
  }

  unsigned n_points=points.size();
  dir.resize(n_points);
  for (unsigned i=0;i<n_points;++i)
  {
    if (end0_[i]==end1_[i])
    {
      dir[i]=vgl_vector_2d<double>(1,0);
    }
    else
    {
      vgl_vector_2d<double> t = points[end0_[i]]
                              - points[end1_[i]];
      double L=t.length();
      if (L<=1e-8)
        dir[i]=vgl_vector_2d<double>(1,0);
      else
      {
        // Normal is (-t.y(),t.x())/L
        dir[i]=vgl_vector_2d<double>(-t.y()/L,t.x()/L);
      }
    }
  }
}

//: Compute mean distance between equivalent points, measured along normals to curves
//  Where normal not defined, use the absolute distance.
double msm_pose_maker::mean_normal_distance(const msm_points& points1,
                                            const msm_points& points2) const
{
  assert(points1.size()==points2.size());
  if (points1.size()==0) return 0.0;
  std::vector<vgl_vector_2d<double> > dir(points1.size());
  create_vectors(points1,dir);
  double sum_d=0;
  for (unsigned i=0;i<points1.size();++i)
  {
    vgl_vector_2d<double> dp = points2[i]-points1[i];
    if (defined(i)) sum_d += std::fabs(dot_product(dp,dir[i]));
    else            sum_d += dp.length();
  }
  return sum_d/points1.size();
}

//=======================================================================

void msm_pose_maker::print_summary(std::ostream& os) const
{
  os<<" n_points: "<<end0_.size();
}

constexpr static short version_no = 1;

//: Save class to binary file stream
void msm_pose_maker::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no);
  vsl_b_write(bfs,end0_);
  vsl_b_write(bfs,end1_);
}


//: Load class from binary file stream
void msm_pose_maker::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,end0_);
      vsl_b_read(bfs,end1_);
      break;
    default:
      std::cerr << "msm_pose_maker::b_read() :\n"
               << "Unexpected version number " << version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_pose_maker& b)
{ b.b_write(bfs); }

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_pose_maker& b)
{ b.b_read(bfs); }

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_pose_maker& b)
{
  b.print_summary(os);
  return os;
}
