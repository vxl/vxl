 #ifndef EIGEN_MATRIX_PRODUCT_MMA_BFLOAT16_ALTIVEC_H
 #define EIGEN_MATRIX_PRODUCT_MMA_BFLOAT16_ALTIVEC_H

namespace Eigen {

namespace internal {

EIGEN_STRONG_INLINE void pgerMMAbfloat16(__vector_quad* acc, const Packet8bf& a, const Packet8bf& b, int maskX, int maskY)
{
  switch(maskX){
    case 15:
      switch(maskY){
        case 0b1111:          
          __builtin_mma_xvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val));
          break;
        case 0b0011:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b1111, 0b11, 0b11);
          break;
        case 0b0001:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b1111, 0b1, 0b11);
          break;
        case 0b0111:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b1111, 0b111, 0b11);
          break;
      }
      break;
    case 3:
      switch(maskY){
        case 0b1111:
          __builtin_mma_xvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val));
          break;
        case 0b0011:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b11, 0b11, 0b11);
          break;
        case 0b0001:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b11, 0b1, 0b11);
          break;
        case 0b0111:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b11, 0b111, 0b11);
          break;
      }
      break;
    case 1:
      switch(maskY){
        case 0b1111:
          __builtin_mma_xvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val));
          break;
        case 0b0011:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b1, 0b11, 0b11);
          break;
        case 0b0001:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b1, 0b1, 0b11);
          break;
        case 0b0111:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b1, 0b111, 0b11);
          break;
      }
      break;
    case 0b0111:
      switch(maskY){
        case 0b1111:
          __builtin_mma_xvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val));
          break;
        case 0b0011:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b111, 0b11, 0b11);
          break;
        case 0b0001:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b111, 0b1, 0b11);
          break;
        case 0b0111:
          __builtin_mma_pmxvbf16ger2pp(acc, reinterpret_cast<Packet16uc>(a.m_val), reinterpret_cast<Packet16uc>(b.m_val), 0b111, 0b111, 0b11);
          break;
      }
      break;
  }
}

EIGEN_STRONG_INLINE void scaleAndStore(float* result, float* acc, Packet4f pAlpha)
{
  Packet4f result_block = ploadu<Packet4f>(result);
  Packet4f packet_pmadd = pmadd(pload<Packet4f>(acc), pAlpha, result_block);
  pstoreu(result, packet_pmadd);
}

template<int num_packets, bool zero>
EIGEN_STRONG_INLINE Packet8bf loadLhsBfloat16(const bfloat16* indexA)
{
  Packet8bf lhs1 = ploadu<Packet8bf>(indexA);
  Packet8bf lhs2;
  const int packet_size = 8; //We fit 8 bfloat16 on a 128 register
  if(zero){
    lhs2 = pset1<Packet8bf>(Eigen::bfloat16(0));
  }
  else lhs2 = ploadu<Packet8bf>(indexA + num_packets*packet_size);
  return vec_mergeh(lhs1.m_val, lhs2.m_val);
}

template<bool zero>
EIGEN_STRONG_INLINE Packet8bf loadLhsBfloat16ExtraRows(const bfloat16* indexA, Index strideA, Index row, int extra_rows)
{
  EIGEN_ALIGN16 bfloat16 lhs_array[8] = {Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0)};
  int count = 0;
  const bfloat16* idxA = indexA + row*strideA;
  for(int row_count = 0; row_count < extra_rows; row_count++){
    lhs_array[count++] = *idxA;
    if(!zero) lhs_array[count] = *(idxA+1);
    count++;
    idxA += strideA;
  }
  return pload<Packet8bf>(lhs_array);
}

template<bool zero>
EIGEN_STRONG_INLINE Packet8bf loadRhsBfloat16(const bfloat16* baseB, Index strideB, int i, int k)
{
  const bfloat16* indexB = baseB + strideB*4*i + (k*4);
  Packet8bf rhs1 = ploadu<Packet8bf>(indexB);
  if(zero){
    Packet8bf rhs2 = pset1<Packet8bf>(Eigen::bfloat16(0));
    return vec_mergeh(rhs1.m_val, rhs2.m_val);
  }
  //r = vec_perm (a, b, c)
  //Let v be the concatenation of a and b.
  //Each byte of r selected by using the least-significant 5 bits of the corresponding byte of c as an index into v
  //We need this elements from rhs: 0, 4, 1, 5, 2, 6, 3, 7
  Packet16uc c = {0x0u, 0x1u, 0x8u, 0x9u, 0x2u, 0x3u, 0xAu, 0xB, 0x4, 0x5, 0xCu, 0xDu, 0x6u, 0x7u, 0xEu, 0xFu};
  return vec_perm(rhs1.m_val, rhs1.m_val, c);
}

