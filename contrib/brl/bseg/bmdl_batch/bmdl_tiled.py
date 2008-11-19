##########################################################################
# Created by Gamze D. Tunali
#            LEMS, Brown University
#            Nov 11, 2008
##########################################################################
import bmdl_batch
bmdl_batch.register_processes();
bmdl_batch.register_datatypes();
xdim=2000;
ydim=2000;
xover=50;
yover=50;
bmdl_batch.init_process("bmdlModelingProcess");
bmdl_batch.set_params_process("./modeling_params.xml");
bmdl_batch.set_input_string(0,"C://test_images//BaghdadLIDAR//dem_1m_a1_baghdad_tile39.tif");
bmdl_batch.set_input_string(1, "C://test_images//BaghdadLIDAR//dem_1m_a2_baghdad_tile39.tif");
bmdl_batch.set_input_string(2, ""); 
bmdl_batch.set_input_string(3, "C://test_images//IMESH//bmdl_kmz");
bmdl_batch.set_input_unsigned(4, xdim);
bmdl_batch.set_input_unsigned(5, ydim);
bmdl_batch.set_input_unsigned(6, xover);
bmdl_batch.set_input_unsigned(7, yover);
bmdl_batch.run_process();
