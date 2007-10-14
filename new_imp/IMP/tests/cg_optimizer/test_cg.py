import unittest
import IMP, IMP.test

class WoodsFunc(IMP.Restraint):
    """Woods function for four input values, defined as an IMP restraint"""
    def __init__(self, model, particles):
        IMP.Restraint.__init__(self)
        self.model_data = model.get_model_data()
        self.indices = [p.get_float_index("x") for p in particles]

    def evaluate(self, calc_deriv):
        (x1, x2, x3, x4) = [self.model_data.get_float(i) for i in self.indices]
        a = x2 - x1 * x1
        b = x4 - x3 * x3
        e = 100.0 * a * a + (1.0 - x1) ** 2+ 90.0 * b * b + (1.0 - x3) ** 2 \
            + 10.1 * ((x2 - 1.0) ** 2 + (x4 - 1.0) ** 2) \
            + 19.8 * (x2 - 1.0) * (x4 - 1.0)
        if calc_deriv:
            dx = [-2.0 * (200.0 * x1 * a + 1.0 - x1),
                   2.0 * (100.0 * a + 10.1 * (x2 - 1.0) + 9.9 * (x4 - 1.0)),
                  -2.0 * (180.0 * x3 * b + 1.0 - x3),
                   2.0 * (90.0 * b + 10.1 * (x4 - 1.0) + 9.9 * (x2 - 1.0))]
            for (i, d) in zip(self.indices, dx):
                self.model_data.add_to_deriv(i, d)
        return e

class CGOptimizerTests(IMP.test.IMPTestCase):
    def test_cg_woods_func(self):
        """Check that we can optimize the Woods function with CG"""
        self._test_starting_conditions((-3.0, -1.0, -3.0, -1.0))
        self._test_starting_conditions((2.0, 3.0, 8.0, -5.0))

    def _test_starting_conditions(self, starting_values):
        """Test the optimizer with given starting conditions"""
        model = IMP.Model()
        particles = []

        for value in starting_values:
            p = IMP.Particle()
            model.add_particle(p)
            particles.append(p)
            p.add_float("x", value, True)
        rsr = WoodsFunc(model, particles)
        model.add_restraint(rsr)
        model.set_up_trajectory('', False, False)
        opt = IMP.ConjugateGradients()
        e = opt.optimize(model, 100, 1e-5)
        model_data = model.get_model_data()
        for p in particles:
            val = model_data.get_float(p.get_float_index("x"))
            self.assertAlmostEqual(val, 1.0, places=1)
        self.assertAlmostEqual(e, 0.0, places=2)

if __name__ == '__main__':
    unittest.main()
