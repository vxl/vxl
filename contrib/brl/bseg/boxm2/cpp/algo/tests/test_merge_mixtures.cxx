//:
// \file
// \author Vishal Jain
// \date 26-Jan-2011

#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <brdb/brdb_value.h>

#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>

void test_merge_mixtures()
{
    vnl_vector_fixed<unsigned char,8> mog3_1;
    mog3_1[0]=42; mog3_1[3]=126; mog3_1[6]=205;
    mog3_1[1]=23; mog3_1[4]=23 ; mog3_1[7]=23;
    mog3_1[2]=82; mog3_1[5]=82 ;

    vnl_vector_fixed<unsigned char,8> mog3_2;
    mog3_2.fill(0);
    mog3_2[0]=42 ; mog3_2[3]=126;
    mog3_2[1]=23 ; mog3_2[4]=23;
    mog3_2[2]=173; mog3_2[5]=83;

    vnl_vector_fixed<unsigned char,8> mog3_f;
    boxm2_mog3_grey_processor::merge_mixtures(mog3_1,0.5,mog3_2,0.5,mog3_f);

    bool flag=true;
    vnl_vector_fixed<unsigned char,8> mog3_gt;
    mog3_gt.fill(0);
    mog3_gt[0]=147; mog3_gt[3]=42; mog3_gt[6]=83;
    mog3_gt[1]=27 ; mog3_gt[4]=23; mog3_gt[7]=36;
    mog3_gt[2]=67 ; mog3_gt[5]=50;
    for (unsigned i=0;i<8;i++)
        if (mog3_f[i]!=mog3_gt[i])
            flag=false;

    TEST_EQUAL("Test for Merging to MoGs ",flag,true);
}


TESTMAIN(test_merge_mixtures);
