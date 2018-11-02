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
// 1.0     |2000/06/08| Francois BERTEL          |Split of 2D and 3D
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/01| Peter TU                 |Creation
//*****************************************************************************

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

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
  vsol_spatial_object_2d* clone() const override {return new T(v1_,v2_); }
  std::string is_a() const override { return std::string("example_edge_3d"); }

  bool operator==(T const& e) const { return v1_==e.v1() && v2_==e.v2(); }
  bool operator==(E const& e) const override { return !e.cast_to_edge_2d() && operator==((T const&)e); }
  bool operator==(S const&  ) const override { return false; } // could be worked out better; see vtol_edge_2d.cxx
  T& operator=(T const& e) { v1_=e.v1(); v2_=e.v2(); return *this; }

  void print(std::ostream &strm=std::cout) const override { strm<<"<example_edge_3d> with id "<<get_id()<<'\n'; }
  void describe(std::ostream &strm=std::cout,int=0) const override { print(strm); v1_->print(strm); v2_->print(strm); }
  void copy_geometry(vtol_edge const& e) override { v1()->copy_geometry(*e.v1()); v2()->copy_geometry(*e.v2()); }
  bool compare_geometry(vtol_edge const& e) const override { return e.cast_to_edge_2d() == nullptr && operator==(*(T const*)(&e)); }
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
  bool operator==(V const& v) const override { return !v.cast_to_vertex_2d() && operator==((T const&)v); }
  bool operator==(S const&  ) const override { return false; } // could be worked out better; see vtol_vertex_2d.cxx
  T& operator=(T const& v) { x_=v.x();y_=v.y();z_=v.z(); return *this; }
  T& operator=(V const& v) override { if (!v.cast_to_vertex_2d()) operator=((T const&)v); return *this; }

  vtol_edge_sptr new_edge(vtol_vertex_sptr const& v) override
  {
    assert(v->cast_to_vertex_2d()==nullptr);
    assert(v != this);
    std::list<vtol_topology_object*>::const_iterator zp;
    for (zp=superiors_.begin();zp!=superiors_.end();++zp)
    {
      const std::list<vtol_topology_object*> *sups=(*zp)->superiors_list();
      std::list<vtol_topology_object*>::const_iterator ep;
      for (ep=sups->begin();ep!=sups->end();++ep)
      {
        vtol_edge_sptr e=(*ep)->cast_to_edge();
        if (e->v1()==v||e->v2()==v) return e;
      }
    }
    return new example_edge_3d(this,v);
  }
  vsol_spatial_object_2d* clone() const override {return new T(x(),y(),z()); }
  std::string is_a() const override { return std::string("example_vertex_3d"); }
  void copy_geometry(const vtol_vertex&v) override { assert(v.cast_to_vertex_2d()==nullptr); operator=(*(T const*)(&v)); }
  bool compare_geometry(const vtol_vertex&v) const override { return v.cast_to_vertex_2d()==nullptr && operator==(*(T const*)(&v)); }

  void print(std::ostream &strm=std::cout) const override { strm<<"<vertex "<<x()<<','<<y()<<','<<z()<<"> with id "<<get_id()<<'\n'; }
  void describe(std::ostream &strm=std::cout,int b=0) const override { print(strm);describe_inferiors(strm,b);describe_superiors(strm,b); }
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
    std::vector<signed char> dirs;
    edge_list elist;
    bool done=false;
    auto vi=vl.begin();
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
  vsol_spatial_object_2d* clone() const override {vertex_list vl; this->vertices(vl); return new T(vl); }

  std::string is_a() const override { return std::string("example_face_3d"); }
  bool operator==(T const&) const { return false; }
  bool operator==(F const&) const override { return false; }
  bool operator==(S const&) const override { return false; }
  void print(std::ostream &strm=std::cout) const override { strm << "<example_face_3d>"; }
  void describe(std::ostream &strm=std::cout, int=0) const override { print(strm); }
  void copy_geometry(vtol_face const& /*f*/) override { /* edge(0)->copy_geometry(f.edge(0)); */ } // NYI
  bool compare_geometry(vtol_face const& f) const override { return f.cast_to_face_2d()==nullptr && operator==(*(T const*)(&f)); }
  vtol_face* copy_with_arrays(topology_list&, topology_list&) const override { return new T(*this); } // NYI
  vtol_face* shallow_copy_with_no_links() const override { return new T(*this); }
#undef T
#undef S
};

