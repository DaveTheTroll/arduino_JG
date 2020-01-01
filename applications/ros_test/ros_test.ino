#define STM32F1
#include <ros.h>
#include <std_msgs/String.h>

ros::NodeHandle nh;

void onMessage(const std_msgs::String & msg)
{
  Serial.println(msg.data);
}

ros::Subscriber<std_msgs::String> sub("to_arduino", &onMessage );

void setup() {
  Serial.begin(9600);
  nh.initNode();
  nh.subscribe(sub);
}

void loop() {
  nh.spinOnce();
  delay(1);
}
