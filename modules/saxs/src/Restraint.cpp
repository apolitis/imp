/**
 *  \file kernel::Restraint.h
 *  \brief Calculate score based on fit to SAXS profile.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include <IMP/saxs/Restraint.h>
#include <IMP/base/log.h>

#include <IMP/atom/Hierarchy.h>
#include <IMP/core/LeavesRefiner.h>

IMPSAXS_BEGIN_NAMESPACE

Restraint::Restraint(const kernel::Particles& particles, const Profile* exp_profile,
                     FormFactorType ff_type) :
    IMP::Restraint(IMP::internal::get_model(particles), "SAXS restraint"),
    ff_type_(ff_type) {

  profile_fitter_ = new ProfileFitter<ChiScore>(exp_profile);
  derivative_calculator_ = new DerivativeCalculator(exp_profile);

  IMP::base::map<kernel::ParticleIndex, kernel::Particles> rigid_bodies;
  for(unsigned int i=0; i< particles.size(); ++i) {
    if(core::RigidMember::get_is_setup(particles[i])) {
      kernel::ParticleIndex pi =
        core::RigidMember(particles[i]).get_rigid_body().get_particle_index();
      rigid_bodies[pi].push_back(particles[i]);
    } else {
      if(atom::Atom::get_is_setup(particles[i])) {
        particles_.push_back(particles[i]);
      }
    }
  }

  rigid_bodies_profile_ = new Profile();
  for(IMP::base::map<kernel::ParticleIndex, kernel::Particles>::iterator it =
        rigid_bodies.begin(); it!= rigid_bodies.end(); it++) {
    rigid_bodies_.push_back(it->second);
    // compute non-changing profile
    IMP_NEW(Profile, rigid_part_profile, ());
    rigid_part_profile->calculate_profile(rigid_bodies_.back(), ff_type);
    rigid_bodies_profile_->add(rigid_part_profile);
  }

  IMP_LOG_TERSE("SAXS kernel::Restraint constructor: " << particles_.size()
           << " atom particles " << rigid_bodies_.size() << " rigid bodies\n");
}


ModelObjectsTemp Restraint::do_get_inputs() const {
  kernel::ModelObjectsTemp pts(particles_.begin(), particles_.end());
  unsigned int sz=pts.size();
  for (unsigned int i=0; i< sz; ++i) {
    pts.push_back(atom::Hierarchy(particles_[i]).get_parent());
  }
  for (unsigned int i=0; i< rigid_bodies_.size(); ++i) {
    pts.insert(pts.end(), rigid_bodies_[i].begin(), rigid_bodies_[i].end());
    for (unsigned int j=0; j< rigid_bodies_[i].size(); ++j) {
      // add the residue particle since that is needed too
      pts.push_back(atom::Hierarchy(rigid_bodies_[i][j]).get_parent());
    }
  }
  return pts;
}

void Restraint::compute_profile(Profile* model_profile) {
  // add non-changing profile
  model_profile->add(rigid_bodies_profile_);
  IMP_NEW(Profile, profile, (model_profile->get_min_q(),
                             model_profile->get_max_q(),
                             model_profile->get_delta_q()));
  // compute inter-rigid bodies contribution
  for(unsigned int i=0; i<rigid_bodies_.size(); i++) {
    for(unsigned int j=i+1; j<rigid_bodies_.size(); j++) {
      profile->calculate_profile(rigid_bodies_[i], rigid_bodies_[j], ff_type_);
      model_profile->add(profile);
    }
  }
  // compute non rigid body particles contribution
  if(particles_.size() > 0) {
    profile->calculate_profile(particles_, ff_type_);
    model_profile->add(profile);
    // compute non rigid body particles - rigid bodies contribution
    for(unsigned int i=0; i<rigid_bodies_.size(); i++) {
      profile->calculate_profile(rigid_bodies_[i], particles_, ff_type_);
      model_profile->add(profile);
    }
  }
}

//! Calculate the score and the derivatives for particles of the restraint.
/** \param[in] acc If true (not nullptr), partial first derivatives should be
                          calculated.
    \return score associated with this restraint for the given state of
            the model.
*/
double Restraint::unprotected_evaluate(DerivativeAccumulator *acc) const {

  IMP_LOG_TERSE("SAXS kernel::Restraint::evaluate score\n");

  IMP_NEW(Profile, model_profile,());
  const_cast<Restraint*>(this)->compute_profile(model_profile);
  Float score = profile_fitter_->compute_score(model_profile);
  bool calc_deriv = acc? true: false;
  if(!calc_deriv) return score;

  IMP_LOG_TERSE("SAXS kernel::Restraint::compute derivatives\n");

  // do we need to resample the curve since it's just been created??
  // yes, since it does not correspond to the experimental one
  IMP_NEW(Profile, resampled_profile, ());
  profile_fitter_->resample(model_profile, resampled_profile);

  bool use_offset = false;
  std::vector<double> effect_size; //gaussian model-specific derivative weights
  effect_size = derivative_calculator_->compute_gaussian_effect_size(
          resampled_profile, profile_fitter_, use_offset);
  derivative_calculator_->compute_all_derivatives(particles_, rigid_bodies_,
          rigid_bodies_decorators_, resampled_profile, effect_size, acc);

  IMP_LOG_TERSE("SAXS kernel::Restraint::done derivatives, score "
                << score << "\n");
  return score;
}

IMPSAXS_END_NAMESPACE
