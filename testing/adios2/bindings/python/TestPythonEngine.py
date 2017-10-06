
class TestPythonEngine():
    def __init__(self):
        print('Inside TestPythonEngine python constructor')
        
    def Write(self):
        print('TestPythonEngine.Write called')

    def Writef(self):
        print('Inside TestPythonEngine.Writef')

    def Advance(self, timeoutSeconds):
        print('Inside TestPythonEngine.Advance')

    def Close(self, transportIndex):
        print('Inside TestPythonEngine.Close')
