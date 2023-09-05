#include "rpl_ros2/roadmap_publisher.hpp"

// stl
#include <functional>
#include <memory>
// rpl
// rpl_msgs
using std::placeholders::_1;

rpl_ros2::RoadmapNode::RoadmapNode() : rclcpp::Node("roadmap")
{
  auto qos   = rclcpp::QoS(rclcpp::KeepLast(1), rmw_qos_profile_sensor_data);
  this->sub_ = this->create_subscription<rpl::WorldDescriptor>(
      "world_description", qos, std::bind(&RoadmapNode::world_descriptor_cb, this, _1));
  this->pub_ = this->create_publisher<rpl::RoadMap>(
      "roadmap_topic", qos);
}

void rpl_ros2::RoadmapNode::world_descriptor_cb(const rpl::WorldDescriptor &msg)
{
  this->rm = std::move(rpl::RoadMap(msg.obstacles_outer, msg.gates[0]));
  this->rm.execute();
  this->rm.remove_out_of_bounds(msg.border_outer);
  for (;;)
  {
    this->pub_->publish(this->rm);
    usleep(1000000); // TODO: every second?
  }
}

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<rpl_ros2::RoadmapNode>());
  rclcpp::shutdown();
  return 0;
}