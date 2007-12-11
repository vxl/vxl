// This is brl/bbas/bmsh3d/algo/bmsh3d_mesh_bnd.h
//---------------------------------------------------------------------
#ifndef _bmsh3d_mesh_bnd_h_
#define _bmsh3d_mesh_bnd_h_
//:
// \file
// \brief mesh boundary tracing
//
// \author
//  MingChing Chang  Dec 14, 2006
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <bmsh3d/bmsh3d_mesh.h>

class bmsh3d_bnd_chain
{
 protected:
  vcl_vector<bmsh3d_halfedge*> HE_list_;

 public:
  //=========== Constructor & Destructor ===========
  bmsh3d_bnd_chain () {
  }
  virtual ~bmsh3d_bnd_chain () {
    HE_list_.clear();
  }

  //=========== Data Accessing ===========
  vcl_vector<bmsh3d_halfedge*>& HE_list() {
    return HE_list_;
  }
  bmsh3d_halfedge* HE_list (unsigned int i) const {
    return HE_list_[i];
  }
  unsigned int num_edges () const {
    return HE_list_.size();
  }

  //=========== Querying Functions ===========
  bool is_V_incident_via_HE (const bmsh3d_vertex* V);
  void trace_polyline (vcl_vector<vgl_point_3d<double> >& polyline_vertices);

  //=========== Modification Functions ===========
  void trace_bnd_chain (bmsh3d_halfedge* startHE);
};

// #########################################################

class bmsh3d_bnd_chain_set
{
 protected:
  bmsh3d_mesh*     mesh_;
  vcl_vector<bmsh3d_bnd_chain*> chainset_;

 public:
  //=========== Constructor & Destructor ===========
  bmsh3d_bnd_chain_set (bmsh3d_mesh* mesh) {
    mesh_ = mesh;
  }
  virtual ~bmsh3d_bnd_chain_set () {
    for (unsigned int i=0; i<chainset_.size(); i++) {
      _del_bnd_chain (chainset_[i]);
    }
  }

  //=========== Data Accessing ===========
  vcl_vector<bmsh3d_bnd_chain*>& chainset() {
    return chainset_;
  }

  virtual bmsh3d_bnd_chain* _new_bnd_chain () {
    return new bmsh3d_bnd_chain ();
  }
  virtual void _del_bnd_chain (bmsh3d_bnd_chain* BC) {
    delete BC;
  }

  //=========== Modification Functions ===========
  virtual void detect_bnd_chains ();

  void remove_large_bnd_chain (unsigned int th);
};

#endif

