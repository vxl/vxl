__constant int sel0[4]={1,0,0,0};
__constant int sel1[4]={2,2,1,1};
__constant int sel2[4]={3,3,3,2};

float cofactor_ij_v1(__local float * mat4x4, unsigned int row, unsigned int col)
{
    // Let's first define the 3x3 matrix:
    unsigned int col0 = sel0[col];
    unsigned int col1 = sel1[col];
    unsigned int col2 = sel2[col];
    unsigned int row0 = sel0[row];
    unsigned int row1 = sel1[row];
    unsigned int row2 = sel2[row];
    // Computer the det of the 3x3 matrix:
    //
    //   [ a b c ]
    //   [ d e f ] = aei - ahf + dhc - dbi + gbf - gec = (aei + dhc + gbf) - (ahf + dbi + gec)
    //   [ g h i ]
    //
    float pos_part1 = mat4x4[col0+row0*4] * mat4x4[col1+row1*4] * mat4x4[col2+row2*4]; // aei
    float pos_part2 = mat4x4[col0+row1*4] * mat4x4[col1+row2*4] * mat4x4[col2+row0*4]; // dhc
    float pos_part3 = mat4x4[col0+row2*4] * mat4x4[col1+row0*4] * mat4x4[col2+row1*4]; // gbf

    float neg_part1 = mat4x4[col0+row0*4] * mat4x4[col1+row2*4] * mat4x4[col2+row1*4]; // ahf
    float neg_part2 = mat4x4[col0+row1*4] * mat4x4[col1+row0*4] * mat4x4[col2+row2*4]; // dbi
    float neg_part3 = mat4x4[col0+row2*4] * mat4x4[col1+row1*4] * mat4x4[col2+row0*4]; // gec

    float pos_part  = pos_part1 + pos_part2 + pos_part3;
    float neg_part  = neg_part1 + neg_part2 + neg_part3;
    float det3x3    = pos_part - neg_part;

    if ( (col + row) & 1 )
        return -det3x3;
    else
        return det3x3;
}

float determinant(__local float * mat4x4, __local float * cofactor4x4)
{
    float det = 0.0f;
    for ( int col = 0; col < 4; col++ )
        det += mat4x4[col+0*4] * cofactor4x4[col+0*4];
    return det;
}

// Assuming 4x4 workgroup
bool onl_inverse_4x4(__local float * mat4x4,__local float * cofactor4x4, __local float * invmat4x4)
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llidr = (uchar)(get_local_id(0));
    uchar llidc = (uchar)(get_local_id(1));
    uchar llid = (uchar)(llidr + 4*llidc);

    if (llidr < 4 && llidc < 4)
        cofactor4x4[llid] =  cofactor_ij_v1(mat4x4,llidr,llidc);
    barrier(CLK_LOCAL_MEM_FENCE);
    if (llidr < 4 && llidc < 4)
    {
        float det = determinant(mat4x4,cofactor4x4);
        uchar llid_transpose = (uchar)(llidc + 4*llidr);
        // put it in the inverse of the matrix.
        invmat4x4[llid_transpose] = cofactor4x4[llid] / det ;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}

// Assuming 4x4 workgroup
bool onl_outerproduct_4x4(__local float * v1,__local float * v2, __local float * out4x4)
{
    uchar llidr = (uchar)(get_local_id(0));
    uchar llidc = (uchar)(get_local_id(1));
    if (llidr < 4 && llidc < 4)
    {
        uchar llid = (uchar)(llidr + 4*llidc);
        out4x4[llid] = v1[llidr]*v2[llidc] ;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}
