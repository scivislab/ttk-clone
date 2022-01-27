/// \ingroup base
/// \class ttk::MetricDistortion
/// \author Mathieu Pont <mathieu.pont@lip6.fr>
/// \date 2022.
///
/// This module defines the %MetricDistortion class that computes TODO
///

#pragma once

// ttk common includes
#include <Debug.h>
#include <Triangulation.h>

namespace ttk {

  /**
   * The MetricDistortion class provides methods to compute TODO
   */
  class MetricDistortion : virtual public Debug {

  public:
    MetricDistortion();

    void
      computeSurfaceCurvature(std::vector<std::vector<double>> &surfacePoints,
                              std::vector<std::vector<int>> &surfaceCells,
                              std::vector<std::vector<double>> &distanceMatrix,
                              std::vector<double> &surfaceCurvature,
                              std::vector<double> &metricCurvature,
                              std::vector<double> &ratioCurvature) {
      unsigned int dim = surfacePoints.size();
      surfaceCurvature = std::vector<double>(dim, std::nan(""));
      metricCurvature = std::vector<double>(dim, std::nan(""));
      ratioCurvature = std::vector<double>(dim, std::nan(""));

      std::vector<std::vector<std::tuple<int, int>>> point2CellPoints(dim);
      for(unsigned int i = 0; i < surfaceCells.size(); ++i) {
        if(surfaceCells[i].size() < 3 or surfaceCells[i].size() > 4)
          continue;
        auto cellNoPoints = surfaceCells[i].size();
        for(unsigned int j = 0; j < cellNoPoints; ++j) {
          std::tuple<int, int> tup
            = std::make_tuple((j + 1) % cellNoPoints, (j + 2) % cellNoPoints);
          if(cellNoPoints == 4)
            tup = std::make_tuple(
              (j != 3 ? j + 1 : j - 2), (j != 0 ? j - 1 : j + 2));
          point2CellPoints[surfaceCells[i][j]].push_back(tup);
        }
      }

      for(unsigned int i = 0; i < dim; ++i) {
        double sumAngleSurface = 0.0, sumAngleMetric = 0.0;

        for(unsigned int j = 0; j < point2CellPoints[i].size(); ++j) {
          auto tup = point2CellPoints[i][j];
          int i0 = std::get<0>(tup);
          int i1 = std::get<1>(tup);
          auto p_i0 = surfacePoints[i0];
          auto p_i1 = surfacePoints[i1];
          auto p_i = surfacePoints[i];
          sumAngleSurface
            += cosineLaw(l2Distance(p_i, p_i0), l2Distance(p_i, p_i1),
                         l2Distance(p_i0, p_i1));
          if(distanceMatrix.size() != 0)
            sumAngleMetric
              += cosineLaw(distanceMatrix[i][i0], distanceMatrix[i][i1],
                           distanceMatrix[i0][i1]);
        }

        surfaceCurvature[i] = 2 * M_PI - sumAngleSurface;

        if(distanceMatrix.size() != 0) {
          metricCurvature[i] = 2 * M_PI - sumAngleMetric;
          ratioCurvature[i] = metricCurvature[i] / surfaceCurvature[i];
        }
      }
    }

    void
      computeSurfaceDistance(std::vector<std::vector<double>> &surfacePoints,
                             std::vector<std::vector<int>> &surfaceCells,
                             std::vector<std::vector<double>> &distanceMatrix,
                             std::vector<double> &surfaceDistance,
                             std::vector<double> &metricDistance,
                             std::vector<double> &ratioDistance) {
      unsigned int dim = surfaceCells.size();
      surfaceDistance = std::vector<double>(dim, std::nan(""));
      metricDistance = std::vector<double>(dim, std::nan(""));
      ratioDistance = std::vector<double>(dim, std::nan(""));
      for(unsigned int i = 0; i < dim; ++i) {
        if(surfaceCells[i].size() != 2)
          continue;

        int i0 = surfaceCells[i][0];
        int i1 = surfaceCells[i][1];

        surfaceDistance[i] = l2Distance(surfacePoints[i0], surfacePoints[i1]);

        if(distanceMatrix.size() != 0) {
          metricDistance[i] = distanceMatrix[i0][i1];
          ratioDistance[i] = metricDistance[i] / surfaceDistance[i];
        }
      }
    }

    void computeSurfaceArea(std::vector<std::vector<double>> &surfacePoints,
                            std::vector<std::vector<int>> &surfaceCells,
                            std::vector<std::vector<double>> &distanceMatrix,
                            std::vector<double> &surfaceArea,
                            std::vector<double> &metricArea,
                            std::vector<double> &ratioArea) {
      unsigned int dim = surfaceCells.size();
      surfaceArea = std::vector<double>(dim, std::nan(""));
      metricArea = std::vector<double>(dim, std::nan(""));
      ratioArea = std::vector<double>(dim, std::nan(""));
      for(unsigned int i = 0; i < dim; ++i) {
        if(surfaceCells[i].size() < 3 or surfaceCells[i].size() > 4)
          continue;

        int i0 = surfaceCells[i][0];
        int i1 = surfaceCells[i][1];
        int i2 = surfaceCells[i][2];

        surfaceArea[i] = triangleArea3D(
          surfacePoints[i0], surfacePoints[i1], surfacePoints[i2]);

        if(distanceMatrix.size() != 0)
          metricArea[i] = triangleAreaFromSides(distanceMatrix[i0][i1],
                                                distanceMatrix[i1][i2],
                                                distanceMatrix[i2][i1]);

        if(surfaceCells[i].size() == 4) {
          int i3 = surfaceCells[i][3];
          surfaceArea[i] += triangleArea3D(
            surfacePoints[i1], surfacePoints[i2], surfacePoints[i3]);
          if(distanceMatrix.size() != 0)
            metricArea[i] += triangleAreaFromSides(distanceMatrix[i1][i2],
                                                   distanceMatrix[i2][i3],
                                                   distanceMatrix[i3][i1]);
        }

        if(distanceMatrix.size() != 0)
          ratioArea[i] = metricArea[i] / surfaceArea[i];
      }
    }

    //-------------------------------------------------------------------------
    // Utils
    //-------------------------------------------------------------------------
    double cosineLaw(double a, double b, double c) {
      // Get the angle opposite to edge c
      return std::acos((a * a + b * b - c * c) / (2.0 * a * b));
    }

    double l2Distance(std::vector<double> &p1, std::vector<double> &p2) {
      double distance = 0.0;
      for(unsigned int i = 0; i < p1.size(); ++i)
        distance += std::pow(p1[i] - p2[i], 2);
      return std::sqrt(distance);
    }

    double triangleArea3D(std::vector<double> &x1,
                          std::vector<double> &x2,
                          std::vector<double> &x3) {
      std::vector<double> AB(3), AC(3);
      for(unsigned int i = 0; i < 3; ++i) {
        AB[i] = x2[i] - x1[i];
        AC[i] = x3[i] - x1[i];
      }
      return std::sqrt(std::pow(AB[1] * AC[2] - AB[2] * AC[1], 2)
                       + std::pow(AB[2] * AC[0] - AB[0] * AC[2], 2)
                       + std::pow(AB[0] * AC[1] - AB[1] * AC[0], 2))
             / 2.0;
    }

    double triangleAreaFromSides(double a, double b, double c) {
      double s = (a + b + c) / 2.0;
      return std::sqrt(s * (s - a) * (s - b) * (s - c));
    }
  }; // MetricDistortion class

} // namespace ttk
