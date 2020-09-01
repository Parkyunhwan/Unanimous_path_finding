#include <ros/ros.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/common/common_headers.h>
#include <pcl/io/pcd_io.h>


int main(int argc, char** argv)
{
    ros::init(argc, argv,"get_min_xy");
    pcl::PointCloud<pcl::PointXYZ> cloud;
    pcl::io::loadPCDFile<pcl::PointXYZ> (argv[1], cloud);

      double xMax = cloud.points[0].x, yMax = cloud.points[0].y, xMin = cloud.points[0].x, yMin = cloud.points[0].y;
      double x,y;

      for (size_t i = 0; i < cloud.size(); i++) {
          x = cloud.points[i].x;
          y = cloud.points[i].y;
          if (xMax < x) {
            xMax = x;
          }
          if (xMin > x) {
            xMin = x;
          }
          if (yMax < y) {
            yMax = y;
          }
          if (yMin > y) {
            yMin = y;
          }
        }
    std::cout << "xMax xMin : " << xMax << " " << xMin;
    std::cout << std::endl;
    std::cout << "yMax yMin : " << yMax << " " << yMin;
    std::cout << std::endl;
}

