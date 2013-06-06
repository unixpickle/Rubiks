#include "randperm.h"

Perm * rand_perm(int length) {
    int * indexes = (int *)malloc(sizeof(int) * length);
    int i, count = length;
    for (i = 0; i < length; i++) {
        indexes[i] = i;
    }
    Perm * p = perm_identity(length);
    srand(time(NULL));
    while (count > 0) {
        int pickIndex = rand() % count;
        int value = indexes[pickIndex];
        int newCount = 0;
        for (i = 0; i < count; i++) {
            if (i != pickIndex) {
                indexes[newCount] = indexes[i];
                newCount++;
            }
        }
        count = newCount;
        p->map[newCount] = value;
    }
    return p;
}

void rand_change_parity(Perm * p) {
    int temp = p->map[1];
    p->map[1] = p->map[0];
    p->map[0] = temp;
}
