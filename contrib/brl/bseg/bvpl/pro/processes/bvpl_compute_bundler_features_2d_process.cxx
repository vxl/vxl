//this is /contrib/bm/bseg/bvpl/pro/bvpl_compute_bundler_features_2d_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for computing 2d features from 3d bundler points.
//
// \author Brandon A. Mayer
// \date Sept  8, 2011
// \verbatim
//  Modifications
//
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <bvpl/util/bvpl_bundler_features_2d.h>

#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>

#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>

#include<vgl/vgl_point_2d.h>
#include<vgl/vgl_point_3d.h>

#include<vnl/vnl_matrix_fixed.h>
#include<vnl/vnl_vector_fixed.h>
#include<vnl/vnl_vector.h>

#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_convert.h>

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vul/vul_arg.h>
#include <vul/vul_file.h>

struct kernel
{
	vgl_point_2d<unsigned> min_pt;
	vgl_point_2d<unsigned> max_pt;
	vnl_vector<double> w;
	vcl_vector<vgl_point_2d<unsigned> > locs;
};

namespace bvpl_compute_bundler_features_2d_globals
{
	const unsigned n_inputs_ = 4;
	const unsigned n_outputs_ = 1;
}//end bvpl_compute_bundler_features_2d_process_globals

bool bvpl_compute_bundler_features_2d_process_cons( bprb_func_process& pro )
{
	using namespace bvpl_compute_bundler_features_2d_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "vcl_string";//bundler output file
	input_types_[i++] = "vcl_string";//original image glob
	input_types_[i++] = "vcl_string";//bad camera file
	input_types_[i++] = "vcl_string";//kernel directory
	
	output_types_[0] = "bvpl_bundler_features_2d_sptr";

	if(!pro.set_input_types(input_types_))
	{
		vcl_cerr << "----ERROR---- bvpl_compute_bundler_features_2d_process_cons\n"
			     << "\tCOULD NOT SET INPUT TYPES.\n"
				 << __FILE__ << '\n'
				 << __LINE__ << '\n' << vcl_flush;
		return false;
	}

	if(!pro.set_output_types(output_types_))
	{
		vcl_cerr << "----ERROR---- bvpl_compute_bundler_features_2d_process_cons\n"
				 << "\tCOULD NOT SET OUTPUT TYPES.\n"
				 << __FILE__ << '\n'
				 << __LINE__ << '\n' << vcl_flush;
		return false;
	}

	return true;
}//end bvpl_compute_bundler_features_2d_process_cons

