
# import adios2
# from adios2 import EnginePy

print("Inside dummyEngine.py, module-level print statement")

# class DummyEngine(adios2.EnginePy):
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
        print('Inside DummyEngine.Writef, StaticMemberVariable is currently %d' % DummyEngine.StaticMemberVariable)
        DummyEngine.StaticMemberVariable += 1
        print('Inside DummyEngine.Writef, incremented variable to %d' % DummyEngine.StaticMemberVariable)

    def Advance(self, timeoutSeconds):
        # EnginePy.Advance(self, timeoutSeconds)
        print('DummyEngine just deferred Write to superclass')

    def Close(self, transportIndex):
        # EnginePy.Close(self, transportIndex)
        print('DummyEngine just deferred Close to superclass')
