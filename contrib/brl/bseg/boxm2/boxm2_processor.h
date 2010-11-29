#ifndef boxm2_processor_h
#define boxm2_processor_h


#include <boxm2/boxm2_process.h>
#include <brdb/brdb_value_sptr.h>
#include <vcl_vector.h>
//: boxm2_processor: base class implemenation for cpp and opencl processors

class boxm2_processor
{

public:
    boxm2_processor(){}
    ~boxm2_processor(){}

    virtual bool  init()=0;
    virtual bool  run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output)=0;
    virtual bool  finish()=0;

};

#endif