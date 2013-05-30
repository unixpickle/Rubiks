#include "permutation.h"
#include <stdio.h>

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <permutation>\n", argv[0]);
        return 1;
    }
    Perm * p = perm_from_string(argv[1]);
    
    printf("Finding sqrt(");
    perm_print(p);
    printf(")\n");
    
    PermList roots;
    perm_inv_power(p, 2, &roots);
    printf("Found %d square root%s\n", roots.length, roots.length == 1 ? "" : "s");
    int i, numEven = 0, numOdd = 0;
    for (i = 0; i < roots.length; i++) {
        perm_print(roots.perms[i]);
        int parity = perm_parity(roots.perms[i]);
        printf(" [%s parity]\n", parity == 0 ? "even" : "odd");
        if (parity) numOdd ++;
        else numEven ++;
    }
    printf("Total of %d odd parity and %d even parity\n", numOdd, numEven);
    perm_list_destroy(&roots);
    
    
    return 0;
}
