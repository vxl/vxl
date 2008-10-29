// This is brl/bbas/imesh/algo/imesh_pca.cxx
#include "imesh_pca.h"
//:
// \file

#include <imesh/imesh_fileio.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>


imesh_pca_mesh::imesh_pca_mesh(const vcl_vector<imesh_mesh>& meshes)
  : imesh_mesh(meshes[0]), mean_verts_(this->vertices().clone())
{
  vnl_matrix<double> M = compute_mean(meshes);
  vnl_svd<double> A(M);

  std_devs_ = A.W().diagonal();
  pc_ = A.U().transpose();

  params_.set_size(std_devs_.size());
  params_.fill(0.0);
}


//: compute and set the mean return the deviations matrix
vnl_matrix<double>
imesh_pca_mesh::compute_mean(const vcl_vector<imesh_mesh>& meshes)
{
  const unsigned num_training = meshes.size();
  vnl_matrix<double> M(this->num_verts()*3,num_training);
  vnl_vector<double> mean(this->num_verts()*3,0.0);

  for (unsigned int i=0; i<num_training; ++i)
  {
    assert(meshes[i].num_verts() == this->num_verts());
    const imesh_vertex_array<3>& verts = meshes[i].vertices<3>();
    for (unsigned v=0; v<verts.size(); ++v) {
      M(3*v,i) = verts[v][0];
      M(3*v+1,i) = verts[v][1];
      M(3*v+2,i) = verts[v][2];
      mean[3*v] += verts[v][0];
      mean[3*v+1] += verts[v][1];
      mean[3*v+2] += verts[v][2];
    }
  }

  mean /= num_training;

  for (unsigned int i=0; i<num_training; ++i)
  {
    M.set_column(i,M.get_column(i) - mean);
    imesh_vertex_array<3>& verts = this->vertices<3>();
    imesh_vertex_array<3>& mverts = this->mean_vertices<3>();
    for (unsigned v=0; v<verts.size(); ++v) {
      verts[v][0] = mverts[v][0] = mean[3*v];
      verts[v][1] = mverts[v][1] = mean[3*v+1];
      verts[v][2] = mverts[v][2] = mean[3*v+2];
    }
  }

  return M;
}


//: Constructor from a mesh, mean, standard deviations, and PC matrix
imesh_pca_mesh::imesh_pca_mesh(const imesh_mesh& mesh,
                               const vnl_vector<double>& mean,
                               const vnl_vector<double>& std_devs,
                               const vnl_matrix<double>& pc)
  : imesh_mesh(mesh)
{
  init(mean,std_devs,pc);
}


//: Copy Constructor
imesh_pca_mesh::imesh_pca_mesh(const imesh_pca_mesh& other)
  : imesh_mesh(other),
    std_devs_(other.std_devs_),
    pc_(other.pc_),
    mean_verts_((other.mean_verts_.get()) ? other.mean_verts_->clone() : 0),
    params_(other.params_)
{
}


//: Construct from a mesh with no variation
imesh_pca_mesh::imesh_pca_mesh(const imesh_mesh& mesh)
  : imesh_mesh(mesh), mean_verts_(this->vertices().clone())
{
}


//: Initialize the PCA data (assuming mesh data is already set)
//  Use this to add PCA data after a mesh has be loaded from a file
void imesh_pca_mesh::init(const vnl_vector<double>& mean,
                          const vnl_vector<double>& std_devs,
                          const vnl_matrix<double>& pc)
{
  std_devs_ = std_devs;
  pc_ = pc;
  mean_verts_.reset(new imesh_vertex_array<3>(this->num_verts()));

  assert(this->num_verts()*3 == mean.size());
  assert(this->num_verts()*3 == pc.columns());
  assert(std_devs.size() == pc.rows());

  imesh_vertex_array<3>& mverts = this->mean_vertices<3>();
  for (unsigned int i=0; i<mverts.size(); ++i)
  {
    mverts[i] = imesh_vertex<3>(mean[3*i],mean[3*i+1],mean[3*i+2]);
  }
  params_ = this->project(this->vertices());
}


