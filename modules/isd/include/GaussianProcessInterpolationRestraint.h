/**
 *  \file IMP/isd/GaussianProcessInterpolationRestraint.h
 *  \brief kernel::Restraint and ScoreState for GaussianProcessInterpolation
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#ifndef IMPISD_GAUSSIAN_PROCESS_INTERPOLATION_RESTRAINT_H
#define IMPISD_GAUSSIAN_PROCESS_INTERPOLATION_RESTRAINT_H

#include <IMP/isd/isd_config.h>
#include <IMP/macros.h>
#include <boost/scoped_ptr.hpp>
#include <IMP/kernel/Restraint.h>
#include <IMP/isd/GaussianProcessInterpolation.h>
#include <IMP/isd/MultivariateFNormalSufficient.h>
#include <IMP/base/Pointer.h>
#include <IMP/algebra/eigen3/Eigen/Dense>

#include <IMP/ScoreState.h>

IMPISD_BEGIN_NAMESPACE

class GaussianProcessInterpolationScoreState;

//! gaussian process restraint
/* the restraint is a multivariate normal distribution on the vector of
* observations with mean and standard deviation given by the posterior of the
* gaussian process.
*/
class IMPISDEXPORT GaussianProcessInterpolationRestraint
    : public kernel::Restraint {
   private:
    // checks and makes necessary updates
    void update_mean_and_covariance();
    base::Pointer<GaussianProcessInterpolation> gpi_;
    IMP::base::PointerMember<MultivariateFNormalSufficient> mvn_;
    IMP::base::PointerMember<GaussianProcessInterpolationScoreState> ss_;
    // number of observation points
    unsigned M_;

    void create_score_state();

   public:
    // this is a restraint on other restraints. It first constructs the
    // necessary vectors from GaussianProcessInterpolation, then creates a
    // multivariate normal distribution around it. Upon evaluation, it
    // checks if parameters have changed, reconstructs the matrix if
    // necessary, changes the DA weight and passes it to the functions.
    GaussianProcessInterpolationRestraint(kernel::Model *m,
                                          GaussianProcessInterpolation *gpi);

    // to call this, you need to update the scorestate before.
    // calling model.evaluate(False) is enough.
    double get_probability() const { return mvn_->density(); }

    void stats() const { return mvn_->stats(); }

    // use conjugate gradients when possible (default false)
    void set_use_cg(bool use, double tol) { mvn_->set_use_cg(use, tol); }

    // get minus log normalization and minus exponent separately
    double get_minus_log_normalization() const;
    double get_minus_exponent() const;

    // get hessian of the minus log likelihood
    IMP_Eigen::MatrixXd get_hessian() const;

    // get log determinant of hessian
    double get_logdet_hessian() const;

    // call this one from python
    FloatsList get_hessian(bool unused) const;

   public:
    double unprotected_evaluate(IMP::DerivativeAccumulator *accum) const
        IMP_OVERRIDE;
    IMP::kernel::ModelObjectsTemp do_get_inputs() const IMP_OVERRIDE;
    IMP_OBJECT_METHODS(GaussianProcessInterpolationRestraint);
    ;

    // to allow the scorestate to get the restraint's objects
    friend class GaussianProcessInterpolationScoreState;
};

#if !defined(IMP_DOXYGEN) && !defined(SWIG)
class IMPISDEXPORT GaussianProcessInterpolationScoreState : public ScoreState {
   private:
    IMP::base::WeakPointer<GaussianProcessInterpolationRestraint> gpir_;

   private:
    GaussianProcessInterpolationScoreState(
        GaussianProcessInterpolationRestraint *gpir)
        : ScoreState(gpir->get_model(),
                     "GaussianProcessInterpolationScoreState%1%"),
          gpir_(gpir) {}

   public:
    // only the GPIR can create this and add it to the model
    friend class GaussianProcessInterpolationRestraint;
    virtual void do_before_evaluate() IMP_OVERRIDE;
    virtual void do_after_evaluate(DerivativeAccumulator *da) IMP_OVERRIDE;
    virtual kernel::ModelObjectsTemp do_get_inputs() const IMP_OVERRIDE;
    virtual kernel::ModelObjectsTemp do_get_outputs() const IMP_OVERRIDE;
    IMP_OBJECT_METHODS(GaussianProcessInterpolationScoreState);
};
#endif

IMPISD_END_NAMESPACE

#endif /* IMPISD_GAUSSIAN_PROCESS_INTERPOLATION_RESTRAINT_H */
