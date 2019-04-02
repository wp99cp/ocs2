//
// Created by ruben on 02.11.18.
//

#include <gtest/gtest.h>

#include <ocs2_core/loopshaping/LoopshapingDefinition.h>
#include <ocs2_core/loopshaping/LoopshapingCost.h>
#include <ocs2_core/loopshaping/LoopshapingConstraint.h>
#include <ocs2_core/cost/QuadraticCostFunction.h>
#include <experimental/filesystem>

using namespace ocs2;

const std::experimental::filesystem::path pathToTest(__FILE__);
const std::string settingsFile_r = std::string(pathToTest.parent_path()) + "/loopshaping_r.conf";
const std::string settingsFile_s = std::string(pathToTest.parent_path()) + "/loopshaping_s.conf";
const auto inf_ = std::numeric_limits<double>::infinity();

TEST(testLoopshapingDefinition, SISO_Definition) {
  boost::property_tree::ptree pt;
  boost::property_tree::read_info(settingsFile_r, pt);

  SISOFilterDefinition filter0(pt, "r_filter", "Filter0");
  std::cout << "\nFilter0" << std::endl;
  filter0.print();

  SISOFilterDefinition filter1(pt, "r_filter", "Filter1");
  std::cout << "\nFilter1" << std::endl;
  filter1.print();

  ASSERT_TRUE(true);
}

TEST(testLoopshapingDefinition, MIMO_Definition) {

  MIMOFilterDefinition filter;
  filter.loadSettings(settingsFile_s, "s_inv_filter");
  filter.print();

  ASSERT_TRUE(true);
}

TEST(testLoopshapingDefinition, Loopshaping_Definition_r) {

  LoopshapingDefinition filter;
  filter.loadSettings(settingsFile_r);
  filter.print();

  ASSERT_TRUE(true);
}

TEST(testLoopshapingDefinition, Loopshaping_Definition_s) {

  LoopshapingDefinition filter;
  filter.loadSettings(settingsFile_s);
  filter.print();

  ASSERT_TRUE(true);
}

TEST(testLoopshapingDefinition, Cost_Augmentation_r) {
  std::shared_ptr<LoopshapingDefinition> filter(new LoopshapingDefinition());
  filter->loadSettings(settingsFile_r);
  filter->print();

  constexpr size_t n_sys = 5;
  constexpr size_t m_sys = 3;
  constexpr size_t n_q = 0;
  constexpr size_t n_r = 4;
  constexpr size_t n_s = 0;
  constexpr size_t m_s = 0;
  constexpr size_t n_tot = n_sys + n_q + n_r + n_s;
  constexpr size_t m_tot = m_sys + m_s;

  Eigen::Matrix<double, n_sys, n_sys> Q;
  Eigen::Matrix<double, m_sys, n_sys> P;
  Eigen::Matrix<double, m_sys, m_sys> R;
  Eigen::Matrix<double, n_tot, n_tot> Q_augmented;
  Eigen::Matrix<double, m_tot, n_tot> P_augmented;
  Eigen::Matrix<double, m_tot, m_tot> R_augmented;
  Q.setIdentity();
  P.setZero();
  R.setIdentity();
  Q_augmented.setConstant(inf_);
  P_augmented.setConstant(inf_);
  R_augmented.setConstant(inf_);

  Eigen::Matrix<double, n_sys, 1> x_sys;
  Eigen::Matrix<double, m_sys, 1> u_sys;
  Eigen::Matrix<double, n_q + n_r + n_s, 1> x_filter;
  Eigen::Matrix<double, m_s, 1> u_filter;
  Eigen::Matrix<double, n_tot, 1> x_tot;
  Eigen::Matrix<double, m_tot, 1> u_tot;
  x_sys.setZero();
  u_sys.setZero();
  x_filter.setZero();
  u_filter.setZero();
  filter->concatenateSystemAndFilterState(x_sys, x_filter, x_tot);
  filter->concatenateSystemAndFilterInput(u_sys, u_filter, u_tot);

  QuadraticCostFunction<n_sys, m_sys> quadraticCost(Q, R, x_sys, u_sys, Q, x_sys, P);
  LoopshapingCost<n_tot, m_tot, n_sys, m_sys, n_q + n_r + n_s, m_s> loopshapingCost(quadraticCost, filter);
  loopshapingCost.setCurrentStateAndControl(0.0, x_tot, u_tot);

  loopshapingCost.getIntermediateCostSecondDerivativeState(Q_augmented);
  loopshapingCost.getIntermediateCostDerivativeInputState(P_augmented);
  loopshapingCost.getIntermediateCostSecondDerivativeInput(R_augmented);

  std::cout << "Q:\n" << Q << std::endl;
  std::cout << "Q_augmented:\n" << Q_augmented << std::endl;

  std::cout << "P:\n" << P << std::endl;
  std::cout << "P_augmented:\n" << P_augmented << std::endl;

  std::cout << "R:\n" << R << std::endl;
  std::cout << "R_augmented:\n" << R_augmented << std::endl;

  ASSERT_TRUE(Q_augmented.array().isFinite().all());
  ASSERT_TRUE(P_augmented.array().isFinite().all());
  ASSERT_TRUE(R_augmented.array().isFinite().all());
}

