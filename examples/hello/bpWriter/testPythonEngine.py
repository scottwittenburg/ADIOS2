import sys

from mpi4py import MPI
import numpy
import adios2

MODULE_LEVEL_VARIABLE = 0

def printSysModules():
	# print('List of existing module names:')
	# for moduleName in sys.modules:
	# 	print('    %s' % moduleName)
	if 'testPythonEngine' in sys.modules:
		print('this module is already imported:')
		print(sys.modules['testPythonEngine'])
	else:
		print('this module is not already imported')


# from dummyEngine import DummyEngine

print('Hello from the testPythonEngine module')
printSysModules()


class DummyEngine():
    StaticMemberVariable = 0

    # def __init__(self, name, openMode):
    def __init__(self):
        # EnginePy.__init__(self, name, openMode, mpiComm)
        print('Constructing DummyEngine python class')
        # print(name, openMode)

    def Write(self):
        # EnginePy.Write(self, variable, array)
        print('DummyEngine.Write called')

    def Writef(self):
    	global MODULE_LEVEL_VARIABLE

        print('Inside DummyEngine.Writef, StaticMemberVariable is currently %d' % DummyEngine.StaticMemberVariable)
        DummyEngine.StaticMemberVariable += 1
        print('Inside DummyEngine.Writef, incremented StaticMemberVariable variable to %d' % DummyEngine.StaticMemberVariable)

        print('Inside DummyEngine.Writef, MODULE_LEVEL_VARIABLE is currently %d' % MODULE_LEVEL_VARIABLE)
        MODULE_LEVEL_VARIABLE += 1
        print('Inside DummyEngine.Writef, incremented MODULE_LEVEL_VARIABLE variable to %d' % MODULE_LEVEL_VARIABLE)

    def Advance(self, timeoutSeconds):
        # EnginePy.Advance(self, timeoutSeconds)
        print('DummyEngine just deferred Write to superclass')

    def Close(self, transportIndex):
        # EnginePy.Close(self, transportIndex)
        print('DummyEngine just deferred Close to superclass')


if __name__ == '__main__':
	print('Hello from the testPythonEngine module __main__ function')
	printSysModules()

	print('inside testPythonEngine.py: StaticMemberVariable is initially set to %d' % DummyEngine.StaticMemberVariable)
	DummyEngine.StaticMemberVariable += 1
	print('inside testPythonEngine.py: after incrementing StaticMemberVariable, it is now %d' % DummyEngine.StaticMemberVariable)

	print('inside testPythonEngine.py: MODULE_LEVEL_VARIABLE is initially set to %d' % MODULE_LEVEL_VARIABLE)
	MODULE_LEVEL_VARIABLE += 1
	print('inside testPythonEngine.py: after incrementing MODULE_LEVEL_VARIABLE, it is now %d' % MODULE_LEVEL_VARIABLE)

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
	bpIO = adios.DeclareIO('BPFile_N2N')

	# ADIOS Variable name, shape, start, offset, constant dims
	ioArray = bpIO.DefineVariable(
	    'bpArray', [size * Nx], [rank * Nx], [Nx], adios2.ConstantDims)

	# Engine derived class, spawned to start IO operations
	bpIO.SetEngine('PythonPluginEngine')
	bpIO.SetParameters(PluginName='FirstPythonPlugin', PluginModule='testPythonEngine', PluginClass='DummyEngine')

	# ADIOS Engine
	bpFileWriter = bpIO.Open('npArray.bp', adios2.OpenModeWrite)

	# Now the module should have been imported again
	print('Module should have been imported by the C++ side now')
	printSysModules()

	bpFileWriter.Write(ioArray, myArray)

	print('inside testPythonEngine.py: after calling Write, static member is now %d' % DummyEngine.StaticMemberVariable)
	print('inside testPythonEngine.py: after calling Write, MODULE_LEVEL_VARIABLE is now %d' % MODULE_LEVEL_VARIABLE)

	bpFileWriter.Close()
