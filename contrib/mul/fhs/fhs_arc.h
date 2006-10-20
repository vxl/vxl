// This is mul/fhs/fhs_arc.h
#ifndef fhs_arc_h_
#define fhs_arc_h_
//:
// \file
// \author Tim Cootes
// \brief Link between one node and another

#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>

//: Link between one node and another
//  Indicates that position of node j, p(j) = p(i) + (N(dx,var_x),N(dy,var_y))
//  where N(m,var) is a gaussian with mean m and variance var
class fhs_arc
{
private:
  unsigned i_;
  unsigned j_;
  double dx_;
  double dy_;
  double var_x_;
  double var_y_;
public:
    //: Default constructor
  fhs_arc()
    : i_(0),j_(0),dx_(0),dy_(0),var_x_(1.0),var_y_(1.0) {}

    //: Constructor
  fhs_arc(int i, int j, double dx, double dy, double var_x, double var_y)
    : i_(i),j_(j),dx_(dx),dy_(dy),var_x_(var_x),var_y_(var_y) {}

  //: Return arc from j to i (ie directions reversed)
  fhs_arc flipped() const
  { return fhs_arc(j(),i(),-dx(),-dy(),var_x(),var_y()); }

    //: Index of first node
  unsigned i() const { return i_; }

    //: Index of second node
  unsigned j() const { return j_; }

    //: Mean x offset of j() from i()
  double dx() const { return dx_; }

    //: Mean y offset of j() from i()
  double dy() const { return dy_; }

    //: Variance of x offset of j() from i()
  double var_x() const { return var_x_; }

    //: Variance of y offset of j() from i()
  double var_y() const { return var_y_; }

    //: Write to binary stream
  void b_write(vsl_b_ostream& bfs) const;

    //: Read from binary stream
  void b_read(vsl_b_istream& bfs);
};

//: Re-order list of arcs so that parents precede their children
//  Assumes that there are n nodes (indexed 0..n-1),
//  thus n-1 arcs defining a tree.
//  On exit children[i] gives list of children of node i
bool fhs_order_tree_from_root(const vcl_vector<fhs_arc>& arc0,
                         vcl_vector<fhs_arc>& new_arc,
                         vcl_vector<vcl_vector<unsigned> >& children,
                         unsigned new_root);


//: Print
vcl_ostream& operator<<(vcl_ostream& os, const fhs_arc& c);

//: Print set
vcl_ostream& operator<<(vcl_ostream& os, const vcl_vector<fhs_arc>& arc);

//: Save
inline void vsl_b_write(vsl_b_ostream& bfs, const fhs_arc& t)
{
  t.b_write(bfs);
}

//: Load
inline void vsl_b_read(vsl_b_istream& bfs, fhs_arc& t)
{
  t.b_read(bfs);
}

//: Print
void vsl_print_summary(vcl_ostream& os, const fhs_arc& t);

#endif // fhs_arc_h_
