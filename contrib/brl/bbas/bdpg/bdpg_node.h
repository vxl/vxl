#ifndef bdpg_node_h_
#define bdpg_node_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A node that holds state for the dynamic program
// \verbatim
//  Modifications
//   Initial version February 5, 2005
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
class bdpg_node : public vbl_ref_count
{
 public:

  bdpg_node();
  ~bdpg_node();
  //: previous row on path
  int previous_row(){return previous_row_;}

  //: previous col on previous row that optimizes the path
  int previous_col(){return previous_col_;}

  //: set previous row on path
  void set_previous_row(const int row){previous_row_ = row;}

  //: set optimum previous col on previous row
  void set_previous_col(const int col){previous_col_ = col;}

  //:the prior probablity of a node
  void set_prior_prob(const double p);

  //:update the current probability of the node
  void update_prob(const double p);

  //:update the current probability of the node
  void update_log_prob(const double log_p){log_prob_+=log_p;}

  //:the current probability
  double p();

  //:the current log probability
  double log_p(){return log_prob_;}

 private:
  int previous_row_;//!< the previous row in the path to this node
  int previous_col_;//!< the previous column in the path to this node
  double prior_prob_;//!< the prior probablity of the node
  double log_prob_;//!< the log probablity of the node
};
#include <bdpg/bdpg_node_sptr.h>
#endif // bdpg_node_h_
