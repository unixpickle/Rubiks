#include "stickermap.h"

static int _sticker_map_apply_face(StickerMap * map, const int * indices, const char * str);
static int _sticker_map_apply_face_cont(StickerMap * map, const int * indices,
                                        const char * str, int color);

StickerMap * sticker_map_new_identity() {
    StickerMap * map = (StickerMap *)malloc(sizeof(StickerMap));
    bzero(map->indices, 54);
    int i;
    for (i = 0; i < 54; i++) {
        map->indices[i] = i;
    }
    return map;
}

StickerMap * sticker_map_copy(StickerMap * map) {
    StickerMap * mapCopy = (StickerMap *)malloc(sizeof(StickerMap));
    memcpy(mapCopy->indices, map->indices, 54 * sizeof(unsigned char));
    return mapCopy;
}

StickerMap * sticker_map_copy_into(StickerMap * dest, StickerMap * map) {
    int i;
    memcpy(dest->indices, map->indices, 54 * sizeof(unsigned char));
}

void sticker_map_rotate(StickerMap * map,
                       const unsigned char * indices,
                       int indexCount,
                       int shiftFactor) {
    unsigned char * mapValues = (unsigned char *)malloc(indexCount);
    int i;
    for (i = 0; i < indexCount; i++) {
        mapValues[i] = map->indices[indices[i]];
    }
    for (i = 0; i < indexCount; i++) {
        int index = i - shiftFactor;
        while (index < 0) {
            index += indexCount;
        }
        index = index % indexCount; // for negative shifting
        map->indices[indices[i]] = mapValues[index];
    }
    free(mapValues);
}

void sticker_map_multiply(StickerMap * output,
                         const StickerMap * left,
                         const StickerMap * right) {
    int i;
    for (i = 0; i < 54; i++) {
        output->indices[i] = right->indices[left->indices[i]];
    }
}

StickerMap * sticker_map_inverse(StickerMap * map) {
    StickerMap * mapInv = sticker_map_new_identity();
    int i;
    for (i = 0; i < 54; i++) {
        unsigned char mapIndex = map->indices[i];
        mapInv->indices[mapIndex] = i;
    }
    return mapInv;
}

void sticker_map_free(StickerMap * map) {
    free(map);
}

StickerMap * sticker_map_create_top() {
    unsigned char topIndices[] = {29, 28, 27, 15, 3, 4, 5, 17};
    unsigned char topRing[] = {26, 14, 2, 38, 44, 50, 6, 18, 30, 51, 45, 39};
    StickerMap * ident = sticker_map_new_identity();
    sticker_map_rotate(ident, topIndices, 8, 2);
    sticker_map_rotate(ident, topRing, 12, 3);
    return ident;
}

StickerMap * sticker_map_create_bottom() {
    unsigned char bottomIndices[] = {33, 34, 35, 23, 11, 10, 9, 21};
    unsigned char bottomRing[] = {24, 12, 0, 36, 42, 48, 8, 20, 32, 53, 47, 41};
    StickerMap * ident = sticker_map_new_identity();
    sticker_map_rotate(ident, bottomIndices, 8, 2);
    sticker_map_rotate(ident, bottomRing, 12, 3);
    return ident;
}

StickerMap * sticker_map_create_right() {
    unsigned char rightIndices[] = {39, 40, 41, 47, 53, 52, 51, 45};
    unsigned char rightRing[] = {26, 25, 24, 35, 34, 33, 32, 31, 30, 29, 28, 27};
    StickerMap * ident = sticker_map_new_identity();
    sticker_map_rotate(ident, rightIndices, 8, 2);
    sticker_map_rotate(ident, rightRing, 12, 3);
    return ident;
}

StickerMap * sticker_map_create_left() {
    unsigned char leftIndices[] = {38, 37, 36, 42, 48, 49, 50, 44};
    unsigned char leftRing[] = {2, 1, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3};
    StickerMap * ident = sticker_map_new_identity();
    sticker_map_rotate(ident, leftIndices, 8, 2);
    sticker_map_rotate(ident, leftRing, 12, 3);
    return ident;
}

