#pragma once
#include <api/nlopt.h>

namespace NLOptTesting
{
	void run(nlopt_algorithm = NLOPT_LN_COBYLA, bool = false);
}