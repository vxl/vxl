// This is brl/bpro/bpro_batch/bpro_batch.h
#ifndef bpro_batch_h_
#define bpro_batch_h_
//:
// \file
// \brief External function declarations
// \author J.L. Mundy
// \date February 5, 2008
//
//
// \verbatim
//  Modifications
// \endverbatim
#include "Python.h"

typedef struct
{
  PyObject_HEAD
  unsigned int id;
  char* type;
} dbvalue;

// METHOD_NUM defined as one more than the actual number of processes to include trailing NULL process
constexpr int METHOD_NUM = 40;
void register_basic_datatypes();
extern PyMethodDef batch_methods[];


#endif // bpro_batch_h_
