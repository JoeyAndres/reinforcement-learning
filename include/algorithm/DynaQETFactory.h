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

#include "ReinforcementLearningFactory.h"
#include "LearningAlgorithm.h"
#include "DynaQET.h"

namespace rl {
namespace algorithm {

/*!\class DynaQET
 * \brief Factory method for DynaQET.
 * \tparam S State data type.
 * \tparam A Action data type.
 */
template<class S, class A>
class DynaQETFactory : public ReinforcementLearningFactory<S, A> {
 public:
  DynaQETFactory(rl::FLOAT stepSize,
                 rl::FLOAT discountRate,
                 const policy::spPolicy<S, A>& policy,
                 rl::UINT simulationIterationCount,
                 rl::FLOAT stateTransitionGreediness,
                 rl::FLOAT stateTransitionStepSize,
                 rl::FLOAT lambda) {
    this->_instance = spReinforcementLearning<S, A>(
      new DynaQET<S, A>(
        stepSize,
        discountRate,
        policy,
        simulationIterationCount,
        stateTransitionGreediness,
        stateTransitionStepSize,
        lambda));
  }
};

}  // namespace algorithm
}  // namespace rl
