import IMP.test
import os
import sys

class Tests(IMP.test.ApplicationTestCase):
    def test_wrong_number_args(self):
        """Test idock.py with wrong number of arguments"""
        p = self.run_python_application('idock.py', [])
        out, err = p.communicate()
        self.assertNotEqual(p.returncode, 0)

    def test_em2d_without_pixel_size(self):
        """Test idock.py with class averages but no pixel size"""
        p = self.run_python_application('idock.py',
                                        ['--em2d', 'foo', 'file1', 'file2'])
        out, err = p.communicate()
        self.assertIn('please specify pixel size', err)
        self.assertNotEqual(p.returncode, 0)

    def test_no_data(self):
        """Test idock.py with no experimental data"""
        p = self.run_python_application('idock.py',
                                        ['file1', 'file2'])
        out, err = p.communicate()
        self.assertIn('please provide one or more types', err)
        self.assertNotEqual(p.returncode, 0)

    def test_idock_init(self):
        """Test IDock class init"""
        app = self.import_python_application('idock.py')
        dock = app.IDock('x', 'y', 'z')
        self.assertEqual(dock.opts, 'x')
        self.assertEqual(dock.receptor, 'y')
        self.assertEqual(dock.ligand, 'z')

    def make_idock_with_captured_subprocess(self):
        """Make an IDock instance. Capture the command lines of any
           subprocesses, rather than actually running them."""
        app = self.import_python_application('idock.py')
        class Dummy(app.IDock):
            def _run_binary(self, *args):
                self.run_binary_args = args
        class Opts(object):
            pass
        return Dummy(Opts(), '', '')

    def test_run_patch_dock_binary(self):
        """Test run_patch_dock_binary()"""
        dock = self.make_idock_with_captured_subprocess()
        dock.opts.patch_dock = 'pd_dir'
        dock.run_patch_dock_binary('pd', ['foo', 'bar'])
        self.assertEqual(dock.run_binary_args, ('pd_dir', 'pd', ['foo', 'bar']))

    def test_run_binary(self):
        """Test _run_binary()"""
        class DummySubprocess(object):
            args = None
            class Popen(object):
                def __init__(self, *args):
                    DummySubprocess.args = self.args = args
                def wait(self):
                    if 'bad' in self.args[0][0]:
                        return 1
                    else:
                        return 0
        app = self.import_python_application('idock.py')
        oldsubproc = app.subprocess
        dock = app.IDock(None, '', '')
        try:
            app.subprocess = DummySubprocess
            self.assertRaises(OSError, dock._run_binary, 'testpath', 'bad', [])
            dock._run_binary('testpath', 'bin', ['arg1', 'arg2'])
            self.assertEqual(app.subprocess.args,
                             (['testpath/bin', 'arg1', 'arg2'],))
            dock._run_binary('', 'bin', ['arg1', 'arg2'])
            self.assertEqual(app.subprocess.args,
                             (['bin', 'arg1', 'arg2'],))
        finally:
            app.subprocess = oldsubproc

    def test_make_patch_dock_surfaces(self):
        """Test make_patch_dock_surfaces()"""
        dock = self.make_idock_with_captured_subprocess()
        dock.opts.patch_dock = 'pd_dir'
        dock.receptor = 'testrecep'
        dock.ligand = 'testlig'
        dock.make_patch_dock_surfaces()
        self.assertEqual(dock.run_binary_args,
                         ('pd_dir', 'buildMS.pl', ['testrecep', 'testlig']))

    def test_make_patch_dock_parameters(self):
        """Test make_patch_dock_parameters()"""
        dock = self.make_idock_with_captured_subprocess()
        dock.opts.patch_dock = 'pd_dir'
        dock.opts.type = 'AA'
        dock.receptor = 'testrecep'
        dock.ligand = 'testlig'

        dock.opts.precision = 1
        dock.make_patch_dock_parameters()
        self.assertEqual(dock.run_binary_args, ('pd_dir', 'buildParams.pl',
                                        ['testrecep', 'testlig', '4.0', 'AA']))

        dock.opts.precision = 2
        dock.make_patch_dock_parameters()
        self.assertEqual(dock.run_binary_args, ('pd_dir', 'buildParams.pl',
                                        ['testrecep', 'testlig', '2.0', 'AA']))

        dock.opts.precision = 3
        dock.make_patch_dock_parameters()
        self.assertEqual(dock.run_binary_args, ('pd_dir', 'buildParamsFine.pl',
                                        ['testrecep', 'testlig', '2.0', 'AA']))

    def test_get_filename(self):
        """Test get_filename()"""
        app = self.import_python_application('idock.py')
        class Opts(object):
            pass
        dock = app.IDock(Opts(), 'y', 'z')
        dock.opts.prefix = 'foo'
        self.assertEqual(dock.get_filename('bar'), 'foobar')

    def test_do_patch_dock_docking(self):
        """Test do_patch_dock_docking()"""
        def do_dock():
            dock = self.make_idock_with_captured_subprocess()
            dock.opts.prefix = 'foo'
            dock.opts.patch_dock = 'pd_dir'
            dock.do_patch_dock_docking()
            return dock
        if os.path.exists('foodocking.res'):
            os.unlink('foodocking.res')
        dock = do_dock()
        self.assertEqual(dock.run_binary_args, ('pd_dir', 'patch_dock.Linux',
                                           ['params.txt', 'foodocking.res']))
        open('foodocking.res', 'w').write('foo\n' * 36)
        # Recovery should be skipped if results file is too short
        dock = do_dock()
        self.assertEqual(dock.run_binary_args, ('pd_dir', 'patch_dock.Linux',
                                           ['params.txt', 'foodocking.res']))
        open('foodocking.res', 'w').write('foo\n' * 37)
        dock = do_dock()
        self.assertFalse(hasattr(dock, 'run_binary_args'))
        os.unlink('foodocking.res')

if __name__ == '__main__':
    IMP.test.main()