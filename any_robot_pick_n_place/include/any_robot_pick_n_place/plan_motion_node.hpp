// Copyright 2023 Vishnuprasad Prachandabhanu
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PLAN_MOTION_NODE_HPP
#define PLAN_MOTION_NODE_HPP

#include "geometry_msgs/msg/pose.hpp"
#include "moveit/moveit_cpp/moveit_cpp.h"
#include "moveit/moveit_cpp/planning_component.h"
#include "rclcpp/rclcpp.hpp"

#include <behaviortree_cpp/bt_factory.h>

class PlanMotionNode : public BT::SyncActionNode
{
  protected:
    rclcpp::Node::SharedPtr node_;
    moveit_cpp::MoveItCppPtr moveit_cpp_;
    moveit_cpp::PlanningComponentPtr planning_component_;
    std::string planning_goup_name_;
    std::string planning_base_frame_;
    std::string planning_end_effector_frame_;

  public:
    PlanMotionNode(const std::string &name, const BT::NodeConfiguration &config) : BT::SyncActionNode(name, config)
    {
        node_ = config.blackboard->get<rclcpp::Node::SharedPtr>("node");
        moveit_cpp_ = config.blackboard->get<moveit_cpp::MoveItCppPtr>("moveit_cpp");
        planning_component_ = config.blackboard->get<moveit_cpp::PlanningComponentPtr>("planning_component");
        planning_goup_name_ = config.blackboard->get<std::string>("planning_group_name");
        planning_base_frame_ = config.blackboard->get<std::string>("planning_base_frame");
        planning_end_effector_frame_ = config.blackboard->get<std::string>("planning_end_effector_frame");
    }

    static BT::PortsList providedPorts()
    {
        const char *op_description = "Plan Action Description";
        return {BT::InputPort<std::string>("description", op_description),
                BT::InputPort<moveit::core::RobotStatePtr>("start_state"),
                BT::InputPort<moveit::core::RobotStatePtr>("goal_state"),
                BT::OutputPort<robot_trajectory::RobotTrajectoryPtr>("trajectory"),
                BT::OutputPort<moveit::core::RobotStatePtr>("next_start_state")};
    }

    BT::NodeStatus tick() override
    {
        if (getInput<std::string>("description").has_value())
        {
            RCLCPP_INFO(node_->get_logger(), "Planning %s", getInput<std::string>("description").value().c_str());
        }

        if (getInput<moveit::core::RobotStatePtr>("start_state").has_value())
        {
            planning_component_->setStartState(*getInput<moveit::core::RobotStatePtr>("start_state").value());
        }
        else
        {
            planning_component_->setStartStateToCurrentState();
        }

        if (getInput<moveit::core::RobotStatePtr>("goal_state").has_value())
        {
            planning_component_->setGoal(*getInput<moveit::core::RobotStatePtr>("goal_state").value());
        }

        auto plan = planning_component_->plan();

        if (!plan)
        {
            auto error_code = plan.error_code;
            RCLCPP_ERROR(node_->get_logger(), "Planning failed: %s",
                         moveit::core::error_code_to_string(error_code).c_str());
            return BT::NodeStatus::FAILURE;
        }

        setOutput<robot_trajectory::RobotTrajectoryPtr>("trajectory", plan.trajectory);
        setOutput<moveit::core::RobotStatePtr>("next_start_state", plan.trajectory->getLastWayPointPtr());
        RCLCPP_INFO(node_->get_logger(), "Planning succeeded for: %s",
                    getInput<std::string>("description").value().c_str());
        return BT::NodeStatus::SUCCESS;
    }
};

#endif // PLAN_MOTION_NODE_HPP
