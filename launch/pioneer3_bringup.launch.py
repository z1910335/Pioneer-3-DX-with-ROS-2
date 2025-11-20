from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    pioneer_aria = Node(
        package="pioneer_aria",
        executable="pioneer_aria_node",  # double-check with `ros2 pkg executables pioneer_aria`
        name="pioneer_base",
        parameters=[{
            "port": "/dev/ttyS0",   # TODO: correct port on the robot
            "baud": 9600,
        }],
        output="screen",
    )

    base_controller = Node(
        package="pioneer3",
        executable="base_controller",
        name="base_controller",
        output="screen",
    )

    lidar = Node(
        package="pioneer3",
        executable="lidar_node.py",
        name="lidar_node",
        output="screen",
    )

    static_tf = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="base_to_laser_tf",
        # x y z roll pitch yaw  parent      child
        arguments=["0.0", "0.0", "0.25", "0", "0", "0", "base_link", "base_laser"],
        output="screen",
    )

    rviz = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        # you can add a config later: arguments=["-d", "<path-to>/pioneer3.rviz"]
        output="screen",
    )

    return LaunchDescription([
        pioneer_aria,
        base_controller,
        lidar,
        static_tf,
        rviz,
    ])
