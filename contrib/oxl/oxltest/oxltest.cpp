// oxltest.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"

#include <mvl/HomgOperator3D.h>
#include <vcl_iostream.h>

int main()
{
  HomgOperator3D::sort_points(0,0);

  vcl_cout << "Hello World!\n";
  return 0;
}

