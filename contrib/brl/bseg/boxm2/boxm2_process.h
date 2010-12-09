#ifndef boxm2_process_h
#define boxm2_process_h

#include <vcl_vector.h>
#include <brdb/brdb_value_sptr.h>
#include <vcl_iostream.h>

class boxm2_process
{
  public: 
    virtual bool init() = 0;
    virtual bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output) = 0; 
};

#endif
