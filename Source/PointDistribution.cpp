#define INDEXING_2D(particleid, particlecomponent) ((2 * (particleid) + (particlecomponent)))
#define INDEXING_3D(particleid, particlecomponent) ((3 * (particleid) + (particlecomponent)))
#define _USE_MATH_DEFINES
#include <glm/detail/func_geometric.hpp>
#include "PointDistribution.h"
#include "RecursiveSphere.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <ctime>

namespace PointDistribution
{
	static std::ofstream handle_file_to_save;

	constexpr double tolerance_gradient_check = 1e-2;
	static unsigned number_iterations = 0;

	typedef struct { unsigned index; bool use_gradient; } constraint_data;
	typedef struct { bool use_gradient; bool save_data; } function_data;

	static inline std::string get_time_as_string()
	{
		time_t time_value = 0;  time(&time_value);
		return std::to_string(time_value);
	}

	static bool check_gradient(const double & f_1, const double & f_0, const double & g, const double & h)
	{
		double g_approximation = ((f_1 - f_0) / h);
		double g_norm = std::abs(g - g_approximation);
		return (g_norm > tolerance_gradient_check) ? true : false;
	}

	namespace Dimension2D
	{
		static inline double inner_product_2d
		(
			const double * x, 
			unsigned i, 
			unsigned j, 
			unsigned k, 
			unsigned l
		)
		{
			return ((x[i] * x[j]) + (x[k] * x[l]));
		}

		static void generate_points_in_2d_sphere_uniformly(unsigned n, double * x)
		{
			const unsigned number_points = (n / 2);
			for (unsigned i = 0; i < number_points; ++i)
			{
				double normalized_index = i / double(number_points - 1);
				double normalized_angle = (M_PI * normalized_index);

				x[INDEXING_2D(i, 0)] = std::cos(normalized_angle);
				x[INDEXING_2D(i, 1)] = std::sin(normalized_angle);
			}
		}

		static void generate_points_in_2d_sphere_randomly(unsigned n, double * x)
		{
			unsigned number_points = (n / 2);
			for (unsigned i = 0; i < number_points; ++i)
			{
				unsigned random_angle = unsigned(360.0 * (rand() / double(RAND_MAX)));
				double angle_to_radians = (M_PI / 180.0) * random_angle;

				x[INDEXING_2D(i, 0)] = std::cos(angle_to_radians);
				x[INDEXING_2D(i, 1)] = std::sin(angle_to_radians);
			}
		}

		static double compute_energy_function_2d(unsigned n, const double * x)
		{
			double energy = 0.0;
			const unsigned number_particles = (n / 2);
			for (unsigned i = 0; i < number_particles; ++i)
			{
				unsigned i0 = INDEXING_2D(i, 0);
				unsigned i1 = INDEXING_2D(i, 1);

				for (unsigned j = 0; j < i; ++j)
				{
					unsigned j0 = INDEXING_2D(j, 0);
					unsigned j1 = INDEXING_2D(j, 1);

					double normij = sqrt(
						1.0 * inner_product_2d(x, i0, i0, i1, i1) +
						1.0 * inner_product_2d(x, j0, j0, j1, j1) -
						2.0 * inner_product_2d(x, i0, j0, i1, j1)
					);
					energy += (1.0 / normij);
				}
			}
			return energy;
		}

