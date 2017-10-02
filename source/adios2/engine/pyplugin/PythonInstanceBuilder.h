/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * PythonInstanceBuilder.h Instantiate some arbitrary python class
 *
 *  Created on: Oct 02, 2017
 *      Author: Scott Wittenburg <scott.wittenburg@kitware.com>
 */

#ifndef ADIOS2_ENGINE_PYPLUGIN_PYTHONINSTANCEBUILDER_H_
#define ADIOS2_ENGINE_PYPLUGIN_PYTHONINSTANCEBUILDER_H_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace adios2
{
namespace PythonInstanceBuilder
{
	pybind11::object BuildInstance(const std::string& className, 
		                           const std::string* moduleName = nullptr)
	{
	    pybind11::dict globals = pybind11::globals();

	    if (globals.contains(className.c_str()))
	    {
	        // In this case the engine class is already available directly as a
	        // key within globals, it was probably gotten via:
	        //
	        //     'from <module>import <class>'
	        //
	        pybind11::object engineConstructor =
	            globals[className.c_str()];
	        return engineConstructor();
	    }
	    else
	    {
	        // In case it was imported via:
	        //
	        //     'import <module>'
	        //
	        // then one of the globals is refers to a module where we should
	        // be able to find it
	        for (auto item : globals)
	        {
	            pybind11::object valueObj = globals[item.first];
	            try
	            {
	                pybind11::object engineConstructor =
	                    valueObj.attr(className.c_str());
	                return engineConstructor();
	            }
	            catch (std::exception e) {}
	        }

	        // If we still haven't found the class we're looking for, maybe the
	        // user provided the name of the module from which we can import it.
	        if (moduleName != nullptr)
	        {
	            pybind11::object engineModule =
	                pybind11::module::import((*moduleName).c_str());
	            // Put the module into the globals so python side can access it
	            globals[(*moduleName).c_str()] = engineModule;
	            pybind11::object engineConstructor =
	                engineModule.attr(className.c_str());
	            return engineConstructor();
	        }
	    }

	    // Unable to instantiate the engine in this case
	    throw std::runtime_error("PythonEngine: Engine class was not present "
	                             "in main module, nor was a plugin module "
	                             "name provided in the parameters.  Unable to "
	                             "instantiate python engine.");
	}
}
}

#endif /* ADIOS2_ENGINE_PYPLUGIN_PYTHONINSTANCEBUILDER_H_ */
