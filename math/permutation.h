#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    int * map;
    int length;
} Perm;

typedef struct {
    Perm ** perms;
    int length;
} PermList;

Perm * perm_identity(int length);
Perm * perm_copy(Perm * p);
Perm * perm_from_bytes(const uint8_t * rawPerm, uint8_t mask, uint8_t len);
Perm * perm_from_string(const char * string);

int perm_compare(Perm * a, Perm * b);

Perm * perm_multiply(Perm * left, Perm * right);
void perm_inv_power(Perm * perm, int power, PermList * listOut);
Perm * perm_power(Perm * perm, int power);

// returns 1 if odd, 0 if even
int perm_parity(Perm * perm);

void perm_print(Perm * p);

void perm_free(Perm * perm);

void perm_list_init(PermList * list);
void perm_list_append(PermList * list, Perm * p);
void perm_list_destroy(PermList * list);