		static double compute_energy_derivative_2d(unsigned n, const double * x, unsigned particle_id, unsigned direction)
		{
			assert(direction < 2);

			unsigned i0 = INDEXING_2D(particle_id, 0);
			unsigned i1 = INDEXING_2D(particle_id, 1);

			double derivative = 0.0;
			const unsigned number_particles = (n / 2);
			for (unsigned j = 0; j < number_particles; ++j)
			{
				if (j == particle_id) continue;

				unsigned j0 = INDEXING_2D(j, 0);
				unsigned j1 = INDEXING_2D(j, 1);

				double innerproduct = (
					1.0 * inner_product_2d(x, i0, i0, i1, i1) +
					1.0 * inner_product_2d(x, j0, j0, j1, j1) -
					2.0 * inner_product_2d(x, i0, j0, i1, j1)
					);
				double distance = std::sqrt(innerproduct);

				double difference = 0.0;
				switch (direction)
				{
				case 0: difference = (x[i0] - x[j0]); break;
				case 1: difference = (x[i1] - x[j1]); break;
				default: std::cerr << "Bad direction, is a 2 component (x, y) function\n"; break;
				}

				derivative += (difference / (distance * distance * distance));
			}

			return (-derivative);
		}
	}

	namespace Dimension3D
	{
		static inline double inner_product_3d
		(
			const double * x,
			unsigned i, 
			unsigned j, 
			unsigned k, 
			unsigned l, 
			unsigned m, 
			unsigned n
		)
		{
			return ((x[i] * x[j]) + (x[k] * x[l]) + (x[m] * x[n]));
		}

		static void generate_points_in_3d_sphere_randomly(unsigned n, double * x)
		{
			unsigned number_points = (n / 3);
			for (unsigned i = 0; i < number_points; ++i)
			{
				unsigned random_theta1 = unsigned(360.0 * (rand() / double(RAND_MAX)));
				unsigned random_theta2 = unsigned(360.0 * (rand() / double(RAND_MAX)));
				double theta1_to_radians = (M_PI / 180.0) * random_theta1;
				double theta2_to_radians = (M_PI / 180.0) * random_theta2;

				x[INDEXING_3D(i, 0)] = std::cos(theta1_to_radians) * std::sin(theta2_to_radians);
				x[INDEXING_3D(i, 1)] = std::sin(theta1_to_radians) * std::sin(theta2_to_radians);
				x[INDEXING_3D(i, 2)] = std::cos(theta2_to_radians);
			}
		}

		static void generate_points_in_3d_sphere_uniformly(unsigned n, double * x)
		{
			const unsigned number_particles = (n / 3);

			unsigned recursive_level = 0;
			if (number_particles <= 0x00008) recursive_level = 0;
			else if (number_particles > 0x00008 && number_particles <= 0x00020) recursive_level = 1;
			else if (number_particles > 0x00020 && number_particles <= 0x00080) recursive_level = 2;
			else if (number_particles > 0x00080 && number_particles <= 0x00200) recursive_level = 3;
			else if (number_particles > 0x00080 && number_particles <= 0x00800) recursive_level = 4;
			else if (number_particles > 0x00800 && number_particles <= 0x02000) recursive_level = 5;
			else if (number_particles > 0x02000 && number_particles <= 0x08000) recursive_level = 6;
			else if (number_particles > 0x02000 && number_particles <= 0x08000) recursive_level = 6;
			else if (number_particles > 0x08000 && number_particles <= 0x20000) recursive_level = 7;
			else if (number_particles > 0x20000 && number_particles <= 0x80000) recursive_level = 8;
			else std::cout << "not implemented, and probably you do not need more than 4 million of particles ... \n";

			RecursiveSphere recursiveSphere;
			recursiveSphere.recompute(recursive_level);

			for (unsigned particle_id = 0; particle_id < number_particles; ++particle_id)
			{
				auto face = recursiveSphere.getFace(particle_id);
				glm::vec3 center_mass = ((face.v1 + face.v2 + face.n3) * 0.33333f);
				center_mass = glm::normalize(center_mass);

				x[INDEXING_3D(particle_id, 0)] = center_mass.x;
				x[INDEXING_3D(particle_id, 1)] = center_mass.y;
				x[INDEXING_3D(particle_id, 2)] = center_mass.z;
			}
		}

