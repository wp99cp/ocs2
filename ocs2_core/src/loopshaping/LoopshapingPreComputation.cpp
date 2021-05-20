/******************************************************************************
Copyright (c) 2020, Ruben Grandia. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include <ocs2_core/loopshaping/LoopshapingPreComputation.h>

namespace ocs2 {

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
LoopshapingPreComputation::LoopshapingPreComputation(const PreComputation& systemPreComputation,
                                                     std::shared_ptr<LoopshapingDefinition> loopshapingDefinition)
    : systemPreCompPtr_(systemPreComputation.clone()),
      filteredSystemPreCompPtr_(systemPreComputation.clone()),
      loopshapingDefinition_(std::move(loopshapingDefinition)) {}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
LoopshapingPreComputation::LoopshapingPreComputation(const LoopshapingPreComputation& other)
    : loopshapingDefinition_(other.loopshapingDefinition_) {
  if (other.systemPreCompPtr_ != nullptr) {
    systemPreCompPtr_.reset(other.systemPreCompPtr_->clone());
    filteredSystemPreCompPtr_.reset(other.filteredSystemPreCompPtr_->clone());
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
LoopshapingPreComputation* LoopshapingPreComputation::clone() const {
  return new LoopshapingPreComputation(*this);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
void LoopshapingPreComputation::request(Request requestFlags, scalar_t t, const vector_t& x, const vector_t& u) {
  x_system_ = loopshapingDefinition_->getSystemState(x);
  u_system_ = loopshapingDefinition_->getSystemInput(x, u);
  x_filter_ = loopshapingDefinition_->getFilterState(x);
  u_filter_ = loopshapingDefinition_->getFilteredInput(x, u);

  if (systemPreCompPtr_ != nullptr) {
    systemPreCompPtr_->request(requestFlags, t, x_system_, u_system_);
    if (requestFlags & Request::Cost) {
      // state-input cost function is evaluated on both u_system and u_filter.
      filteredSystemPreCompPtr_->request(requestFlags, t, x_system_, u_filter_);
    }
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
void LoopshapingPreComputation::requestPreJump(Request requestFlags, scalar_t t, const vector_t& x) {
  x_system_ = loopshapingDefinition_->getSystemState(x);
  x_filter_ = loopshapingDefinition_->getFilterState(x);

  if (systemPreCompPtr_ != nullptr) {
    systemPreCompPtr_->requestPreJump(requestFlags, t, x_system_);
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
void LoopshapingPreComputation::requestFinal(Request requestFlags, scalar_t t, const vector_t& x) {
  x_system_ = loopshapingDefinition_->getSystemState(x);
  x_filter_ = loopshapingDefinition_->getFilterState(x);

  if (systemPreCompPtr_ != nullptr) {
    systemPreCompPtr_->requestFinal(requestFlags, t, x_system_);
  }
}

}  // namespace ocs2
