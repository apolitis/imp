/**
 *  \file SAXSScore.h   \brief A class for SAXS scoring and its derivatives
 *
 *  Copyright 2007-8 Sali Lab. All rights reserved.
 *
 */

#ifndef IMPSAXS_SAXS_SCORE_H
#define IMPSAXS_SAXS_SCORE_H

#include "config.h"
#include "FormFactorTable.h"
#include "RadialDistributionFunction.h"
#include "SAXSProfile.h"

#include <IMP/Model.h>

#include <iostream>
#include <vector>

IMPSAXS_BEGIN_NAMESPACE

/*
 Basic SAXS scoring class
 */

class IMPSAXSEXPORT SAXSScore {
public:
  //! init for theoretical profile
  SAXSScore(FormFactorTable* ff_table,
            SAXSProfile* exp_saxs_profile,
            const SAXSProfile& model_saxs_profile,
            const std::vector<Particle*>& particles);

  //! compute chi value (assumes the same sampling range!)
  Float compute_chi_score(const SAXSProfile& model_saxs_profile);

  //! compute  derivatives on atom iatom - iatom is NOT part of rigid body
  std::vector<IMP::algebra::Vector3D> calculate_chi_derivative(
                           const SAXSProfile& model_saxs_profile,
                           const std::vector<Particle*>& particles) {
    return calculate_chi_real_derivative(model_saxs_profile, particles);
  }

  Float get_scaling(void) { return c_; }
  Float set_scaling(Float c) { c_ = c;  return c_; }

  Float get_offset(void) { return offset_; }
  Float set_offset(Float offset) { offset_ = offset;  return offset_; }

  int get_mesh_sinc_(void) { return mesh_sinc_; }
  int set_mesh_sinc_(int mesh_sinc) {mesh_sinc_ = mesh_sinc; return mesh_sinc_;}

private:
  int init(const SAXSProfile& model_saxs_profile,
           const std::vector<Particle*>& particles);

  //! compute  derivatives on atom iatom - iatom is NOT part of rigid body
  std::vector<IMP::algebra::Vector3D> calculate_chi_real_derivative (
                           const SAXSProfile& model_saxs_profile,
                           const std::vector<Particle*>& particles);

  void write_SAXS_fit_file(const std::string& file_name,
                           const SAXSProfile& model_saxs_profile) const;

protected:
  FormFactorTable* ff_table_;       // pointer to form factors table
  SAXSProfile* exp_saxs_profile_;   // pointer to experimental saxs profile

  Float c_;  // scale constant
  Float offset_, chi_square_;   // offset and Chi-square
  std::vector< std::vector<Float> > sincval_array_;

  //! lookup table for sinc function and cos function
  std::vector<Float> sinc_lookup_, cos_lookup_, zero_formfactor_;
  std::vector<Float> r_, r_square_reciprocal_;
  int mesh_sinc_;     //! how many points per 1 unit in sinc
  unsigned int nr_;
};

IMPSAXS_END_NAMESPACE

#endif  /* IMPSAXS_SAXS_SCORE_H */