int main()
{
  // We want to make 4 vertices and put them into two faces of 3 vertices each
  std::cout << "Creating vertices\n";

  vtol_vertex_sptr v1=new example_vertex_3d(0.0,0.0,0.0);

  vtol_vertex_sptr v2=new example_vertex_3d(0.0,1.0,2.0);
  vtol_vertex_sptr v3=new example_vertex_3d(1.0,1.0,1.0);
  vtol_vertex_sptr v4=new example_vertex_3d(1.0,0.0,-1.0);

  v1->describe(std::cout, 8);
  v2->describe(std::cout, 8);
  v3->describe(std::cout, 8);
  v4->describe(std::cout, 8);

  std::cout << "Creating faces\n";

  std::vector<vtol_vertex_sptr> lv;
  // or equivalently:    vertex_list lv;
  lv.push_back(v1);
  lv.push_back(v2);
  lv.push_back(v3);

  std::cout<<"lv filled\n";

  vtol_face_sptr f1=new example_face_3d(lv);

  std::cout<<"Face f1 created\n";

  f1->describe(std::cout, 8);

  lv.clear();

  lv.push_back(v3);
  lv.push_back(v4);
  lv.push_back(v1);

  vtol_face_sptr f2=new example_face_3d(lv);

  std::cout<<"Face f2 created\n";

  f2->describe(std::cout, 8);

  std::cout << "Creating a block\n";

  std::vector<vtol_face_sptr> faces;
  // Or equivalently:    face_list faces;

  faces.push_back(f1);
  faces.push_back(f2);
  std::cout<<"faces filled\n";

  vtol_block_sptr b1=new vtol_block(faces);
  std::cout<<"Block b1 created\n";
  b1->describe(std::cout, 8);

  std::cout <<"Accessors: vertices(), zero_chains() ... blocks()\n\n";
  vertex_list vl;
  zero_chain_list zcl;
  edge_list el;
  one_chain_list ocl;
  face_list fl;
  two_chain_list tcl;
  block_list bl;

  std::cout <<"Sizes of superiors: vertex expects       1 3 3 2 2 1 1 - gets  ";
  v1->vertices(vl); std::cout << vl.size() << ' ';
  v1->zero_chains(zcl); std::cout << zcl.size() << ' ';
  v1->edges(el); std::cout << el.size() << ' ';
  v1->one_chains(ocl); std::cout << ocl.size() << ' ';
  v1->faces(fl); std::cout << fl.size() << ' ';
  v1->two_chains(tcl); std::cout << tcl.size() << ' ';
  v1->blocks(bl); std::cout << bl.size() << '\n';

  vtol_edge_sptr ed=v1->new_edge(v2);
  vtol_zero_chain_sptr zc=ed->zero_chain();
  std::cout <<"                    zero_chain expects   2 1 1 1 1 1 1 - gets  ";
  zc->vertices(vl); std::cout << vl.size() << ' ';
  zc->zero_chains(zcl); std::cout << zcl.size() << ' ';
  zc->edges(el); std::cout << el.size() << ' ';
  zc->one_chains(ocl); std::cout << ocl.size() << ' ';
  zc->faces(fl); std::cout << fl.size() << ' ';
  zc->two_chains(tcl); std::cout << tcl.size() << ' ';
  zc->blocks(bl); std::cout << bl.size() << '\n';

  vtol_edge_sptr ed13= v1->new_edge(v3);
  std::cout <<"                    edge expects         2 2 1 2 2 1 1 - gets  ";
  ed13->vertices(vl); std::cout << vl.size() << ' ';
  ed13->zero_chains(zcl); std::cout << zcl.size() << ' ';
  ed13->edges(el); std::cout << el.size() << ' ';
  ed13->one_chains(ocl); std::cout << ocl.size() << ' ';
  ed13->faces(fl); std::cout << fl.size() << ' ';
  ed13->two_chains(tcl); std::cout << tcl.size() << ' ';
  ed13->blocks(bl); std::cout << bl.size() << '\n';

  vtol_one_chain_sptr oc1=f1->get_one_chain(0);
  std::cout <<"                    one_chain expects    3 6 3 1 1 1 1 - gets  ";
  oc1->vertices(vl); std::cout << vl.size() << ' ';
  oc1->zero_chains(zcl); std::cout << zcl.size() << ' ';
  oc1->edges(el); std::cout << el.size() << ' ';
  oc1->one_chains(ocl); std::cout << ocl.size() << ' ';
  oc1->faces(fl); std::cout << fl.size() << ' ';
  oc1->two_chains(tcl); std::cout << tcl.size() << ' ';
  oc1->blocks(bl); std::cout << bl.size() << '\n';

  std::cout <<"                    face expects         3 6 3 1 1 1 1 - gets  ";
  f1->vertices(vl); std::cout << vl.size() << ' ';
  f1->zero_chains(zcl); std::cout << zcl.size() << ' ';
  f1->edges(el); std::cout << el.size() << ' ';
  f1->one_chains(ocl); std::cout << ocl.size() << ' ';
  f1->faces(fl); std::cout << fl.size() << ' ';
  f1->two_chains(tcl); std::cout << tcl.size() << ' ';
  f1->blocks(bl); std::cout << bl.size() << '\n';

  return 0;
}
