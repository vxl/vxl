// This is gel/vsrl/vsrl_token.cxx
#include "vsrl_token.h"
//:
// \file
// In a dynamic progamming problem we are given the task of
// assigning one set of tokens to another set of tokens.
// This class represents the token.

vsrl_token::vsrl_token()
{
  index_ = 0-1;
  assigned_token_ = 0;
  x_=0;
  y_=0;
}

vsrl_token::~vsrl_token()
{
}

//: get the index of this token
int vsrl_token::get_index()
{
  return index_;
}

//: set the index of this token
void vsrl_token::set_index(int index)
{
  index_ = index;
}

//: get the assigned token
vsrl_token* vsrl_token::get_assigned_token()
{
  return assigned_token_;
}

//: set the assigned token
void vsrl_token::set_assigned_token(vsrl_token *tok)
{
  assigned_token_ = tok;
}

//: set the position
void vsrl_token::set_position(double x, double y)
{
  x_=x;
  y_=y;
}

//: get the position
void vsrl_token::get_position(double &x, double &y)
{
  x= x_;
  y= y_;
}

//: what is the direct cost of assigning this token to tok
double vsrl_token::cost(vsrl_token * /*tok*/)
{
  return 0;
}

//: what is the incremental cost of going from from this to tok2 given the assignment going from tok_p to tok_2p
double vsrl_token::incremental_cost(vsrl_token * /*tok2*/, vsrl_token * /*tok_p*/, vsrl_token * /*tok_2p*/)
{
  return 0;
}

