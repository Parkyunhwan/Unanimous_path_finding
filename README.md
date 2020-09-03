# Unanimous_path_finding
-> 3D & 2D path finding

# Execution

### 3D to 2D map making
 <img src=https://user-images.githubusercontent.com/47476276/91859421-6535b900-eca5-11ea-9ed3-5802f62076f2.gif width="50%">

### 3D & 2D path finding
 <img src=https://user-images.githubusercontent.com/47476276/91859505-7f6f9700-eca5-11ea-8521-d02785d8d9a2.png width="50%">
 
 <img src=https://user-images.githubusercontent.com/47476276/91859481-78488900-eca5-11ea-8e64-1380d22faba6.png width="50%">

 

# Direction

## launch path planning
`roslaunch footstep_planner footstep_planner_complete.launch (always run with server)`

## load map
`rosrun map_server map_server /home/yunan/catkin_ws/src/cloud_to_map/map/Hyungnam11.yaml`
- run any map

## publish&save path
`rosrun cloud_to_map publish_start_goal (always run with server)`

## open rviz setting
### open file
`~/catkin_ws/src/cloud_to_map/3drviz.rviz`


## link connect
`rosrun tf static_transform_publisher 0 0 0 0 0 0 base_link map 10`

## publish pcd to pointcloud
```
rosrun pcl_ros pcd_to_pointcloud [pcd file]
rosrun pcl_ros pcd_to_pointcloudcopy [pcd file]
```
```
ex)
rosrun pcl_ros pcd_to_pointcloud /home/yunan/Desktop/map2d3d/launchmap/lssu.pcd
rosrun pcl_ros pcd_to_pointcloudcopy /home/yunan/Desktop/map2d3d/launchmap/hn_mirae.pcd
```
## Reference

 * For related articles, detailed information about robots, demonstration videos, and motion images, please refer to the link below.
 
 [![unanimous_shot](https://user-images.githubusercontent.com/47476276/92101304-e614c200-ee17-11ea-809f-5a0835ee8205.png)](https://youtu.be/Q69kimFzcJ0) 
 
 * https://www.youtube.com/watch?v=rWDjQ4hLrtE&t=14s
 * https://scatch.ssu.ac.kr/%eb%89%b4%ec%8a%a4%ec%84%bc%ed%84%b0/%ec%a3%bc%ec%9a%94%eb%89%b4%ec%8a%a4/page/3/?slug=20058-2&f&keyword
 * http://www.dhnews.co.kr/news/articleView.html?idxno=116170
 * http://ibook.ssu.ac.kr/Viewer/Q9ZQBTSP2694 (6p)
