import unittest

import numpy
import adios2

from TestPythonEngine import TestPythonEngine

###
### Create a testcase class with some tests
###
class TestOpenPythonEngineFromPython(unittest.TestCase):
    def testCreateEngine(self):
		# User data
		myArray = numpy.array([0., 1., 2., 3., 4., 5., 6., 7., 8., 9.])
		Nx = myArray.size

		adios = adios2.ADIOS(adios2.DebugON)

		# ADIOS IO
		bpIO = adios.DeclareIO("BPFile_N2N")

		# ADIOS Variable name, shape, start, offset, constant dims
		ioArray = bpIO.DefineVariable(
		    "bpArray", [Nx], [0], [Nx], adios2.ConstantDims)

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
