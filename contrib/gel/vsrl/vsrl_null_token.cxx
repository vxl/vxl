// This is gel/vsrl/vsrl_null_token.cxx
#include "vsrl_null_token.h"

vsrl_null_token::vsrl_null_token()
{
}

// destructor

vsrl_null_token::~vsrl_null_token()
{
}

double vsrl_null_token::cost(vsrl_token * /*tok*/)
{
  // this is a null assignment
  return cost_;
}

void vsrl_null_token::set_cost(double cost)
{
  // set the cost
  cost_=cost;
}

double vsrl_null_token::get_cost()
{
  // get the cost
  return cost_;
}