		static double compute_energy_derivative_3d(unsigned n, const double * x, unsigned particle_id, unsigned direction)
		{
			assert(direction < 3);

			unsigned i0 = INDEXING_3D(particle_id, 0);
			unsigned i1 = INDEXING_3D(particle_id, 1);
			unsigned i2 = INDEXING_3D(particle_id, 2);

			double derivative = 0.0;
			const unsigned number_particles = (n / 3);
			for (unsigned j = 0; j < number_particles; ++j)
			{
				if (j == particle_id) continue;

				unsigned j0 = INDEXING_3D(j, 0);
				unsigned j1 = INDEXING_3D(j, 1);
				unsigned j2 = INDEXING_3D(j, 2);

				double innerproduct = (
					1.0 * inner_product_3d(x, i0, i0, i1, i1, i2, i2) +
					1.0 * inner_product_3d(x, j0, j0, j1, j1, j2, j2) -
					2.0 * inner_product_3d(x, i0, j0, i1, j1, i2, j2)
					);
				double distance = std::sqrt(innerproduct);

				double difference = 0.0;
				switch (direction)
				{
				case 0: difference = (x[i0] - x[j0]); break;
				case 1: difference = (x[i1] - x[j1]); break;
				case 2: difference = (x[i2] - x[j2]); break;
				default: std::cerr << "Bad direction, is a 3 component (x, y, z) function\n"; break;
				}

				derivative += (difference / (distance * distance * distance));
			}

			return (-derivative);
		}

		static double compute_energy_function_3d(unsigned n, const double * x)
		{
			double energy = 0.0;
			const unsigned number_particles = (n / 3);
			for (unsigned i = 0; i < number_particles; ++i)
			{
				unsigned i0 = INDEXING_3D(i, 0);
				unsigned i1 = INDEXING_3D(i, 1);
				unsigned i2 = INDEXING_3D(i, 2);

				for (unsigned j = 0; j < i; ++j)
				{
					unsigned j0 = INDEXING_3D(j, 0);
					unsigned j1 = INDEXING_3D(j, 1);
					unsigned j2 = INDEXING_3D(j, 2);

					double normij = sqrt(
						1.0 * inner_product_3d(x, i0, i0, i1, i1, i2, i2) +
						1.0 * inner_product_3d(x, j0, j0, j1, j1, j2, j2) -
						2.0 * inner_product_3d(x, i0, j0, i1, j1, i2, j2)
					);
					energy += (1.0 / normij);
				}
			}
			return energy;
		}
	}

	static double objective_function_2d(unsigned n, const double * x, double * g, void * data)
	{
		using namespace Dimension2D;

		function_data * d = (function_data *)data;
		number_iterations++;

		double function_evaluation = compute_energy_function_2d(n, x);

		if (d->use_gradient && g)
		{
			memset((void *)g, 0, sizeof(double) * n);
			for (unsigned index = 0; index < n; ++index) 
				g[index] = compute_energy_derivative_2d(n, x, index / 2, (index % 2));

			double f_perturbed = 0.0;
			const double h = 1e-8;
			
			double * x_perturbed = new double[n];
			memcpy((void *)x_perturbed, (const void *)x, sizeof(double) * n);
			for (unsigned i = 0; i < n; ++i)
			{
				x_perturbed[i] += h;
				f_perturbed = compute_energy_function_2d(n, x_perturbed);
				x_perturbed[i] -= h;

				if (check_gradient(f_perturbed, function_evaluation, g[i], h))
				{ std::cerr << "incorrect gradient computation\n"; }
			}
			delete[] x_perturbed;
		}

		return function_evaluation;
	}

	static double constraint_2d(unsigned n, const double * x, double * g, void * data)
	{
		using namespace Dimension2D;

		constraint_data * d = (constraint_data *)data;
		const unsigned i = (2 * d->index + 0);
		const unsigned j = (2 * d->index + 1);

		double norm_distance = std::sqrt(inner_product_2d(x, i, i, j, j));

		if (d->use_gradient && g)
		{
			memset((void *)g, 0, sizeof(double) * n);
			g[i] = x[i] / norm_distance;
			g[j] = x[j] / norm_distance;
		}

		return (norm_distance - 1.0);
	}

