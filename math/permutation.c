#include "permutation.h"

typedef struct {
    Perm * currentPerm;
    int ** cache;
    int power;
    Perm * goal;
    PermList * list;
} RootContext;

static void recursive_root_search(RootContext * ctx, int depth, int * indexesLeft);
static int check_partial_perm_power(Perm * partial, int depth, Perm * dest, int power);

/*
typedef struct {
    int * map;
    int length;
} Perm;
*/

Perm * perm_identity(int length) {
    Perm * p = (Perm *)malloc(sizeof(Perm));
    p->map = (int *)malloc(sizeof(int) * length);
    p->length = length;
    int i;
    for (i = 0; i < length; i++) {
        p->map[i] = i;
    }
    return p;
}

Perm * perm_copy(Perm * p) {
    Perm * np = (Perm *)malloc(sizeof(Perm));
    np->map = (int *)malloc(sizeof(int) * p->length);
    np->length = p->length;
    memcpy(np->map, p->map, sizeof(int) * p->length);
    return np;
}

Perm * perm_from_bytes(const uint8_t * rawPerm, uint8_t mask, uint8_t len) {
    Perm * p = (Perm *)malloc(sizeof(Perm));
    p->map = (int *)malloc(sizeof(int) * len);
    p->length = len;
    int i;
    for (i = 0; i < len; i++) {
        p->map[i] = rawPerm[i];
    }
    return p;
}

Perm * perm_from_string(const char * string) {
    Perm * p = (Perm *)malloc(sizeof(Perm));
    p->map = (int *)malloc(sizeof(int));
    p->length = 0;
    
    // read each whitespace-separated word from the string
    // and parse it as a number indexed from 1 to n
    char * buffer = (char *)malloc(strlen(string) + 1);
    int wordStart = 0;
    int hasWord = 0;
    int i;
    for (i = 0; i <= strlen(string); i++) {
        if (isspace(string[i]) || string[i] == 0) {
            if (hasWord) {
                hasWord = 0;
                p->length++;
                if (p->length > 0) {
                    p->map = (int *)realloc(p->map, sizeof(int) * p->length);
                }
                memcpy(buffer, &string[wordStart], i - wordStart);
                buffer[i - wordStart] = 0;
                p->map[p->length - 1] = atoi(buffer) - 1;
            }
        } else {
            if (!hasWord) {
                hasWord = 1;
                wordStart = i;
            }
        }
    }
    free(buffer);
    return p;
}

int perm_compare(Perm * a, Perm * b) {
    if (a->length != b->length) {
        abort();
    }
    int i;
    for (i = 0; i < a->length; i++) {
        if (a->map[i] != b->map[i]) return i + 1;
    }
    return 0;
}

Perm * perm_multiply(Perm * left, Perm * right) {
    if (left->length != right->length) {
        abort();
    }
    Perm * p = (Perm *)malloc(sizeof(Perm));
    p->map = (int *)malloc(sizeof(int) * left->length);
    p->length = left->length;
    int i;
    for (i = 0; i < p->length; i++) {
        p->map[i] = left->map[right->map[i]];
    }
    return p;
}

void perm_inv_power(Perm * perm, int power, PermList * permsOut) {
    // brute force solutions!
    perm_list_init(permsOut);
    RootContext ctx;
    ctx.power = power;
    ctx.goal = perm;
    ctx.currentPerm = perm_identity(perm->length);
    ctx.cache = (int **)malloc(sizeof(int *) * perm->length);
    ctx.list = permsOut;
    int * indexesLeft = (int *)malloc(sizeof(int) * perm->length);
    int i;
    for (i = 0; i < perm->length; i++) {
        indexesLeft[i] = i + 1;
        ctx.cache[i] = (int *)malloc(sizeof(int) * perm->length);
    }
    recursive_root_search(&ctx, 0, indexesLeft);
    for (i = 0; i < perm->length; i++) {
        free(ctx.cache[i]);
    }
    free(ctx.cache);
    free(indexesLeft);
    perm_free(ctx.currentPerm);
}

Perm * perm_power(Perm * perm, int power) {
    int i;
    Perm * p = perm_identity(perm->length);
    for (i = 0; i < power; i++) {
        Perm * np = perm_multiply(perm, p);
        perm_free(p);
        p = np;
    }
    return p;
}

int perm_parity(Perm * perm) {
    // perform swooshes
    int parity = 0, i;
    Perm * temp = perm_copy(perm);
    
    for (i = 0; i < perm->length; i++) {
        if (temp->map[i] == i) continue;
        int wantIndex = 0, j;
        for (j = i + 1; j < perm->length; j++) {
            if (perm->map[j] == i) {
                wantIndex = j;
                break;
            }
        }
        if (wantIndex <= i) abort();
        int swapValue = temp->map[i];
        temp->map[i] = i;
        temp->map[wantIndex] = swapValue;
        parity ^= 1;
    }
    
    perm_free(temp);
    return parity;
}

void perm_print(Perm * p) {
    int i;
    for (i = 0; i < p->length; i++) {
        if (i != 0) {
            printf(" ");
        }
        printf("%d", p->map[i] + 1);
    }
}

void perm_free(Perm * perm) {
    free(perm->map);
    free(perm);
}

/***********
 * PRIVATE *
 ***********/

static void recursive_root_search(RootContext * ctx, int depth, int * indexesLeft) {
    int i;
    if (ctx->goal->length == depth) {
        Perm * power = perm_power(ctx->currentPerm, ctx->power);
        if (perm_compare(power, ctx->goal) == 0) {
            Perm * addPerm = perm_copy(ctx->currentPerm);
            perm_list_append(ctx->list, addPerm);
        }
        perm_free(power);
        return;
    }
    if (!check_partial_perm_power(ctx->currentPerm, depth,
                                  ctx->goal, ctx->power)) {
        return;
    }
    int * indexCopy = ctx->cache[depth];
    memcpy(indexCopy, indexesLeft, sizeof(int) * ctx->goal->length);
    for (i = 0; i < ctx->goal->length; i++) {
        if (indexCopy[i] != 0) {
            int j = indexCopy[i] - 1;
            indexCopy[i] = 0;
            ctx->currentPerm->map[depth] = j;
            recursive_root_search(ctx, depth + 1, indexCopy);
            indexCopy[i] = j + 1;
        }
    }
}

static int check_partial_perm_power(Perm * partial, int depth, Perm * dest, int power) {
    int i, j;
    for (i = 0; i < depth; i++) {
        int didFail = 0;
        int value = partial->map[i];
        for (j = 1; j < power; j++) {
            if (value >= depth) {
                didFail = 1;
                break;
            }
            value = partial->map[value];
        }
        if (didFail) continue;
        if (value != dest->map[i]) return 0;
    }
    return 1;
}

/*********************
 * Permutation Lists *
 *********************/

void perm_list_init(PermList * list) {
    list->perms = NULL;
    list->length = 0;
}

void perm_list_append(PermList * list, Perm * p) {
    if (!list->perms) {
        list->perms = (Perm **)malloc(sizeof(Perm *));
        list->length = 1;
    } else {
        list->length++;
        list->perms = (Perm **)realloc(list->perms, sizeof(Perm *) * list->length);
    }
    list->perms[list->length - 1] = p;
}

void perm_list_destroy(PermList * list) {
    int i;
    for (i = 0; i < list->length; i++) {
        perm_free(list->perms[i]);
    }
    if (list->perms) free(list->perms);
}
