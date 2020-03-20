#include <ros/ros.h>

#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseArray.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>

#include <std_srvs/SetBool.h>
#include <geometry_msgs/Twist.h>

#include <std_srvs/Trigger.h>
#include <std_msgs/Float64.h>

#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/server/simple_action_server.h>

#include <std_msgs/Bool.h>
#include <rsm_msgs/GetNavigationGoal.h>
#include <rsm_msgs/GoalCompleted.h>

typedef actionlib::SimpleActionServer<move_base_msgs::MoveBaseAction> MoveBaseActionServer;

namespace rsm {

/**
 * @class AdditionsServiceProvider
 * @brief Class that establishes communication between the different states and the RSM's
 * 		  periphery including the GUI for the states and classes defined in rsm_additions package. It offers
 * 		  services and publishes topics based on the variables that need to be saved during state transitions.
 */
class AdditionsServiceProvider {

public:
	AdditionsServiceProvider();
	~AdditionsServiceProvider();
	void publishTopics();

private:
	ros::NodeHandle _nh;

	ros::ServiceServer _set_navigation_to_reverse_service;
	ros::Subscriber _reverse_mode_cmd_vel_subscriber;
	ros::Publisher _reverse_mode_cmd_vel_publisher;

	ros::Publisher _failed_goals_publisher;

	ros::Subscriber frontiers_marker_array_subscriber;
	ros::Publisher _exploration_goals_publisher;
	ros::ServiceClient _get_navigation_goal_service;
	ros::Publisher _goal_obsolete_publisher;
	ros::Subscriber _exploration_goal_subscriber;
	ros::Subscriber _exploration_mode_subscriber;

	ros::ServiceServer _reset_kinect_position_serivce;
	ros::Publisher _kinetic_joint_controller;

	/**
	 * SimpleActionServer for making Explore Lite run and lead it to believe it talks to Move Base
	 */
	MoveBaseActionServer* as;
	/**
	 *Topic name for the autonomy cmd vel topic to be recorded
	 */
	std::string _autonomy_cmd_vel_topic;
	/**
	 * List of all extracted frontier centers
	 */
	geometry_msgs::PoseArray _exploration_goals;
	/**
	 * List of previously failed goals
	 */
	geometry_msgs::PoseArray _failed_goals;
	/**
	 * Tolerance for comparing if the current goal is still in the list of exploration goals
	 */
	double _exploration_goal_tolerance;
	/**
	 * Mode of exploration (0=complete goal, 1=interrupt goal when exploration goals vanished)
	 */
	bool _exploration_mode;
	/**
	 * Is the Calculate Goal plugin used
	 */
	bool _calculate_goal_plugin_used;
	/**
	 * Is navigation goal still an exploration goal
	 */
	bool _goal_obsolete;

	/**
	 * Callback for receiving autonomy cmd vel messages and save the ones not equals zero in the cirular buffer
	 * @param msg Cmd vel autonomy message
	 */
	void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg);

	bool setNavigationToReverse(std_srvs::SetBool::Request &req,
			std_srvs::SetBool::Response &res);
	/**
	 * Receives cmd vel and negate linear movement so robot goes backwards
	 * @param cmd_vel cmd vel generated for backward movement
	 */
	void reverseModeCmdVelCallback(
			const geometry_msgs::Twist::ConstPtr& cmd_vel);

	/**
	 * Callback for requests to the SimpleActionServer that just responds with "Goal reached"
	 * @param frontier_goal
	 */
	void navigationGoalCallback(
			const move_base_msgs::MoveBaseGoalConstPtr& frontier_goal);
	/**
	 * Callback for frontier visualization generated by Explore Lite. Extracts the centers of these
	 * frontiers and saves them in a list
	 * @param frontiers Marker list containing frontiers as points and nearest frontier points for
	 * each frontier as sphere
	 */
	void frontierCallback(
			const visualization_msgs::MarkerArray::ConstPtr& frontiers);
	/**
	 * Publish list of extracted frontier centers for further calculation
	 */
	void publishExplorationGoals();
	/**
	 * Publish list of previously failed goals
	 */
	void publishFailedGoals();
	/**
	 * Publish if current exploration goal is obsolete if exploration mode is set to interrupt
	 */
	void publishGoalObsolete();

	/**
	 * Callback for exploration goal status
	 * @param goal_status Current goal status and pose
	 */
	void explorationGoalCallback(
			const rsm_msgs::GoalStatus::ConstPtr& goal_status);
	/**
	 * Callback for exploration mode
	 * @param exploration_mode Exploration mode (0=complete goal, 1=interrupt goal when exploration goals vanished)
	 */
	void explorationModeCallback(
			const std_msgs::Bool::ConstPtr& exploration_mode);
	/**
	 * Checks if the current navigation goal is still present as an exploration goal
	 * @return Returns true if the current navigation goal is still an exploration goal to be explored
	 */
	bool navGoalIncludedInFrontiers();

	bool resetKinectPosition(std_srvs::Trigger::Request &req,
			std_srvs::Trigger::Response &res);
};

}
