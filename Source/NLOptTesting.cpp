#include "NLOptTesting.h"
#include <cstdio>
#include <cmath>

namespace NLOptTesting
{
	typedef struct { bool use_gradient; double a; double b; } constraint_data;
	typedef struct { bool use_gradient; } function_data;

	static double constraint(unsigned n, const double * x, double * g, void * data)
	{
		constraint_data * d = (constraint_data *)data;
		const double a = d->a;
		const double b = d->b;

		const double base = (a * x[0] + b);

		if (d->use_gradient && g)
		{
			g[0] = 3.0 * a * std::pow(base, 2);
			g[1] = -1.0;
		}

		return (std::pow(base, 3) - x[1]);
	}

	static double objective_function(unsigned n, const double * x, double * g, void * data)
	{
		function_data * d = (function_data *)data;

		if (d->use_gradient && g)
		{
			g[0] = 0.0;
			g[1] = 0.5 / std::sqrt(x[1]);
		}

		return std::sqrt(x[1]);
	}

	void run(nlopt_algorithm algorithm, bool use_gradient)
	{
		nlopt_opt opt = nlopt_create(algorithm, 2);

		function_data f[1] = { use_gradient };
		nlopt_set_min_objective(opt, objective_function, &f[0]);
		nlopt_set_xtol_rel(opt, 1e-4);
		nlopt_set_maxeval(opt, 1000);

		constraint_data c[2] = { { use_gradient, 2.0, 0.0 }, { use_gradient, -1.0, 1.0 } };
		nlopt_add_inequality_constraint(opt, constraint, &c[0], 1e-8);
		nlopt_add_inequality_constraint(opt, constraint, &c[1], 1e-8);

		double x[2] = { 1.234, 5.678 };
		double minize_function_value = 0.0;

		nlopt_optimize(opt, x, &minize_function_value);

		printf("f(%f, %f) = %f\n", x[0], x[1], minize_function_value);

		nlopt_destroy(opt);
	}
};
