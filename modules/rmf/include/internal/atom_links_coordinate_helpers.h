/**
 *  \file IMP/rmf/atom_links.h
 *  \brief Handle read/write of kernel::Model data from/to files.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPRMF_ATOM_LINKS_COORDINATE_HELPERS_H
#define IMPRMF_ATOM_LINKS_COORDINATE_HELPERS_H

#include <IMP/rmf/rmf_config.h>
#include <RMF/NodeHandle.h>
#include <RMF/FileHandle.h>
#include <RMF/decorators.h>
#include <IMP/algebra/Transformation3D.h>
IMPRMF_BEGIN_INTERNAL_NAMESPACE

enum {
  STATIC_RB = 1,
  FRAME_RB = 2,
  STATIC_XYZ = 4,
  FRAME_XYZ = 8
} NodeCoords;

inline void copy_to_frame_reference_frame(const algebra::Transformation3D &tr,
                                          RMF::NodeHandle n,
                                          RMF::ReferenceFrameFactory rff) {
  RMF::ReferenceFrame rf = rff.get(n);
  algebra::Vector3D translation = tr.get_translation();
  algebra::Vector4D quaternion = tr.get_rotation().get_quaternion();
  rf.set_frame_translation(RMF::Vector3(translation.coordinates_begin(),
                                          translation.coordinates_end()));
  rf.set_frame_rotation(RMF::Vector4(quaternion.coordinates_begin(),
                                       quaternion.coordinates_end()));
}
inline void copy_to_frame_particle(const algebra::Vector3D &v,
                                     RMF::NodeHandle n,
                                     RMF::IntermediateParticleFactory ipf) {
  RMF::IntermediateParticle ip = ipf.get(n);
  ip.set_frame_coordinates(
      RMF::Floats(v.coordinates_begin(), v.coordinates_end()));
}
inline void copy_to_static_reference_frame(const algebra::Transformation3D &tr,
                                           RMF::NodeHandle n,
                                           RMF::ReferenceFrameFactory rff) {
  RMF::ReferenceFrame rf = rff.get(n);
  algebra::Vector3D translation = tr.get_translation();
  algebra::Vector4D quaternion = tr.get_rotation().get_quaternion();
  rf.set_static_translation(RMF::Vector3(translation.coordinates_begin(),
                                         translation.coordinates_end()));
  rf.set_static_rotation(RMF::Vector4(quaternion.coordinates_begin(),
                                      quaternion.coordinates_end()));
}
inline void copy_to_static_particle(const algebra::Vector3D &v,
                                    RMF::NodeHandle n,
                                    RMF::IntermediateParticleFactory ipf) {
  RMF::IntermediateParticle ip = ipf.get(n);
  ip.set_static_coordinates(
      RMF::Floats(v.coordinates_begin(), v.coordinates_end()));
}
inline algebra::Vector3D get_coordinates(
    RMF::NodeConstHandle n, RMF::IntermediateParticleConstFactory ipf) {
  RMF::Vector3 coords = ipf.get(n).get_coordinates();
  return algebra::Vector3D(coords.begin(), coords.end());
}
inline algebra::Transformation3D get_transformation(
    RMF::NodeConstHandle n, RMF::ReferenceFrameConstFactory rfcf) {
  RMF::Vector3 coords = rfcf.get(n).get_translation();
  RMF::Vector4 quat = rfcf.get(n).get_rotation();
  return algebra::Transformation3D(
      algebra::Rotation3D(algebra::Vector4D(quat.begin(), quat.end())),
      algebra::Vector3D(coords.begin(), coords.end()));
}

IMPRMF_END_INTERNAL_NAMESPACE

#endif /* IMPRMF_ATOM_LINKS_COORDINATE_HELPERS_H */
