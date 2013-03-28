main: legacy_build test_build korfsolver_build

legacy_build: representation_build
	cd legacy && make

test_build: indexing_build
	cd test && make

korfsolver_build: indexing_build
	cd korfsolver && make

indexing_build: representation_build
	cd indexing && make

representation_build:
	cd representation && make

clean:
	rm brutesolve
	rm facesolve
	cd representation && make clean
	cd indexing && make clean
	cd korfsolver && make clean
	cd test && make clean