	static inline double constraint_2d_positive(unsigned n, const double * x, double * g, void * data)
	{
		return (+constraint_2d(n, x, g, data));
	}

	static inline double constraint_2d_negative(unsigned n, const double * x, double * g, void * data)
	{
		return (-constraint_2d(n, x, g, data));
	}

	static double objective_function_3d(unsigned n, const double * x, double * g, void * data)
	{
		using namespace Dimension3D;

		function_data * d = (function_data *)data;

		if (d->save_data)
		{
			if (handle_file_to_save.is_open())
			{
				for (unsigned i = 0; i < n; ++i) handle_file_to_save << x[i] << "\n";
				handle_file_to_save << "Iteration " << number_iterations << "\n";
			}
		}

		number_iterations++;

		double function_evaluation = compute_energy_function_3d(n, x);

		if (d->use_gradient && g)
		{
			memset((void *)g, 0, sizeof(double) * n);
			for (unsigned index = 0; index < n; ++index)
				g[index] = compute_energy_derivative_3d(n, x, index / 3, (index % 3));

			double f_perturbed = 0.0;
			const double h = 1e-8;

			double * x_perturbed = new double[n];
			memcpy((void *)x_perturbed, (const void *)x, sizeof(double) * n);
			for (unsigned i = 0; i < n; ++i)
			{
				x_perturbed[i] += h;
				f_perturbed = compute_energy_function_3d(n, x_perturbed);
				x_perturbed[i] -= h;

				if (check_gradient(f_perturbed, function_evaluation, g[i], h))
				{ std::cerr << "incorrect gradient computation\n"; }
			}

			delete[] x_perturbed;
		}

		return function_evaluation;
	}

	static double constraint_3d(unsigned n, const double * x, double * g, void * data)
	{
		using namespace Dimension3D;

		constraint_data * d = (constraint_data *)data;
		const unsigned i = INDEXING_3D(d->index, 0);
		const unsigned j = INDEXING_3D(d->index, 1);
		const unsigned k = INDEXING_3D(d->index, 2);

		double norm_distance = std::sqrt(inner_product_3d(x, i, i, j, j, k, k));

		if (d->use_gradient && g)
		{
			memset((void *)g, 0, sizeof(double) * n);
			g[i] = x[i] / norm_distance;
			g[j] = x[j] / norm_distance;
			g[k] = x[k] / norm_distance;
		}

		return (norm_distance - 1.0);
	}

	static inline double constraint_3d_positive(unsigned n, const double * x, double * g, void * data)
	{
		return (+constraint_3d(n, x, g, data));
	}

	static inline double constraint_3d_negative(unsigned n, const double * x, double * g, void * data)
	{
		return (-constraint_3d(n, x, g, data));
	}

