main: math_build util_build legacy_build test_build korfsolver_build pocketcube_build solver_build roots_build random_build

roots_build: representation_build math_build util_build
	cd math/roots && make

random_build: representation_build math_build util_build
	cd math/random && make

math_build:
	cd math && make

pocketcube_build: representation_build indexing_build
	cd pocketcube && make

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

util_build:
	cd util && make

solver_build:
	cd solver && make

clean:
	cd math && make clean
	cd util && make clean
	cd legacy && make clean
	cd representation && make clean
	cd indexing && make clean
	cd korfsolver && make clean
	cd test && make clean
	cd pocketcube && make clean
	cd solver && make clean
	cd math/roots && make clean
	cd math/random && make clean
