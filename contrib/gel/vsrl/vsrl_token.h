#ifndef vsrl_token_h
#define vsrl_token_h
//:
// \file
// In a dynamic progamming problem we are given the task of
// assigning one set of tokens to another set of tokens.
// This class represents the abstract token.

#include <vcl_cmath.h>
#include <vcl_cstddef.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>

class vsrl_token
{
 public:

  // constructor
  vsrl_token();

  // destructor
  virtual ~vsrl_token();

  // does this token represent a null assignment?
  virtual bool null_token() {return false;}

  // is this token an intensity token ?

  virtual bool intensity_token() {return false;}

  // what is the direct cost of assigning this token to tok
  virtual double cost(vsrl_token *tok)=0;

  // get the index of this token
  int get_index();

  // set the id of this token
  void set_index(int index);

  // get the assigned token
  vsrl_token* get_assigned_token();

  // set the assigned token
  void set_assigned_token(vsrl_token *tok);

  // set the x and y position

  void set_position(double x, double y);

  // get the x and y position

  void get_position(double &x, double &y);

  double get_x() {return _x;}
  double get_y() {return _y;}

 private:
  int _index;
  vsrl_token* _assigned_token;

  double _x; // the x position
  double _y; // the y position
};

#endif // vsrl_token_h
