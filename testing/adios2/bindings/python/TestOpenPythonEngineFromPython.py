import unittest

from mpi4py import MPI
import numpy
import adios2

from TestPythonEngine import TestPythonEngine

###
### Create a testcase class with some tests
###
class TestOpenPythonEngineFromPython(unittest.TestCase):
    def testCreateEngine(self):
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
		bpIO.SetParameters(PluginName="DoNotReallyCare", PluginClass="TestPythonEngine")

		# ADIOS Engine
		bpFileWriter = bpIO.Open("npArray.bp", adios2.OpenModeWrite)
		bpFileWriter.Write(ioArray, myArray)

		bpFileWriter.Close()

###
### Trigger the tests
###
if __name__ == '__main__':
    unittest.main()
