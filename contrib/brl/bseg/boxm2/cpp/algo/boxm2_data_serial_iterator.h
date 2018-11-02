#ifndef boxm2_data_serial_iterator_h_
#define boxm2_data_serial_iterator_h_
//:
// \file

#include <iostream>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boct/boct_bit_tree.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class F>
void boxm2_data_serial_iterator(int data_buff_length,
                                F functor)
{
    for (int i=0; i<data_buff_length; ++i)
    {
        // counter
        if ( i%(data_buff_length/10) == 0 ) std::cout<<'.';
        functor.process_cell(i);
    }
}


template <class F>
void boxm2_data_leaves_serial_iterator(boxm2_block * blk,
                                       int data_buff_length,
                                       F functor)
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

    const boxm2_array_3d<uchar16>&  trees = blk->trees();

    int currIndex = 0;                                //curr tree being looked at
    boxm2_array_3d<uchar16>::const_iterator blk_iter;
    for (blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter, ++currIndex)
    {
        uchar16 tree  = (*blk_iter);
        boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), 4);

        for (int i=0; i<585; i++)
        {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = (i-1)>>3;           //Bit_index of parent bit
            bool validParent = curr_tree.bit_at(pi) || (i==0); // special case for root
            if (validParent )//&& curr_tree.bit_at(i)==0)
            {
                int depth = curr_tree.depth_at(i);
                double side_len = 1.0/ double(1<<depth);
                int index = curr_tree.get_data_index(i);
                functor.process_cell(index,curr_tree.is_leaf(i),float(side_len*blk->sub_block_dim().x()));
            }
        }
    }
}

#endif // boxm2_data_serial_iterator_h_
