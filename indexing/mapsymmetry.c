#include "mapsymmetry.h"

static int compare_data(const uint16_t * ptr1, const uint16_t * ptr2, int count);

uint16_t map_symmetry_apply_corners(const MapSymmetry * sym, uint16_t cornerInfo) {
    uint16_t newValue = 0;
    int i;
    for (i = 0; i < 8; i++) {
        int oldIndex = sym->corners[i];
        uint16_t oldValue = (cornerInfo >> (oldIndex * 2)) & 3;
        newValue |= oldValue << (i * 2);
    }
    return newValue;
}

uint16_t map_symmetry_apply_edges(const MapSymmetry * sym, uint16_t edgesInfo) {
    int i;
    uint16_t newValue = 0;
    for (i = 0; i < 12; i++) {
        int oldIndex = sym->edges[i];
        uint16_t oldFlag = (edgesInfo >> oldIndex) & 1;
        newValue |= oldFlag << i;
    }
    return newValue;
}

void map_symmetry_compute_lowest(const MapSymmetry * syms, int numSym, 
                                 const uint16_t * ptrsIn, uint16_t * ptrsOut) {
    int i;
    uint16_t lowest[3];
    memcpy((void *)lowest, (void *)ptrsIn, sizeof(uint16_t) * 3);
    for (i = 0; i < numSym; i++) {
        uint16_t ptrs[3];
        const MapSymmetry * sym = &syms[i];
        ptrs[0] = map_symmetry_apply_edges(sym, ptrsIn[0]);
        ptrs[1] = map_symmetry_apply_corners(sym, ptrsIn[1]);
        ptrs[2] = map_symmetry_apply_edges(sym, ptrsIn[2]);
        // printf("symmetrized %d: %d, %d, %d\n", i, ptrs[0], ptrs[1], ptrs[2]);
        if (compare_data(ptrs, lowest, 3) < 0) {
            // printf("picking this one!\n");
            memcpy((void *)lowest, (void *)ptrs, sizeof(uint16_t) * 3);
        }
    }
    memcpy((void *)ptrsOut, (void *)lowest, sizeof(uint16_t) * 3);
}

static int compare_data(const uint16_t * ptr1, const uint16_t * ptr2, int count) {
    int i;
    for (i = 0; i < count; i++) {
        if (ptr1[i] > ptr2[i]) return 1;
        else if (ptr2[i] > ptr1[i]) return -1;
    }
    return 0;
}
