#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>

#include <vector>
#include <CoverageControl/vec2d.h>
#include <CoverageControl/typedefs.h>
#include <CoverageControl/parameters.h>
#include <CoverageControl/bivariate_normal_distribution.h>
#include <CoverageControl/world_idf.h>
#include <CoverageControl/robot_model.h>
#include <CoverageControl/coverage_system.h>
#include <CoverageControl/voronoi.h>
#include <CoverageControl/geographiclib_wrapper.h>


using namespace CoverageControl;
typedef std::vector<BivariateNormalDistribution> BNDVector;
namespace py = pybind11;
using namespace pybind11::literals;


PYBIND11_MODULE(pyCoverageControl, m) {
	m.doc() = "CoverageControl library";

	py::bind_vector<std::vector<double>>(m, "DblVector");

	m.def("Point2", []() {return Point2(0, 0);});
	m.def("Point2", [](double const &a, double const &b) { return Point2(a, b);});

	py::bind_vector<PointVector>(m, "PointVector");

	py::class_<PolygonFeature>(m, "PolygonFeature")
		.def(py::init<>())
		.def(py::init<PointVector const &, float const>())
		.def_readwrite("poly", &PolygonFeature::poly)
		.def_readwrite("imp", &PolygonFeature::imp)
		.def_readwrite("size", &PolygonFeature::size)
		;
	py::bind_vector<std::vector<PolygonFeature>>(m, "PolygonVector");

	py::class_<Edge>(m, "Edge")
		.def(py::init<double const, double const, double const, double const>())
		.def_readwrite("x1", &Edge::x1)
		.def_readwrite("y1", &Edge::y1)
		.def_readwrite("x2", &Edge::x2)
		.def_readwrite("y2", &Edge::y2)
		;
	py::bind_vector<std::vector<Edge>>(m, "EdgeList");

	py::class_<VoronoiCell>(m, "VoronoiCell")
		.def_readonly("cell", &VoronoiCell::cell)
		.def_readonly("mass", &VoronoiCell::mass)
		.def_readonly("centroid", &VoronoiCell::centroid)
		.def_readonly("site", &VoronoiCell::site)
		;
	py::bind_vector<std::vector<VoronoiCell>>(m, "VoronoiCells");

	py::class_<BivariateNormalDistribution>(m, "BivariateNormalDistribution")
		.def(py::init<>())
		.def(py::init<Point2 const, double const, double const>())
		.def(py::init<Point2 const, Point2 const, double const, double const>())
		;

	py::bind_vector<BNDVector>(m, "BNDVector");

	py::class_<WorldIDF>(m, "WorldIDF")
		.def(py::init<Parameters const &>())
		.def("AddUniformDistributionPolygon", &WorldIDF::AddUniformDistributionPolygon)
		.def("AddNormalDistribution", py::overload_cast<BivariateNormalDistribution const &>(&WorldIDF::AddNormalDistribution))
		.def("AddNormalDistribution", py::overload_cast<BNDVector const &>(&WorldIDF::AddNormalDistribution))
		.def("GenerateMap", &WorldIDF::GenerateMap)
		.def("GenerateMapCuda", &WorldIDF::GenerateMapCuda)
		.def("GetWorldMap", &WorldIDF::GetWorldMap, py::return_value_policy::reference_internal)
		.def("WriteWorldMap", &WorldIDF::WriteWorldMap)
		.def("GetNormalizationFactor", &WorldIDF::GetNormalizationFactor)
		.def("PrintMapSize", &WorldIDF::PrintMapSize)
		;

	py::class_<RobotModel>(m, "RobotModel")
		.def(py::init<Parameters const &, Point2 const, WorldIDF const>())
		.def("StepControl", &RobotModel::StepControl)
		.def("UpdateRobotPosition", &RobotModel::UpdateRobotPosition)
		.def("GetGlobalStartPosition", &RobotModel::GetGlobalStartPosition)
		.def("GetGlobalCurrentPosition", &RobotModel::GetGlobalCurrentPosition)
		.def("GetRobotMap", &RobotModel::GetRobotMap, py::return_value_policy::reference_internal)
		.def("GetRobotLocalMap", &RobotModel::GetRobotLocalMap, py::return_value_policy::reference_internal)
		.def("GetSensorView", &RobotModel::GetSensorView, py::return_value_policy::reference_internal)
		.def("GetExplorationMap", &RobotModel::GetExplorationMap, py::return_value_policy::reference_internal)
		;

	py::class_<CoverageSystem>(m, "CoverageSystem")
		.def(py::init<Parameters const &, int const, int const>())
		.def(py::init<Parameters const &, WorldIDF const &, PointVector const &>())
		.def(py::init<Parameters const &, BNDVector const &, PointVector const &>())
		.def("GetWorldIDF", &CoverageSystem::GetWorldIDF, py::return_value_policy::reference_internal)
		.def("StepControl", &CoverageSystem::StepControl)
		.def("UpdateRobotPositions", &CoverageSystem::UpdateRobotPositions)
		.def("GetRobotPositions", &CoverageSystem::GetRobotPositions)
		.def("GetRobotPosition", &CoverageSystem::GetRobotPosition)
		.def("GetRobotLocalMap", &CoverageSystem::GetRobotLocalMap, py::return_value_policy::reference_internal)
		.def("GetRobotSensorView", &CoverageSystem::GetRobotSensorView, py::return_value_policy::reference_internal)
		.def("GetCommunicationMap", &CoverageSystem::GetCommunicationMap, py::return_value_policy::reference_internal)
		.def("ComputeVoronoiCells", &CoverageSystem::ComputeVoronoiCells, py::return_value_policy::reference_internal)
		.def("GetVoronoiCells", &CoverageSystem::GetVoronoiCells, py::return_value_policy::copy)
		/* .def("GetVoronoiEdges", &CoverageSystem::GetVoronoiEdges) */
		.def("StepLloyd", &CoverageSystem::StepLloyd)
		.def("Lloyd", &CoverageSystem::Lloyd)
		.def("LloydOffline", &CoverageSystem::LloydOffline, py::return_value_policy::copy)
		.def("StepOracle", &CoverageSystem::StepOracle)
		.def("StepOracleN", &CoverageSystem::StepOracleN)
		.def("GetOracleMap", &CoverageSystem::GetOracleMap, py::return_value_policy::reference_internal)
		.def("GetVoronoiCell", &CoverageSystem::GetVoronoiCell, py::return_value_policy::copy)
		.def("StepDataGenerationLocal", &CoverageSystem::StepDataGenerationLocal, py::return_value_policy::copy)
		.def("GetRobotObstacleMap", &CoverageSystem::GetRobotObstacleMap, py::return_value_policy::copy)
		.def("GetRobotExplorationMap", &CoverageSystem::GetRobotExplorationMap, py::return_value_policy::reference_internal)
		;

	py::class_<Parameters>(m, "Parameters")
		.def(py::init<>())
		.def(py::init<std::string const &>())
		.def("SetConfig", &Parameters::SetConfig)
		.def_readonly("pResolution", &Parameters::pResolution)
		.def_readonly("pWorldMapSize", &Parameters::pWorldMapSize)
		.def_readonly("pRobotMapSize", &Parameters::pRobotMapSize)
		.def_readonly("pUnknownImportance", &Parameters::pUnknownImportance)
		.def_readonly("pLocalMapSize", &Parameters::pLocalMapSize)
		.def_readonly("pCommunicationRange", &Parameters::pCommunicationRange)
		.def_readonly("pRobotInitDist", &Parameters::pRobotInitDist)
		.def_readonly("pUpdateRobotMap", &Parameters::pUpdateRobotMap)
		.def_readonly("pUpdateSensorView", &Parameters::pUpdateSensorView)
		.def_readonly("pSensorSize", &Parameters::pSensorSize)
		.def_readonly("pTimeStep", &Parameters::pTimeStep)
		.def_readonly("pMaxRobotSpeed", &Parameters::pMaxRobotSpeed)
		.def_readonly("pEpisodeSteps", &Parameters::pEpisodeSteps)
		.def_readonly("pTruncationBND", &Parameters::pTruncationBND)
		.def_readonly("pNorm", &Parameters::pNorm)
		.def_readonly("pMinSigma", &Parameters::pMinSigma)
		.def_readonly("pMaxSigma", &Parameters::pMaxSigma)
		.def_readonly("pMinPeak", &Parameters::pMinPeak)
		.def_readonly("pMaxPeak", &Parameters::pMaxPeak)
		.def_readonly("pLloydNumTries", &Parameters::pLloydNumTries)
		.def_readonly("pLloydMaxIterations", &Parameters::pLloydMaxIterations)
		;

	py::class_<GeoLocalTransform>(m, "GeoLocalTransform")
		.def(py::init<double, double, double>())
		.def("Reset", &GeoLocalTransform::Reset)
		.def("Forward", &GeoLocalTransform::Forward)
		.def("Reverse", &GeoLocalTransform::Reverse)
		;

}
