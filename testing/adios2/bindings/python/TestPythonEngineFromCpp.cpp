/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 */

#include <vector>
#include <adios2.h>
#include <gtest/gtest.h>

TEST(PythonEngineTest, CreatePythonEngineFromCPlusPlus)
{
    /** Application variable */
    std::vector<float> myFloats = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const std::size_t Nx = myFloats.size();

    /** ADIOS class factory of IO class objects, DebugON is recommended */
    adios2::ADIOS adios(adios2::DebugON);

    /*** IO class object: settings and factory of Settings: Variables,
     * Parameters, Transports, and Execution: Engines */
    adios2::IO &io = adios.DeclareIO("PythonPluginIO");

    /** global array: name, { shape (total dimensions) }, { start (local) },
     * { count (local) }, all are constant dimensions */
    adios2::Variable<float> &var = io.DefineVariable<float>(
        "data", {}, {}, {Nx}, adios2::ConstantDims);

    /** Engine derived class, spawned to start IO operations */
    io.SetEngine("PythonPluginEngine");
    io.SetParameters({{"PluginName", "TestPythonPlugin"},
                      {"PluginModule", "TestPythonEngine"},
                      {"PluginClass", "TestPythonEngine"}});
    auto writer = io.Open("TestPythonPlugin", adios2::OpenMode::Write);

    ASSERT_NE(writer.get(), nullptr);

    /** Write variable for buffering */
    writer->Write<float>(var, myFloats.data());

    /** Create bp file, engine becomes unreachable after this*/
    writer->Close();
}

//******************************************************************************
// main
//******************************************************************************

int main(int argc, char **argv)
{
// #ifdef ADIOS2_HAVE_MPI
//     MPI_Init(nullptr, nullptr);
// #endif

    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

// #ifdef ADIOS2_HAVE_MPI
//     MPI_Finalize();
// #endif

    return result;
}
