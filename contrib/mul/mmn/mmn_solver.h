#ifndef  mmn_solver_h_
#define mmn_solver_h_
//:
// \file
// \brief Base for classes which solve Markov Random Field problems
// \author Tim Cootes

#include <mmn/mmn_arc.h>
#include <vnl/vnl_fwd.h>

#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_memory.h>
#include <vcl_iosfwd.h>

//: Base for classes which solve Markov Random Field problems.
class mmn_solver
{
protected:

public:

    //: Dflt ctor
  mmn_solver();

    //: Destructor
  virtual ~mmn_solver();

    //: Input the arcs that define the graph
  virtual void set_arcs(unsigned num_nodes,
                        const vcl_vector<mmn_arc>& arcs) =0;

  //: Find values for each node with minimise the total cost
  //  \param node_cost: node_cost[i][j] is cost of selecting value j for node i
  //  \param pair_cost: pair_cost[a](i,j) is cost of selecting values (i,j) for nodes at end of arc a.
  //  \param x: On exit, x[i] gives choice for node i
  // NOTE: If arc a connects nodes v1,v2, the associated pair_cost is ordered
  // with the node with the lowest index being the first parameter.  Thus if
  // v1 has value i1, v2 has value i2, then the cost of this choice is
  // (v1<v2?pair_cost(i1,i2):pair_cost(i2,i1))
  // Returns the minimum cost
  virtual double solve(
                 const vcl_vector<vnl_vector<double> >& node_cost,
                 const vcl_vector<vnl_matrix<double> >& pair_cost,
                 vcl_vector<unsigned>& x) =0;

  //: Initialise from a text stream
  virtual bool set_from_stream(vcl_istream &is);

    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Create a copy on the heap and return base class pointer
  virtual mmn_solver* clone() const = 0;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const =0;

    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const =0;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) =0;

  static vcl_auto_ptr<mmn_solver>
    create_from_stream(vcl_istream &is);
};

  //: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mmn_solver& b);

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mmn_solver& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mmn_solver& b);

  //: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mmn_solver& b);

  //: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mmn_solver* b);

#endif


