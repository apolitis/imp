/**
 *  \file Sampler.cpp
 *  \brief Storage of a model, its restraints,
 *                         constraints and particles.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/Sampler.h"
#include <limits>

IMP_BEGIN_NAMESPACE


Sampler::Sampler(Model *m,
                 std::string nm): Object(nm),
                                  model_(m){
  sf_= m->create_model_scoring_function();
}

ConfigurationSet *Sampler::get_sample() const {
  IMP_OBJECT_LOG;
  set_was_used(true);
  //IMP_LOG(TERSE, "Sampling " << num_opt << " particles."<<std::endl);
  /*if (num_opt == 0) {
    IMP_WARN("There are no particles to optimize."<<std::endl);
    return NULL;
    }*/
  return do_sample();
}

void Sampler::set_scoring_function(ScoringFunction *sf) {
  sf_=sf;
}

Sampler::~Sampler(){}

IMP_END_NAMESPACE
