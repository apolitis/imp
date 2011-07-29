/**
 *  \file ClosePairContainer.cpp   \brief A list of ParticlePairs.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. Close rights reserved.
 *
 */

#include "IMP/core/internal/CoreClosePairContainer.h"
#include <IMP/core/BoxSweepClosePairsFinder.h>
#include <IMP/core/GridClosePairsFinder.h>
#include <IMP/core/internal/CoreListPairContainer.h>
#include <IMP/core/internal/pair_helpers.h>
#include <IMP/core/internal/close_pairs_helpers.h>
#include <IMP/algebra/internal/tnt_array2d.h>
#include <IMP/PairModifier.h>
#include <IMP/utility.h>
#include <algorithm>


#include <IMP/core/RigidClosePairsFinder.h>
#include <IMP/core/rigid_bodies.h>


IMPCORE_BEGIN_INTERNAL_NAMESPACE

IntKey InList::key_= IntKey("in list temp");

IMP_LIST_IMPL(CoreClosePairContainer,
              PairFilter,
              pair_filter,
              PairFilter*,
              PairFilters);


CoreClosePairContainer::CoreClosePairContainer(SingletonContainer *c,
                                                 double distance,
                                                 ClosePairsFinder *cpf,
                                       double slack):
  internal::ListLikePairContainer(c->get_model(), "ClosePairContainer") {
 initialize(c, distance, slack,
             cpf);
}

CoreClosePairContainer::~CoreClosePairContainer()
{
}

void CoreClosePairContainer::initialize(SingletonContainer *c, double distance,
                                         double slack,
                                         ClosePairsFinder *cpf) {
  initialize_active_container(get_model());
  moved_count_=0;
  slack_=slack;
  distance_=distance;
  c_=c;
  cpf_=cpf;
  cpf_->set_distance(distance_+2*slack_);
  first_call_=true;
  moved_=cpf_->get_moved_singleton_container(c_, slack_);
}

IMP_ACTIVE_CONTAINER_DEF(CoreClosePairContainer,
                         {
                           IMP_CHECK_OBJECT(moved_);
                           IMP_CHECK_OBJECT(this);
                           moved_->set_log_level(l);
                         });


void CoreClosePairContainer::set_slack(double s) {
  slack_=s;
  cpf_->set_distance(distance_+2*slack_);
  ParticleIndexPairs et;
  update_list(et);
  first_call_=true;
}


ContainersTemp CoreClosePairContainer
::get_state_input_containers() const {
  ContainersTemp ret= cpf_->get_input_containers(c_->get_particles());
  ret.push_back(c_);
  ret.push_back(moved_);
  return ret;
}


ParticlesTemp CoreClosePairContainer::get_state_input_particles() const {
  ParticlesTemp ret(cpf_->get_input_particles(c_->get_particles()));
  if (get_number_of_pair_filters() >0) {
    ParticlesTemp retc;
    for (PairFilterConstIterator it= pair_filters_begin();
         it != pair_filters_end(); ++it) {
      for (unsigned int i=0; i< ret.size(); ++i) {
        ParticlesTemp cur= (*it)->get_input_particles(ret[i]);
        retc.insert(retc.end(), cur.begin(), cur.end());
      }
    }
    ret.insert(ret.end(), retc.begin(), retc.end());
  }
  return ret;
}

void CoreClosePairContainer::check_duplicates_input() const {
  ParticlesTemp ps= c_->get_particles();
  std::sort(ps.begin(), ps.end());
  IMP_USAGE_CHECK(std::unique(ps.begin(), ps.end())==ps.end(),
                  "Duplicates in input");
}

