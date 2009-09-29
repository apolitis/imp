import IMP
import IMP.core
import IMP.atom
import IMP.helper

# This example addes a restraint on nonbonded interactions
# Since it is between two rigid bodies, internal interactions are ignored

m= IMP.Model()
# The particles in the rigid bodies
rbps0= IMP.core.create_xyzr_particles(m, 50, 1)
rbps1= IMP.core.create_xyzr_particles(m, 50, 1)

rbp0= IMP.Particle(m)
rbp1= IMP.Particle(m)

rbss0 = IMP.helper.create_rigid_body(rbp0, IMP.core.XYZs(rbps0))
m.add_score_state(rbss0)

rbss1 = IMP.helper.create_rigid_body(rbp1, IMP.core.XYZs(rbps1))
m.add_score_state(rbss1)

# make sure the rigid bodies have a large enough radius to include their members
IMP.helper.cover_rigid_body(IMP.core.RigidBody(rbp0))
IMP.helper.cover_rigid_body(IMP.core.RigidBody(rbp1))

lsc= IMP.core.ListSingletonContainer()
lsc.add_particle(rbp0)
lsc.add_particle(rbp1)

# Set up the nonbonded list
nbl= IMP.core.ClosePairsScoreState(lsc)
nbl.set_close_pairs_finder(IMP.core.RigidClosePairsFinder(IMP.core.RigidMembersRefiner()))
m.add_score_state(nbl)
# Set the amount particles need to move before the list is updated
nbl.set_slack(2.0)

# Set up excluded volume
ps= IMP.core.SphereDistancePairScore(IMP.core.HarmonicLowerBound(0,1))
evr= IMP.core.PairsRestraint(ps, nbl.get_close_pairs_container())
evri= m.add_restraint(evr)

# Set up optimizer
o= IMP.core.ConjugateGradients()
o.set_model(m)

o.optimize(1000)
