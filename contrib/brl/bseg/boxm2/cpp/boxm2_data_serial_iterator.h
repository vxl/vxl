#ifndef boxm2_data_serial_iterator_h_
#define boxm2_data_serial_iterator_h_
//:
// \file



#include <vul/vul_timer.h>

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>

#include <boct/boct_bit_tree2.h>

#include <vcl_algorithm.h>

template<class F>
void boxm2_data_serial_iterator(boxm2_block * blk_sptr,
                                 F functor)
{
    for(unsigned i=0;i<blk_sptr->num_buffers()*65536;i++)
        functor.process_cell(i);
}



#endif // boxm2_data_serial_iterator_h_
