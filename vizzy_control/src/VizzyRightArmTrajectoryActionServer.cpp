/**
 *  File: VizzyRightArmTrajectoryActionServer.cpp
 *  Desc: Class for interfacing moveIt with Vizzy's right arm.
 *  Auth: Plinio Moreno
 *
 *
 */



#include "VizzyRightArmTrajectoryActionServer.h"

using namespace std;

VizzyRightArmTrajectoryActionServer::VizzyRightArmTrajectoryActionServer(const std::string & name, const ros::NodeHandle &nh)
    : action_server_(nh,name, false)
{
    stop_execution = node_handle_.advertise<std_msgs::Bool>("/right_arm_trajectory_cancel", 1);
    trajectory_from_move_it = node_handle_.advertise<trajectory_msgs::JointTrajectory>("/right_arm_trajectory_from_moveit", 1);
    action_server_.registerGoalCallback(boost::bind(&VizzyRightArmTrajectoryActionServer::goalCallback, this));
    action_server_.registerPreemptCallback(boost::bind(&VizzyRightArmTrajectoryActionServer::preemptCB, this));
    info_from_bridge = node_handle_.subscribe("/right_arm_trajectory_feedback", 1, &VizzyRightArmTrajectoryActionServer::actionBridgeCallback,this);
    action_server_.start();
    action_active=false;
}


VizzyRightArmTrajectoryActionServer::~VizzyRightArmTrajectoryActionServer()
{
}

void VizzyRightArmTrajectoryActionServer::actionBridgeCallback(const std_msgs::Int16::ConstPtr& msg)
{
    if (!action_active)
        return;
    if (msg->data == 0){
      control_msgs::FollowJointTrajectoryResult result;
      ROS_INFO("Could not reach the goal.");
      result.error_code = control_msgs::FollowJointTrajectoryResult::PATH_TOLERANCE_VIOLATED;
      action_server_.setSucceeded(result);
    }
    else if (msg->data==1){
      control_msgs::FollowJointTrajectoryResult result;
      ROS_INFO("Goal reached!!");
      result.error_code = control_msgs::FollowJointTrajectoryResult::SUCCESSFUL;
      action_server_.setSucceeded(result);
    }
    return;
}

void VizzyRightArmTrajectoryActionServer::goalCallback()
{
    action_active=true;
    goal_msg = action_server_.acceptNewGoal();
    trajectory_from_move_it.publish(goal_msg->trajectory);
    return;
}

void VizzyRightArmTrajectoryActionServer::preemptCB()
{
   ROS_INFO("joint_velocity_controller: Preempted");
   // set the action state to preempted
   action_active=false;
   action_server_.setPreempted();
   return;
}
