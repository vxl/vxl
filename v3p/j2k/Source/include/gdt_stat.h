/*  gdt_stat.h
 *
 *  Constants which are referenced from gdt_pub.h and gdt_proj.h
 *
 */

/* --- Status values for transformation function, gdt_transform() --- 
 *     Some values are returned from the projection routines
 */

#ifndef   _GDT_STAT_H
#define   _GDT_STAT_H   1


#define   TRANS_OK                0
                                    /* Good value */

#define   TRANS_NOT_SET          -1
                                    /* No transformation set up */

#define   TRANS_COMP_ERROR       -2
                                    /* computational error 
                                     * (eg. bad convergence) 
                                     */

#define   TRANS_OUT_OF_BOUNDS     1
                                    /* Input out of bounds */

#define   TRANS_INFINITE          2
                                    /* Result too large to be represented */

#define   TRANS_MULTI             3
                                    /* Multiple answers (e.g. point maps onto a 
                                     * line
                                     */
#define   TRANS_CIRCLE            4
                                    /* Multiple on a circle of radius x or y
                                     */

#endif   /* end of ifndef    _GDT_STAT_H   */