//: Set the pca parameters
void imesh_pca_mesh::set_params(const vnl_vector<double>& p)
{
  assert(params_.size() >= p.size());
  unsigned int i=0;
  for (; i<p.size(); ++i)
    params_[i] = p[i];
  // fill the rest with zeros
  for (; i<params_.size(); ++i)
    params_[i] = 0.0;

  imesh_vertex_array<3>& verts = this->vertices<3>();
  const imesh_vertex_array<3>& mverts = this->mean_vertices<3>();
  for (unsigned i=0; i<verts.size(); ++i) {
    imesh_vertex<3>& v = verts[i];
    const imesh_vertex<3>& mv = mverts[i];
    v = mv;
    for (unsigned j=0; j<p.size(); ++j) {
      v[0] += pc_(j,3*i)  *params_[j];
      v[1] += pc_(j,3*i+1)*params_[j];
      v[2] += pc_(j,3*i+2)*params_[j];
    }
  }
}

//: Set an individual pca parameter
// This is done by incremental difference, errors may accumulate
// over many calls.
void imesh_pca_mesh::set_param(unsigned int idx, double param)
{
  double diff = param - params_[idx];
  params_[idx] = param;

  imesh_vertex_array<3>& verts = this->vertices<3>();
  for (unsigned i=0; i<verts.size(); ++i) {
    imesh_vertex<3>& v = verts[i];
    v[0] += pc_(idx,3*i)  *diff;
    v[1] += pc_(idx,3*i+1)*diff;
    v[2] += pc_(idx,3*i+2)*diff;
  }
}


//: Reset all the PCA parameters to zero
//  Returning to the mean mesh
void imesh_pca_mesh::set_mean()
{
  params_.fill(0.0);

  imesh_vertex_array<3>& verts = this->vertices<3>();
  const imesh_vertex_array<3>& mverts = this->mean_vertices<3>();
  for (unsigned v=0; v<verts.size(); ++v) {
    verts[v] = mverts[v];
  }
}


//: Project mesh vertices into the PCA parameter space
vnl_vector<double>
imesh_pca_mesh::project(const imesh_vertex_array_base& vertices) const
{
  assert(dynamic_cast<const imesh_vertex_array<3>*>(&vertices));
  const imesh_vertex_array<3>& verts =
      static_cast<const imesh_vertex_array<3>&>(vertices);

  const imesh_vertex_array<3>& mverts = this->mean_vertices<3>();

  const unsigned int num_verts = this->num_verts();
  vnl_vector<double> vals(3*num_verts);
  for (unsigned int i=0; i<num_verts; ++i)
  {
    const imesh_vertex<3>& mv = mverts[i];
    const imesh_vertex<3>& v = verts[i];
    vals[3*i]   = v[0] - mv[0];
    vals[3*i+1] = v[1] - mv[1];
    vals[3*i+2] = v[2] - mv[2];
  }

  return pc_*vals;
}


//=============================================================================
// External functions


//: Compute the image Jacobians at each vertex for PCA parameters in the result
//  Matrix n, row i is the image space derivative
//  at vertex n with respect to the ith pca parameter
vcl_vector<vnl_matrix<double> >
imesh_pca_image_jacobians(const vpgl_proj_camera<double>& camera,
                          const imesh_pca_mesh& mesh)
{
  // convert vertices to vgl points
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();
  const unsigned int num_verts = mesh.num_verts();
  vcl_vector<vgl_point_3d<double> > pts(num_verts);
  for (unsigned int i=0; i<num_verts; ++i)
    pts[i] = verts[i];

  // compute the Jacobians at each point
  vcl_vector<vnl_matrix_fixed<double,2,3> > J = image_jacobians(camera,pts);

  // map the image Jacobians into PCA Jacobians
  const vnl_matrix<double>& pc = mesh.principal_comps();
  vcl_vector<vnl_matrix<double> > img_jac(num_verts);
  for (unsigned int i=0; i<num_verts; ++i)
  {
    vnl_matrix<double> dir_3d(3,pc.rows());
    pc.extract(dir_3d,3*i,0);

    img_jac[i] = J[i]*dir_3d;
  }

  return img_jac;
}


