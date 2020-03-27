/// \ingroup base
/// \class ttk::PersistenceDiagramDistanceMatrix
/// \author Jules Vidal <jules.vidal@lip6.fr>
/// \author Pierre Guillou <pierre.guillou@lip6.fr>
/// \date March 2020
///
/// \b Related \b publication \n
/// "Progressive Wasserstein Barycenters of Persistence Diagrams" \n
/// Jules Vidal, Joseph Budin and Julien Tierny \n
/// Proc. of IEEE VIS 2019.\n
/// IEEE Transactions on Visualization and Computer Graphics, 2019.
///
/// \sa PersistenceDiagramClustering

#pragma once

#include <array>
#include <limits>

#include <Auction.h>
#include <Wrapper.h>

namespace ttk {

  using DiagramTuple = std::tuple<ttk::SimplexId,
                                  ttk::CriticalType,
                                  ttk::SimplexId,
                                  ttk::CriticalType,
                                  double,
                                  ttk::SimplexId,
                                  double,
                                  float,
                                  float,
                                  float,
                                  double,
                                  float,
                                  float,
                                  float>;
  using MatchingTuple = std::tuple<ttk::SimplexId, ttk::SimplexId, double>;

  class PersistenceDiagramDistanceMatrix : public Debug {

  public:
    void execute(std::vector<std::vector<DiagramTuple>> &intermediateDiagrams);

    double getMostPersistent(int type = -1);
    double getLessPersistent(int type = -1);

    double computeDistance(const BidderDiagram<double> &D1,
                           const BidderDiagram<double> &D2,
                           const double delta_lim);
    double computeDistance(const BidderDiagram<double> D1,
                           const GoodDiagram<double> D2,
                           const double delta_lim);
    double computeDistance(BidderDiagram<double> *const D1,
                           const GoodDiagram<double> *const D2,
                           const double delta_lim);
    double computeDistance(const GoodDiagram<double> &D1,
                           const GoodDiagram<double> &D2,
                           const double delta_lim);

    GoodDiagram<double>
      centroidWithZeroPrices(const GoodDiagram<double> centroid);
    BidderDiagram<double> centroidToDiagram(const GoodDiagram<double> centroid);
    GoodDiagram<double> diagramToCentroid(const BidderDiagram<double> diagram);
    BidderDiagram<double>
      diagramWithZeroPrices(const BidderDiagram<double> diagram);

    void setBidderDiagrams();

    std::vector<double> enrichCurrentBidderDiagrams(
      std::vector<double> previous_min_persistence,
      std::vector<double> min_persistence,
      std::vector<std::vector<double>> initial_diagonal_prices,
      std::vector<std::vector<double>> initial_off_diagonal_points,
      std::vector<int> min_points_to_add,
      bool add_points_to_barycenter);

    std::vector<std::vector<double>> getDistanceMatrix();

    inline void resetDosToOriginalValues() {
      do_min_ = original_dos[0];
      do_sad_ = original_dos[1];
      do_max_ = original_dos[2];
    }

    inline void setNumberOfInputs(int numberOfInputs) {
      numberOfInputs_ = numberOfInputs;
    }

    inline void setNumberOfClusters(const int k) {
      k_ = k;
    }

    inline void setWasserstein(const std::string &wasserstein) {
      wasserstein_ = (wasserstein == "inf") ? -1 : stoi(wasserstein);
    }

    inline void setUseProgressive(const bool use_progressive) {
      use_progressive_ = use_progressive;
    }

    inline void setUseKmeansppInit(const bool use_kmeanspp) {
      use_kmeanspp_ = use_kmeanspp;
    }

    inline void setUseKDTree(const bool use_kdtree) {
      use_kdtree_ = use_kdtree;
    }

    inline void setUseAccelerated(const bool use_accelerated) {
      use_accelerated_ = use_accelerated;
    }

    inline void setTimeLimit(const double time_limit) {
      time_limit_ = time_limit;
    }

    inline void setPairTypeClustering(const int pairTypeClustering) {
      pairTypeClustering_ = pairTypeClustering;
    }

