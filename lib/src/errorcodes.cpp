#include "errorcodes.h"

// cf. https://www.mathworks.com/help/matlab/apiref/mexerrmsgidandtxt.html and
//      https://www.mathworks.com/help/matlab/ref/mexception.html

#define COMPONENTFIELD "MEXlibCZI"

/*static*/const char* ErrorIds::UnknownCommand = COMPONENTFIELD ":UnknownCommand";

/*static*/const char* ErrorIds::InvalidArguments = COMPONENTFIELD ":InvalidArguments";