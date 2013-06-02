#include "maproot.h"

typedef struct {
    void ** pointers;
    int length;
} PointerList;

static PointerList * pointer_list_new();
static void pointer_list_add(PointerList * list, void * ptr);
static void pointer_list_free(PointerList * list, int freeDeep);
static void pointer_list_add_list(PointerList * list, const PointerList * addList);

// compute the orientation roots for a specific permutation
static void _compute_corner_roots(PointerList * list, Perm * cornerPerm,
                                  const uint8_t * corners, int power,
                                  const uint8_t * soFar, int depth);

static void _compute_edge_roots(PointerList * list, Perm * edgePerm,
                                const uint8_t * edges, int power,
                                const uint8_t * soFar, int depth);

static uint16_t _compute_edge_orientations(const uint8_t * edges);

// compute the roots for a specific permutation
static PointerList * _all_roots(Perm * cornerPerm, Perm * edgePerm,
                                RubiksMap * map, int power, int max);

RubiksMap ** maproot_take_nth_root(RubiksMap * object, int power,
                                   int * countOut, int maxCount) {
    Perm * cornerPerm = perm_from_bytes(object->pieces, 0xf, 8);
    Perm * edgePerm = perm_from_bytes(&object->pieces[8], 0xf, 12);
    PermList cornerRoots;
    PermList edgeRoots;
    perm_inv_power(cornerPerm, power, &cornerRoots);
    perm_inv_power(edgePerm, power, &edgeRoots);
    perm_free(cornerPerm);
    perm_free(edgePerm);
    
    int i, j;
    PointerList * maps = pointer_list_new();
    for (i = 0; i < edgeRoots.length; i++) {
        Perm * edgeRoot = edgeRoots.perms[i];
        for (j = 0; j < cornerRoots.length; j++) {
            Perm * cornerRoot = cornerRoots.perms[j];
            if (perm_parity(edgeRoot) != perm_parity(cornerRoot)) {
                continue;
            }
            int max = (maxCount < 0 ? -1 : maxCount - maps->length);
            PointerList * list = _all_roots(cornerRoot, edgeRoot, object, power, max);
            pointer_list_add_list(maps, list);
            pointer_list_free(list, 0);
            if (maps->length >= maxCount && maxCount != -1) {
                break;
            }
        }
        if (maps->length >= maxCount && maxCount != -1) {
            break;
        }
    }
    
    *countOut = maps->length;
    
    perm_list_destroy(&cornerRoots);
    perm_list_destroy(&edgeRoots);
    RubiksMap ** mapArray = (RubiksMap **)malloc(sizeof(void *) * maps->length);
    memcpy(mapArray, maps->pointers, maps->length * sizeof(void *));
    pointer_list_free(maps, 0);
    return mapArray;
}

/***********
 * PRIVATE *
 ***********/

static PointerList * pointer_list_new() {
    PointerList * list = (PointerList *)malloc(sizeof(PointerList));
    bzero(list, sizeof(PointerList));
    return list;
}

static void pointer_list_add(PointerList * list, void * ptr) {
    list->length++;
    if (!list->pointers) {
        list->pointers = (void *)malloc(sizeof(void *));
        list->pointers[0] = ptr;
    } else {
        list->pointers = (void *)realloc(list->pointers, sizeof(void *) * list->length);
        list->pointers[list->length - 1] = ptr;
    }
}

static void pointer_list_free(PointerList * list, int freeDeep) {
    if (freeDeep) {
        int i;
        for (i = 0; i < list->length; i++) {
            free(list->pointers[i]);
        }
    }
    if (list->pointers) free(list->pointers);
    free(list);
}

static void pointer_list_add_list(PointerList * list, const PointerList * addList) {
    if (!list->pointers) {
        list->pointers = (void **)malloc(sizeof(void *) * (list->length + addList->length));
    } else {
        list->pointers = (void **)realloc(list->pointers, 
                                          sizeof(void *) * (list->length + addList->length));
    }
    memcpy(&list->pointers[list->length], addList->pointers,
           addList->length * sizeof(void *));
    list->length += addList->length;
}