TEST(testLoopshapingDefinition, Cost_Augmentation_s) {
  std::shared_ptr<LoopshapingDefinition> filter(new LoopshapingDefinition());
  filter->loadSettings(settingsFile_s);
  filter->print();

  constexpr size_t n_sys = 5;
  constexpr size_t m_sys = 3;
  constexpr size_t n_q = 0;
  constexpr size_t n_r = 0;
  constexpr size_t n_s = 4;
  constexpr size_t m_s = 3;
  constexpr size_t n_tot = n_sys + n_q + n_r + n_s;
  constexpr size_t m_tot = m_sys + m_s;

  Eigen::Matrix<double, n_sys, n_sys> Q;
  Eigen::Matrix<double, m_sys, n_sys> P;
  Eigen::Matrix<double, m_sys, m_sys> R;
  Eigen::Matrix<double, n_tot, n_tot> Q_augmented;
  Eigen::Matrix<double, m_tot, n_tot> P_augmented;
  Eigen::Matrix<double, m_tot, m_tot> R_augmented;
  Q.setIdentity();
  P.setZero();
  R.setIdentity();
  Q_augmented.setConstant(inf_);
  P_augmented.setConstant(inf_);
  R_augmented.setConstant(inf_);

  Eigen::Matrix<double, n_sys, 1> x_sys;
  Eigen::Matrix<double, m_sys, 1> u_sys;
  Eigen::Matrix<double, n_q + n_r + n_s, 1> x_filter;
  Eigen::Matrix<double, m_s, 1> u_filter;
  Eigen::Matrix<double, n_tot, 1> x_tot;
  Eigen::Matrix<double, m_tot, 1> u_tot;
  x_sys.setZero();
  u_sys.setZero();
  x_filter.setZero();
  u_filter.setZero();
  filter->concatenateSystemAndFilterState(x_sys, x_filter, x_tot);
  filter->concatenateSystemAndFilterInput(u_sys, u_filter, u_tot);

  QuadraticCostFunction<n_sys, m_sys> quadraticCost(Q, R, x_sys, u_sys, Q, x_sys, P);
  LoopshapingCost<n_tot, m_tot, n_sys, m_sys, n_q + n_r + n_s, m_s> loopshapingCost(quadraticCost, filter);
  loopshapingCost.setCurrentStateAndControl(0.0, x_tot, u_tot);

  loopshapingCost.getIntermediateCostSecondDerivativeState(Q_augmented);
  loopshapingCost.getIntermediateCostDerivativeInputState(P_augmented);
  loopshapingCost.getIntermediateCostSecondDerivativeInput(R_augmented);

  std::cout << "Q:\n" << Q << std::endl;
  std::cout << "Q_augmented:\n" << Q_augmented << std::endl;

  std::cout << "P:\n" << P << std::endl;
  std::cout << "P_augmented:\n" << P_augmented << std::endl;

  std::cout << "R:\n" << R << std::endl;
  std::cout << "R_augmented:\n" << R_augmented << std::endl;

  ASSERT_TRUE(Q_augmented.array().isFinite().all());
  ASSERT_TRUE(P_augmented.array().isFinite().all());
  ASSERT_TRUE(R_augmented.array().isFinite().all());
}


TEST(testLoopshapingDefinition, Constraint_augmentation) {
  std::shared_ptr<LoopshapingDefinition> filter(new LoopshapingDefinition());
  filter->loadSettings(settingsFile_s);
  filter->print();

  constexpr size_t n_sys = 5;
  constexpr size_t m_sys = 3;
  constexpr size_t n_q = 0;
  constexpr size_t n_r = 0;
  constexpr size_t n_s = 4;
  constexpr size_t m_s = 3;
  constexpr size_t n_tot = n_sys + n_q + n_r + n_s;
  constexpr size_t m_tot = m_sys + m_s;

  Eigen::Matrix<double, n_sys, 1> x_sys;
  Eigen::Matrix<double, m_sys, 1> u_sys;
  Eigen::Matrix<double, n_q + n_r + n_s, 1> x_filter;
  Eigen::Matrix<double, m_s, 1> u_filter;
  Eigen::Matrix<double, n_tot, 1> x_tot;
  Eigen::Matrix<double, m_tot, 1> u_tot;
  x_sys.setZero();
  u_sys.setZero();
  x_filter.setZero();
  u_filter.setZero();
  filter->concatenateSystemAndFilterState(x_sys, x_filter, x_tot);
  filter->concatenateSystemAndFilterInput(u_sys, u_filter, u_tot);

  Eigen::Matrix<double, m_tot, n_tot> C_augmented;
  Eigen::Matrix<double, m_tot, m_tot> D_augmented;
  C_augmented.setConstant(inf_);
  D_augmented.setConstant(inf_);

  LoopshapingConstraint<n_tot, m_tot, n_sys, m_sys, n_q + n_r + n_s, m_s> loopshapingConstraint(filter);
  loopshapingConstraint.setCurrentStateAndControl(0.0, x_tot, u_tot);

  loopshapingConstraint.getConstraint1DerivativesState(C_augmented);
  loopshapingConstraint.getConstraint1DerivativesControl(D_augmented);

  std::cout << "C_augmented:\n" << C_augmented << std::endl;
  std::cout << "D_augmented:\n" << D_augmented << std::endl;

  ASSERT_TRUE(C_augmented.topRows(m_s).array().isFinite().all());
  ASSERT_TRUE(D_augmented.topRows(m_s).array().isFinite().all());
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}