#include "configcounter.h"

void writeConfigurations(FILE * file, CCTableNode * node, CCUserInfo * info);

static unsigned const char CubeCornerIndices[] = {2, 26, 0, 24, 30, 6, 32, 8, 5, 29, 3, 27, 11, 35, 9, 33, 39, 51, 41, 53, 50, 38, 48, 36};

int main (int argc, const char * argv[]) {
	const char * outputFile = NULL;
	int depth = 0;
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <output> <depth>\n", argv[0]);
		return 1;
	}
	outputFile = argv[1];
	depth = atoi(argv[2]);

	RubiksMap * map = cube_identity();
    CCUserInfo info;
    info.significantIndexCount = 24;
    info.indexTableDepth = 5; // provides a good split
    info.maximumDepth = depth;
    info.baseMap = map;
    info.significantIndices = CubeCornerIndices;
	CCTableNode * node = cc_compute_table(info);
	rubiks_map_free(map);
    
	FILE * fp = fopen("output.dat", "w");
	writeConfigurations(fp, node, &info);
	fclose(fp);
	return 0;
}

void writeConfigurations(FILE * file, CCTableNode * node, CCUserInfo * info) {
	int i;
	if (node->rawEntries) {
		int entrySize = info->significantIndexCount + info->maximumDepth + 2;
		int count = node->rawEntriesCount;
		fwrite(node->rawEntries, entrySize, count, file);
	}
	for (i = 0; i < 6; i++) {
		CCTableNode * subnode = node->subnodes[i];
		if (subnode) {
			writeConfigurations(file, subnode, info);
		}
	}
}

