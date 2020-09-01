#include <ros/ros.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <std_msgs/Bool.h>
#include <nav_msgs/Path.h>
#include <std_srvs/Trigger.h>
#include <humanoid_nav_msgs/PlanFootsteps.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>

using namespace std;

#define HOST "localhost"
#define PORT 5000
#define BUFF_SIZE 1024
//#define SOCKET_PATH "/home/yoonje/catkin_ws/src/cloud_to_map/path/connection.sock" // Socket file location
#define SOCKET_PATH "/home/yunan/catkin_ws/src/cloud_to_map/path/connection.sock" // Socket file location

#define PATH_SUCCESS 13
#define PATH_FAILED 12
#define WRONG 6

//#define REQUEST_PATH "/home/yoonje/PycharmProjects/unanimous/static/request/request.txt"
//#define NAV_PATH "/home/yoonje/PycharmProjects/unanimous/static/path/path.txt"
#define REQUEST_PATH "/home/yunan/catkin_ws/src/cloud_to_map/path/request.txt"
#define NAV_PATH "/home/yunan/catkin_ws/src/cloud_to_map/path/path.txt"
class PathCommunication{
    public:
       ros::NodeHandle nh;
       ros::Publisher GoalPosePub;
       ros::Publisher StartPosePub;
       ros::Publisher Combi_Pub;
       ros::ServiceClient Mapclient;
       ros::Subscriber path_sub;
       ros::Subscriber re_sub;
       ros::ServiceClient client;
       nav_msgs::Path combi_path;
       int complete_signal;
       int wrong_pose_signal;
       std::string map_name;
       std::ifstream inFile;
       FILE *fp;
       float start_x;
       float start_y;
       float goal_x;
       float goal_y;
       float z;

       PathCommunication()
       {
           GoalPosePub = nh.advertise<geometry_msgs::PoseStamped>("goal", 1 );
           StartPosePub = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 1);
           Combi_Pub = nh.advertise<nav_msgs::Path>("combi_path",1);
           path_sub = nh.subscribe("/footstep_planner/path",1,&PathCommunication::convertpath,this);
           re_sub = nh.subscribe("/wrong_pose",1,&PathCommunication::wrongPose,this);
           Mapclient = nh.serviceClient<std_srvs::Trigger>("map_reload");
           client = nh.serviceClient<humanoid_nav_msgs::PlanFootsteps>("plan_footsteps");
           complete_signal=0;
           wrong_pose_signal=0;
           combi_path.header.frame_id = "map";
       }

       int loadfile()
       {
            std::string file_path(REQUEST_PATH);
            std::string nav_path(NAV_PATH);
            inFile.open(file_path.c_str());
            remove(nav_path.c_str() );
       }

       int closefile()
       {
            if(inFile.is_open()==true)
                inFile.close();
            //fclose(fp);
       }

       int mapServiceClient()
       {    
            //std::string file_path(REQUEST_PATH);
            //std::ifstream inFile(file_path.c_str());
            //std::string map_name;

            // if(inFile.is_open())
            //     getline(inFile, map_name);
            // inFile.close();
            nh.setParam("/reload_map",map_name);
            std_srvs::Trigger srv;
            if (Mapclient.call(srv))
            {
                std::cout << "map_reload_success" << srv.response.success << std::endl;
            }
            // else{
            //     ROS_ERROR("Failed to reload map");
            //     return 1;
            // }
            return 0;
       }
       void pathpub()
       {
           //read file__code
            geometry_msgs::PoseStamped goal_pose;
            geometry_msgs::PoseWithCovarianceStamped start_pose;
            static int seq_i;
            humanoid_nav_msgs::PlanFootsteps value;

            value.request.start.x = start_x;
            value.request.start.y = start_y;
            value.request.start.theta = 0.0;

            value.request.goal.x = goal_x;
            value.request.goal.y = goal_y;
            value.request.goal.theta = 0.0;
            client.call(value);
            

       }
       void wrongPose(const std_msgs::BoolConstPtr& ptr)
       {
		if(ptr->data == true){
            wrong_pose_signal=1;
            ROS_ERROR("wrong_pose..Please Retry");
        }
       }
       void publish_combi_path()
       {
            Combi_Pub.publish(combi_path);
       }
       void convertpath(const nav_msgs::PathConstPtr& ptr)
        {
            // FILE *fp = fopen(NAV_PATH,"w");
            fp = fopen(NAV_PATH,"a");

            double xCell;
            double yCell;
            double x,y;
            geometry_msgs::PoseStamped state;
            state.header.stamp = ros::Time::now();
            state.header.frame_id = "map";

            for(int i=0; i < ptr->poses.size(); i++)
            {
                // std::cout << "x : " << ptr->poses[i].pose.position.x;
                // std::cout << "y : " << ptr->poses[i].pose.position.y << std::endl;
                xCell = ptr->poses[i].pose.position.x;
                yCell = ptr->poses[i].pose.position.y;
                state.pose.position.x = ptr->poses[i].pose.position.x;
                state.pose.position.y = ptr->poses[i].pose.position.y;
                state.pose.position.z = z;
                combi_path.poses.push_back(state);
                // // x = (xCell+xMin) * cellResolution;
                // // x = (xCell+xMax) * cellResolution;
                // x = ((xCell) * 0.7) + (-23.521973);
                // y = ((yCell) * 0.7) + (-137.115036);

                fprintf(fp,"%.4f,%.4f,%.4f \n",xCell,yCell,z);
                

            }
            combi_path.header = state.header;
            fclose(fp);
            std::cout << "Path input is complete!" << std::endl;
            complete_signal=1;
            std::cout << "signal : " << complete_signal << std::endl;

        }
        void fileReader()
        {
            //std::string file_path(REQUEST_PATH);
            //std::ifstream inFile(file_path.c_str());
            std::string line_buffer;
            int count;
            if(inFile.is_open()){
                getline(inFile, line_buffer);
                count = line_buffer.length();
                cout << "count" << count << endl;
                map_name=line_buffer.substr(0, count-2); 
                cout << map_name << endl;

                getline(inFile, line_buffer);
                start_x = stof(line_buffer);
                getline(inFile, line_buffer);
                start_y = stof(line_buffer);
                getline(inFile, line_buffer);// cut off z value
                getline(inFile, line_buffer);
                goal_x = stof(line_buffer);
                getline(inFile, line_buffer);
                goal_y = stof(line_buffer);
                getline(inFile, line_buffer);
                z = stof(line_buffer);            
            }
            else
            {
                std::cout << "open failed ..!" << std::endl;
            }
            
            //inFile.close();
                std::cout <<"map_name : "<< map_name << std::endl;
                std::cout <<"start_x, start_y : "<< start_x << ", "<< start_y <<std::endl; 
                std::cout <<" goal_x,  goal_y : "<< goal_x << ", "<< goal_y <<std::endl;         
        }


};

