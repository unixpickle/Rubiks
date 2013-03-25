int symmetry_operation_compose(int left, int right);
int symmetry_operation_inverse(int op);
int symmetry_operation_find(const unsigned char * orig, const unsigned char * res);
void symmetry_operation_perform(int op, unsigned char * data);
