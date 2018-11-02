// This is brl/bbas/imesh/algo/imesh_pca.h
#ifndef imesh_pca_h_
#define imesh_pca_h_
//:
// \file
// \brief Mesh PCA parameterization
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date June 26, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <iostream>
#include <vector>
#include <imesh/imesh_mesh.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vpgl/vpgl_proj_camera.h>


//: Principal Component Analysis on a set of mesh vertices
class imesh_pca_mesh : public imesh_mesh
{
 public:
  //: Default Constructor
  imesh_pca_mesh() = default;

  //: Constructor from a vector of meshes with the same topology
  imesh_pca_mesh(const std::vector<imesh_mesh>& meshes);

  //: Constructor from a mesh, mean, standard deviations, and PC matrix
  imesh_pca_mesh(const imesh_mesh& mesh,
                 const vnl_vector<double>& mean,
                 const vnl_vector<double>& std_devs,
                 const vnl_matrix<double>& pc);

  //: Copy Constructor
  imesh_pca_mesh(const imesh_pca_mesh& other);

  //: Assignment operator
  imesh_pca_mesh& operator=(const imesh_pca_mesh& other);

  //: Initialize the PCA data (assuming mesh data is already set)
  //  Use this to add PCA data after a mesh has be loaded from a file
  void init(const vnl_vector<double>& mean,
            const vnl_vector<double>& std_devs,
            const vnl_matrix<double>& pc);

  //: Set the pca parameters
  void set_params(const vnl_vector<double>& p);

  //: Set an individual pca parameter
  // This is done by incremental difference, errors may accumulate
  // over many calls.
  void set_param(unsigned int i, double param);

  //: Access the params
  const vnl_vector<double>& params() const {return params_;}

  //: Access the principal components
  const vnl_matrix<double>& principal_comps() const {return pc_; }

  //: Access the standard deviations
  const vnl_vector<double>& std_devs() const {return std_devs_;}

  //: Reset all the PCA parameters to zero
  //  Returning to the mean mesh
  void set_mean();

  //: Project mesh vertices into the PCA parameter space
  vnl_vector<double> project(const imesh_vertex_array_base& verts) const;

  //: Access the vector of mean vertices
  const imesh_vertex_array_base& mean_vertices() const { return *mean_verts_; }
  imesh_vertex_array_base& mean_vertices() { return *mean_verts_; }

  //: Access the vector of mean vertices cast to a dimension
  template <unsigned int d>
  const imesh_vertex_array<d>& mean_vertices() const
  {
    assert(dynamic_cast<imesh_vertex_array<d>*>(mean_verts_.get()));
    return static_cast<const imesh_vertex_array<d>&>(*mean_verts_);
  }
  template <unsigned int d>
  imesh_vertex_array<d>& mean_vertices()
  {
    assert(dynamic_cast<imesh_vertex_array<d>*>(mean_verts_.get()));
    return static_cast<imesh_vertex_array<d>&>(*mean_verts_);
  }

 protected:
  //: compute and set the mean return the deviations matrix
  vnl_matrix<double> compute_mean(const std::vector<imesh_mesh>& meshes);
  //: Construct from a mesh with no variation
  imesh_pca_mesh(const imesh_mesh& mesh);

  vnl_vector<double> std_devs_;
  vnl_matrix<double> pc_;
  std::unique_ptr<imesh_vertex_array_base> mean_verts_;

  vnl_vector<double> params_;
};


//: Compute the image Jacobians at each vertex for PCA parameters in the result:
//  Matrix n, row i is the image space derivative
//  at vertex n with respect to the ith pca parameter
std::vector<vnl_matrix<double> >
imesh_pca_image_jacobians(const vpgl_proj_camera<double>& camera,
                          const imesh_pca_mesh& mesh);


//: Read a PCA mesh from a mean mesh and a pca file
imesh_pca_mesh imesh_read_pca(const std::string& mean_file,
                              const std::string& pca_file);

//: Read a PCA file
bool imesh_read_pca(const std::string& pca_file,
                    vnl_vector<double>& mean,
                    vnl_vector<double>& std_devs,
                    vnl_matrix<double>& pc);

//: Write the mean mesh and PCA file
void imesh_write_pca(const std::string& mesh_file,
                     const std::string& pca_file,
                     const imesh_pca_mesh& pmesh);

//: Write a PCA file
bool imesh_write_pca(const std::string& filename,
                     const vnl_vector<double>& mean,
                     const vnl_vector<double>& std_devs,
                     const vnl_matrix<double>& pc);


#endif // imesh_pca_h_
