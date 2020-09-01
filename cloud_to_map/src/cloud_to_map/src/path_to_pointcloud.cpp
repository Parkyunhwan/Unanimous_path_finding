#include <ros/ros.h>
#include <nav_msgs/Path.h>
void convertpath(const nav_msgs::PathConstPtr& ptr)
{
    FILE *fp = fopen("path.txt","w");
    double xCell;
    double yCell;
    double x,y;
    //fprintf(fp,"x_value y_value\n");
    for(int i=0; i < ptr->poses.size(); i++)
    {
        std::cout << "x : " << ptr->poses[i].pose.position.x;
        std::cout << "y : " << ptr->poses[i].pose.position.y << std::endl;
        xCell = ptr->poses[i].pose.position.x;
        yCell = ptr->poses[i].pose.position.y;
        
        // // x = (xCell+xMin) * cellResolution;
        // // x = (xCell+xMax) * cellResolution;
        // x = ((xCell) * 0.7) + (-23.521973);
        // y = ((yCell) * 0.7) + (-137.115036);

        fprintf(fp,"%.7f,%.7f \n",xCell,yCell);

    }

    fclose(fp);

}
int main(int argc, char** argv)
{
    ros::init(argc,argv,"path_subscriber");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/footstep_planner/path",100,convertpath);
    ros::Rate loop_rate(10);
    loop_rate.sleep();
    ros::spin();
}

