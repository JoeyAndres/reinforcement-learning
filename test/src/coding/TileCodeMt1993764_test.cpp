#include <vector>
#include <iostream>

#include "rl"

#include "../../lib/catch.hpp"

using namespace rl;
using namespace rl::algorithm;
using namespace rl::algorithm;
using namespace std;

SCENARIO("TileCodeMt1993764 retrieves the correct feature vector",
         "[TileCodeMt1993764]") {
  GIVEN("TileCodeMt1993764 instance") {
    vector<DimensionInfo<rl::FLOAT> > dimensionalInfoVector = {
      DimensionInfo<rl::FLOAT>(-0.5F, 0.5F, 3, 0),
      DimensionInfo<rl::FLOAT>(-0.5F, 0.5F, 3, 0)
    };

    TileCodeMt1993764 tileCode(dimensionalInfoVector, 4, 100);

    WHEN ("TileCodeMt1993764::getDimension is called.") {
      THEN ("Returns 3") {
        REQUIRE(tileCode.getDimension() == 2);
      }
    }

    WHEN ("TileCodeMt1993764::getSize is called.") {
      THEN ("Returns 100") {
        REQUIRE(tileCode.getSize() == 100);
      }
    }

    // The rest are hash in which I don't want to predict. They are all tested
    // by TileCode_test.cpp anyway.
  }
}