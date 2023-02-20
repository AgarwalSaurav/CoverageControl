#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <thread>

#include <CoverageControl/constants.h>
#include <CoverageControl/parameters.h>
#include <CoverageControl/typedefs.h>
#include <CoverageControl/bivariate_normal_distribution.h>
#include <CoverageControl/world_idf.h>
#include <CoverageControl/robot_model.h>
#include <CoverageControl/generate_world_map.ch>
#include <CoverageControl/coverage_system.h>
#include <CoverageControl/algorithms/lloyd_global_online.h>
#include <CoverageControl/algorithms/simul_explore_exploit.h>
#include <CoverageControl/algorithms/lloyd_local_voronoi.h>
#include <CoverageControl/algorithms/oracle_global_offline.h>

using namespace CoverageControl;

/* typedef LloydGlobalOnline CoverageAlgorithm; */
/* typedef LloydLocalVoronoi CoverageAlgorithm; */
typedef OracleGlobalOffline CoverageAlgorithm;
/* typedef OracleSimulExploreExploit CoverageAlgorithm; */

int main(int argc, char** argv) {
	Parameters params;
	/* params.pSensorSize = 16; */
	if (argc == 2) {
		std::string parameter_file = argv[1];
		params = Parameters(parameter_file);
	}
	std::cout << "Processor count: " << std::thread::hardware_concurrency() << std::endl;

	int num_robots = 15;
	int num_dists = 10;
	CoverageSystem env(params, num_dists, num_robots);
	CoverageAlgorithm oracle(params, num_robots, env);

	std::string dir = "data/test/";
	env.PlotWorldMap(dir);
	auto goals = oracle.GetGoals();
	for(int ii = 0; ii < params.pEpisodeSteps; ++ii) {
		std::cout << "Step: " << ii << std::endl;
		bool cont_flag = oracle.Step();
		auto actions = oracle.GetActions();
		env.StepActions(actions);
		if(ii%1 == 0) {
			env.RecordPlotData();
		}
		if(cont_flag == false) {
			break;
		}
	}
	std::cout << "Converged" << std::endl;
	std::cout << "Exploration ratio: " << env.GetExplorationRatio() << " Weighted: " << env.GetWeightedExplorationRatio() << std::endl;
	std::cout << "Coverage objective: " << env.GetObjectiveValue() << std::endl;
	auto zero_actions = PointVector(num_robots, Point2(0,0));

	for(int ii = 0; ii < 90; ++ii) {
		env.StepActions(zero_actions);
		env.RecordPlotData();
	}

	env.RenderRecordedMap(dir, "CoverageControl_ExploreExploit.mp4");
	return 0;
}