    inline void setAlpha(const double alpha) {
      geometrical_factor_ = alpha;
    }
    inline void setLambda(const double lambda) {
      lambda_ = lambda;
    }
    inline void setForceUseOfAlgorithm(const bool forceUseOfAlgorithm) {
      forceUseOfAlgorithm_ = forceUseOfAlgorithm;
    }
    inline void setDeterministic(const bool deterministic) {
      deterministic_ = deterministic;
    }

    inline void setUseDeltaLim(const bool UseDeltaLim) {
      UseDeltaLim_ = UseDeltaLim;
      if(UseDeltaLim_) {
        epsilon_min_ = 1e-8;
      } else {
        epsilon_min_ = 5e-5;
      }
    }

    inline void setDistanceWritingOptions(const int distanceWritingOptions) {
      distanceWritingOptions_ = distanceWritingOptions;
    }
    inline void setDeltaLim(const double deltaLim) {
      deltaLim_ = deltaLim;
    }

    inline void setUseFullDiagrams(const bool arg) {
      useFullDiagrams_ = arg;
    }

    std::vector<std::vector<double>> getDiagramsDistMat();

  protected:
    bool barycenter_inputs_reset_flag;
    bool precision_criterion_{false};
    bool precision_max_{false};
    bool precision_min_{false};
    bool precision_sad_{false};
    bool forceUseOfAlgorithm_{false};
    bool deterministic_{true};
    int wasserstein_{2};
    double geometrical_factor_{1};
    double deltaLim_;
    bool UseDeltaLim_{false};
    int distanceWritingOptions_{0};
    // lambda : 0<=lambda<=1
    // parametrizes the point used for the physical (critical) coordinates of
    // the persistence paired lambda = 1 : extremum (min if pair min-sad, max if
    // pair sad-max) lambda = 0 : saddle (bad stability) lambda = 1/2 : middle
    // of the 2 critical points of the pair
    double lambda_;

    int k_;
    int numberOfInputs_;
    bool use_progressive_{true};
    bool use_accelerated_;
    bool use_kmeanspp_;
    bool use_kdtree_{true};
    double time_limit_{std::numeric_limits<double>::max()};

    double epsilon_min_{1e-8};
    std::array<double, 3> epsilon_;
    double cost_;
    double cost_min_{0};
    double cost_sad_{0};
    double cost_max_{0};

    std::vector<std::vector<int>> current_bidder_ids_min_;
    std::vector<std::vector<int>> current_bidder_ids_sad_;
    std::vector<std::vector<int>> current_bidder_ids_max_;
    std::vector<std::vector<DiagramTuple>> inputDiagramsMin_;
    std::vector<std::vector<DiagramTuple>> inputDiagramsSaddle_;
    std::vector<std::vector<DiagramTuple>> inputDiagramsMax_;

    std::array<bool, 3> original_dos;

    bool do_min_;
    std::vector<BidderDiagram<double>> bidder_diagrams_min_;
    std::vector<BidderDiagram<double>> current_bidder_diagrams_min_;
    std::vector<GoodDiagram<double>> centroids_min_;
    std::vector<GoodDiagram<double>> centroids_with_price_min_;

    bool do_sad_;
    std::vector<BidderDiagram<double>> bidder_diagrams_saddle_;
    std::vector<BidderDiagram<double>> current_bidder_diagrams_saddle_;
    std::vector<GoodDiagram<double>> centroids_saddle_;
    std::vector<GoodDiagram<double>> centroids_with_price_saddle_;

    bool do_max_;
    std::vector<BidderDiagram<double>> bidder_diagrams_max_;
    std::vector<BidderDiagram<double>> current_bidder_diagrams_max_;
    std::vector<GoodDiagram<double>> centroids_max_;
    std::vector<GoodDiagram<double>> centroids_with_price_max_;

    std::vector<std::vector<int>> clustering_;
    std::vector<std::vector<int>> old_clustering_;
    std::vector<int> inv_clustering_;

    std::vector<std::vector<int>> centroids_sizes_;

    std::vector<bool> r_;
    std::vector<double> u_;
    std::vector<std::vector<double>> l_;
    std::vector<std::vector<double>> centroidsDistanceMatrix_{};
    bool useFullDiagrams_{false};

    int n_iterations_;
    int pairTypeClustering_;
  };
} // namespace ttk
