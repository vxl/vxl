#ifndef boxm2_data_serial_iterator_h_
#define boxm2_data_serial_iterator_h_
//:
// \file

#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>

template <class F>
void boxm2_data_serial_iterator(boxm2_block * blk_sptr,
                                F functor)
{
    for (int i=0; i<blk_sptr->num_buffers()*65536; ++i)
        functor.process_cell(i);
}


#endif // boxm2_data_serial_iterator_h_