int main(int argc,char** argv)
{
    ros::init(argc,argv,"start_goal_publisher");
    char buffer[BUFF_SIZE];
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    socklen_t client_addr_size;
    int server_sock, client_sock;
    int msg_size;
    PathCommunication pc;
    ros::Rate loop_rate(10000);

    signal(SIGPIPE, SIG_IGN);

    unlink(SOCKET_PATH);

    // open server socket
    if((server_sock = socket(PF_FILE, SOCK_STREAM, 0)) == -1)
    {
        std::cout << "Can't open stream socket\n";
        exit(0);
    }

    // server_addr init NULL
    memset(&server_addr, 0, sizeof(server_addr));

    // server_addr setting
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);
    

    // bind the socket
    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
    {
        std::cout << "Can't bind local address.\n";
        exit(0);
    }

    std::cout << "waiting connection request.\n";

    // wait for connection
    if(listen(server_sock, 5) < 0)
    {
        std::cout << "Can't listening connect.\n";
        exit(0);
    }

    // loop for persistent server
   // while(1)
   // {

        // buffer init
        memset(buffer, 0x00, sizeof(buffer));
    
        // client_addr init and create
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr_size = sizeof(client_addr);
      
        // client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size); // wait infinit.........
               

        // if(client_sock < 0)
        // {
        //     std::cout << "Server: accept failed.\n";
        //     exit(0);
        // }
        
        // loop for several client socket communication
            std::string Requestfile_path(REQUEST_PATH);
            int numLines = 0;
            ifstream in(Requestfile_path.c_str());
            //while ( ! in.eof() )
            while ( in.good() )
            {
            std::string line;
            std::getline(in, line);
            ++numLines;
            }
            cout << --numLines << endl;
            int num=0;
            pc.loadfile();
        while((numLines/7)!=num)
        {
            num++;
            std::cout << "waiting for Request.." << std::endl;
            // msg_size = read(client_sock, buffer, BUFF_SIZE);
            //  if(msg_size == 0) // read() return 0 when connection broken. escape inner loop.
            //      break;
           
             std::cout << "Received data: "<< buffer << endl;

            //  if (strcmp(buffer,"PATH")==0)
            //  {
                 pc.wrong_pose_signal=0;
                 pc.complete_signal=0;

                pc.fileReader();
                pc.mapServiceClient();
                std::cout << "I got a start & goal.." << pc.complete_signal << std::endl;
                pc.pathpub();
                
     
                 while(pc.complete_signal==0 && pc.wrong_pose_signal==0){
                     ros::spinOnce();
                     loop_rate.sleep();
                     std::cout << pc.wrong_pose_signal;
                }
                 if(pc.wrong_pose_signal==0){
                //  write(client_sock, "PATH_SUCCESS\0", PATH_SUCCESS);
                  ROS_INFO("nice..\n");
                 }
                 else{
                //  write(client_sock, "PATH_FAILED\0", PATH_FAILED);
                  ROS_INFO("Retry..\n");
                  pc.wrong_pose_signal=0;
                 }
             //}
            //  else
            //  {
            //      std::cout << "This message is not PATH...(wrong message)" << std::endl;
            //      write(client_sock, "WRONG", WRONG);
            //  }
        }
        pc.publish_combi_path();
        pc.closefile();
        // client socket remove because connection was broken
         //close(client_sock);
    //}
    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}

