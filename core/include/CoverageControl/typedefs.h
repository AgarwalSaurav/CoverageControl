/**
 * Contains typedefs
 **/

#ifndef COVERAGECONTROL_TYPEDEFS_H_
#define COVERAGECONTROL_TYPEDEFS_H_

#include <vector>
#include <Eigen/Dense> // Eigen is used for maps

namespace CoverageControl {

	struct Edge {
		double x1, y1, x2, y2;
		Edge(double const x1_in, double const y1_in, double const x2_in, double const y2_in) : x1{x1_in}, y1{y1_in}, x2{x2_in}, y2{y2_in} {}
	};
	typedef Eigen::Vector2d Point2;
	typedef Eigen::Vector3d Point3;
	typedef Eigen::MatrixXd MapTypeDbl;
	typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MapType;
	typedef Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MapTypeBool;

	typedef std::vector<Point2> PointVector;
	struct PolygonFeature {
		PointVector poly;
		float imp = 0;
		int size = 0;
		PolygonFeature() {  }
		PolygonFeature(PointVector const &p, float const i) : poly{p}, imp{i} { size = poly.size(); }
	};

} /* namespace CoverageControl */
#endif /* COVERAGECONTROL_TYPEDEFS_H_ */

