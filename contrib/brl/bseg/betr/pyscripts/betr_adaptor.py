# import the batch module and dbvalue from init
# set the global variable, batch, on init before importing this file
import brl_init
import betr_batch as batch
dbvalue = brl_init.register_batch(batch)


class BetrException(brl_init.BrlException):
    pass
#############################################################################
# PROVIDES higher level betr python functions to make batch
# code more readable/refactored
#############################################################################


# create a betr site to hold event data such as event objects
def create_betr_site(lon, lat, elev):
    batch.init_process("betrCreateSiteProcess")
    # set site location in wgs84 deg, deg, meters
    batch.set_input_float(0, lon)
    batch.set_input_float(1, lat)
    batch.set_input_float(2, elev)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        site = dbvalue(id, type)
        return site
    raise BetrException("failed to create site")

# create a betr event trigger to support change detection


def create_betr_event_trigger(lon, lat, elev, name, verbose=False):
    batch.init_process("betrCreateEventTriggerProcess")
    # set site location in wgs84 deg, deg, meters
    batch.set_input_float(0, lon)
    batch.set_input_float(1, lat)
    batch.set_input_float(2, elev)
    batch.set_input_string(3, name)
    batch.set_input_bool(4, verbose)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        event_trigger = dbvalue(id, type)
        return event_trigger
    raise BetrException("failed to create event trigger")

# add a geo object to the site, not associated with an event trigger


def add_site_object(site, name, lon, lat, elev, geom_path):
    batch.init_process("betrAddEventObjectProcess")
    batch.set_input_from_db(0, site)
    batch.set_input_string(1, name)
    batch.set_input_float(2, lon)
    batch.set_input_float(3, lat)
    batch.set_input_float(4, elev)
    batch.set_input_string(5, geom_path)
    status = batch.run_process()
    if(not status):
        raise BetrException("failed to add event object")

# add an event trigger to the site


def add_event_trigger(site, event_trigger):
    batch.init_process("betrAddEventTriggerProcess")
    batch.set_input_from_db(0, site)
    batch.set_input_from_db(1, event_trigger)
    status = batch.run_process()
    if(not status):
        raise BetrException("failed to add event trigger")

# add an event trigger object to the event trigger
def add_event_trigger_event_object(event_trigger, name, lon, lat, elev, geom_path):
    add_event_trigger_object(event_trigger, name, lon, lat, elev, geom_path, False)

def add_event_trigger_reference_object(event_trigger, name, lon, lat, elev, geom_path):
    add_event_trigger_object(event_trigger, name, lon, lat, elev, geom_path, True)

