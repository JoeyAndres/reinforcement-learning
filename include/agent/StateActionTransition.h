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

#include <cassert>
#include <algorithm>
#include <iostream>
#include <random>
#include <functional>

#include "../declares.h"
#include "../algorithm/QLearning.h"
#include "StateActionTransitionException.h"

namespace rl {
namespace agent {

/*! \class StateActionTransition
 *  \brief Used for <i>modeling</i> environment.
 *
 * This module is represents the possible transition states for some state. Every
 * call to StateTransition::update(state, reward), will increase the value of its
 * frequency and at the same time update the reward value. Call to getNextState
 * would then would return a state on the basis of how likely it occurs. Call to
 * getReward would return the reward of the given state.
 */
template<class S>
class StateActionTransition {
 public:
  /**
   * @param greedy determines how random is getNextState() is. A value of
   * 1.0 means getNextState returns based on current likelihood of a state
   * occuring (not random). With 0.0, it will not rely on the likelihood
   * of state and return a random nextstate.
   * @param stepSize determines how the frequency is updated. A low value
   * yields to a more accurate model of the environment but slower in learning
   * environment. A value of 1.0 yields to forgeting the frequency information
   * of all other transition states, suitable for deterministic environment.
   */
  StateActionTransition(const rl::FLOAT, const rl::FLOAT stepSize);

  /**
   * Copy-constructor.
   * @param sat StateActionTransition to copy.
   */
  StateActionTransition(const StateActionTransition& sat);

  /**
   * By adding a new state, this updates the information of all other states.
   * Frequency of all state, not nextState will decrease and frequency of
   * nextState will increase. The reward will update the value of the nextState.
   * @param nextState to be added or increased frequency value and update reward value.
   * @param reward to update the value of the nextState.
   */
  virtual void update(const spState<S>& nextState, const rl::FLOAT reward);

  /**
   * Given a state returns its latest reward info.
   * @param state to be queried of reward.
   * @return reward of the state.
   * @throw StateActionTransitionException when given state don't exist.
   */
  virtual rl::FLOAT getReward(const spState<S>& state) const
  throw(StateActionTransitionException);

  /**
   * @return the possible next state based on its value. Bigger value, better
   * chance of occuring.
   * @throw StateActionTransitionException when given state don't exist.
   */
  virtual const spState<S>& getNextState() const
  throw(StateActionTransitionException);

  /**
   * @return the number of transition states.
   */
  size_t getSize() const;

  /**
   * @param stepSize change the current step size.
   */
  void setStepSize(rl::FLOAT stepSize);

  /**
   * @return current step size.
   */
  rl::FLOAT getStepSize() const;

  /**
   * @param greedy change the greediness of the current state transition.
   */
  void setGreedy(rl::FLOAT greedy);

  /**
   * @return current greediness.
   */
  rl::FLOAT getGreedy() const;

 private:
  bool _findState(const spState<S>& state) const;

 private:
  // Keeps track of all the possible transistion states and their
  // corresponding frequency and reward.
  spStateXMap<S, FLOAT> _stateActionTransitionFrequency;
  spStateXMap<S, FLOAT> _stateActionTransitionReward;

  rl::FLOAT _greedy;
  rl::FLOAT _stepSize;