//: Read a PCA mesh from a mean mesh and a pca file
imesh_pca_mesh imesh_read_pca(const vcl_string& mean_file,
                              const vcl_string& pca_file)
{
  imesh_mesh mean;
  vnl_vector<double> m,s;
  vnl_matrix<double> P;
  imesh_read(mean_file,mean);
  imesh_read_pca(pca_file,m,s,P);
  return imesh_pca_mesh(mean,m,s,P);
}


//: Read a PCA file
bool imesh_read_pca(const vcl_string& pca_file,
                    vnl_vector<double>& mean,
                    vnl_vector<double>& std_devs,
                    vnl_matrix<double>& pc)
{
  vcl_ifstream ifs(pca_file.c_str());
  if (!ifs.is_open())
    return false;

  vcl_vector<double> data;
  if (ifs.peek() == '#') {
    vcl_string s;
    ifs >> s;
    if (s == "#mag") {
      vcl_string line;
      vcl_getline(ifs,line);
      vcl_stringstream ss(line);
      double val;
      while (ss >> val) {
        data.push_back(val);
      }
      std_devs.set_size(data.size());
      for (unsigned i=0; i<std_devs.size(); ++i)
        std_devs[i] = data[i];
    }
  }
  vcl_string line;
  data.clear();
  vcl_vector<vcl_vector<double> > data_M;
  while (vcl_getline(ifs,line).good()) {
    vcl_stringstream ss(line);
    double val;
    ss >> val;
    data.push_back(val);
    vcl_vector<double> row;
    while (ss >> val) {
      row.push_back(val);
    }
    data_M.push_back(row);
  }
  mean.set_size(data.size());
  pc.set_size(data_M[0].size(),mean.size());
  for (unsigned i=0; i<mean.size(); ++i) {
    mean[i] = data[i];
    for (unsigned j=0; j<pc.rows(); ++j) {
      pc(j,i) = data_M[i][j];
    }
  }

  return true;
}


//: Write the mean mesh and PCA file
void imesh_write_pca(const vcl_string& mesh_file,
                     const vcl_string& pca_file,
                     const imesh_pca_mesh& pmesh)
{
  const vnl_vector<double>& std_dev = pmesh.std_devs();
  const vnl_matrix<double>& pc = pmesh.principal_comps();
  const imesh_vertex_array<3>& mverts = pmesh.mean_vertices<3>();
  const unsigned int num_data = pc.columns();
  vnl_vector<double> mean(num_data);
  for (unsigned int i=0; i<num_data; ++i)
    mean[i] = mverts[i/3][i%3];

  vcl_auto_ptr<imesh_vertex_array_base> verts(mverts.clone());
  vcl_auto_ptr<imesh_face_array_base> faces(pmesh.faces().clone());
  imesh_mesh mean_mesh(verts,faces);

  imesh_write_pca(pca_file,mean,std_dev,pc);
  imesh_write_obj(mesh_file,mean_mesh);
}


//: Write a PCA file
bool imesh_write_pca(const vcl_string& filename,
                     const vnl_vector<double>& mean,
                     const vnl_vector<double>& svals,
                     const vnl_matrix<double>& vars)
{
  const unsigned int num_comps = svals.size();
  const unsigned int num_data = mean.size();
  vcl_ofstream ofs(filename.c_str());
  ofs << "#mag";
  for (unsigned int i=0; i<num_comps; ++i)
    ofs << ' ' << svals[i];
  ofs << '\n';
  for (unsigned int j=0; j<num_data; ++j) {
    ofs << mean[j];
    for (unsigned int i=0; i<num_comps; ++i)
      ofs << ' ' << vars(i,j);
    ofs << '\n';
  }

  ofs.close();
  return true;
}
