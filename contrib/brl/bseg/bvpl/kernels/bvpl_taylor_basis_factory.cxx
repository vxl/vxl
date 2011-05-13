//:
// \file
// \author Isabel Restrepo
// \date 25-Jan-2011

#include "bvpl_taylor_basis_factory.h"
#include <vcl_map.h>

//: Constructor from filename
bvpl_taylor_basis_factory::bvpl_taylor_basis_factory(vcl_string filename)
{
  //initialize variables
  filename_ = filename;
  angle_ = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;
  parallel_axis_ = canonical_parallel_axis_;

  //this skernel is symmetric around main axis
  angular_resolution_=0;

  //create the default kernel
  create_canonical();
}

void bvpl_taylor_basis_factory::create_canonical()
{
  typedef vgl_point_3d<float> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  vcl_ifstream ifs(filename_.c_str());

  //set the dimension of the 3-d bounding box containing the kernels
  if (!ifs.eof())
  {
    ifs >> min_point_;
    ifs >> max_point_;
  }
  vcl_cout << " Max point: " << max_point_ << " Min point: " << min_point_ << vcl_endl;
  
  while (!ifs.eof())
  {
    vgl_point_3d<float> this_loc;
    float weight;
    ifs >> this_loc;
    ifs >> weight;
    canonical_kernel_.push_back(vcl_pair<point_3d,dispatch>(this_loc, dispatch(weight)));
  }

  //set the current kernel
  kernel_ = canonical_kernel_;
  factory_name_ = name();
}


/************************bvpl_taylor_basis_loader**************************************/

void bvpl_taylor_basis_loader::create_basis(vcl_map<vcl_string, bvpl_kernel_sptr> &taylor_basis)
{
  //get filenames, iterate through files reading the kernels
  vcl_vector<vcl_string> filenames;
  files(filenames);

  vcl_vector<vcl_string>::iterator file_it =filenames.begin();

  for (; file_it != filenames.end(); file_it++)
  {
    vcl_string filename = path_ + '/' + *file_it + ".txt";
    vcl_cout << "Reading kernel file : " << filename << vcl_endl;
    bvpl_taylor_basis_factory factory(filename);
    bvpl_kernel_sptr kernel = new bvpl_kernel(factory.create());
    //kernel->print();
    taylor_basis.insert(vcl_pair<vcl_string, bvpl_kernel_sptr>(*file_it, kernel));
  }
}

//: Kernels needed for 2 degree approximation of 3D functions
void bvpl_taylor_basis_loader::files(vcl_vector<vcl_string> &filenames)
{
  if (degree_ == 2)
  {
    filenames.push_back("I0");
    filenames.push_back("Ix");
    filenames.push_back("Iy");
    filenames.push_back("Iz");
    filenames.push_back("Ixx");
    filenames.push_back("Iyy");
    filenames.push_back("Izz");
    filenames.push_back("Ixy");
    filenames.push_back("Ixz");
    filenames.push_back("Iyz");
  }
}

