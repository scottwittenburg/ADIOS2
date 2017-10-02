from mpi4py import MPI
import numpy
import adios2

# from dummyEngine import DummyEngine
# import dummyEngine

def do_adios_stuff():
	# print('inside testPythonEngine.py: static member is initially set to %d' % DummyEngine.StaticMemberVariable)
	# DummyEngine.StaticMemberVariable += 1
	# print('inside testPythonEngine.py: after incrementing static member, it is now %d' % DummyEngine.StaticMemberVariable)

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
	bpIO.SetParameters(PluginName="FirstPythonPlugin", PluginModule="dummyEngine", PluginClass="DummyEngine")
	# bpIO.SetParameters(PluginName="FirstPythonPlugin", PluginClass="DummyEngine")

	# ADIOS Engine
	bpFileWriter = bpIO.Open("npArray.bp", adios2.OpenModeWrite)
	bpFileWriter.Write(ioArray, myArray)

	# print('inside testPythonEngine.py: after calling Write, static member is now %d' % dummyEngine.DummyEngine.StaticMemberVariable)

	bpFileWriter.Close()