template<bool zero>
EIGEN_STRONG_INLINE Packet8bf loadRhsBfloat16ExtraCols(const bfloat16* blockB, Index strideB, Index offsetB, Index col, int i, int k, int extra_cols)
{
  EIGEN_ALIGN16 bfloat16 rhs_vector[8] = {Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0), Eigen::bfloat16(0)};
  const bfloat16* indexB = blockB + ((col+4*i)*strideB)+k+offsetB;
  for(int c = 0; c < extra_cols; c++){
    rhs_vector[2*c] = *indexB;
    if(!zero) rhs_vector[2*c+1] = *(indexB+1);
    indexB += strideB;
  }
  return pload<Packet8bf>(rhs_vector);
}

template<int num_acc, int num_packets, bool zero, bool rhs_extra_cols, bool lhs_extra_rows>
EIGEN_STRONG_INLINE void KLoop
(
  const bfloat16* indexA,
  const bfloat16* indexB,
  __vector_quad (&quad_acc)[num_acc],
  Index strideA,
  Index strideB,
  Index offsetB,
  Index k,
  Index row,
  Index col,
  int extra_rows,
  int extra_cols,
  int mask_rows = 0xF,
  int mask_cols = 0xF
)
{
  Packet8bf lhs;
  Packet8bf rhs[num_acc];
  if(lhs_extra_rows) lhs = loadLhsBfloat16ExtraRows<zero>(indexA+k, strideA, row, extra_rows);
  else lhs = loadLhsBfloat16<num_packets, zero>(indexA + k*num_packets*8); //a packet of bfloat16 has 8 elements
  for(int i = 0; i < num_acc; i++){
    if(!rhs_extra_cols)
      rhs[i] = loadRhsBfloat16<zero>(indexB, strideB, i, k);
    else{
      rhs[i] = loadRhsBfloat16ExtraCols<zero>(indexB, strideB, offsetB, col, i, k, extra_cols);
    }
    pgerMMAbfloat16(&(quad_acc[i]), rhs[i], lhs, mask_cols, mask_rows);
  }
}

template<const int num_acc, const int standard_block_size, const int num_packets, bool rhsExtraCols = false, bool lhsExtraRows = false>
void colLoopBody(Index* p_col, Index row, Index depth, Index cols, Index rows, int offset_row, int block_index, Packet4f pAlpha, const bfloat16* indexA, Index strideA, const bfloat16* blockB, Index strideB, Index offsetB, float* result, int extra_cols = 0, int extra_rows = 0, int mask_cols = 0xF, int mask_rows = 0xF)
{
  int col = *p_col;
  int count;
  int max, step, bound;
  const bfloat16* indexB;

  if(num_acc == 1) bound = 0;
  else bound = 1;

  if(rhsExtraCols){
    count = 0;
    max = 1;
    step = 1;
    indexB = blockB;
  }
  else{
    count = col;
    step = num_acc * 4; //each accumulator has 4 elements
    max = cols/step;
    indexB = blockB + 4*offsetB + strideB*col;
  }

  while(count/step + bound < max){
    Index k = 0;
    EIGEN_ALIGN32 float acc[num_acc][4][4];
    __vector_quad quad_acc[num_acc];
 
    for(int i = 0; i < num_acc; i++)
      __builtin_mma_xxsetaccz(&(quad_acc[i]));

    if(depth%2 != 0){
      KLoop<num_acc, num_packets, true, rhsExtraCols, lhsExtraRows>(indexA, indexB, quad_acc, strideA, strideB, offsetB, k, row, col, extra_rows, extra_cols, mask_rows, mask_cols);
      k = 1;
    }
    for(; k/2 < depth/2; k += 2){ 
      KLoop<num_acc, num_packets, false, rhsExtraCols, lhsExtraRows>(indexA, indexB, quad_acc, strideA, strideB, offsetB, k, row, col, extra_rows, extra_cols, mask_rows, mask_cols);
    }
    for(int i = 0; i < num_acc; i++){
      __builtin_mma_disassemble_acc((void*)acc[i], &(quad_acc[i]));
      if(lhsExtraRows){
        for(int x = 0; x < extra_cols; x++){
          for(int y = 0; y < extra_rows; y++){
            result[((col+i*4)+x)*rows + row + y] += acc[i][x][y]*(pAlpha[0]);
          }
        }
      }
      else{
        if(rhsExtraCols){
          for(int x = 0; x < cols-col; x++){
            scaleAndStore(result + ((col+i*4)+x)*rows + row + offset_row,acc[i][x], pAlpha);
          }
        }
        else{
          for(int x = 0; x < 4; x++){
            scaleAndStore(result + ((col+i*4)+x)*rows + (block_index*16) + offset_row,acc[i][x], pAlpha);
          }
        }
      }
    }
    count += step;
    if(!rhsExtraCols) {
      indexB += strideB*step;
      col += step;
    }
  }
  *p_col = col;
}