void CoreClosePairContainer::check_list(bool check_slack) const {
  IMP_IF_CHECK(USAGE_AND_INTERNAL) {
    ParticleIndexPairs cur= get_access();
    IMP::compatibility::set<ParticleIndexPair> existings(cur.begin(),
                                                         cur.end());
    unsigned int num=cur.size();
    if(0) std::cout << num;
    for (unsigned int j=0; j< num; ++j) {
      for (unsigned int i=0; i< get_number_of_pair_filters(); ++i) {
        IMP_INTERNAL_CHECK(!get_pair_filter(i)->get_contains(get_model(),
                                                             cur[j]),
                           "Pair " << cur[j]
                           << " should have been filtered by filter "
                           << get_pair_filter(i)->get_name());
        IMP_INTERNAL_CHECK(!get_pair_filter(i)
                           ->get_contains(get_model(),
                                          ParticleIndexPair((cur[j])[0],
                                                            (cur[j])[1])),
                           "Filter is not symmetric on pair " << cur[j]
                           << get_pair_filter(i)->get_name());
      }
      IMP_INTERNAL_CHECK(existings.find(ParticleIndexPair((cur[j])[1],
                                                          (cur[j])[0]))
                         == existings.end(),
                         "Can't have both a pair and its reverse in the list"
                         << cur[j]);
    }
    IMP_INTERNAL_CHECK(existings.size() == num,
                       "Not all particle pairs in list are unique: "
                       << num
                       << " vs " << existings.size() << std::endl);
    double threshold=distance_-.1;
    if (check_slack) {
      threshold+=2*slack_;
    }
    for (unsigned int i=0; i< c_->get_number_of_particles(); ++i) {
      for (unsigned int j=0; j< i; ++j) {
        XYZR a(c_->get_particle(i)), b(c_->get_particle(j));
        double d= core::get_distance(a,b);
        if (d < threshold) {
          if (RigidMember::particle_is_instance(a)
              && RigidMember::particle_is_instance(b)
              && RigidMember(a).get_rigid_body()
              == RigidMember(b).get_rigid_body())
            continue;
          ParticleIndexPair pp(a->get_index(),b->get_index());
          ParticleIndexPairs pps(1, pp);
          internal::filter_close_pairs(this, pps);
          IMP_INTERNAL_CHECK(pps.empty()
                             || existings.find(pp) != existings.end()
                             || existings.find(ParticleIndexPair(pp[1], pp[0]))
                             != existings.end(), "Particle pair "
                             << a->get_name()
                             << " and " << b->get_name()
                             << " not found in list: "
                             << IMP::core::XYZR(a) << std::endl
                             << IMP::core::XYZR(b)
                           << " distance "
                             << core::get_distance(a, b)
                             << " vs " << distance_ << ". "
                             << " info " << moved_->get_distance_moved(a)
                             << " and " << moved_->get_distance_moved(b));
        }
      }
    }
    if (dynamic_cast<RigidClosePairsFinder*>(cpf_.get())) {
      for (ParticlePairIterator it=particle_pairs_begin();
           it != particle_pairs_end(); ++it) {
        if (RigidMember::particle_is_instance((*it)[0])
            && RigidMember::particle_is_instance((*it)[1])) {
          IMP_INTERNAL_CHECK(RigidMember((*it)[0]).get_rigid_body()
                             != RigidMember((*it)[1]).get_rigid_body(),
                             "Pair should not have two particles from the same "
                             << "rigid body");
        }
      }
    }
  }
}

namespace {
  struct Found {
    ParticlesTemp ps_;
    Found(const ParticlesTemp &ps): ps_(ps){
      std::sort(ps_.begin(), ps_.end());
    }
    bool operator()(const ParticlePair &pp) const {
      if (std::binary_search(ps_.begin(), ps_.end(), pp[0])
          || std::binary_search(ps_.begin(), ps_.end(), pp[1])) {
        return true;
      } else {
        return false;
      }
    }
  };
}

void CoreClosePairContainer::do_first_call() {
  IMP_LOG(TERSE, "Handling first call of ClosePairContainer." << std::endl);
  IMP_IF_CHECK(USAGE_AND_INTERNAL) {
    check_duplicates_input();
  }
  do_rebuild();
  first_call_=false;
}

