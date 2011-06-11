#include <testlib/testlib_test.h>

#include "open_cl_test_data.h"
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpl/vpl.h>

// reading and writing a tree in an array form
static void test_tree_read_write()
{
    boct_tree<short,float> *tree=open_cl_test_data::tree<float>();

    vcl_vector<vnl_vector_fixed<int, 4> > cell_array;
    vcl_vector<vnl_vector_fixed<float, 16> > data_array;
    vnl_vector_fixed<int, 4> root_cell(0);
    root_cell[0]=-1; // no parent
    root_cell[1]=-1; // no children at the moment
    root_cell[2]=-1; // no data at the moment
    cell_array.push_back(root_cell);

    boxm_ocl_convert<float>::copy_to_arrays(tree->root(),cell_array,data_array,0);

    vcl_size_t cells_size    =cell_array.size();
    vcl_size_t cell_data_size=data_array.size();

    int *cells = NULL;
    float * cell_data = NULL;

    cells=(cl_int *)boxm_ocl_utils::alloc_aligned(cells_size,sizeof(cl_int4),16);
    cell_data=(cl_float *)boxm_ocl_utils::alloc_aligned(cell_data_size,sizeof(cl_float8),16);

    if (cells== NULL||cell_data == NULL )
    {
        vcl_cout << "Failed to allocate host memory. (tree input)\n";
        return ;
    }

    // copy the data from vectors to arrays
    for (unsigned i = 0, j = 0; i<cell_array.size()*4; i+=4, j++)
        for (unsigned k = 0; k<4; ++k)
            cells[i+k]=cell_array[j][k];

    // appearance model is 8 and alpha is 1
    for (unsigned i = 0, j = 0; i<data_array.size()*16; i+=16, j++)
        for (unsigned k = 0; k<16; ++k)
            cell_data[i+k]=data_array[j][k];

    vcl_string  treefile="treefile.bin";
    vcl_string  treedatafile="datafile.bin";

    boxm_ocl_utils::writetree(treefile,cells,cells_size);
    boxm_ocl_utils::writetreedata(treedatafile,cell_data,cell_data_size);

    unsigned int treesize=0;
    unsigned int treedatasize=0;

    cl_int * rcells  =boxm_ocl_utils::readtree(treefile,treesize);
    cl_float *rcell_data =boxm_ocl_utils::readtreedata(treedatafile,treedatasize);

    bool flag=true;
    for (unsigned i=0;i<treesize;i++)
        if (rcells[i]!=cells[i])
            flag=false;

    for (unsigned i=0;i<treedatasize;i++)
        if (rcell_data[i]!=cell_data[i])
            flag=false;

    vpl_unlink(treefile.c_str());
    vpl_unlink(treedatafile.c_str());

    TEST("Read Write of tree and data arrays",flag,true);
}

static void test_file_io()
{
    test_tree_read_write();
}

TESTMAIN(test_file_io);