  mutable std::random_device _randomDevice;
};

template<class S>
StateActionTransition<S>::StateActionTransition(const rl::FLOAT greedy,
                                                const rl::FLOAT stepSize)
    : _greedy(greedy),
      _stepSize(stepSize) {
}

template<class S>
StateActionTransition<S>::StateActionTransition(
    const StateActionTransition& sat)
    : _greedy(sat._greedy),
      _stepSize(sat._stepSize),
      _stateActionTransitionFrequency(sat._stateActionTransitionFrequency),
      _stateActionTransitionReward(sat._stateActionTransitionReward) {
}

template<class S>
void StateActionTransition<S>::update(const spState<S>& nextState,
                                      const rl::FLOAT reward) {
  _stateActionTransitionFrequency.insert(
    spStateAndReward<S>(nextState, 0.0F));
  _stateActionTransitionReward.insert(
    spStateAndReward<S>(nextState, reward));

  // Update Frequency.
  // --Lower the value of all other frequencies.
  for (auto iter = _stateActionTransitionFrequency.begin();
       iter != _stateActionTransitionFrequency.end(); iter++) {
    auto state = iter->first;
    if (state != nextState) {
      _stateActionTransitionFrequency[state] =
          _stateActionTransitionFrequency[state]
              + _stepSize * (0.0F - _stateActionTransitionFrequency[state]);
    }
  }

  // --Increase the value of the next frequency.
  _stateActionTransitionFrequency[nextState] =
      _stateActionTransitionFrequency[nextState]
          + _stepSize * (1.0F - _stateActionTransitionFrequency[nextState]);

  // Note, no need to lower the reward. Updating the reward will suffice.
  _stateActionTransitionReward[nextState] =
      _stateActionTransitionReward[nextState]
          + 1.0F * (reward - _stateActionTransitionReward[nextState]);

  assert(
      _stateActionTransitionFrequency.size()
          == _stateActionTransitionReward.size());
}

template<class S>
rl::FLOAT StateActionTransition<S>::getReward(const spState<S>& state) const
throw(StateActionTransitionException) {
  StateActionTransitionException exception("state not yet added.");
  if (_findState(state) == false) {
    throw exception;
  }

  return _stateActionTransitionReward.at(state);;
}

template<class S>
bool StateActionTransition<S>::_findState(const spState<S>& state) const {
  bool found = _stateActionTransitionFrequency.find(state)
      != _stateActionTransitionFrequency.end();
  return found;
}

template<class S>
const spState<S>& StateActionTransition<S>::getNextState() const
throw(StateActionTransitionException) {
  StateActionTransitionException exception("nextStates are empty.");
  if (_stateActionTransitionFrequency.size() == 0) {
    throw exception;
  }

  // Get a random number in [0.0,freqValSum].
  std::uniform_real_distribution<rl::FLOAT> distributionRandomSelection(0.0F,
                                                                        1.0F);
  rl::FLOAT randomNumberRandomSelection = distributionRandomSelection(
      _randomDevice);

  // If randomNumberRandomSelection > greediness, return a random state.
  if (randomNumberRandomSelection > _greedy) {
    auto it = _stateActionTransitionFrequency.begin();
    std::advance(it, _randomDevice() % _stateActionTransitionFrequency.size());
    return it->first;
  }

  // http://stackoverflow.com/questions/1761626/weighted-random-numbers
  // Acquire the sum of all the frequency values.
  rl::FLOAT freqValSum = 0.0F;
  for (auto iter = _stateActionTransitionFrequency.begin();
      iter != _stateActionTransitionFrequency.end(); iter++) {
    freqValSum += iter->second;
  }

  // Get a random number in [0.0,freqValSum] for weighted selection.
  std::uniform_real_distribution<rl::FLOAT> distributionWeightedSelection(
      0.0F, freqValSum);
  rl::FLOAT randomNumberWeightedSelection = distributionWeightedSelection(
      _randomDevice);

  // Get the one that matches the random value.
  auto iter = _stateActionTransitionFrequency.begin();
  for (; iter != _stateActionTransitionFrequency.end(); iter++) {
    rl::FLOAT currentFreq = iter->second;

    // If current frequency is greater than highest frequency and at the
    // time lower than the current random number then
    // Error = randomNumber-highestFreq is of the lowest, therefore,
    // a candidate.
    if (currentFreq > randomNumberWeightedSelection) {
      return iter->first;
    }

    randomNumberWeightedSelection -= currentFreq;
  }

  assert(false);  // Note supposed to reach here.
}

template<class S>
size_t StateActionTransition<S>::getSize() const {
  return _stateActionTransitionFrequency.size();
}

template<class S>
void StateActionTransition<S>::setStepSize(rl::FLOAT stepSize) {
  this->_stepSize = stepSize;
}

template<class S>
rl::FLOAT StateActionTransition<S>::getStepSize() const {
  return _stepSize;
}

template<class S>
void StateActionTransition<S>::setGreedy(rl::FLOAT greedy) {
  this->_greedy = greedy;
}

template<class S>
rl::FLOAT StateActionTransition<S>::getGreedy() const {
  return _greedy;
}

}  // namespace agent
}  // namespace rl
