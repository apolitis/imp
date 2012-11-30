/**
 *  \file anchor_utilities.cpp
 *  \brief Tools for handling anchors.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#include <IMP/multifit/anchor_utilities.h>
#include <IMP/statistics/VQClustering.h>
#include <IMP/algebra/vector_search.h>
#include <IMP/atom/Atom.h>
#include <boost/graph/metric_tsp_approx.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/graphviz.hpp>

IMPMULTIFIT_BEGIN_NAMESPACE

void get_anchors_for_density(em::DensityMap *dmap, int number_of_means,
                             float density_threshold,
                             std::string pdb_filename,
                             std::string cmm_filename,
                             std::string seg_filename,
                             std::string txt_filename)
{
  dmap->set_was_used(true);
  IMP_NEW(multifit::DensityDataPoints, ddp, (dmap,density_threshold));
  IMP::statistics::VQClustering vq(ddp, number_of_means);
  ddp->set_was_used(true);
  vq.run();
  multifit::DataPointsAssignment assignment(ddp, &vq);
  multifit::AnchorsData ad(assignment.get_centers(),
                           *(assignment.get_edges()));
  multifit::write_pdb(pdb_filename, assignment);
  //also write cmm string into a file:
  if (cmm_filename != "") {
    multifit::write_cmm(cmm_filename, "anchor_graph", ad);
  }
  if (seg_filename != "") {
    float apix = dmap->get_spacing();
    multifit::write_segments_as_mrc(dmap, assignment, apix, apix,
                                    density_threshold, seg_filename);
  }
  if (txt_filename != "") {
    multifit::write_txt(txt_filename, ad);
  }
}

IMPMULTIFIT_END_NAMESPACE