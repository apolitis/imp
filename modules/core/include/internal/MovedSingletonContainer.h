/**
 *  \file MovedSingletonContainer.h
 *  \brief Keep track of the maximum change of a set of attributes.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#ifndef IMPCORE_INTERNAL_MOVED_SINGLETON_CONTAINER_H
#define IMPCORE_INTERNAL_MOVED_SINGLETON_CONTAINER_H

#include <IMP/core/core_config.h>

#include <IMP/ScoreState.h>
#include <IMP/kernel/Particle.h>
#include <IMP/SingletonContainer.h>
#include <IMP/SingletonModifier.h>
#include <IMP/kernel/internal/InternalDynamicListSingletonContainer.h>
#include <IMP/algebra/Sphere3D.h>
#include <IMP/base/Pointer.h>
#include "../XYZR.h"
#include "../rigid_bodies.h"
#include <IMP/kernel/internal/container_helpers.h>
#include <vector>

IMPCORE_BEGIN_INTERNAL_NAMESPACE

class IMPCOREEXPORT MovedSingletonContainer
    : public IMP::internal::ListLikeSingletonContainer {
 private:
  double threshold_;
  base::Pointer<SingletonContainer> pc_;
  int pc_version_;
  bool reset_all_;
  bool reset_moved_;
  virtual kernel::ParticleIndexes get_all_possible_indexes() const IMP_OVERRIDE;
  virtual kernel::ModelObjectsTemp do_get_inputs() const IMP_OVERRIDE;
  virtual void do_before_evaluate() IMP_OVERRIDE;
  virtual kernel::ParticleIndexes get_range_indexes() const IMP_OVERRIDE;
  IMP_OBJECT_METHODS(MovedSingletonContainer);
  virtual kernel::ParticleIndexes do_get_moved() = 0;
  virtual void do_reset_all() = 0;
  virtual void do_reset_moved() = 0;
  virtual kernel::ParticleIndexes do_initialize() = 0;

 public:
  void do_after_evaluate(DerivativeAccumulator *);
  void initialize();
  virtual void validate() const = 0;
  //! Track the changes with the specified keys.
  MovedSingletonContainer(SingletonContainer *pc, double threshold,
                          std::string name);

  //! Measure differences from the current value.
  void reset();

  //! Reset the positions for the moved particles
  void reset_moved();

  //! Return the container storing the particles
  SingletonContainer *get_singleton_container() const { return pc_; }
  void set_threshold(double d);
  double get_threshold() const { return threshold_; }
#ifndef IMP_DOXYGEN
  kernel::Restraints create_decomposition(SingletonScore *) const {
    IMP_NOT_IMPLEMENTED;
  }
#endif
};

class IMPCOREEXPORT XYZRMovedSingletonContainer
    : public MovedSingletonContainer {
  base::Vector<algebra::Sphere3D> backup_;
  base::set<int> moved_;
  virtual kernel::ParticleIndexes do_get_moved();
  virtual void do_reset_all();
  virtual void do_reset_moved();
  virtual kernel::ParticleIndexes do_initialize();

 public:
  virtual void validate() const;
  //! Track the changes with the specified keys.
  XYZRMovedSingletonContainer(SingletonContainer *pc, double threshold);
};

class IMPCOREEXPORT RigidMovedSingletonContainer
    : public MovedSingletonContainer {
  base::Vector<std::pair<algebra::Sphere3D, algebra::Rotation3D> > backup_;
  kernel::ParticleIndexes bodies_;
  base::set<int> moved_;
  IMP::base::map<kernel::ParticleIndex, kernel::ParticleIndexes> rbs_members_;
  virtual kernel::ParticleIndexes do_get_moved();
  virtual void do_reset_all();
  virtual void do_reset_moved();
  virtual kernel::ParticleIndexes do_initialize();
  void do_initialize_particle(kernel::ParticleIndex pi);
  virtual void validate() const;
  void check_estimate(core::RigidBody rbs,
                      std::pair<algebra::Sphere3D, algebra::Rotation3D> s,
                      double d) const;

  double get_distance_estimate(unsigned int i) const {
    core::XYZR xyz(get_model(), bodies_[i]);
    if (!core::RigidBody::get_is_setup(get_model(), bodies_[i])) {
      return (xyz.get_coordinates() - backup_[i].first.get_center())
          .get_magnitude();
    } else {
      core::RigidBody rb(get_model(), bodies_[i]);
      double dr = std::abs(xyz.get_radius() - backup_[i].first.get_radius());
      double dx = (xyz.get_coordinates() - backup_[i].first.get_center())
                      .get_magnitude();
      algebra::Rotation3D nrot =
          rb.get_reference_frame().get_transformation_to().get_rotation();
      algebra::Rotation3D diffrot = backup_[i].second.get_inverse() * nrot;
      double angle = algebra::get_axis_and_angle(diffrot).second;
      double drot =
          std::abs(angle * xyz.get_radius());  // over estimate, but easy
      double ret = dr + dx + drot;
      IMP_IF_CHECK(USAGE_AND_INTERNAL) {
        check_estimate(core::RigidBody(get_model(), bodies_[i]), backup_[i],
                       ret);
      }
      return ret;
    }
  }
  std::pair<algebra::Sphere3D, algebra::Rotation3D> get_data(
      kernel::ParticleIndex p) const {
    if (!core::RigidBody::get_is_setup(get_model(), p)) {
      return std::make_pair(core::XYZR(get_model(), p).get_sphere(),
                            algebra::Rotation3D());
    } else {
      return std::make_pair(core::XYZR(get_model(), p).get_sphere(),
                            core::RigidBody(get_model(), p)
                                .get_reference_frame()
                                .get_transformation_to()
                                .get_rotation());
    }
  }
  kernel::ParticlesTemp get_input_particles() const;

 public:
  //! Track the changes with the specified keys.
  RigidMovedSingletonContainer(SingletonContainer *pc, double threshold);
};
IMP_OBJECTS(MovedSingletonContainer, MovedSingletonContainers);

IMPCORE_END_INTERNAL_NAMESPACE

#endif /* IMPCORE_INTERNAL_MOVED_SINGLETON_CONTAINER_H */
