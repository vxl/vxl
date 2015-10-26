#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable

inline void AtomicAdd(volatile __global float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;
    do {
        prevVal.floatVal = *source;
        newVal.floatVal = prevVal.floatVal + operand;
    } while (atomic_cmpxchg((volatile __global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}

inline void AtomicAddFloat8(volatile __global float8 *source, const float8 operand) {
    __global float* source_fptr = (__global float*) source;
    const float* operand_fptr  =  (const float*) &operand;
    for(unsigned i = 0; i<8;i++){
      AtomicAdd(&source_fptr[i],operand_fptr[i]);
    }

}
