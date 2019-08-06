#include <ocs2_comm_interfaces/SystemObservation.h>
#include <ocs2_comm_interfaces/ocs2_ros_interfaces/TargetPoseCommand.h>

// Command
#include <ocs2_comm_interfaces/ocs2_ros_interfaces/command/ModeSequence_ROS_Interface.h>
#include <ocs2_comm_interfaces/ocs2_ros_interfaces/command/TargetTrajectories_ROS_Interface.h>

// Common
#include <ocs2_comm_interfaces/ocs2_ros_interfaces/common/RosMsgConversions.h>

// MPC
#include <ocs2_comm_interfaces/ocs2_ros_interfaces/mpc/MPC_ROS_Interface.h>

// MRT
#include <ocs2_comm_interfaces/ocs2_interfaces/MPC_MRT_Interface.h>
#include <ocs2_comm_interfaces/ocs2_ros_interfaces/mrt/MRT_ROS_Interface.h>

// TaskListener
#include <ocs2_comm_interfaces/ocs2_ros_interfaces/task_listener/TaskListenerBase.h>

// dummy target for clang toolchain
int main() { return 0; }