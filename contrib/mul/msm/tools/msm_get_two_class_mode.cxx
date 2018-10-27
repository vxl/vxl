//:
// \file
// \brief Tool to generate the linear mode which best separates two classes.
// \author Tim Cootes

#include <iostream>
#include <cstdlib>
#include <vul/vul_arg.h>
#include <vsl/vsl_quick_file.h>
#include <msm/msm_shape_model.h>
#include <msm/msm_shape_instance.h>
#include <msm/msm_add_all_loaders.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_stats_1d.h>
#include <msm/msm_box_limiter.h>
#include <vnl/algo/vnl_svd.h>
#include <mbl/mbl_index_sort.h>

void print_usage()
{
  std::cout << "msm_get_two_class_mode -s shape_model.msm -l1 class1_list.txt -l2 class2_list.txt\n"
           << "Loads in shape model and two lists of shapes (image_list format).\n"
           << "Calculates the linear direction in parameter space which best separates the two.\n"
           << "Creates a new shape model with a single mode which shows this mode.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

void load_shapes(const std::string& points_dir,
                 const std::vector<std::string>& filenames,
                 std::vector<msm_points>& shapes)
{
  unsigned n=filenames.size();

  shapes.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    std::string path = points_dir+"/"+filenames[i];
    if (!shapes[i].read_text_file(path))
    {
      mbl_exception_parse_error x("Failed to load points from "+path);
      mbl_exception_error(x);
    }
  }
}

void load_shapes(const std::string& image_list_path,
                 std::vector<msm_points>& shapes,
                 std::vector<std::string>& points_names)
{
  std::ifstream ifs(image_list_path.c_str());
  if (!ifs)
  {
    std::string error_msg = "Failed to open file: "+image_list_path;
    throw (mbl_exception_parse_error(error_msg));
  }

  mbl_read_props_type props = mbl_read_props_ws(ifs);
  std::string image_dir=props.get_optional_property("image_dir","./");
  std::string points_dir=props.get_optional_property("points_dir","./");

  std::vector<std::string> image_names;

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  load_shapes(points_dir,points_names,shapes);
}

void get_shape_params(const std::vector<msm_points>& shapes,
                      msm_shape_instance& sm_inst,
                      std::vector<vnl_vector<double> >& b)
{
  unsigned n=shapes.size();
  b.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    sm_inst.fit_to_points(shapes[i]);
    b[i]=sm_inst.params();
  }
}

vnl_vector<double> calc_mean(std::vector<vnl_vector<double> >& v)
{
  vnl_vector<double> sum;
  if (v.size()>0) sum=v[0];
  for (unsigned i=1;i<v.size();++i) sum+=v[i];
  sum/=v.size();
  return sum;
}

vnl_matrix<double> calc_covar(const vnl_vector<double>& mean,
                              const std::vector<vnl_vector<double> >& v)
{
  vnl_matrix<double> sum(mean.size(),mean.size());
  sum.fill(0.0);
  for (const auto & k : v)
  {
    vnl_vector<double> dv=k-mean;
    for (unsigned j=0;j<mean.size();++j)
      for (unsigned i=0;i<mean.size();++i)
        sum(i,j)+=dv[i]*dv[j];
  }
  return sum/v.size();
}


//: Create file containing points (b[i].d1,b[i],d2)
void plot_scatter(const std::string& path,
                  const std::vector<vnl_vector<double> >& b,
                  const vnl_vector<double>& d1,
                  const vnl_vector<double>& d2)
{
  std::ofstream ofs(path.c_str());
  for (const auto & i : b)
  {
    ofs<<dot_product(d1,i)<<" "<<dot_product(d2,i)<<std::endl;
  }
  ofs.close();
  std::cout<<"Saved scatter to "<<path<<std::endl;
}

//: Create text file containing points of ROC curve
//  x[i] contains score to be used for classification.
//  First nc1 elements are assumed to be from class 1.
void plot_roc(const std::string& path,
              const std::vector<double>& x,
              unsigned nc1)
{
  unsigned n=x.size();
  unsigned nc2=n-nc1;
  unsigned n_true_pos=nc2, n_false_pos=nc1;
  double area=0.0;

  std::vector<unsigned> index;
  mbl_index_sort(x,index);
  unsigned last_i=0;
  unsigned max_n_pts=100;
  unsigned step=std::max(1u,n/max_n_pts);


  std::ofstream ofs(path.c_str());
  ofs<<"1 1"<<std::endl;
  for (unsigned i=0;i<n;++i)
  {
    if (index[i]>=nc1)
      n_true_pos--;
    else
    {
      n_false_pos--;
      area += double(n_true_pos)/nc2 * (1.0/nc1);
    }

    if (i>0 && std::fabs(x[index[i]]-x[index[i-1]])<1e-6) continue;  // Avoid splitting samples with same value

    double tpr = double(n_true_pos)/nc2;
    double fpr = double(n_false_pos)/nc1;

    if (i>last_i+step)
    {
      ofs<<fpr<<" "<<tpr<<std::endl;
      last_i=i;
    }
  }
  ofs<<"0 0"<<std::endl;
  ofs.close();
  std::cout<<"ROC saved to "<<path<<std::endl;
  std::cout<<"Area under ROC curve: "<<area<<std::endl;
}

