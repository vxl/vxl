//:
// \file
// \brief Tool to estimate point matches under reflection
// If a shape is approximately symmetric, then reflecting it leads to a similar shape,
// but with a different point order.  This estimates the re-ordering of the points
// implied by the reflection.
// The output is useful when training models with symmetry (enabling doubling of training set).

// \author Tim Cootes

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <mbl/mbl_parse_int_list.h>
#include <vul/vul_arg.h>
#include <vul/vul_string.h>

#include <msm/msm_add_all_loaders.h>
#include <msm/msm_similarity_aligner.h>

#include <msm/msm_shape_model_builder.h>

void print_usage()
{
  std::cout << "msm_guess_ref_sym -i image_list.txt -r0 1 -r1 7 -o ref_sym.txt\n"
           << "Compute mean shape.  Reflect it. Estimate the re-ordering of the points implied by the reflection.\n"
           << "List defining re-ordering saved to named output text file.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters (mostly text)
struct tool_params
{
  //: Directory containing images
  std::string image_dir;

  //: Directory containing points
  std::string points_dir;

  //: File to save shape model to
  std::string shape_model_path;

  //: List of image names
  std::vector<std::string> image_names;

  //: List of points file names
  std::vector<std::string> points_names;

  //: Parse named text file to read in data
  //  Throws a mbl_exception_parse_error if fails
  void read_from_file(const std::string& path);
};

//
//: Parse named text file to read in data
//  Throws a mbl_exception_parse_error if fails
void tool_params::read_from_file(const std::string& path)
{
  std::ifstream ifs(path.c_str());
  if (!ifs)
  {
    std::string error_msg = "Failed to open file: "+path;
    throw (mbl_exception_parse_error(error_msg));
  }

  mbl_read_props_type props = mbl_read_props_ws(ifs);

  image_dir=props.get_optional_property("image_dir","./");
  points_dir=props.get_optional_property("points_dir","./");
  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

}

// Return points reflected so that points[r0] maps to points[r1]
msm_points reflect_points(const msm_points& points, int r0, int r1)
{
  vgl_point_2d<double> p0=points[r0];
  vgl_point_2d<double> p1=points[r1];
  vgl_vector_2d<double> u=p1-p0;
  vgl_point_2d<double> c=midpoint(p0,p1);
  u/=u.length();

  msm_points new_points=points;

  for (unsigned i=0;i<points.size();++i)
  {
    double z=dot_product(points[i]-c,u);
    new_points.set_point(i,points[i]-2*z*u);
  }
  return new_points;
}


double estimate_matches(const msm_points& points1, const msm_points& points2,
                        std::vector<unsigned>& ref_sym)
{
  // Compute all distances
  unsigned n = points1.size();
  vnl_matrix<double> D(n,n);
  for (unsigned i=0;i<n;++i)
    for (unsigned j=0;j<n;++j)
      D(i,j)=(points1[i]-points2[j]).length();

  // Now select best matches, point by point
  // Note that this may lead to inconsistencies.
  //  Perhaps fix in a later version
  ref_sym.resize(n);

  double error_sum=0;
  for (unsigned i=0;i<n;++i)
  {
    // Find closest point
    unsigned best_j=0;
    double min_d=D(i,0);
    for (unsigned j=1;j<n;++j)
      if (D(i,j)<min_d) { min_d=D(i,j); best_j=j; }

    ref_sym[i]=best_j;
    error_sum+=min_d;
  }

  return error_sum;
}

//: If indices starting at index[i] ascend, write them out as first:last
bool write_stepped_list(std::ostream& os, const std::vector<unsigned>& index,
                          unsigned& i, int step)
{
  if (i>=index.size()-2) return false;
  if (index[i+1]!=index[i]+step) return false;
  if (index[i+2]!=index[i]+2*step) return false;
  // i,i+1,i+2 are in sequence.
  // Find end of sequence
  unsigned j=i+2;
  while (j+1<index.size() && (index[j+1]==unsigned(index[j]+step))) ++j;

  // Sequence runs from index i to index j
  os<<index[i]<<":"<<index[j]<<" ";
  i=j+1;
  return true;
}

//: Write index values to a stream, using X:Y format for ascending or descending sequences
inline void write_indices(std::ostream& os, const std::vector<unsigned>& index)
{
  unsigned i=0;
  while (i<index.size())
  {
    if (!write_stepped_list(os,index,i,+1))
    {
      if (!write_stepped_list(os,index,i,-1))
      {
        os<<index[i]<<" ";
        ++i;
      }
    }
  }
}


//: Main function
int main(int argc, char** argv)
{
  vul_arg<std::string> image_list_path("-i","Image list");
  vul_arg<std::string> out_path("-o","Output path","ref_sym.txt");
  vul_arg<int> ref_pt0("-r0","Ref point 0",-1);
  vul_arg<int> ref_pt1("-r1","Ref point 1",-1);
  vul_arg_parse(argc,argv);

  msm_add_all_loaders();

  if (image_list_path()=="" || ref_pt0()<0 || ref_pt1()<0)
  {
    print_usage();
    return 0;
  }

  tool_params params;
  try
  {
    params.read_from_file(image_list_path());
  }
  catch (mbl_exception_parse_error& e)
  {
    std::cerr<<"Error: "<<e.what()<<std::endl;
    return 1;
  }

  std::cout<<"Building mean from "<<params.image_names.size()<<" examples."<<std::endl;

  // === Load in all the shapes ===
  unsigned n_egs=params.points_names.size();
  std::vector<msm_points> points(n_egs);
  msm_load_shapes(params.points_dir,params.points_names,points);

  // Align the shapes and compute the mean shape
  msm_similarity_aligner aligner;
  std::vector<vnl_vector<double> > pose_to_ref;
  vnl_vector<double> average_pose;
  msm_points mean_shape;

  aligner.align_set(points,mean_shape,pose_to_ref,average_pose,msm_aligner::mean_pose);

  msm_points ref_shape = reflect_points(mean_shape,ref_pt0(),ref_pt1());

  std::cout<<"Estimating matches..."<<std::endl;

  std::vector<unsigned> ref_sym;
  double sym_err=estimate_matches(mean_shape,ref_shape,ref_sym);

  std::cout<<"Mean point error: "<<sym_err/mean_shape.size()<<std::endl;

  // Check result is self consistent
  for (unsigned i=0;i<ref_sym.size();++i)
  {
    if (ref_sym[ref_sym[i]]!=i)
      std::cerr<<"Inconsistent result: "<<i<<"->"<<ref_sym[i]
              <<" but "<<ref_sym[i]<<"->"<<ref_sym[ref_sym[i]]<<std::endl;
  }

  std::ofstream ofs(out_path().c_str());
  if (!ofs)
    std::cerr<<"Failed to open "<<out_path()<<std::endl;
  else
  {
    ofs<<"reflection_symmetry: { ";
    write_indices(ofs,ref_sym);
    ofs<<" }"<<std::endl;
    ofs.close();
    std::cout<<"Saved point match list to "<<out_path()<<std::endl;
  }
  return 0;
}
