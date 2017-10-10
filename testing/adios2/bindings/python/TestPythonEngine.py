
import sys

# When importing from C++, this path is missing, and thus numpy is
# unable to import the __future__ module.  This is a temporary
# workaround to allow that to happen.
sys.path.append('/usr/lib/python2.7')

# When importing from C++, this path is also missing.  This lives in the
# virtual environment directory and is part of the path automatically
# when running the virtual env python.
sys.path.append('/data/scott/projects/adios2/adios-env/lib/python2.7/site-packages')


import adios2
import numpy      # Required when importing this module from C++

print('Inside TestPythonEngine module')

class TestPythonEngine(adios2.Engine):
    def __init__(self, engineType, io, name, openMode):
        adios2.Engine.__init__(self, engineType, io, name, openMode)
        print('Inside TestPythonEngine python ctor, just called parent ctor')

        # Calls Engine::Init() if no Init() method is defined here
        self.Init()

    # def Init(self):
    #     print('Inside TestPythonEngine Init()')
        
    def DoWrite(self, variable, values):
        print('Inside TestPythonEngine.DoWrite')
        print(variable)
        print(values)

    def Advance(self, timeoutSeconds):
        print('Inside TestPythonEngine.Advance')

    def Close(self, transportIndex):
        print('Inside TestPythonEngine.Close')