StickerMap * sticker_map_create_front() {
    unsigned char frontIndices[] = {26, 25, 24, 12, 0, 1, 2, 14};
    unsigned char frontRing[] = {3, 15, 27, 39, 40, 41, 35, 23, 11, 36, 37, 38};
    StickerMap * ident = sticker_map_new_identity();
    sticker_map_rotate(ident, frontIndices, 8, 2);
    sticker_map_rotate(ident, frontRing, 12, 3);
    return ident;
}

StickerMap * sticker_map_create_back() {
    unsigned char backIndices[] = {30, 31, 32, 20, 8, 7, 6, 18};
    unsigned char backRing[] = {5, 17, 29, 51, 52, 53, 33, 21, 9, 48, 49, 50};
    StickerMap * ident = sticker_map_new_identity();
    sticker_map_rotate(ident, backIndices, 8, 2);
    sticker_map_rotate(ident, backRing, 12, 3);
    return ident;
}

// things which used to be inside cube.h

const static int frontIndices[] = {2, 14, 26, 1, 13, 25, 0, 12, 24};
const static int backIndices[] = {30, 18, 6, 31, 19, 7, 32, 20, 8};
const static int rightIndices[] = {39, 45, 51, 40, 46, 52, 41, 47, 53};
const static int leftIndices[] = {50, 44, 38, 49, 43, 37, 48, 42, 36};
const static int topIndices[] = {5, 17, 29, 4, 16, 28, 3, 15, 27};
const static int bottomIndices[] = {11, 23, 35, 10, 22, 34, 9, 21, 33};

StickerMap * sticker_map_user_input() {
    StickerMap * map = sticker_map_new_identity();
    char inputString[64];
    printf("Colors:\n\
           front: 1\n\
           back:  2\n\
           up:    3\n\
           down:  4\n\
           right: 5\n\
           left:  6\n\n");
    printf("** Enter colors from left-to-right top-to-bottom.**\n");
    printf("Enter front side: ");
    fflush(stdout);
    fgets(inputString, 64, stdin);
    if (!_sticker_map_apply_face_cont(map, frontIndices, inputString, 1)) goto failureHandler;
    printf("Enter back side (Y2): ");
    fflush(stdout);
    fgets(inputString, 64, stdin);
    if (!_sticker_map_apply_face_cont(map, backIndices, inputString, 2)) goto failureHandler;
    printf("Enter up side (Y2 X): ");
    fflush(stdout);
    fgets(inputString, 64, stdin);
    if (!_sticker_map_apply_face_cont(map, topIndices, inputString, 3)) goto failureHandler;
    printf("Enter down side (X2): ");
    fflush(stdout);
    fgets(inputString, 64, stdin);
    if (!_sticker_map_apply_face_cont(map, bottomIndices, inputString, 4)) goto failureHandler;
    printf("Enter right side (X Y): ");
    fflush(stdout);
    fgets(inputString, 64, stdin);
    if (!_sticker_map_apply_face_cont(map, rightIndices, inputString, 5)) goto failureHandler;
    printf("Enter left side (Y2): ");
    fflush(stdout);
    fgets(inputString, 64, stdin);
    if (!_sticker_map_apply_face_cont(map, leftIndices, inputString, 6)) goto failureHandler;
    return map;
    
failureHandler:
    sticker_map_free(map);
    return NULL;
}

StickerMap * sticker_map_identity() {
    StickerMap * map = sticker_map_new_identity();
    _sticker_map_apply_face(map, topIndices, "333333333");
    _sticker_map_apply_face(map, bottomIndices, "444444444");
    _sticker_map_apply_face(map, leftIndices, "666666666");
    _sticker_map_apply_face(map, rightIndices, "555555555");
    _sticker_map_apply_face(map, frontIndices, "111111111");
    _sticker_map_apply_face(map, backIndices, "222222222");
    return map;
}

