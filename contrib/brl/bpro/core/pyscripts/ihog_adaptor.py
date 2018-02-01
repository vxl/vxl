# import the batch module and dbvalue from init
# set the global variable, batch,  on init before importing this file
import brl_init
dbvalue = brl_init.DummyBatch()
batch = brl_init.DummyBatch()

# compute offsets based on mutual information
class IhogException(brl_init.BrlException):
    pass

def register_translational(tgr_img, tgr_img_mask, from_img, search_radius=0):
    batch.init_process('ihogRegisterTranslationalProcess')
    batch.set_input_from_db(0, tgr_img)
    batch.set_input_from_db(1, tgr_img_mask)
    batch.set_input_from_db(2, from_img)
    batch.set_input_int(3, search_radius)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        trans_x = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        trans_y = batch.get_output_double(id)
        return (trans_x, trans_y)
    else:
        raise IhogException("Failed to register")

# compute cost surface image based on mutual information
def compute_mi_cost_surface(tgr_img, tgr_img_mask, from_img, search_radius):
    batch.init_process('ihogComputeMiCostSurfaceProcess')
    batch.set_input_from_db(0, tgr_img)
    batch.set_input_from_db(1, tgr_img_mask)
    batch.set_input_from_db(2, from_img)
    batch.set_input_int(3, search_radius)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        cost_image = dbvalue(id, type)
        return cost_image
    else:
        raise IhogException("Failed to compute mutual information cost surface")

# compute mutual information, this is actually entropy diff so the lower
# the better
def mutual_information(tgr_img, from_img, tgr_img_mask):
    batch.init_process('ihogMutualInformationProcess')
    batch.set_input_from_db(0, tgr_img)
    batch.set_input_from_db(1, from_img)
    batch.set_input_from_db(2, tgr_img_mask)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        minfo = batch.get_output_float(id)
        batch.remove_data(id)
        return minfo
    else:
        raise IhogException("Failed to compute mutual information")
