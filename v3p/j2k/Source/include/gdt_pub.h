
/* gdt_public.h */
/**
** EDITS
** [01] 11Jun98 pg	added gdt protos 
*/
#ifndef ERM_GDTH
#define ERM_GDTH

#include  "platform.h"
#include  "gdt_stat.h"

/*============================*/
/*    TYPES AND CONSTANTS     */

typedef long int  t_coord_sys_id  ;

typedef struct { double x; double y; double z; } point3;

/* --- status values for the transformation set-up --- */
/* NB the signs of the statuses below must not be changed */
#define   TRN_STAT_OK      0
#define   TRN_STAT_INRES   1
#define   TRN_STAT_COMP    2
#define   TRN_STAT_BAD     3
#define   TRN_STAT_PROTO  -1

/* --- Modes for the transformation --- */
#define   TRN_MODE_INFINITE       1
#define   TRN_MODE_FINITE         2
#define   TRN_MODE_RESTRICTED     3
#define   TRN_MODE_NATURAL        4
#define   TRN_MODE_CURRENT        5

/* --- Longline modes (for polyline conversion) --- */
#define   LONG_MER_PAR            0
#define   LONG_LOXODROME          1
#define   LONG_NORMAL_SECTION     2
#define   LONG_STRAIGHT           3

/* Validation flags ( can be returned from gdt_inq_status_coord_sys() )   */
#define   GDT_CHK_M_SPHEROID     1
#define   GDT_CHK_M_LONGITUDE    2

/* --- Status values for transformation function, gdt_transform() --- */
/*     are defined in  "gdt_stat.h"  */


/* --- Modes for use with gdt_debug() --- */
#define   GDT_INIT          1
#define   GDT_BASE          2
#define   GDT_COORD_SYS     4
#define   GDT_TRANS         8
#define   GDT_TRANS_BRIEF   16
#define   GDT_ALL          ~0


/*===============================================*/
/*    FUNCTION PROTOTYPES (public functions)     */


long int        gdt_install_error( void (*error_func)()  );
long int        gdt_install_error_f( void (*error_func)()  );

t_coord_sys_id  gdt_create_coord_sys( char*  datum_name, 
                                      char*  proj_name, 
                                      char*  units );

t_coord_sys_id  gdt_crelock_coord_sys( char*  datum_name, 
                                       char*  proj_name, 
                                       char*  units );

long int        gdt_check_creation_coord_sys( char*  datum_name, 
                                              char*  proj_name, 
                                              char*  units );

long int        gdt_inq_status_coord_sys( t_coord_sys_id *id);

t_coord_sys_id  gdt_copy_coord_sys  ( t_coord_sys_id  *source_id );
long int        gdt_lock_coord_sys  ( t_coord_sys_id  *id );
long int        gdt_delete_coord_sys( t_coord_sys_id  *id );
t_coord_sys_id  gdt_next_coord_sys  ( t_coord_sys_id  *next_flag);

long int        gdt_mod_coord_sys   ( t_coord_sys_id  *id, 
                                      char            *entity_name, 
                                      char            *attrib_name, 
                                      char            *data_type,
                                      void            *data);

long int        gdt_inq_coord_sys   ( t_coord_sys_id  *id, 
                                      char            *entity_name, 
                                      char            *attrib_name, 
                                      char            *data_type,
                                      void            *data,
                                      int              size);

long int        gdt_set_source( t_coord_sys_id  *source_id );
long int        gdt_set_target( t_coord_sys_id  *target_id );
long int        gdt_inq_trans_status(void);
long int        gdt_mod_num_points( long int *num_points );
long int        gdt_mod_trans_mode( long int *new_mode );

t_coord_sys_id  gdt_inq_source();
t_coord_sys_id  gdt_inq_target();
long int        gdt_inq_trans_mode( long int *test_mode );

long int        gdt_inq_next_base( char * next_base );

long int  gdt_transform( double  * x_in,
                         double  * y_in,
                         double  * z_in,
                         double  * x_out,
                         double  * y_out,
                         double  * z_out);

long int  gdt_trans_array( long  int   *npoints,
                           point3  xyz_in_array[],
                           point3  xyz_out_array[] );

long int  gdt_trans_polyline( long int *longline_type,
                              long  int   *npoints,
                              point3  xyz_in_array[],
                              long  int   *max_lines,
                              long  int   *max_points,
                              long  int   lines[][2],
                              point3  xyz_out_array[] );

long int  gdt_debug( long int mode );

/* --- EOF --- */

#endif /* ERM_GDTH */
