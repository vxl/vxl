//*****************************************************************************
// File name: example3d.cxx
// Description: Use of topology in 3D
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.1     |2002/12/09| Peter Vanroose           |Revitalised and adapted
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/08| François BERTEL          |Split of 2D and 3D
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/01| Peter TU                 |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>

#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_block.h>
#include <vtol/vtol_block_sptr.h>
#include <vbl/vbl_smart_ptr.h>

class example_vertex_3d; // forward declaration

// Since there is no vtol_edge_3d, create one here:
class example_edge_3d : public vtol_edge
{
#define T example_edge_3d
#define S vsol_spatial_object_2d
#define V vbl_smart_ptr<example_vertex_3d>
#define E vtol_edge
 public:
  T(V const& v1, V const& v2);
  T(vtol_vertex_sptr const& v1, vtol_vertex_sptr const& v2);
  virtual vsol_spatial_object_2d_sptr clone() const {return new T(v1_,v2_); }

  bool operator==(T const& e) const { return v1_==e.v1() && v2_==e.v2(); }
  bool operator==(E const& e) const { return !e.cast_to_edge_2d() && operator==((T const&)e); }
  bool operator==(S const&  ) const { return false; } // could be worked out better; see vtol_edge_2d.cxx
  T& operator=(T const& e) { v1_=e.v1(); v2_=e.v2(); return *this; }

  virtual void print(vcl_ostream &strm=vcl_cout) const { strm<<"<example_edge_3d> with id "<<get_id()<<'\n'; }
  virtual void describe(vcl_ostream &strm=vcl_cout,int=0) const { print(strm); v1_->print(strm); v2_->print(strm); }
  virtual void copy_geometry(vtol_edge const& e) { v1()->copy_geometry(*e.v1()); v2()->copy_geometry(*e.v2()); }
  virtual bool compare_geometry(vtol_edge const& e) const { return e.cast_to_edge_2d() == 0 && operator==(*(T const*)(&e)); }
#undef T
#undef S
#undef V
#undef E
};

// Since there is no vtol_vertex_3d, create one here:
class example_vertex_3d : public vtol_vertex
{
  double x_;
  double y_;
  double z_;
#define T example_vertex_3d
#define V vtol_vertex
#define S vsol_spatial_object_2d
 public:
  T(double x, double y, double z) : x_(x), y_(y), z_(z) {}

  double x() const { return x_; }
  double y() const { return y_; }
  double z() const { return z_; }

  bool operator==(T const& v) const { return x_==v.x()&&y_==v.y()&&z_==v.z(); }
  bool operator==(V const& v) const { return !v.cast_to_vertex_2d() && operator==((T const&)v); }
  bool operator==(S const&  ) const { return false; } // could be worked out better; see vtol_vertex_2d.cxx
  T& operator=(T const& v) { x_=v.x();y_=v.y();z_=v.z(); return *this; }
  V& operator=(V const& v) { if (!v.cast_to_vertex_2d()) return operator=((T const&)v); else return *this; }

  virtual vtol_edge_sptr new_edge(vtol_vertex_sptr const& v)
  {
    assert(v->cast_to_vertex_2d()==0);
    assert(v != this);
    vcl_list<vtol_topology_object*>::const_iterator zp;
    for (zp=superiors_.begin();zp!=superiors_.end();++zp)
    {
      const vcl_list<vtol_topology_object*> *sups=(*zp)->superiors_list();
      vcl_list<vtol_topology_object*>::const_iterator ep;
      for (ep=sups->begin();ep!=sups->end();++ep)
      {
        vtol_edge_sptr e=(*ep)->cast_to_edge();
        if (e->v1()==v||e->v2()==v) return e;
      }
    }
    return new example_edge_3d(this,v);
  }
  virtual vsol_spatial_object_2d_sptr clone() const {return new T(x(),y(),z()); }
  virtual void copy_geometry(const vtol_vertex&v) { assert(v.cast_to_vertex_2d()==0); operator=(*(T const*)(&v)); }
  virtual bool compare_geometry(const vtol_vertex&v) const { return v.cast_to_vertex_2d()==0 && operator==(*(T const*)(&v)); }

  void print(vcl_ostream &strm=vcl_cout) const { strm<<"<vertex "<<x()<<','<<y()<<','<<z()<<"> with id "<<get_id()<<'\n'; }
  void describe(vcl_ostream &strm=vcl_cout,int b=0) const { print(strm);describe_inferiors(strm,b);describe_superiors(strm,b); }
#undef T
#undef V
#undef S
};

#define T example_edge_3d
#define V vbl_smart_ptr<example_vertex_3d>
T::T(V const& v1, V const& v2)
{
  v1_=v1->cast_to_vertex(); v2_=v2->cast_to_vertex();
  link_inferior(new vtol_zero_chain(v1_,v2_));
}
#undef V

T::T(vtol_vertex_sptr const& v1, vtol_vertex_sptr const& v2)
{ v1_=v1; v2_=v2; link_inferior(new vtol_zero_chain(v1_,v2_)); }
#undef T

