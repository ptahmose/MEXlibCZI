#include "include_matlabheaders.h"
#include "exportedFunctions.h"
#include "mexFunctions.h"
#include "src/mexapi.h"
#include "src/CziReaderManager.h"
//#include <Windows.h>
#include <memory>

#include "errorcodes.h"
#include "src/utils.h"

using namespace std;

static bool gIsInitialized = false;

static void OnShutdown()
{
    CziReaderManager::GetInstance().RemoveAllInstances();
}

static void Initialize()
{
    MexApi::GetInstance().MexAtExit(OnShutdown);
}

/*
 Some notes:
  - if using mexErrMsgIdAndTxt, we leave the method by a trap of something; however this method does not return (and
     therefore no cleanup happens here).
  - here it is stated that the same happens if pressing "Ctrl-C" -> https://www.mathworks.com/help/matlab/matlab_external/automatic-cleanup-of-temporary-arrays.html
 */
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    if (!gIsInitialized)
    {
        Initialize();
        gIsInitialized = true;
    }

    auto mexApi = MexApi::GetInstance();

    if (nrhs < 1)
    {
        mexApi.MexErrMsgIdAndTxt("MATLAB:MEXlibCZI:invalidNumInputs", "One input required.");
    }

    /* input must be a string */
    if (!MexApi::GetInstance().MxIsChar((const MexArray*)(prhs[0])))
    {
        mexApi.MexErrMsgIdAndTxt("MATLAB:MEXlibCZI:inputNotString", "Input must be a string.");
    }

    bool errorOccurred = false;
    char* errorText = nullptr;
    const char* errorId = nullptr;
    {
        auto upArgStr = mexApi.UpMxArrayToMatlabAllocatedUtf8String((const MexArray*)(prhs[0]));
        auto func = CMexFunctions::GetInstance().FindFunc(upArgStr.get());
        if (func != nullptr)
        {
            MatlabArgs args = { nlhs,(MexArray**)plhs,nrhs,(const MexArray**)prhs };

            try
            {
                func->pfnCheckArguments(&args);
            }
            catch (exception& excp)
            {
                errorOccurred = true;
                errorText = MexApi::GetInstance().MxStrDup(excp.what());
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
                    errorText = MexApi::GetInstance().MxStrDup(excp.what());
                    errorId = ErrorIds::InvalidArguments;
                }
            }
        }
        else
        {
            errorOccurred = true;
            stringstream ss;
            ss << "The string \"" << upArgStr.get() << "\" is not a known command.";
            errorText = MexApi::GetInstance().MxStrDup(ss.str());
            errorId = ErrorIds::UnknownCommand;
        }
    }

    if (errorOccurred)
    {
        // Remember that we do NOT return from this call - we should make sure that no resource-cleanup is
        //  done beyond this point. The memory for the errorText is allocated with Matlab's malloc - therefore,
        //  it should be released automatically.
        mexApi.MexErrMsgIdAndTxt(errorId, errorText);
    }
}