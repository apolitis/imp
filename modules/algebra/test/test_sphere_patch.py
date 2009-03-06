import unittest
import IMP
import IMP.utils
import IMP.test
import IMP.algebra
import IMP.display
import math
class Sphere3DPatchTests(IMP.test.TestCase):
    """Test rigid transformations"""

    def setUp(self):
        """Build a set of test vectors"""
        IMP.test.TestCase.setUp(self)

    def test_sphere_patch_construction(self):
        """Check that a patch of a sphere is constructed correctly"""
        center = IMP.algebra.Vector3D(0.0,0.0,0.0)
        radius=5.0
        sph = IMP.algebra.Sphere3D(center,radius)
        xy_plane = IMP.algebra.Plane3D(IMP.algebra.Vector3D(0.,0.,0.),IMP.algebra.Vector3D(0.,0.,1.))
        patch = IMP.algebra.Sphere3DPatch(sph,xy_plane)
        self.assertEqual(patch.get_plane().is_above(IMP.algebra.Vector3D(1.,0.,1.)),True)
        self.assertEqual(patch.get_contains(IMP.algebra.Vector3D(0.0,1.0,0.4)),True)
        self.assertEqual(patch.get_contains(IMP.algebra.Vector3D(3.0,3.0,8.0)),False)
        self.assertEqual(patch.get_contains(IMP.algebra.Vector3D(0.0,1.0,-0.4)),False)

    def test_uniform_cover(self):
        """Check uniform cover on a patch of a sphere"""
        center = IMP.algebra.Vector3D(0.0,0.0,0.0)
        radius=5.0
        sph = IMP.algebra.Sphere3D(center,radius)
        xz_plane = IMP.algebra.Plane3D(IMP.algebra.Vector3D(0.,0.,0.),IMP.algebra.Vector3D(0.,1.,0.))
        patch = IMP.algebra.Sphere3DPatch(sph,xz_plane)
        numpts=600
        points=IMP.algebra.uniform_cover(patch,numpts)
        #check that the centroid of the sampled points make sense
        sampled_centroid = IMP.algebra.Vector3D(0.0,radius/2,0.0)
        excpeted_sampled_centroid = IMP.algebra.Vector3D(0.0,radius/2,0.0)
        self.assertEqual(len(points),numpts)
        for p in points:
            sampled_centroid = sampled_centroid + p
        sampled_centroid = sampled_centroid * (1.0/len(points))
        sampled_centroid.show()
        w= IMP.display.CGOWriter("cover")
        w.set_file_name(self.get_tmp_file_name("cover.py"))
        for p in points:
            w.add_geometry(IMP.display.SphereGeometry(IMP.algebra.Sphere3D(p, 1)))
        w.add_geometry(IMP.display.SphereGeometry(IMP.algebra.Sphere3D(center, 4)))
        self.assertInTolerance((sampled_centroid-excpeted_sampled_centroid).get_magnitude(),0,
                               4*radius/numpts**.5)


if __name__ == '__main__':
    unittest.main()