// Since there is no vtol_face_3d, create one here:
class example_face_3d : public vtol_face
{
#define T example_face_3d
#define F vtol_face
#define S vsol_spatial_object_2d
 public:
  T(vertex_list &vl) {
    assert(vl.size()>2);
    vcl_vector<signed char> dirs;
    edge_list elist;
    bool done=false;
    vertex_list::iterator vi=vl.begin();
    for (vtol_vertex_sptr v02,v01=(*vi); !done; v01=v02) {
      if (++vi==vl.end()) { vi=vl.begin(); done=true; }
      v02=(*vi);
      vtol_edge_sptr newedge=v01->new_edge(v02);
      elist.push_back(newedge);
      if (*v02 == *(newedge->v2())) dirs.push_back((signed char)1);
      else                          dirs.push_back((signed char)(-1));
    }
    link_inferior(new vtol_one_chain(elist,dirs,true));
  }
  virtual vsol_spatial_object_2d_sptr clone() const {vertex_list* vl=(const_cast<T*>(this))->vertices();
                                                     T* f=new T(*vl);delete vl;return f;}
  bool operator==(T const&) const { return false; }
  bool operator==(F const&) const { return false; }
  bool operator==(S const&) const { return false; }
  virtual void print(vcl_ostream &strm=vcl_cout) const { strm << "<example_face_3d>"; }
  virtual void describe(vcl_ostream &strm=vcl_cout, int=0) const { print(strm); }
  virtual void copy_geometry(vtol_face const& /*f*/) { /* edge(0)->copy_geometry(f.edge(0)); */ } // NYI
  virtual bool compare_geometry(vtol_face const& f) const { return f.cast_to_face_2d()==0 && operator==(*(T const*)(&f)); }
  virtual vtol_face* copy_with_arrays(topology_list&, topology_list&) const { return new T(*this); } // NYI
  virtual vtol_face* shallow_copy_with_no_links() const { return new T(*this); }
#undef T
#undef S
};

int main()
{
  // We want to make 4 vertices and put them into two faces of 3 vertices each
  vcl_cout << "Creating vertices\n";

  vtol_vertex_sptr v1=new example_vertex_3d(0.0,0.0,0.0);

  vtol_vertex_sptr v2=new example_vertex_3d(0.0,1.0,2.0);
  vtol_vertex_sptr v3=new example_vertex_3d(1.0,1.0,1.0);
  vtol_vertex_sptr v4=new example_vertex_3d(1.0,0.0,-1.0);

  v1->describe(vcl_cout, 8);
  v2->describe(vcl_cout, 8);
  v3->describe(vcl_cout, 8);
  v4->describe(vcl_cout, 8);

  vcl_cout << "Creating faces\n";

  vcl_vector<vtol_vertex_sptr> lv;
  // or equivalently:    vertex_list lv;
  lv.push_back(v1);
  lv.push_back(v2);
  lv.push_back(v3);

  vcl_cout<<"lv filled\n";

  vtol_face_sptr f1=new example_face_3d(lv);

  vcl_cout<<"Face f1 created\n";

  f1->describe(vcl_cout, 8);

  lv.clear();

  lv.push_back(v3);
  lv.push_back(v4);
  lv.push_back(v1);

  vtol_face_sptr f2=new example_face_3d(lv);

  vcl_cout<<"Face f2 created\n";

  f2->describe(vcl_cout, 8);

  vcl_cout << "Creating a block\n";

  vcl_vector<vtol_face_sptr> faces;
  // Or equivalently:    face_list faces;

  faces.push_back(f1);
  faces.push_back(f2);
  vcl_cout<<"faces filled\n";

  vtol_block_sptr b1=new vtol_block(faces);
  vcl_cout<<"Block b1 created\n";
  b1->describe(vcl_cout, 8);

  vcl_cout <<"Accessors: vertices(), zero_chains() ... blocks()\n\n"

           <<"Sizes of superiors: vertex expects       1 3 3 2 2 1 1 - gets  ";
  vertex_list *vl=v1->vertices(); vcl_cout << vl->size() << ' ';
  zero_chain_list *zcl=v1->zero_chains(); vcl_cout << zcl->size() << ' ';
  edge_list *el=v1->edges(); vcl_cout << el->size() << ' ';
  one_chain_list *ocl=v1->one_chains(); vcl_cout << ocl->size() << ' ';
  face_list *fl=v1->faces(); vcl_cout << fl->size() << ' ';
  two_chain_list *tcl=v1->two_chains(); vcl_cout << tcl->size() << ' ';
  block_list *bl=v1->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vtol_edge_sptr ed=v1->new_edge(v2);
  vtol_zero_chain_sptr zc=ed->zero_chain();
  vcl_cout <<"                    zero_chain expects   2 1 1 1 1 1 1 - gets  ";
  vl=zc->vertices(); vcl_cout << vl->size() << ' ';
  zcl=zc->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=zc->edges(); vcl_cout << el->size() << ' ';
  ocl=zc->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=zc->faces(); vcl_cout << fl->size() << ' ';
  tcl=zc->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=zc->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vtol_edge_sptr ed13= v1->new_edge(v3);
  vcl_cout <<"                    edge expects         2 2 1 2 2 1 1 - gets  ";
  vl=ed13->vertices(); vcl_cout << vl->size() << ' ';
  zcl=ed13->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=ed13->edges(); vcl_cout << el->size() << ' ';
  ocl=ed13->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=ed13->faces(); vcl_cout << fl->size() << ' ';
  tcl=ed13->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=ed13->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vtol_one_chain_sptr oc1=f1->get_one_chain(0);
  vcl_cout <<"                    one_chain expects    3 6 3 1 1 1 1 - gets  ";
  vl=oc1->vertices(); vcl_cout << vl->size() << ' ';
  zcl=oc1->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=oc1->edges(); vcl_cout << el->size() << ' ';
  ocl=oc1->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=oc1->faces(); vcl_cout << fl->size() << ' ';
  tcl=oc1->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=oc1->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vcl_cout <<"                    face expects         3 6 3 1 1 1 1 - gets  ";
  vl=f1->vertices(); vcl_cout << vl->size() << ' ';
  zcl=f1->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=f1->edges(); vcl_cout << el->size() << ' ';
  ocl=f1->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=f1->faces(); vcl_cout << fl->size() << ' ';
  tcl=f1->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=f1->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  return 0;
}