// compute the orientation roots for a specific permutation
static void _compute_corner_roots(PointerList * list, Perm * cornerPerm,
                                  const uint8_t * corners, int power,
                                  const uint8_t * soFar, int depth) {
    int i;
    if (depth == 8) {
        if (!validate_corner_orientation(soFar)) return;        
        uint8_t powerOut[8];
        cube_perm_exp_corners(powerOut, soFar, power);
        if (memcmp(powerOut, corners, 8) == 0) {
            uint8_t * buff = (uint8_t *)malloc(8);
            memcpy(buff, soFar, 8);
            pointer_list_add(list, buff);
        }
        return;
    }
    
    // generate all of the next corner possibilities
    int piece = cornerPerm->map[depth];
    int dest = depth;
    int coset = cube_perm_corner_coset(piece, dest);
    uint8_t nextData[8];
    memcpy(nextData, soFar, 8);
    
    for (i = 0; i < 3; i++) {
        int perms[3] = {0, 4, 5};
        int sym = symmetry_operation_compose(coset, perms[i]);
        nextData[depth] = piece | (sym << 4);
        _compute_corner_roots(list, cornerPerm, corners, power,
                              nextData, depth + 1);
    }
}

static void _compute_edge_roots(PointerList * list, Perm * edgePerm,
                                const uint8_t * edges, int power,
                                const uint8_t * soFar, int depth) {
    int i;
    if (depth == 12) {
        uint16_t orientations = _compute_edge_orientations(soFar);
        if (!validate_edge_orientation(orientations)) return;
        uint8_t powerOut[12];
        cube_perm_exp_edges(powerOut, soFar, power);
        if (memcmp(powerOut, edges, 12) == 0) {
            uint8_t * buff = (uint8_t *)malloc(12);
            memcpy(buff, soFar, 12);
            pointer_list_add(list, buff);
        }
        return;
    }
    // generate all of the next edge possibilities
    int piece = edgePerm->map[depth];
    int dest = depth;
    int sym1 = 0, sym2 = 0;
    cube_perm_edge_symmetries(piece, dest, &sym1, &sym2);
    uint8_t nextData[12];
    memcpy(nextData, soFar, 12);
    for (i = 0; i < 2; i++) {
        int sym = i == 0 ? sym1 : sym2;
        nextData[depth] = piece | (sym << 4);
        _compute_edge_roots(list, edgePerm, edges, power,
                            nextData, depth + 1);
    }
}

static uint16_t _compute_edge_orientations(const uint8_t * edges) {
    // ugly, smelly, disgusting hack.
    RubiksMap * map = rubiks_map_new_identity();
    memcpy(&map->pieces[8], edges, 12);
    uint16_t orientations = rubiks_map_edge_orientations(map);
    rubiks_map_free(map);
    return orientations;
}

// compute the roots for a specific permutation
static PointerList * _all_roots(Perm * cornerPerm, Perm * edgePerm,
                                RubiksMap * map, int power, int max) {
    PointerList * edges = pointer_list_new();
    PointerList * corners = pointer_list_new();
    PointerList * retVal = pointer_list_new();
    
    char temp[12];
    _compute_corner_roots(corners, cornerPerm, map->pieces, power, temp, 0);
    _compute_edge_roots(edges, edgePerm, &map->pieces[8], power, temp, 0);
    
    int i, j;
    for (i = 0; i < edges->length; i++) {
        uint8_t * edgeConfig = edges->pointers[i];
        for (j = 0; j < corners->length; j++) {
            uint8_t * cornerConfig = corners->pointers[j];
            RubiksMap * map = rubiks_map_new_identity();
            memcpy(map->pieces, cornerConfig, 8);
            memcpy(&map->pieces[8], edgeConfig, 12);
            pointer_list_add(retVal, map);
            if (retVal->length == max) break;
        }
        if (retVal->length == max) break;
    }
    
    pointer_list_free(edges, 1);
    pointer_list_free(corners, 1);
    return retVal;                     
}
