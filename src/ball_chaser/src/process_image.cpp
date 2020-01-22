#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    int check = 0;
    
    for(int i = 0; i<img.step; i++){
    	for(int j = 0; j<img.height; j++){
    		if(img.data[i+j*img.step]==white_pixel && img.data[i+j*img.step+1]==white_pixel && img.data[i+j*img.step+2]==white_pixel && i<img.step/3) //Check left section
    		{
    			check += 100;
    			i = img.step/3;
    		}
    		else if(img.data[i+j*img.step]==white_pixel && img.data[i+j*img.step+1]==white_pixel && img.data[i+j*img.step+2]==white_pixel && i<img.step*2/3) //Check middle section
    		{
    			check += 10;
    			i = img.step*2/3;
    		}
    		else if(img.data[i+j*img.step]==white_pixel && img.data[i+j*img.step+1]==white_pixel && img.data[i+j*img.step+2]==white_pixel) //Check right section
    		{
    			check += 1;
    			break;
    		}
    	}
    }
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    if(check == 10 || check == 111) //Move forward
    {
    	drive_robot(0.5,0);
    }
    if(check == 100 || check == 110) //Turn left
    {
    	drive_robot(0.5,0.5);
    }
    if(check == 1 || check == 11) //Turn right
    {
    	drive_robot(0.5,-0.5);
    }
    else //Stop
    {
    	drive_robot(0,0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
