from mpi4py import MPI
import numpy
import adios2

MODULE_LEVEL_VARIABLE = 0

class InlineDummyEngine():
    def __init__(self):
        print('Constructing InlineDummyEngine python class')

    def Write(self):
        print('InlineDummyEngine.Write called')

    def Writef(self):
        global MODULE_LEVEL_VARIABLE
        print('Inside InlineDummyEngine.Writef, MODULE_LEVEL_VARIABLE is currently %d' % MODULE_LEVEL_VARIABLE)
        MODULE_LEVEL_VARIABLE += 1
        print('Inside InlineDummyEngine.Writef, incremented MODULE_LEVEL_VARIABLE variable to %d' % MODULE_LEVEL_VARIABLE)

    def Advance(self, timeoutSeconds):
        print('Inside InlineDummyEngine Advance')

    def Close(self, transportIndex):
        print('Inside InlineDummyEngine Close')


if __name__ == '__main__':
    print('inside testInlinePythonEngine.py __main__: MODULE_LEVEL_VARIABLE is initially set to %d' % MODULE_LEVEL_VARIABLE)
    MODULE_LEVEL_VARIABLE += 1
    print('inside testInlinePythonEngine.py __main__: incremented MODULE_LEVEL_VARIABLE to %d' % MODULE_LEVEL_VARIABLE)

    # MPI
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    # User data
    myArray = numpy.array([0., 1., 2., 3., 4., 5., 6., 7., 8., 9.])
    Nx = myArray.size

    # ADIOS MPI Communicator, debug mode
    adios = adios2.ADIOS(comm, adios2.DebugON)

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

    print('inside testInlinePythonEngine.py __main__: after calling Write, MODULE_LEVEL_VARIABLE is now %d' % MODULE_LEVEL_VARIABLE)

    bpFileWriter.Close()
