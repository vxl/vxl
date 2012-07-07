//:
// \file
// \brief  
// \author Vishal Jain
// \date 06/08/2012
#include <boxm2/reg/ocl/boxm2_ocl_reg_depth_map_to_vol.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>
#include <boxm2/boxm2_util.h>
#include <bpgl/algo/bpgl_transform_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#include <vcl_algorithm.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vcl_iostream.h>
#include <vil/vil_save.h>

#include <bvrml/bvrml_write.h>
// executable to match a depth map to volume 

int main(int argc,  char** argv)
{
	//init vgui (should choose/determine toolkit)
	vul_arg<vcl_string> cam_file("-cam", "cam filename", "");
	vul_arg<vcl_string> depth_file("-depth", "depth map filename", "");
	vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
	vul_arg<vcl_string> vrml_file("-vrml", "vrml filename", "");
	vul_arg_parse(argc, argv);

	//create scene
	boxm2_scene_sptr scene = new boxm2_scene(scene_file());
	vpgl_camera_double_sptr cam = boxm2_util::camera_from_file(cam_file());

	//: create search space 
	vcl_vector< vpgl_perspective_camera<double> > cameras ;
	vnl_vector_fixed<double, 3> t;
	vgl_box_3d<double>  bbox = scene->bounding_box();
	vcl_ofstream os(vrml_file().c_str());
	os  << "#VRML V2.0 utf8\n"
		<< "Background {\n"
		<< "  skyColor [ 0 0 0 ]\n"
		<< "  groundColor [ 0 0 0 ]\n"
		<< "}\n";
	bvrml_write::write_vrml_wireframe_box(os,bbox);
	vgl_point_3d<double> cam_center;
	if( vpgl_perspective_camera<double> * pcam = dynamic_cast<vpgl_perspective_camera<double> *> (cam.ptr() ))
	{
		vrml_write(os, *pcam, (double)2.0);

		cam_center= pcam->camera_center();
		vcl_cout<<"Cam center "<<cam_center<<vcl_endl;
		vcl_cout<<"Principal Ray "<<pcam->principal_axis()<<vcl_endl;
		//vcl_cout<<pcam->camera_center();
		//for( double i = bbox.min_x() ; i < bbox.max_x(); i+=50)
		//{
		//	for( double j = bbox.min_y() ; j < bbox.max_y(); j+=50)
		//	{
		//		//for( double k = bbox.min_z() ; k < bbox.max_z(); k+=50)
		//		{
		//			//for (unsigned num = 0 ; num < 4; num ++)
		//			{
		//				vnl_quaternion<double> q(0,0,0);
		//				vgl_rotation_3d<double> R(q);
		//				t[0] = 0; 
		//				t[1] = 0; 
		//				t[2] = 0;
		//				vpgl_perspective_camera<double> tcam = 
		//					bpgl_transform_camera::transform_perspective_camera(*pcam, R, t, 1);

		//				t[0] = i; 
		//				t[1] = j; 
		//				t[2] = pcam->camera_center().z();
		//				tcam.set_camera_center(vgl_point_3d<double>(t[0],t[1],t[2]));
		//				vrml_write(os, tcam, (double)2.0);
		//				cameras.push_back(tcam);
		//			}
		//		}
		//	}
	}
	os.close();
	vil_image_view_base_sptr img = vil_load(depth_file().c_str());
	if(vil_image_view<float> * fimg  = dynamic_cast<vil_image_view<float> * >(img.ptr()))
	{
		vcl_vector<vcl_string> data_types;
		vcl_vector<vcl_string> identifiers;
		data_types.push_back("alpha");
		data_types.push_back("aux2");
		identifiers.push_back("");
		identifiers.push_back("vis");
		boxm2_stream_scene_cache cache( scene, data_types,identifiers);

		bocl_manager_child_sptr mgr =bocl_manager_child::instance();
		if (mgr->gpus_.size()==0)
			return false;

		bocl_device_sptr  device = mgr->gpus_[0];
		vnl_vector<double> x;
		x.set_size(6);
		x[0]= cam_center.x(); x[1] = cam_center.y() ; x[2] =cam_center.z(); 
		x[3]= 0.0; x[4] = 0.0 ; x[5] =0.0;
		boxm2_ocl_reg_depth_map_to_vol func(cam,fimg,cache,device,2);
		double mi_max = 0.0;
		vul_timer t;
		t.mark();
		vil_image_view<float> output(49,31);
		
		for ( double yinc = -200 ; yinc<=1300; yinc+=50)
		{
			for ( double xinc = -1200 ; xinc<=1200; xinc+=50)
			{	
					x[0]=  cam_center.x() + xinc;
					x[1]=  cam_center.y() + yinc;
					double maxmi = 0.0;
					// angles 
					for (unsigned int k = 0; k < 1; k++)
					{
						x[5] = vnl_math::pi/2 * ( double ) k ;					
						double mi = -func.f(x);
						if ( maxmi < mi ) 
							maxmi = mi;
					}
					output((xinc+1200)/50,(yinc+200)/50) = maxmi;
		
			}
			vcl_cout<<".";
		}
		
		vcl_cout<<"Time "<<t.all();
		vil_save(output,"e:/data.tiff");
	}
	//vnl_powell powell(&func);
	//vnl_vector<double> x(6,0.0);
	//powell.set_x_tolerance(1e-1);	
	//powell.set_max_function_evals(10);
	//vul_timer t ;
	//t.mark();
	//powell.minimize(x);


	//vcl_cout<<"Time Taken is "<<t.all()<<vcl_endl;

	//vgl_rotation_3d<double> r(x[3],x[4],x[5]);

	//vcl_cout<<" B to A "<<vcl_endl;
	//vcl_cout<<"Translation is ("<<x[0]<<","<<x[1]<<","<<x[2]<<")"<<vcl_endl;
	//vcl_cout<<"Rotation is "<<r.as_matrix()<<vcl_endl;

	//vcl_cout<<"Initial Mutual Info "<<func.mutual_info(vgl_rotation_3d<double>(),vgl_vector_3d<double>())<<vcl_endl;
	//vcl_cout<<"Final Mutual Info "<<func.mutual_info(r,vgl_vector_3d<double>(x[0],x[1],x[2]))<<vcl_endl;

	return 0;
}