bool bvpl_compute_bundler_features_2d_process( bprb_func_process& pro )
{
	using namespace  bvpl_compute_bundler_features_2d_globals;

	if( pro.n_inputs() != n_inputs_ )
	{
		vcl_cerr << pro.name()
			     << " bvpl_compute_bundler_features_2d_process: NUMBER OF INPUTS SHOULD BE: "
				 << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	vcl_string bundlerfile		= pro.get_input<vcl_string>(i++);
	vcl_string img_glob			= pro.get_input<vcl_string>(i++);
	vcl_string bad_cam_file		= pro.get_input<vcl_string>(i++);
	vcl_string kernel_dir		= pro.get_input<vcl_string>(i++);

	//------ PARSE BAD CAMERAS --------
	vcl_ifstream bcfile( bad_cam_file );

	if( !bcfile )
	{
		vcl_cerr << "---ERROR---- bvpl_compute_bundler_features_2d_process\n"
				 << "\tERROR OPENING BAD CAMERA FILE.\n"
				 << __FILE__ <<'\n'
				 << __LINE__ << '\n' << vcl_flush;
		return false;
	}

	vcl_set<unsigned> bad_cams;

	while( !bcfile.eof() )
	{
		unsigned c;
		bcfile >> c;
		bad_cams.insert(c);
	}//end bad camera file iteration

	
	//------ READING KERNEL FILES--------

	if( !vul_file::is_directory(kernel_dir) )
	{
		vcl_cerr << "---ERROR---- bvpl_compute_bundler_features_2d_process\n"
			<< "\t KERNEL DIRECTORY NOT VALID.\n"
			<< __FILE__ << '\n'
			<< __LINE__ << '\n' << vcl_flush;
		return false;
	}

	vcl_vector<vcl_string> filenames;
	filenames.push_back("I0");
    filenames.push_back("Ix");
    filenames.push_back("Iy");
	filenames.push_back("Ixx");
	filenames.push_back("Iyy");
	filenames.push_back("Ixy");

	vcl_map<vcl_string, kernel > kernel_map;

	vcl_vector<vcl_string>::const_iterator
		k_itr, k_end = filenames.end();

	for( k_itr = filenames.begin();
			k_itr != k_end; ++k_itr )
	{
		vcl_ifstream kernel_file(kernel_dir + "/" + *k_itr + ".txt");

		if( !kernel_file.good() )
		{
			vcl_cerr << "---ERROR---- bvpl_compute_bundler_features_2d_process\n"
				 << "\t COULD NOT OPEN KERNEL: " << *k_itr << "\n"
				 << __FILE__ << '\n'
			     << __LINE__ << '\n' << vcl_flush;
		return false;
		}

		vgl_point_2d<unsigned> min_pt, max_pt;

		kernel_file >> min_pt;
		kernel_file >> max_pt;

		unsigned nx = max_pt.x() - min_pt.y() + 1;
		unsigned ny = max_pt.y() - min_pt.y() + 1;

		vnl_vector<double> w(nx*ny);

		kernel k;
		k.min_pt = min_pt;
		k.max_pt = max_pt;

		for(unsigned i = 0; !kernel_file.eof(); )
		{
			vgl_point_2d<unsigned> loc;
			kernel_file >> loc;
			k.locs.push_back(loc);
			double weight;
			kernel_file >> weight;
			w[i++] = weight;
		}//end kernel weight iteration

		k.w = w;

		kernel_map.insert(vcl_make_pair(*k_itr,k));
	}//end kernel file iteration

	//------ READING & PROCESSING BUNDLER FILE --------
	vcl_ifstream bfile(bundlerfile);

	if(!bfile)
	{
		vcl_cerr << "---ERROR---- bvpl_compute_bundler_features_2d_process\n"
				 << "\tERROR OPENING BUNDLER OUTPUT FILE.\n"
				 << __FILE__ <<'\n'
				 << __LINE__ << vcl_flush;
		return false;
	}

	vidl_image_list_istream video_stream(img_glob);
	
	if(!video_stream.is_open())
	{
		vcl_cerr << "---ERROR---- bvpl_compute_bundler_features_2d_process\n"
			     << "\tINVALID VIDEO STREAM.\n"
				 << __FILE__ << '\n'
				 << __LINE__ << vcl_flush;
		return false;
	}

	//get image size
	unsigned ni = video_stream.width();
	unsigned nj = video_stream.height();

	//central point of image
	vgl_point_2d<double> principal_point((double)ni/2,(double)nj/2);

	char buffer[1024];
	bfile.getline(buffer,1024); // read the header line

	if (bfile.eof())
	{
		vcl_cerr << "---ERROR---- bvpl_compute_bundler_features_2d_process\n"
				 << "\tMISSING BUNDLER FILE DATA.\n"
				 << __FILE__ << '\n'
				 << __LINE__ << '\n' << vcl_flush;
		return false;
	}

	unsigned num_cams=0, num_pts=0;
	bfile>>num_cams>>num_pts; // reading number of cameras and number of 3-d pts

	for( unsigned  i = 0; i < num_cams; ++i)
	{
		double f, k1, k2;
		vnl_matrix_fixed<double,3,3> R;
		vnl_vector_fixed<double,3> T;

		//reading camera matrices
		//don't need them but need to move file pointer
		bfile>>f>>k1>>k2;
		bfile>>R>>T;
	}//end num_cams iteration

	vcl_vector<bwm_video_corr_sptr> corrs;

	bvpl_bundler_features_2d_sptr bundler_features_sptr =
		new bvpl_bundler_features_2d();

	for( unsigned  i = 0; i < num_pts; ++i )
	{
		double x,y,z;
		
		//read the 3d point
		bfile >> x >> y >> z;

		vgl_point_3d<double> bundler_pt(x,y,z);	

		//read the color value
		//don't need it just move the file ptr
		unsigned r,g,b;
		bfile >> r >> g >> b;

		//read the number of views this 3d pt
		//is associated with

		unsigned num_views;
		bfile >> num_views;

		vcl_map<unsigned, vnl_vector<double> > view_feature_map;

		for( unsigned j = 0; j < num_views; ++j )
		{
			unsigned view_number, key_number;
			double img_x, img_y;

			bfile >> view_number
				  >> key_number
				  >> img_x
				  >> img_y;

			img_x = img_x + principal_point.x();
			img_y = nj - (img_y + principal_point.y());

			if( !bad_cams.count(view_number) )
			{
				if( !video_stream.seek_frame(view_number) )
				{
					vcl_cerr << "---ERROR---- bvpl_compute_bundler_features_2d_process\n"
						<< "\tCOULD NOT FIND FRAME: " << view_number << "\n"
						<< __FILE__ << '\n'
						<< __LINE__ << '\n' << vcl_flush;
					return false;
				}

				vil_image_view<vxl_byte> curr_img;
				vidl_convert_to_view(*video_stream.current_frame(),
										curr_img);

				if( curr_img.nplanes() != 1 ||
					curr_img.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE )
				{
					vil_image_view<vxl_byte> grey_img;
					vil_convert_planes_to_grey(curr_img, grey_img);
					curr_img = grey_img;
				}

				//iterate through the kernels and buld the feature
				//vector for this 3d point and view

				vnl_vector<double> feature_vector(filenames.size());

				for( k_itr = filenames.begin(); 
						k_itr != k_end; ++k_itr )
				{
					kernel k = kernel_map[*k_itr];

					unsigned nx = k.max_pt.x() - k.min_pt.y() + 1;
					unsigned ny = k.max_pt.y() - k.min_pt.y() + 1;
					vnl_vector<double> neighborhood(nx*ny);

					//build this neighborhood for this kernel
					for( unsigned i = 0;
							i < k.locs.size(); ++i )
					{
						int x = img_x + k.locs[i].x();
						int y = img_y + k.locs[i].y();

						//check if pixel index is within bounds
						//else pad with zeros
						if( x > int(0) && y > int(0) )
							if( unsigned(x) < ni && unsigned(y) < nj )
								neighborhood[i] = curr_img(x,y);
						else
							neighborhood[i] = double(0.0);

						double f = dot_product(k.w,neighborhood);

						if(*k_itr == "I0")
							feature_vector[0] = f;
						else if(*k_itr == "Ix")
							feature_vector[1] = f;
						else if(*k_itr == "Iy")
							feature_vector[2] = f;
						else if(*k_itr == "Ixx")
							feature_vector[3] = f;
						else if(*k_itr == "Iyy")
							feature_vector[4] = f;
						else if(*k_itr == "Ixy")
							feature_vector[5] = f;

					}//end kernel loc iteration
				}//end filename iteration

				view_feature_map.insert(vcl_make_pair(view_number,feature_vector));

			}//end if !bad_cams.count(view_number)

		}//end num_views iteration

		bundler_features_sptr->pt_view_feature_map.insert(
			vcl_make_pair(bundler_pt, view_feature_map) );

	}//end num_pts iteration

	pro.set_output_val(0, bundler_features_sptr);

	return true;

}//end bvpl_compute_bundler_features_2d_process