#ifndef mbl_matxvec_h
#define mbl_matxvec_h
#ifdef __GNUC__
#pragma interface
#endif


//:
// \file
// \author Tim Cootes
// \date 3-Oct-97
// \brief Various specialised versions of simple linear algebra operators.
// Modifications
// \verbatim
// TFC		Revised version 3-Oct-97
// TFC		Added TC_MatXVec2
// NPC    Added NC_VecXMat 
// IMS    started conversion to VXL 18 April 2001
// \endverbatim




template class vnl_matrix<double>;
template class vnl_vector<double>;



/*
#include <vcl_vector.h>
	//: Compute R = V*M
	//  R is resized to the number of rows of V * cols of M
void NC_VecXMat(const vnl_vector<double>& V,const vnl_matrix<double>& M,
	     		vnl_matrix<double>& R);

	//: Compute R = M*V
	//  Only use first V.size() columns of M
	//  R is resized to the number of rows of M
void TC_MatXVec(const vnl_matrix<double>& M,
	     		const vnl_vector<double>& V,
	     		vnl_vector<double>& R);
	     		
	//: Compute R = M*V
	//  Only use first V.size() columns of M
	//  Only use first R.size() rows of M
	//  R is not resized - its size determines how many rows to use
void TC_MatXVec2(const vnl_matrix<double>& M,
	     		const vnl_vector<double>& V,
	     		vnl_vector<double>& R);
	     		
void TC_MatXVec(const vnl_matrix<double>& M,
	    		const vnl_vector<double>& V,
	     		vnl_vector<double>& R,
				const vcl_vector<int>& index);
	
	//: Fast Compute R = V' * M = ( M.transpose() * V ).transpose()
	// Only use the first R.size() columns of M
	// R is not resized - its size determines how many columns to use
	// Ensure that V.elems() == M.rows().
void TC_VecXMat(const vnl_vector<double>& V,
	     		const vnl_matrix<double>& M,
	     		vnl_vector<double>& R);

	//: Fast Compute R += V' * M = ( M.transpose() * V ).transpose()
	// Only use the first R.size() columns of M
	// R is not resized - its size determines how many columns to use
	// Ensure that V.elems() == M.rows().
void TC_AddVecXMat(const vnl_vector<double>& V,
	     		const vnl_matrix<double>& M,
	     		vnl_vector<double>& R);

void TC_ProductABt(vnl_matrix<double>& ABt, const vnl_matrix<double>& A,
					const vnl_matrix<double>& B);
void TC_ProductAtB(vnl_matrix<double>& AtB, const vnl_matrix<double>& A,
					const vnl_matrix<double>& B);
void TC_Product(vnl_matrix<double>& AB, const vnl_matrix<double>& A,
					const vnl_matrix<double>& B);
*/
//: ADB = A * D * B where D is diagonal with elements d
void mbl_matxvec_product_adb(vnl_matrix<double>& ADB, 
				                     const vnl_matrix<double>& A,
				                     const vnl_vector<double>& d,
				                     const vnl_matrix<double>& B);
/*
//: Computes MD where D is diagonal with elememts d(i)
void TC_ProductMD(vnl_matrix<double>& MD, const vnl_matrix<double>& M, const vnl_vector<double>& d);

//: Computes DM where D is diagonal with elememts d(i)
void TC_ProductDM(vnl_matrix<double>& DM, const vnl_matrix<double>& M, const vnl_vector<double>& d);
*/

#endif // mbl_matxvec_h


