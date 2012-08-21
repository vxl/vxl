from boxm2_register import boxm2_batch, dbvalue;

# compute offsets based on mutual information
def register_translational(img0, img0_mask, img1, search_radius=0):
    boxm2_batch.init_process('ihogRegisterTranslationalProcess')
    boxm2_batch.set_input_from_db(0,img0)
    boxm2_batch.set_input_from_db(1,img0_mask)
    boxm2_batch.set_input_from_db(2,img1)
    boxm2_batch.set_input_int(3,search_radius)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    trans_x = boxm2_batch.get_output_double(id)
    (id,type) = boxm2_batch.commit_output(1)
    trans_y = boxm2_batch.get_output_double(id)
    return (trans_x, trans_y)

# compute cost surface image based on mutual information
def compute_mi_cost_surface(img0, img0_mask, img1, search_radius):
    boxm2_batch.init_process('ihogComputeMiCostSurfaceProcess')
    boxm2_batch.set_input_from_db(0,img0)
    boxm2_batch.set_input_from_db(1,img0_mask)
    boxm2_batch.set_input_from_db(2,img1)
    boxm2_batch.set_input_int(3,search_radius)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    cost_image = dbvalue(id,type)
    return cost_image

# compute mutual information, this is actually entropy diff so the lower the better
def mutual_information(img0, img1, mask):
    boxm2_batch.init_process('ihogMutualInformationProcess')
    boxm2_batch.set_input_from_db(0,img0)
    boxm2_batch.set_input_from_db(1,img1)
    boxm2_batch.set_input_from_db(2,mask)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    minfo = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    return minfo