int sticker_map_faces_solved(StickerMap * map) {
    char faceFront = map->indices[frontIndices[0]];
    char faceBack = map->indices[backIndices[0]];
    char faceRight = map->indices[rightIndices[0]];
    char faceLeft = map->indices[leftIndices[0]];
    char faceTop = map->indices[topIndices[0]];
    char faceBottom = map->indices[bottomIndices[0]];
    int i;
    for (i = 1; i < 9; i++) {
        if (map->indices[frontIndices[i]] != faceFront) faceFront = -1;
        if (map->indices[backIndices[i]] != faceBack) faceBack = -1;
        if (map->indices[rightIndices[i]] != faceRight) faceRight = -1;
        if (map->indices[leftIndices[i]] != faceLeft) faceLeft = -1;
        if (map->indices[topIndices[i]] != faceTop) faceTop = -1;
        if (map->indices[bottomIndices[i]] != faceBottom) faceBottom = -1;
    }
    int count = 0;
    if (faceFront != -1) count += 1;
    if (faceBack != -1) count += 1;
    if (faceRight != -1) count += 1;
    if (faceLeft != -1) count += 1;
    if (faceTop != -1) count += 1;
    if (faceBottom != -1) count += 1;
    return count;
}

int sticker_map_is_solved(StickerMap * map) {
    char faceFront = map->indices[frontIndices[0]];
    char faceBack = map->indices[backIndices[0]];
    char faceRight = map->indices[rightIndices[0]];
    char faceLeft = map->indices[leftIndices[0]];
    char faceTop = map->indices[topIndices[0]];
    char faceBottom = map->indices[bottomIndices[0]];
    int i;
    for (i = 1; i < 9; i++) {
        if (map->indices[frontIndices[i]] != faceFront) return 0;
        if (map->indices[backIndices[i]] != faceBack) return 0;
        if (map->indices[rightIndices[i]] != faceRight) return 0;
        if (map->indices[leftIndices[i]] != faceLeft) return 0;
        if (map->indices[topIndices[i]] != faceTop) return 0;
        if (map->indices[bottomIndices[i]] != faceBottom) return 0;
    }
    return 1;
}

void sticker_map_print(StickerMap * map) {
    int i;
    for (i = 0; i < 54; i++) {
        printf("%d%s", map->indices[i], i == 53 ? "" : " ");
    }
    printf("\n");
}

StickerMap ** sticker_map_standard_face_turns() {
    StickerMap ** operations = malloc(sizeof(StickerMap *) * 18);
    operations[0] = sticker_map_create_top();
    operations[1] = sticker_map_create_bottom();
    operations[2] = sticker_map_create_right();
    operations[3] = sticker_map_create_left();
    operations[4] = sticker_map_create_front();
    operations[5] = sticker_map_create_back();
    int i;
    for (i = 0; i < 6; i++) {
        StickerMap * doubleTurn = sticker_map_new_identity();
        sticker_map_multiply(doubleTurn, operations[i], operations[i]);
        operations[i + 6] = sticker_map_inverse(operations[i]);
        operations[i + 12] = doubleTurn;
    }
    return operations;
}

const char * sticker_map_to_user_string(StickerMap * map) {
    static char buffer[64];
    const int * indicesList[] = {frontIndices, backIndices, topIndices,
                                 bottomIndices, rightIndices, leftIndices};
    bzero(buffer, 64);
    int i;
    for (i = 0; i < 6; i++) {
        int index = 10 * i;
        int j;
        const int * indices = indicesList[i];
        for (j = 0; j < 9; j++) {
            buffer[index + j] = '1' - 1 + map->indices[indices[j]];
        }
        if (i != 5) buffer[index + 9] = '\n';
    }
    return buffer;
}

static int _sticker_map_apply_face(StickerMap * map, const int * indices, const char * str) {
    return _sticker_map_apply_face_cont(map, indices, str, 0);
}

static int _sticker_map_apply_face_cont(StickerMap * map, const int * indices,
                                        const char * str, int color) {
    int i;
    int numRead = 0;
    for (i = 0; i < 9; i++) {
        char c = str[i];
        if (c == '\n' || c == 0) break;
        if (c < '1' || c > '6') return 0;
        unsigned char value = (unsigned char)c - '1' + 1;
        map->indices[indices[i]] = value;
        numRead++;
    }
    if (color < 1 && numRead != 9) return 0;
    for (i = numRead; i < 9; i++) {
        map->indices[indices[i]] = color;
    }
    return 1;
}
