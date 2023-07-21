# SPDX-FileCopyrightText: 2023 Matej Vargovcik <matej.vargovcik@gmail.com>
# SPDX-License-Identifier: MIT

# Torso of code was made by ChatGPT

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from bluepy.btle import Peripheral, DefaultDelegate

class TrailControl(Node):
    def __init__(self):
        super().__init__('trail_control')
        self.declare_parameter('peripheral_address', 'C8:F0:9E:52:AD:FE')

        # Initialize Bluetooth connection
        success = False
        while rclpy.ok() and not success:
            try:
                self.peripheral = Peripheral(self.get_parameter('peripheral_address').get_parameter_value().string_value)
                self.service = self.peripheral.getServiceByUUID('6E400001-B5A3-F393-E0A9-E50E24DCCA9E')
                self.characteristic = self.service.getCharacteristics('6E400002-B5A3-F393-E0A9-E50E24DCCA9E')[0]
                success = True
                self.get_logger().info("BLE connection initialized. Ready for twist messages")
            except:
                self.get_logger().info("Cannot connect, trying again")

        # Subscribe to Twist messages
        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.twist_callback,
            1)
        

    def twist_callback(self, twist_msg):
        # Calculate differential motor speeds from Twist message
        linear_vel_x = twist_msg.linear.x
        angular_vel_z = twist_msg.angular.z
        left_motor_speed = linear_vel_x - angular_vel_z
        right_motor_speed = linear_vel_x + angular_vel_z

        # # Convert motor speeds to bytes
        # left_speed_byte = int(left_motor_speed * 255.0)
        # right_speed_byte = int(right_motor_speed * 255.0)

        # Construct the message
        # message = bytes([left_speed_byte, right_speed_byte])
        speeds_str = str(left_motor_speed) + " " + str(right_motor_speed) + "\n"
        message = bytes(speeds_str, 'utf-8')

        self.get_logger().info(speeds_str)

        # Send the message via BLE
        try:
            self.characteristic.write(message, True)
        except:
            success = False
            self.get_logger().info("Sending failed, reconnecting")
            while rclpy.ok() and not success:
                try:
                    self.peripheral = Peripheral(self.get_parameter('peripheral_address').get_parameter_value().string_value)
                    self.service = self.peripheral.getServiceByUUID('6E400001-B5A3-F393-E0A9-E50E24DCCA9E')
                    self.characteristic = self.service.getCharacteristics('6E400002-B5A3-F393-E0A9-E50E24DCCA9E')[0]
                    success = True
                    self.get_logger().info("BLE connection initialized. Ready for twist messages")
                except:
                    self.get_logger().info("Cannot connect, trying again")

def main(args=None):
    rclpy.init(args=args)
    node = TrailControl()
    rclpy.spin(node)
    node.peripheral.disconnect()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

