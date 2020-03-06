#define CATCH_CONFIG_RUNNER
#include "../../../Common/Lib/Ext/Catch2/catch.hpp"
#include "CompressonatorTests.h"

int main(int argc, char* argv[]) {
	AssignExpectedColorsToBlocks();
	int result = Catch::Session().run(argc, argv);

	return result;
}