/**
 * \file ChiScoreLog \brief scoring with log intensity
 *
 * Copyright 2007-2010 Sali Lab. All rights reserved.
 *
 */

#include <IMP/saxs/ChiScoreLog.h>

IMPSAXS_BEGIN_NAMESPACE

Float ChiScoreLog::compute_scale_factor(const Profile& exp_profile,
                                        const Profile& model_profile,
                                        const Float offset) const
{
  Float sum1=0.0, sum2=0.0;
  unsigned int profile_size = std::min(model_profile.size(),
                                       exp_profile.size());
  for(unsigned int k=0; k<profile_size; k++) {
    Float square_error =
      square(exp_profile.get_error(k)/exp_profile.get_intensity(k));
    Float weight_tilda = model_profile.get_weight(k) / square_error;
    sum1 += weight_tilda *
      log(exp_profile.get_intensity(k)/model_profile.get_intensity(k));
    sum2 += weight_tilda;
  }
  return std::exp(sum1/sum2);
}

Float ChiScoreLog::compute_score(const Profile& exp_profile,
                                 const Profile& model_profile,
                                 bool use_offset) const
{
  Float c = compute_scale_factor(exp_profile, model_profile);

  Float chi_square = 0.0;
  unsigned int profile_size = std::min(model_profile.size(),
                                       exp_profile.size());
  // compute chi square
  for (unsigned int k=0; k<profile_size; k++) {
    // in the theoretical profile the error equals to 1
    Float square_error = square(log(exp_profile.get_error(k)));
    Float weight_tilda = model_profile.get_weight(k) / square_error;
    Float delta = log(exp_profile.get_intensity(k))
      - log(c*model_profile.get_intensity(k));

    // Exclude the uncertainty originated from limitation of floating number
    if(fabs(delta/(log(exp_profile.get_intensity(k)))) >= 1.0e-15)
        chi_square += weight_tilda * square(delta);
  }
  chi_square /= profile_size;
  //std::cerr << "ChiScoreLog: " << chi_square << std::endl;
  return sqrt(chi_square);
}

IMPSAXS_END_NAMESPACE