def add_event_trigger_object(event_trigger, name, lon, lat, elev, geom_path, is_reference):
    batch.init_process("betrAddEventTriggerObjectProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_string(1, name)
    batch.set_input_float(2, lon)
    batch.set_input_float(3, lat)
    batch.set_input_float(4, elev)
    batch.set_input_string(5, geom_path)
    batch.set_input_bool(6, is_reference)
    status = batch.run_process()
    if(not status):
        raise BetrException("failed to add event trigger object")

    # add a gridded event trigger polygon to the event trigger
def add_gridded_event_poly(event_trigger, name, lon, lat, elev, geom_path, grid_spacing):
    batch.init_process("betrAddGriddedEventTriggerObjectProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_string(1, name)
    batch.set_input_float(2, lon)
    batch.set_input_float(3, lat)
    batch.set_input_float(4, elev)
    batch.set_input_string(5, geom_path)
    batch.set_input_float(6, grid_spacing)
    status = batch.run_process()
    if(not status):
        raise BetrException("failed to add event trigger object")


# set image and camera data on an event trigger
def set_event_trigger_data(event_trigger, ref_imgr, ref_camera, event_imgr, event_camera):
    batch.init_process("betrSetEventTriggerDataProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_from_db(1, ref_imgr)
    batch.set_input_from_db(2, ref_camera)
    batch.set_input_from_db(3, event_imgr)
    batch.set_input_from_db(4, event_camera)
    status = batch.run_process()
    if(not status):
        raise BetrException("failed to set event trigger data")

# set event OR reference image & camera data on an event trigger
def set_event_trigger_event_data(event_trigger, imgr, camera):
    set_event_trigger_partial_data(event_trigger, imgr, camera, False, False)

def set_event_trigger_reference_data(event_trigger, imgr, camera):
    set_event_trigger_partial_data(event_trigger, imgr, camera, True, False)

def add_event_trigger_reference_data(event_trigger, imgr, camera):
    set_event_trigger_partial_data(event_trigger, imgr, camera, True, True)

def set_event_trigger_partial_data(event_trigger, imgr, camera, is_reference, keep_data):
    batch.init_process("betrSetEventTriggerPartialDataProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_from_db(1, imgr)
    batch.set_input_from_db(2, camera)
    batch.set_input_bool(3, is_reference)
    batch.set_input_bool(4, keep_data)
    status = batch.run_process()
    if(not status):
        raise BetrException("failed to set event trigger data")

# set event trigger data with multiple reference images - image/camera paths are given not resources but as string arrays
# inputs are all paths for consistency. Compare to previous adaptor, which passes pointers to images and cameras
def set_event_trigger_data_multi_ref(event_trigger, ref_img_paths, ref_cam_paths, event_img_path, event_cam_path):
    batch.init_process("betrSetEventTriggerDataMultiRefProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_string_array(1, ref_img_paths)
    batch.set_input_string_array(2, ref_cam_paths)
    batch.set_input_string(3, event_img_path)
    batch.set_input_string(4, event_cam_path)
    status = batch.run_process()
    if(not status):
        raise BetrException("failed to set event trigger data")

# execute change detection with a single event region
def execute_event_trigger(event_trigger, algorithm_name, algorithm_params_json = '{}'):
    batch.init_process("betrExecuteEventTriggerProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_string(1, algorithm_name)
    batch.set_input_string(1, algorithm_name, algorithm_params_json)
    status = batch.run_process()
    prob_change = None
    if status:
        (pc_id, pc_type) = batch.commit_output(0)
        prob_change = batch.get_output_float(pc_id)
        return prob_change
    else:
        raise BetrException("failed to execute change detection")
        return prob_change

# execute change detection with a multiple event regions


def execute_event_trigger_multi(event_trigger, algorithm_name, algorithm_params_json = '{}'):
    batch.init_process("betrExecuteEventTriggerMultiProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_string(1, algorithm_name);
    batch.set_input_string(2, algorithm_params_json);
    status = batch.run_process()
    prob_change = None
    evt_names = None
    if status:
        (pc_id, pc_type) = batch.commit_output(0);
        prob_change = batch.get_output_double_array(pc_id);
        (name_id, name_type) = batch.commit_output(1);
        evt_names = batch.get_bbas_1d_array_string(name_id);
        return (prob_change, evt_names)
    else:
        raise BetrException("failed to execute trigger with multiple event regions")
        return (prob_change, evt_names)

# execute change detection with a multiple event regions
def execute_event_trigger_multi_with_change_imgs(event_trigger, algorithm_name, algorithm_params_json = '{}'):
    batch.init_process("betrExecuteEventTriggerMultiWithChImgProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_string(1, algorithm_name);
    batch.set_input_string(2, algorithm_params_json);
    status = batch.run_process()
    prob_change = None
    evt_names = None
    offsets = None
    images = None
    if status:
        # the change score not really a probability
        (pc_id, pc_type) = batch.commit_output(0);
        prob_change = batch.get_output_double_array(pc_id);

        # the name of the event region being processed
        (name_id, name_type) = batch.commit_output(1);
        evt_names = batch.get_bbas_1d_array_string(name_id);

        # the dimensions (num_columns, num_rows,) and offset (column offset, row offset) of the change images
        # stored as [ [nc, nr, off_c, off_r], ...]
        (dims_off_id, dims_off_type) = batch.commit_output(2);
        dims_off = batch.get_bbas_1d_array_int(dims_off_id);

        # one dimensional pixel array stored in column sequence
        (pix_id, pix_type) = batch.commit_output(3);
        pix = batch.get_bbas_1d_array_byte(pix_id)

        # unpack images
        n = len(prob_change)
        k = 0
        img_index = 0
        offsets = [[0. for i in range(2)] for j in range(n)]
        images = []
        for q in range(0, n):
            ni = dims_off[k]
            nj = dims_off[k+1]
            area = ni*nj
            offsets[q][0]= dims_off[k+2]
            offsets[q][1]= dims_off[k+3]
            k += 4
            image = [[0. for i in range(ni)] for j in range(nj)]
            for j in range(0,nj):
                for i in range(0,ni):
                    image[j][i]=pix[img_index]
                    img_index += 1
            images.append(image)
        return (prob_change, evt_names, offsets, images)
    else:
        raise BetrException("failed to add execute trigger with multiple event regions and change images")
        return (prob_change, evt_names, offsets, images)

# execute change detection with a multiple event regions and return event polygons (useful for gridded event cells)
def execute_event_trigger_multi_with_change_imgs_polys(event_trigger, algorithm_name, algorithm_params_json = '{}'):
    batch.init_process("betrExecuteEtrMultiChImgEvtPolyProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_string(1, algorithm_name);
    batch.set_input_string(2, algorithm_params_json);
    status = batch.run_process()
    prob_change = None
    evt_names = None
    offsets = None
    images = None
    nverts = None
    verts = None
    if status:
        # the change score not really a probability
        (pc_id, pc_type) = batch.commit_output(0);
        prob_change = batch.get_output_double_array(pc_id);

        # the name of the event region being processed
        (name_id, name_type) = batch.commit_output(1);
        evt_names = batch.get_bbas_1d_array_string(name_id);

        # the dimensions (num_columns, num_rows,) and offset (column offset, row offset) of the change images
        # stored as [ [nc, nr, off_c, off_r], ...]
        (dims_off_id, dims_off_type) = batch.commit_output(2);
        dims_off = batch.get_bbas_1d_array_int(dims_off_id);

        # one dimensional pixel array stored in column sequence
        (pix_id, pix_type) = batch.commit_output(3);
        pix = batch.get_bbas_1d_array_byte(pix_id)

        # one dimensional pixel array stored in column sequence
        (nverts_id, nverts_type) = batch.commit_output(4);
        nverts = batch.get_bbas_1d_array_int(nverts_id)

        # the polygon vertex array
        (verts_id, verts_type) = batch.commit_output(5);
        verts = batch.get_output_double_array(verts_id);

        # unpack images
        n = len(prob_change)
        k = 0
        img_index = 0
        offsets = [[0. for i in range(2)] for j in range(n)]
        images = []
        for q in range(0, n):
            ni = dims_off[k]
            nj = dims_off[k+1]
            area = ni*nj
            offsets[q][0]= dims_off[k+2]
            offsets[q][1]= dims_off[k+3]
            k += 4
            image = [[0. for i in range(ni)] for j in range(nj)]
            for j in range(0,nj):
                for i in range(0,ni):
                    image[j][i]=pix[img_index]
                    img_index += 1
            images.append(image)
# extract the polygons as a list of lists
        polys = []
        origin = 0;
        for nv_index in range(0,n):
            nv = nverts[nv_index]
            vert_array = []
            for vert_index in range(0,3*nv, 3):
                pt_array = []
                pt_index = origin+vert_index;
                pt_array.append(verts[pt_index]);
                pt_index += 1;
                pt_array.append(verts[pt_index]);
                pt_index += 1;
                pt_array.append(verts[pt_index]);
                vert_array.append(pt_array);
            polys.append(vert_array)
            origin += 3*nv;
        return (prob_change, evt_names, offsets, images, polys)
    else:
        raise BetrException("failed to add execute trigger with multiple event regions, change images and event polys")
        return (prob_change, evt_names, offsets, images, polys)