// Save file containing names[i] x[i], sorted by x[i]
void write_value_list(const std::string& path,
                      const std::vector<std::string>& names,
                      const std::vector<double>& x)
{
  std::vector<unsigned> index;
  mbl_index_sort(x,index);

  std::ofstream ofs(path.c_str());
  if (!ofs) return;
  for (unsigned i=0;i<x.size();++i)
    ofs<<names[index[i]]<<" "<<x[index[i]]<<std::endl;
  ofs.close();
  std::cout<<"Saved ranked list of values and names to "<<path<<std::endl;
}

int main(int argc, char** argv)
{
  vul_arg<std::string> model_path("-s","Shape model path");
  vul_arg<std::string> list_path1("-l1","List of examples for class 1");
  vul_arg<std::string> list_path2("-l2","List of examples for class 2");
  vul_arg<std::string> output_path("-o","New model path","class_model.msm");
  vul_arg_parse(argc,argv);

  msm_add_all_loaders();

  if (model_path()=="" || list_path1()=="" || list_path2()=="")
  {
    print_usage();
    return 0;
  }

  msm_shape_model shape_model;

  if (!vsl_quick_file_load(shape_model,model_path()))
  {
    std::cerr<<"Failed to load shape model from "<<model_path()<<std::endl;
    return 1;
  }

  std::cout<<"Shape Model: "<<shape_model<<std::endl;

  std::vector<msm_points> shapes1, shapes2;
  std::vector<std::string> points_names1, points_names2;

  load_shapes(list_path1(),shapes1,points_names1);
  std::cout<<"Loaded "<<shapes1.size()<<" examples of class 1"<<std::endl;
  load_shapes(list_path2(),shapes2,points_names2);
  std::cout<<"Loaded "<<shapes2.size()<<" examples of class 2"<<std::endl;

  msm_shape_instance sm_instance(shape_model);

  std::vector<vnl_vector<double> > b1,b2;
  get_shape_params(shapes1,sm_instance,b1);
  get_shape_params(shapes2,sm_instance,b2);

  vnl_vector<double> mean1 = calc_mean(b1);
  vnl_vector<double> mean2 = calc_mean(b2);
  vnl_matrix<double> covar1=calc_covar(mean1,b1);
  vnl_matrix<double> covar2=calc_covar(mean2,b2);
  vnl_svd<double> svd(0.5*(covar1+covar2));

  // Simplest approach: Compute direction as mean2-mean1
//  std::cout<<"Initially just compute difference between class means"<<std::endl;
  // Could use LDA, ie multiply by inverse of in-class covar.
  vnl_vector<double>  dir=mean2-mean1;

  // Optimal direction is solution to (mean2-mean1)=S*x
  dir=svd.solve(mean2-mean1);
  dir.normalize();

  std::cout<<"Direction : "<<dir<<std::endl;

  // Compute variance along this direction
  mbl_stats_1d d_stats;
  for (const auto & i : b1) d_stats.obs(dot_product(dir,i));
  for (const auto & i : b2) d_stats.obs(dot_product(dir,i));

  // Construct a new shape model with a single mode based on this.
  vnl_vector<double> dir1 = shape_model.modes()*dir;  // Projection into shape space
  dir1.normalize();  // Should be unit long, but this forces it.

  vnl_matrix<double> new_modes(dir1.size(),1);
  new_modes.set_column(0,dir1);

  vnl_vector<double> new_var(1);
  new_var[0]=d_stats.variance();

  msm_box_limiter box_limiter;
  box_limiter.set_param_var(new_var);
  box_limiter.set_acceptance(0.98);

  msm_shape_model new_shape_model;
  new_shape_model.set(shape_model.mean_points(), new_modes, new_var,
                      shape_model.default_pose(),
                      shape_model.aligner(),
                      box_limiter);

  if (vsl_quick_file_save(new_shape_model,output_path()))
  {
    std::cout<<"Saved shape model containing one mode to "<<output_path()<<std::endl;
  }
  else
  {
    std::cerr<<"Failed to save to "<<output_path()<<std::endl;
    return 2;
  }

  // === Scatter plots ===
  // Construct a vector orthogonal to dir
  vnl_vector<double> dir2(dir.size());
  dir2.fill(0);
  dir2[0]=-dir[1];
  dir2[1]= dir[0];
  if (dir2.magnitude()<1e-5) dir2[0]=1.0;

  std::cout<<"Creating scatter files. First axis is separation direction."<<std::endl;
  plot_scatter("scatter1.txt",b1,dir,dir2);
  plot_scatter("scatter2.txt",b2,dir,dir2);

  // ======== Create ROC =============
  std::vector<double> x, x1(b1.size()), x2(b2.size());
  for (unsigned i=0;i<b1.size();++i) { x1[i]=dot_product(dir,b1[i]); x.push_back(x1[i]); }
  for (unsigned i=0;i<b2.size();++i) { x2[i]=dot_product(dir,b2[i]); x.push_back(x2[i]); }
  plot_roc("ROC.txt",x,b1.size());

  write_value_list("class_value1.txt",points_names1,x1);
  write_value_list("class_value2.txt",points_names2,x2);;


  return 0;
}