template<typename Index, typename Packet, typename RhsPacket, typename DataMapper, const Index accRows, const Index accCols>
void gemmMMAbfloat16(const DataMapper& res, const bfloat16* blockA, const bfloat16* blockB, Index rows, Index depth, Index cols, bfloat16 alpha, Index strideA, Index strideB, Index offsetA, Index offsetB)
{

  if(rows == 0 || cols == 0 || depth == 0) return;
  const Packet4f pAlpha = pset1<Packet4f>(Eigen::bfloat16_impl::bfloat16_to_float(alpha));
  ei_declare_aligned_stack_constructed_variable(float, result, cols*rows, 0);

  for(int j = 0; j < cols; j++){
    for(int i = 0; i < rows; i++){
      result[j*rows + i] = res(i,j);
    }
  }

  Index row = 0;
  Index col = 0;

  if( strideA == -1 ) strideA = depth;
  if( strideB == -1 ) strideB = depth;
  //Packing is done in blocks.
  //There's 3 possible sizes of blocks
  //Blocks of 8 columns with 16 elements (8x16) as col major
  //Blocks of 8 columns with 8 elements (8x8) as col major. This happens when there's 16 > rows > 8 
  //Blocks of 8 columns with <8 elements as row major. This happens when there's less than 8 remaining rows

  //Loop for LHS standard block (8x16)
  int standard_block_size = 16;
  const int standard_blocks_quantity = rows/standard_block_size; //Number of standard blocks
  int bigSuffix = (2*8) * (strideA-offsetA-depth);
  const bfloat16* indexA = blockA;
  int block_index;
  for(block_index = 0; block_index < standard_blocks_quantity; block_index++){
    indexA += 2*8*offsetA;
    for(int offset_row = 0; offset_row < standard_block_size; offset_row += 4){ //This block size has 16 rows maximum
      col = 0;
      colLoopBody<5, 16, 2>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
      colLoopBody<4, 16, 2>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
      colLoopBody<3, 16, 2>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
      colLoopBody<2, 16, 2>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
      colLoopBody<1, 16, 2>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
      if(cols > col){
        int extra_cols= cols-col;
        int shift = (4-extra_cols>= 0) ? 4-extra_cols: 0;
        int mask_cols= 0xF >> shift;
        //Remember: It doesnt make sense use multiple acc to extra_cols as we are unrolling col loop
        colLoopBody<1, 16, 2, true>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result, extra_cols, 4, mask_cols, 0xF);
      }
    }
    row += 16;
    indexA += bigSuffix + 2*8*depth;
  }
  //LHS (8x8) block
  if(rows - standard_blocks_quantity*16 >= 8){
    indexA += 1*8*offsetA + 2*8*offsetA;
    for(int offset_row = 0; offset_row < 8; offset_row += 4){
      col = 0;
      colLoopBody<5, 8, 1>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
      colLoopBody<4, 8, 1>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
      colLoopBody<3, 8, 1>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
      colLoopBody<2, 8, 1>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
      colLoopBody<1, 8, 1>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result);
    }
    if(cols > col){
      int extra_cols= cols-col;
      int shift = (4-extra_cols>= 0) ? 4-extra_cols: 0;
      int mask_cols= 0xF >> shift;

      for(int offset_row = 0; offset_row < 8; offset_row += 4){
        colLoopBody<1, 8, 1, true>(&col, row, depth, cols, rows, offset_row, block_index, pAlpha, indexA+offset_row, strideA, blockB, strideB, offsetB, result, extra_cols, 4, mask_cols, 0xF);
      }
    } //end extra cols
    row += 8;
  }
  //extra rows
  while(row < rows){
    int extra_rows = rows-row;
    int shift = (4-extra_rows >= 0) ? 4-extra_rows : 0;
    int mask_rows = 0xF >> shift;
    int extra_rows_or_four = (extra_rows <= 4) ? extra_rows : 4;

    //This index is the beginning of remaining block. 
    //This last block for LHS is organized as RowMajor
    col = 0;
    colLoopBody<5, 8, 1, false, true>(&col, row, depth, cols, rows, 0, block_index, pAlpha, blockA, strideA, blockB, strideB, offsetB, result, 4, extra_rows_or_four, 0xF, mask_rows);
    colLoopBody<4, 8, 1, false, true>(&col, row, depth, cols, rows, 0, block_index, pAlpha, blockA, strideA, blockB, strideB, offsetB, result, 4, extra_rows_or_four, 0xF, mask_rows);
    colLoopBody<3, 8, 1, false, true>(&col, row, depth, cols, rows, 0, block_index, pAlpha, blockA, strideA, blockB, strideB, offsetB, result, 4, extra_rows_or_four, 0xF, mask_rows);
    colLoopBody<2, 8, 1, false, true>(&col, row, depth, cols, rows, 0, block_index, pAlpha, blockA, strideA, blockB, strideB, offsetB, result, 4, extra_rows_or_four, 0xF, mask_rows);
    colLoopBody<1, 8, 1, false, true>(&col, row, depth, cols, rows, 0, block_index, pAlpha, blockA, strideA, blockB, strideB, offsetB, result, 4, extra_rows_or_four, 0xF, mask_rows);
    if(cols > col){
      int extra_cols= cols-col;
      int shift = (4-extra_cols>= 0) ? 4-extra_cols: 0;
      int mask_cols= 0xF >> shift;
      int extra_cols_or_four = (extra_cols <= 4) ? extra_cols : 4;

      colLoopBody<1, 8, 1, true, true>(&col, row, depth, cols, rows, 0, block_index, pAlpha, blockA, strideA, blockB, strideB, offsetB, result, extra_cols_or_four, extra_rows_or_four, mask_cols, mask_rows);
    }
    row += extra_rows_or_four;
  }

  //Convert back to bfloat16
  for(col = 0; col/4 < cols/4; col += 4){
    int row;
    for(row = 0; row/8 < rows/8; row += 8){
      //get and save block
      PacketBlock<Packet8bf,4> block;
      for(int j = 0; j < 4; j++){
        Packet4f temp_even, temp_odd;
        EIGEN_ALIGN32 float even[4], odd[4];
        for(int i = 0; i < 4; i++){
          even[i] = result[(col + j)*rows + row + i*2];
          odd[i] = result[(col + j)*rows + row + i*2+1];
        }
        temp_even = pload<Packet4f>(even);
        temp_odd = pload<Packet4f>(odd);
        block.packet[j] = F32ToBf16(temp_even, temp_odd);
      }

      res.template storePacketBlock<Packet8bf,4>(row, col, block);
    }
    //extra rows
    while(row < rows){
      for(int col_off = 0; col_off < 4; col_off++){
        res(row, col+col_off) = Eigen::bfloat16(result[(col+col_off)*rows+row]);
      }
      row++;
    }

  }
  //extra cols
  while(col < cols){
    for(int r = 0; r < rows; r++){
      res(r, col) = Eigen::bfloat16(result[col*rows + r]);
    }
    col++;
  }
}


}
}
#endif //EIGEN_MATRIX_PRODUCT_MMA_BFLOAT16_ALTIVEC_H
