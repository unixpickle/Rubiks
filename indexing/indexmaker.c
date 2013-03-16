#include "cubecorners.h"

void writeConfigurations(FILE * file, char * pastColors, int pastCount, TableNode * node);
void configurationWriteLine(FILE * file, char * pastColors, int pastCount, int moveCount);

int main (int argc, const char * argv[]) {
	RubiksMap * map = cube_identity();
	TableNode * node = cubecorners_fewest_moves_table(map);
	rubiks_map_free(map);
	FILE * fp = fopen("output.dat", "w");
	writeConfigurations(fp, NULL, 0, node);
	fclose(fp);
	return 0;
}

void writeConfigurations(FILE * file, char * pastColors, int pastCount, TableNode * node) {
	int i;
	char * nextColors = (char *)malloc(pastCount + 1);
	if (pastCount > 0) {
		memcpy(nextColors, pastColors, pastCount);
	}
	for (i = 0; i < 6; i++) {
		TableNode * subnode = node->subnodes[i];
		if (subnode) {
			nextColors[pastCount] = i;
			if (subnode->moveCount >= 0) {
				if (subnode->moveCount == 0) {
					printf("move count 0, maxDepth %d\n", subnode->maxDepth);
				}
				configurationWriteLine(file, nextColors, pastCount + 1, subnode->moveCount);
			} else {
				writeConfigurations(file, nextColors, pastCount + 1, subnode);
			}
		}
	}
	free(nextColors);
}

void configurationWriteLine(FILE * file, char * pastColors, int pastCount, int moveCount) {
	int i;
	fwrite(&pastCount, 4, 1, file);
	fwrite(pastColors, 1, pastCount, file);
	fwrite(&moveCount, 4, 1, file);
}

