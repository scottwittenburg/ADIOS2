import unittest

import numpy
import adios2

MODULE_LEVEL_VARIABLE = 0

###
### Define a new engine type in python
###
class InlineDummyEngine():
    def __init__(self):
        print('Constructing InlineDummyEngine python class')

    def Write(self):
        global MODULE_LEVEL_VARIABLE
        print('InlineDummyEngine.Write called')
        MODULE_LEVEL_VARIABLE += 1

    def Writef(self):
        global MODULE_LEVEL_VARIABLE
        print('Inside InlineDummyEngine Writef')
        MODULE_LEVEL_VARIABLE += 1

    def Advance(self, timeoutSeconds):
        print('Inside InlineDummyEngine Advance')

    def Close(self, transportIndex):
        print('Inside InlineDummyEngine Close')

###
### Create a testcase class with some tests
###
class TestOpenInlinePythonEngineFromPython(unittest.TestCase):
    def testCreateEngine(self):
        global MODULE_LEVEL_VARIABLE

        rank = 0
        size = 1

        myArray = numpy.array([0., 1., 2., 3., 4., 5., 6., 7., 8., 9.])
        Nx = myArray.size

        self.assertEqual(MODULE_LEVEL_VARIABLE, 0)
        MODULE_LEVEL_VARIABLE += 1
        self.assertEqual(MODULE_LEVEL_VARIABLE, 1)

        # ADIOS MPI Communicator, debug mode
        adios = adios2.ADIOS(adios2.DebugON)

        # ADIOS IO
        bpIO = adios.DeclareIO("BPFile_N2N")

        # ADIOS Variable name, shape, start, offset, constant dims
        ioArray = bpIO.DefineVariable("bpArray", [size * Nx], [rank * Nx], [Nx], adios2.ConstantDims)

        # Engine derived class, spawned to start IO operations
        bpIO.SetEngine("PythonPluginEngine")
        bpIO.SetParameters(PluginName="FirstPythonPlugin", PluginClass="InlineDummyEngine")

        # ADIOS Engine
        bpFileWriter = bpIO.Open("npArray.bp", adios2.OpenModeWrite)

        bpFileWriter.Write(ioArray, myArray)

        self.assertEqual(MODULE_LEVEL_VARIABLE, 2)

        bpFileWriter.Close()

    def testCreateEngineWithWrongName(self):
        adios = adios2.ADIOS(adios2.DebugON)
        bpIO = adios.DeclareIO("BPFile_N2N")

        bpIO.SetEngine("PythonPluginEngine")
        bpIO.SetParameters(PluginName="FirstPythonPlugin", PluginClass="MissingEngine")

        # Make sure exception is raised due to wrong engine name
        with self.assertRaises(RuntimeError):
            bpFileWriter = bpIO.Open("npArray.bp", adios2.OpenModeWrite)

###
### Trigger the tests
###
if __name__ == '__main__':
    unittest.main()
