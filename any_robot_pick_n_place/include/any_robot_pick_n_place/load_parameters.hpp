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

#ifndef LOAD_PARAMETERS_HPP
#define LOAD_PARAMETERS_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "geometry_msgs/msg/pose.hpp"
#include "rclcpp/rclcpp.hpp"

#include <rosparam_shortcuts/rosparam_shortcuts.h>

struct Parameters
{
    std::string arm_group_name;
    geometry_msgs::msg::Pose object_pose;
    geometry_msgs::msg::Pose place_pose;

    void load(const rclcpp::Node::SharePtr &node)
    {
        node->get_parameter("arm_group_name", arm_group_name);

        size_t errors = 0;
        errors += !rosparam_shortcuts::get(node, "object_pose", object_pose);
        errors += !rosparam_shortcuts::get(node, "place_pose", place_pose);
        rosparam_shortcuts::shutdownIfError(errors);
    }
};

#endif