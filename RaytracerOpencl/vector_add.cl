// Add you device OpenCL code
void kernel simpleAdd(global const int* A, global const int* B, global int* C) { 
    C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)];
}