void CoreClosePairContainer::do_incremental() {
  ParticleIndexes moved=moved_->get_indexes();
  IMP_LOG(TERSE, "Handling incremental update of ClosePairContainer"
          << std::endl);
  using IMP::operator<<;
  IMP_LOG(VERBOSE, "Moved " << moved << std::endl);
  ParticleIndexPairs ret= cpf_->get_close_pairs(get_model(),
                                                c_->get_indexes(),
                                                moved);
  ParticleIndexPairs ret1= cpf_->get_close_pairs(get_model(), moved);
  ret.insert(ret.begin(), ret1.begin(), ret1.end());
  internal::fix_order(ret);
  // make one pass
  internal::filter_close_pairs(this, ret);
  if (false) {
    /*internal::filter_same(ret, moved_->get_particles());
    //internal::filter_far(ret, get_distance());
    remove_from_list_if(Found(moved_->get_particles()));
    IMP_LOG(TERSE, "Found " << ret.size() << " pairs." << std::endl);
    add_to_list(ret);*/
  } else {
    internal::filter_same(ret);
    moved_count_+=moved.size();
    if (moved_count_ > .2 *c_->get_number_of_particles()) {
      /*InList il= InList::create(moved);
        remove_from_list_if(il);
        InList::destroy(il);*/
      remove_from_list_if(FarParticle(get_model(), distance_+2*slack_));
      moved_count_=0;
    }
    IMP_LOG(TERSE, "Found " << ret.size() << " pairs." << std::endl);
    add_to_list(ret);
  }
  moved_->reset_moved();
  IMP_LOG(TERSE, "Count is now "
          << std::distance(particle_pairs_begin(),
                           particle_pairs_end()) << std::endl);
}
void CoreClosePairContainer::do_rebuild() {
  IMP_LOG(TERSE, "Handling full update of ClosePairContainer."
          << std::endl);
  ParticleIndexPairs ret= cpf_->get_close_pairs(get_model(), c_->get_indexes());
  internal::fix_order(ret);
  IMP_LOG(TERSE, "Found before filtering " << ret.size()
          << " pairs." << std::endl);
  internal::filter_close_pairs(this, ret);
  IMP_LOG(TERSE, "Found " << ret.size() << " pairs." << std::endl);
  update_list(ret);
  moved_->reset();
  check_list(true);
}

void CoreClosePairContainer::do_before_evaluate() {
  IMP_OBJECT_LOG;
  IMP_CHECK_OBJECT(c_);
  IMP_CHECK_OBJECT(cpf_);
  set_was_used(true);
  IMP_INTERNAL_CHECK(c_->get_is_up_to_date(),
                     "Input container is not up to date.");
  IMP_INTERNAL_CHECK(moved_->get_is_up_to_date(),
                     "Moved container is not up to date.");
  try {
    if (first_call_) {
      do_first_call();
    } else if (moved_->get_number_of_particles() != 0) {
      if (moved_->get_particles().size() < c_->get_number_of_particles()*.2) {
        do_incremental();
      } else {
        do_rebuild();
      }
    } else {
      IMP_LOG(TERSE, "No particles moved more than " << slack_ << std::endl);
    }
  } catch (std::bad_alloc&) {
    IMP_THROW("Ran out of memory when computing close pairs."
              << " Try to reduce the "
              << "slack or reformulate the problem.", ValueException);
  }
}


void CoreClosePairContainer::do_after_evaluate() {
  internal::ListLikePairContainer::do_after_evaluate();
}


void CoreClosePairContainer::do_show(std::ostream &out) const {
  IMP_CHECK_OBJECT(this);
  out << "container " << *c_ << std::endl;
}


ParticlesTemp CoreClosePairContainer::get_contained_particles() const {
  ParticlesTemp ret= c_->get_contained_particles();
  ParticlesTemp nret =cpf_->get_input_particles(c_->get_particles());
  ret.insert(ret.end(), nret.begin(), nret.end());
  return ret;
}

IMPCORE_END_INTERNAL_NAMESPACE
