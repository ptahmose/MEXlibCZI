#pragma once

#include <app_api.h>

struct MatlabArgs
{
    int nlhs;
    /*mxArray*/Parameter* plhs;
    int nrhs;
    const /*mxArray*/Parameter* prhs;
    IAppExtensionFunctions* app_functions;
};

struct CMexFunc
{
    void (*pfnCheckArguments)(MatlabArgs* args);
    void (*pfnExecute)(MatlabArgs* args);
};

class CMexFunctions
{
private:
    struct MexFuncItem
    {
        const char* funcname;
        CMexFunc    func;
    };

    static const MexFuncItem funcItems[];

    static CMexFunctions instance;
public:
    static const CMexFunctions& GetInstance();

    const CMexFunc* FindFunc(const char* functionName) const;
};