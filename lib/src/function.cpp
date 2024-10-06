#include "../include/function.h"
#include <app_api.h>
#include "mexFunctions.h"
#include "errorcodes.h"
#include <exception>
#include <sstream>

#include "implementation/CziWriterManager.h"
#include "implementation/CziReaderManager.h"

using namespace mexlibCZI;
using namespace std;

void mexlibCZI::mexFunction(int nlhs, Parameter* plhs[], int nrhs, const Parameter* prhs[], IAppExtensionFunctions* app_functions)
{
    if (nrhs < 1)
    {
        app_functions->pfn_ReportErrorAndRaiseSignal("MATLAB:MEXlibCZI:invalidNumInputs", "One input required.");
    }

    if (!app_functions->pfn_IsChar(prhs[0]))
    {
        app_functions->pfn_ReportErrorAndRaiseSignal("MATLAB:MEXlibCZI:inputNotString", "Input must be a string.");
    }

    bool errorOccurred = false;
    char* errorText = nullptr;
    const char* errorId = nullptr;

    {
        //auto upArgStr = mexApi.UpMxArrayToMatlabAllocatedUtf8String((const MexArray*)(prhs[0]));
        char* command_name = app_functions->pfn_ConvertToUTF8String(prhs[0]);
            //static_cast<const char*>(app_functions->pfn_GetData(prhs[0]));
        auto func = CMexFunctions::GetInstance().FindFunc(command_name);
        if (func != nullptr)
        {
            app_functions->pfn_Free(command_name);

            MatlabArgs args = { nlhs, plhs, nrhs, prhs, app_functions };
            try
            {
                func->pfnCheckArguments(&args);
            }
            catch (exception& excp)
            {
                errorOccurred = true;
                errorText = app_functions->pfn_StrDupHostAllocated(excp.what());
                errorId = ErrorIds::InvalidArguments;
            }

            if (!errorOccurred)
            {
                try
                {
                    func->pfnExecute(&args);
                }
                catch (exception& excp)
                {
                    errorOccurred = true;
                    errorText = app_functions->pfn_StrDupHostAllocated(excp.what());
                    errorId = ErrorIds::InvalidArguments;
                }
            }
        }
        else
        {
            errorOccurred = true;
            ostringstream ss;
            ss << "The string \"" << command_name << "\" is not a known command.";
            app_functions->pfn_Free(command_name);
            errorText = app_functions->pfn_StrDupHostAllocated(ss.str().c_str());
            errorId = ErrorIds::UnknownCommand;
        }
    }

    if (errorOccurred)
    {
        // Remember that we do NOT return from this call - we should make sure that no resource-cleanup is
        //  done beyond this point. The memory for the errorText is allocated with Matlab's malloc - therefore,
        //  it should be released automatically.
        app_functions->pfn_ReportErrorAndRaiseSignal(errorId, errorText);
        //mexApi.MexErrMsgIdAndTxt(errorId, errorText);
    }
}

void mexlibCZI::OnShutdown()
{
    CziReaderManager::GetInstance().RemoveAllInstances();
    CziWriterManager::GetInstance().RemoveAllInstances();
}

void mexlibCZI::OnInitialize()
{
}