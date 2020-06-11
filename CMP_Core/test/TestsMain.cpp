#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include "CompressonatorTests.h"

int main(int argc, char* argv[]) {
	AssignExpectedColorsToBlocks();
	int result = Catch::Session().run(argc, argv);

	return result;
}