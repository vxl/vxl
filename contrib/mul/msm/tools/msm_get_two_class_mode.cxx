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

void load_shapes(const std::string& image_list_path, std::vector<msm_points>& shapes)
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

  std::vector<std::string> points_names,image_names;

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
                              std::vector<vnl_vector<double> >& v)
{
  vnl_matrix<double> sum(mean.size(),mean.size());
  sum.fill(0.0);
  for (unsigned k=0;k<v.size();++k)
  {
    vnl_vector<double> dv=v[k]-mean;
    for (unsigned j=0;j<mean.size();++j)
      for (unsigned i=0;i<mean.size();++i)
        sum(i,j)+=dv[i]*dv[j];
  }
  return sum/v.size();
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
  load_shapes(list_path1(),shapes1);
  std::cout<<"Loaded "<<shapes1.size()<<" examples of class 1"<<std::endl;
  load_shapes(list_path2(),shapes2);
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
  for (unsigned i=0;i<b1.size();++i) d_stats.obs(dot_product(dir,b1[i]));
  for (unsigned i=0;i<b2.size();++i) d_stats.obs(dot_product(dir,b2[i]));

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

  return 0;
}
