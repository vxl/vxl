// This is gel/vsrl/vsrl_token.h
#ifndef vsrl_token_h
#define vsrl_token_h
//:
// \file
// In a dynamic programming problem we are given the task of
// assigning one set of tokens to another set of tokens.
// This class represents the abstract token.
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

class vsrl_token
{
  int index_;
  vsrl_token* assigned_token_;

  double x_; //!< the x position
  double y_; //!< the y position

 public:

  // constructor
  vsrl_token() : index_(-1), assigned_token_(0), x_(0), y_(0) {}

  // destructor
  virtual ~vsrl_token() {}

  //: does this token represent a null assignment?
  virtual bool null_token() {return false;}

  //: is this token an intensity token ?
  virtual bool intensity_token() {return false;}

  //: what is the direct cost of assigning this token to tok
  virtual double cost(vsrl_token * /*tok*/) { return 0; }

  //: what is the incremental cost of going from from this to tok2 given the assignment going from tok_p to tok_2p
  virtual double incremental_cost(vsrl_token * /*tok2*/, vsrl_token * /*tok_p*/, vsrl_token * /*tok_2p*/) { return 0; }

  //: get the index of this token
  int get_index() { return index_; }

  //: set the index of this token
  void set_index(int index) { index_ = index; }

  //: get the assigned token
  vsrl_token* get_assigned_token() { return assigned_token_; }

  //: set the assigned token
  void set_assigned_token(vsrl_token *tok) { assigned_token_ = tok; }

  //: set the x and y position
  void set_position(double x, double y) { x_=x; y_=y; }

  //: get the x and y position
  void get_position(double &x, double &y) { x= x_; y= y_; }

  double get_x() { return x_; }
  double get_y() { return y_; }
};

#endif // vsrl_token_h