	std::pair<unsigned, float> run_2d
	(
		std::vector<glm::vec2> & optimized_points,
		std::vector<glm::vec2> & initial_points,
		alg optimization_algorithm_to_use,
		const unsigned max_num_iterations,
		const unsigned number_particles,
		const unsigned distribution,
		bool use_equality,
		bool use_gradient,
		bool verbose
	)
	{
		using namespace Dimension2D;
		number_iterations = 0;

		const unsigned number_parameters = (number_particles * 2);
		double * x = new double[number_parameters];

		switch (distribution)
		{
		default: std::cerr << "Not implemented\n";
		case 0: generate_points_in_2d_sphere_uniformly(number_parameters, x); break;
		case 1: generate_points_in_2d_sphere_randomly(number_parameters, x); break;
		}

		for (unsigned i = 0; i < number_particles; ++i) 
		{ initial_points.push_back(glm::vec2(x[2 * i + 0], x[2 * i + 1])); }

		nlopt_opt opt = nlopt_create(optimization_algorithm_to_use, number_parameters);
		nlopt_set_maxeval(opt, max_num_iterations);
		nlopt_set_xtol_rel(opt, 1e-8);

		function_data f[1] = { use_gradient };
		nlopt_set_min_objective(opt, objective_function_2d, &f[0]);

		constraint_data * c = new constraint_data[number_particles];
		for (unsigned i = 0; i < number_particles; ++i) 
		{ 
			c[i].use_gradient = use_gradient;
			c[i].index = i; 


			if (use_equality) nlopt_add_equality_constraint(opt, constraint_2d, &c[i], 1e-8);
			else
			{
				nlopt_add_inequality_constraint(opt, constraint_2d_positive, &c[i], 1e-8);
				nlopt_add_inequality_constraint(opt, constraint_2d_negative, &c[i], 1e-8);
			}
		}

		double minize_function_value = 0.0;
		nlopt_optimize(opt, x, &minize_function_value);

		if (verbose) printf("%d, %d, %f\n", number_particles, number_iterations, minize_function_value);

		for (unsigned i = 0; i < number_particles; ++i) 
		{ optimized_points.push_back(glm::vec2(x[2 * i + 0], x[2 * i + 1])); }

		nlopt_destroy(opt);
		delete[] c;
		delete[] x;

		return std::make_pair(number_iterations, (float)minize_function_value);
	}

	std::pair<unsigned, float> run_3d
	(
		std::vector<glm::vec3> & optimized_points,
		std::vector<glm::vec3> & initial_points,
		alg optimization_algorithm_to_use,
		const unsigned max_num_iterations,
		const unsigned number_particles,
		const unsigned distribution,
		bool use_max_iterations,
		bool use_equality,
		bool use_gradient,
		bool save_data,
		bool verbose
	)
	{
		using namespace Dimension3D;

		if (save_data)
		{
			std::string file_name = "Solutions" + get_time_as_string() + ".txt";
			handle_file_to_save.open(file_name);
		}

		number_iterations = 0;

		const unsigned number_parameters = (number_particles * 3);
		double * x = new double[number_parameters];

		switch (distribution)
		{
		default: std::cerr << "Not implemented\n";
		case 0: generate_points_in_3d_sphere_uniformly(number_parameters, x); break;
		case 1: generate_points_in_3d_sphere_randomly(number_parameters, x); break;
		}

		for (unsigned i = 0; i < number_particles; ++i)
		{ initial_points.push_back(glm::vec3(x[3 * i + 0], x[3 * i + 1], x[3 * i + 2])); }

		nlopt_opt opt = nlopt_create(optimization_algorithm_to_use, number_parameters);
		if (use_max_iterations) nlopt_set_maxeval(opt, max_num_iterations);

		nlopt_set_xtol_rel(opt, 1e-8);

		function_data f[1] = { use_gradient, save_data };
		nlopt_set_min_objective(opt, objective_function_3d, &f[0]);

		constraint_data * c = new constraint_data[number_particles];
		for (unsigned i = 0; i < number_particles; ++i) 
		{ 
			c[i].use_gradient = use_gradient;
			c[i].index = i; 

			if (use_equality) nlopt_add_equality_constraint(opt, constraint_3d, &c[i], 1e-8);
			else
			{
				nlopt_add_inequality_constraint(opt, constraint_3d_positive, &c[i], 1e-8);
				nlopt_add_inequality_constraint(opt, constraint_3d_negative, &c[i], 1e-8);
			}
		}

		double minize_function_value = 0.0;
		nlopt_optimize(opt, x, &minize_function_value);

		if(verbose) printf("%d, %d, %f\n", number_particles, number_iterations, minize_function_value);

		for (unsigned i = 0; i < number_particles; ++i) 
		{ optimized_points.push_back(glm::vec3(x[3 * i + 0], x[3 * i + 1], x[3 * i + 2])); }

		nlopt_destroy(opt);
		delete[] c;
		delete[] x;		
		if (save_data) handle_file_to_save.close();

		return std::make_pair(number_iterations, (float)minize_function_value);
	}
}