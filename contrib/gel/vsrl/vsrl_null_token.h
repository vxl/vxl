#ifndef vsrl_null_token_h
#define vsrl_null_token_h
//:
// \file
// This is a token that is used to represent a null assignment

#include <vsrl/vsrl_token.h>

class vsrl_null_token : public vsrl_token
{
  double cost_;

 public:

  // constructor
  vsrl_null_token();

  // destructor
  ~vsrl_null_token();

  // does this token represent a null assignment?
  virtual bool null_token() {return true;}

  // what is the direct cost of assigning this token to tok
  virtual double cost(vsrl_token *tok);

  // set the cost for this assignment

  void set_cost(double cost);

  // get the cost for this assignment

  double get_cost();
};

#endif // vsrl_null_token_h
