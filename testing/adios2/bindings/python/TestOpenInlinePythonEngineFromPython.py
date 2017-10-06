from Adios2PythonTestBase import Adios2PythonTestBase

if Adios2PythonTestBase.isUsingMpi():
    from mpi4py import MPI

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
        MODULE_LEVEL_VARIABLE += 1
        print('InlineDummyEngine.Write called')

    def Writef(self):
        global MODULE_LEVEL_VARIABLE
        MODULE_LEVEL_VARIABLE += 1
        print('Inside InlineDummyEngine.Writef')

    def Advance(self, timeoutSeconds):
        print('Inside InlineDummyEngine Advance')

    def Close(self, transportIndex):
        print('Inside InlineDummyEngine Close')

###
### Create a testcase class with some tests
###
class TestOpenInlinePythonEngineFromPython(Adios2PythonTestBase):
    def testCreateEngine(self):
        global MODULE_LEVEL_VARIABLE
        self.assertEqual(MODULE_LEVEL_VARIABLE, 0)
        MODULE_LEVEL_VARIABLE += 1
        self.assertEqual(MODULE_LEVEL_VARIABLE, 1)

        rank = 0
        size = 1

        if self.isUsingMpi():
            # MPI
            comm = MPI.COMM_WORLD
            rank = comm.Get_rank()
            size = comm.Get_size()
            adios = adios2.ADIOS(comm, adios2.DebugON)
        else:
            adios = adios2.ADIOS(adios2.DebugON)

        # User data
        myArray = numpy.array([0., 1., 2., 3., 4., 5., 6., 7., 8., 9.])
        Nx = myArray.size

        # ADIOS IO
        bpIO = adios.DeclareIO("BPFile_N2N")

        # ADIOS Variable name, shape, start, offset, constant dims
        ioArray = bpIO.DefineVariable(
            "bpArray", [size * Nx], [rank * Nx], [Nx], adios2.ConstantDims)

        # Engine derived class, spawned to start IO operations
        bpIO.SetEngine("PythonPluginEngine")
        bpIO.SetParameters(PluginName="FirstPythonPlugin", PluginClass="InlineDummyEngine")

        # ADIOS Engine
        bpFileWriter = bpIO.Open("npArray.bp", adios2.OpenModeWrite)
        bpFileWriter.Write(ioArray, myArray)

        self.assertEqual(MODULE_LEVEL_VARIABLE, 2)

        bpFileWriter.Close()

    def testCreateEngineWithWrongName(self):
        if self.isUsingMpi():
            # MPI
            comm = MPI.COMM_WORLD
            adios = adios2.ADIOS(comm, adios2.DebugON)
        else:
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
    Adios2PythonTestBase.main()
