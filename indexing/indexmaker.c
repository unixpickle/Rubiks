#include "configcounter.h"
#include "subproblemindices.h"

void writeConfigurations(FILE * file, CCTableNode * node, CCUserInfo * info);

int main (int argc, const char * argv[]) {
	const char * outputFile = NULL;
	int depth = 0;
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <output> <depth> <subproblem>\n\
\n\nList of subproblems:\n\
- 'corners' - the corner subproblem\n\
- 'edgefront' - the front and top edge pieces\n\
- 'edgeback' - the back and bottom edge pieces\n\
- 'edgeall' - all of the twelve edges\n\n", argv[0]);
		return 1;
	}
	outputFile = argv[1];
	depth = atoi(argv[2]);
	const char * subproblem = argv[3];


    CCUserInfo info;
    info.indexTableDepth = 5; // provides a good split
	if (strcmp(subproblem, "corners") == 0) {
    	info.significantIndexCount = 24;
    	info.significantIndices = CubeCornerIndices;
        info.discardBySignificant = 1;
	} else if (strcmp(subproblem, "edgefront") == 0) {
		printf("performing front edge search\n");
		info.significantIndexCount = 12;
		info.significantIndices = CubeFrontIndices;
        info.discardBySignificant = 0;
	} else if (strcmp(subproblem, "edgeback") == 0) {
		info.significantIndexCount = 12;
		info.significantIndices = CubeBackIndices;
        info.discardBySignificant = 0;
    } else if (strcmp(subproblem, "edgeall") == 0) {
        unsigned char indices[24];
        memcpy(indices, CubeBackIndices, 12);
        memcpy(&indices[12], CubeFrontIndices, 12);
        info.significantIndexCount = 24;
        info.significantIndices = indices;
        info.discardBySignificant = 1;
	} else {
		fprintf(stderr, "Unknown subproblem.\n");
		return 1;
	}
	RubiksMap * map = cube_identity();
    info.maximumDepth = depth;
    info.baseMap = map;
	CCTableNode * node = cc_compute_table(info);
	rubiks_map_free(map);
	printf("saving results...\n");
    
	FILE * fp = fopen(outputFile, "w");
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

