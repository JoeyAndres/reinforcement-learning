/**
 * rl - Reinforcement Learning
 * Copyright (C) 2016  Joey Andres<yeojserdna@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cmath>
#include <vector>
#include <array>

#include "TileCode.h"

using std::vector;
using std::array;

namespace rl {
namespace coding {

/*! \class TileCodeCorrect
 *  \brief The recommend implementation of Tile Coding for medium size state-space.
 *
 *  This is called TileCodeCorrect, since feature vector are generated by careful
 *  offsetting ensuring the feature vectors are unique, in contrast to hashing alternatives
 *  in which there might be a collision thus feature vectors might not be unique. Although
 *  it is correct, it consumes more space than hashing alternative, making it impractical
 *  for high dimensional state-spaces.
 *
 *  \tparam D Number of dimension.
 *  \tparam NUM_TILINGS Number of tilings.
 */
template<size_t D, size_t NUM_TILINGS>
class TileCodeCorrect : public TileCode<D, NUM_TILINGS> {
 public:
  using TileCode<D, NUM_TILINGS>::TileCode;

  FEATURE_VECTOR getFeatureVector(
    const floatArray<D>& parameters) override;
};

template<size_t D, size_t NUM_TILINGS>
FEATURE_VECTOR TileCodeCorrect<D, NUM_TILINGS>::getFeatureVector(
  const floatArray<D>& parameters) {
  FEATURE_VECTOR fv;
  fv.resize(NUM_TILINGS);

  for (rl::INT i = 0; i < NUM_TILINGS; i++) {
    // x1 + x2*x1.gridSize + x3*x1.gridSize*x2.gridSize + ...
    rl::INT hashedIndex = 0;
    rl::INT mult = 1;
    for (size_t j = 0; j < this->getDimension(); j++) {
      hashedIndex += this->paramToGridValue(parameters.at(j), i, j) * mult;
      mult *= this->_dimensionalInfos[j].GetGridCountReal();
    }

    hashedIndex += mult * i;
    assert(hashedIndex <= this->_sizeCache
    /* Size cache exceeded. Illegal feature vector value. */);
    fv[i] = hashedIndex;
  }
  return fv;
}

}  // namespace coding
}  // namespace rl
