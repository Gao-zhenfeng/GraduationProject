#pragma comment(lib, "User32.lib")
#pragma comment(lib, "gdi32.lib")

#include <iostream>
#include <fstream>
#include <string>

#include <pcl/registration/ia_ransac.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/fpfh.h>
#include <pcl/search/kdtree.h>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/filter.h>
#include <pcl/registration/icp.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <time.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/features/fpfh_omp.h>
#include <pcl/features/3dsc.h>//3D形状描述子
#include <pcl/visualization/pcl_plotter.h>//直方图可视化

using namespace std;
using pcl::NormalEstimation;
using pcl::search::KdTree;
typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;

//点云可视化
void visualize_pcd(pcl::PointCloud<pcl::PointXYZ>::Ptr pcd_src,
	pcl::PointCloud<pcl::PointXYZ>::Ptr pcd_tgt,
	pcl::PointCloud<pcl::PointXYZ>::Ptr pcd_sac,
	pcl::PointCloud<pcl::PointXYZ>::Ptr pcd_final)
{
	//int vp_1, vp_2;
	// Create a PCLVisualizer object
	pcl::visualization::PCLVisualizer viewer("registration Viewer");
	//viewer.createViewPort (0.0, 0, 0.5, 1.0, vp_1);
   // viewer.createViewPort (0.5, 0, 1.0, 1.0, vp_2);
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> src_h(pcd_src, 0, 255, 0);
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> tgt_h(pcd_tgt, 0, 0, 255);
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> sac_h(pcd_tgt, 0, 255, 0);
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> final_h(pcd_final, 255, 0, 0);
	viewer.addPointCloud(pcd_src, src_h, "source cloud");
	//viewer.addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(pcd_src, src_normals, 1, 0.1, "src normal");
	viewer.addPointCloud(pcd_tgt, tgt_h, "tgt cloud");
	viewer.addPointCloud(pcd_sac, sac_h, "sac cloud");
	viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2, "sac cloud");
	viewer.addPointCloud(pcd_final, final_h, "final cloud");
	viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "final cloud");

	//viewer.addCoordinateSystem(1.0);
	while (!viewer.wasStopped())
	{
		viewer.spinOnce(100);
		//boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
}

//由旋转平移矩阵计算旋转角度
void matrix2angle(Eigen::Matrix4f& result_trans, Eigen::Vector3f& result_angle)
{
	double ax, ay, az;
	if (result_trans(2, 0) == 1 || result_trans(2, 0) == -1)
	{
		az = 0;
		double dlta;
		dlta = atan2(result_trans(0, 1), result_trans(0, 2));
		if (result_trans(2, 0) == -1)
		{
			ay = M_PI / 2;
			ax = az + dlta;
		}
		else
		{
			ay = -M_PI / 2;
			ax = -az + dlta;
		}
	}
	else
	{
		ay = -asin(result_trans(2, 0));
		ax = atan2(result_trans(2, 1) / cos(ay), result_trans(2, 2) / cos(ay));
		az = atan2(result_trans(1, 0) / cos(ay), result_trans(0, 0) / cos(ay));
	}
	result_angle << ax, ay, az;
}

//读取txt文件
void ReadTXTCloud(const std::string& file_path, pcl::PointCloud<pcl::PointXYZ>::Ptr cloud)
{
	std::ifstream file(file_path.c_str());//c_str()：生成一个const char*指针，指向以空字符终止的数组。
	std::string line;
	pcl::PointXYZ point;
	while (getline(file, line)) {
		std::stringstream ss(line);
		ss >> point.x;
		ss >> point.y;
		ss >> point.z;
		cloud->push_back(point);
	}
	file.close();
}

int main(int argc, char** argv)
{
	//加载点云文件
	PointCloud::Ptr cloud_src_o(new PointCloud);//原点云，待配准
	string filename = "cloud1516";
	ReadTXTCloud("l159.txt", cloud_src_o);

	PointCloud::Ptr cloud_tgt_o(new PointCloud);//目标点云
	//ReadTXTCloud("HelmetCloud.txt", cloud_tgt_o);
	ReadTXTCloud("Helmet-y.txt", cloud_tgt_o);

	//clock_t start = clock();
	//测量点云处理及表面法线计算///////////////////////////////////////////////////////////////////////////////////////////////////////
	//去除NAN点
	std::vector<int> indices_src; //保存去除的点的索引
	pcl::removeNaNFromPointCloud(*cloud_src_o, *cloud_src_o, indices_src);
	std::cout << "remove *cloud_src_o nan" << endl;
	//下采样滤波
	pcl::VoxelGrid<pcl::PointXYZ> voxel_grid;
	//voxel_grid.setLeafSize(0.02, 0.02, 0.02);
	voxel_grid.setLeafSize(1, 1, 1);
	voxel_grid.setInputCloud(cloud_src_o);
	PointCloud::Ptr cloud_src(new PointCloud);
	voxel_grid.filter(*cloud_src);
	std::cout << "down size *cloud_src_o from " << cloud_src_o->size() << "to" << cloud_src->size() << endl;
	pcl::io::savePCDFileASCII("src_down.pcd", *cloud_src);
	//计算表面法线
	pcl::NormalEstimationOMP<pcl::PointXYZ, pcl::Normal> ne_src;
	ne_src.setInputCloud(cloud_src);
	pcl::search::KdTree< pcl::PointXYZ>::Ptr tree_src(new pcl::search::KdTree< pcl::PointXYZ>());
	ne_src.setSearchMethod(tree_src);
	pcl::PointCloud<pcl::Normal>::Ptr cloud_src_normals(new pcl::PointCloud< pcl::Normal>);
	//ne_src.setRadiusSearch(0.04);
	ne_src.setRadiusSearch(100);
	ne_src.compute(*cloud_src_normals);

	//参考点云处理及表面法线计算///////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<int> indices_tgt;
	pcl::removeNaNFromPointCloud(*cloud_tgt_o, *cloud_tgt_o, indices_tgt);
	std::cout << "remove *cloud_tgt_o nan" << endl;

	pcl::VoxelGrid<pcl::PointXYZ> voxel_grid_2;
	//voxel_grid_2.setLeafSize(0.02, 0.02, 0.02);
	voxel_grid_2.setLeafSize(3, 3, 3);
	voxel_grid_2.setInputCloud(cloud_tgt_o);
	PointCloud::Ptr cloud_tgt(new PointCloud);
	voxel_grid_2.filter(*cloud_tgt);
	std::cout << "down size *cloud_tgt_o.pcd from " << cloud_tgt_o->size() << "to" << cloud_tgt->size() << endl;
	pcl::io::savePCDFileASCII("tgt_down.pcd", *cloud_tgt);

	pcl::NormalEstimationOMP<pcl::PointXYZ, pcl::Normal> ne_tgt;
	ne_tgt.setInputCloud(cloud_tgt);
	pcl::search::KdTree< pcl::PointXYZ>::Ptr tree_tgt(new pcl::search::KdTree< pcl::PointXYZ>());
	ne_tgt.setSearchMethod(tree_tgt);
	pcl::PointCloud<pcl::Normal>::Ptr cloud_tgt_normals(new pcl::PointCloud< pcl::Normal>);
	//ne_tgt.setKSearch(20);
	//ne_tgt.setRadiusSearch(0.04);
	ne_tgt.setRadiusSearch(100);
	ne_tgt.compute(*cloud_tgt_normals);

	//计算FPFH//////////////////////////////////////////////////////////////////////////////////////////////////////////
	pcl::FPFHEstimationOMP<pcl::PointXYZ, pcl::Normal, pcl::FPFHSignature33> fpfh_src;
	fpfh_src.setInputCloud(cloud_src);
	fpfh_src.setInputNormals(cloud_src_normals);
	pcl::search::KdTree<PointT>::Ptr tree_src_fpfh(new pcl::search::KdTree<PointT>);
	fpfh_src.setSearchMethod(tree_src_fpfh);
	pcl::PointCloud<pcl::FPFHSignature33>::Ptr fpfhs_src(new pcl::PointCloud<pcl::FPFHSignature33>());
	//fpfh_src.setRadiusSearch(0.05);
	fpfh_src.setRadiusSearch(50);
	fpfh_src.compute(*fpfhs_src);
	std::cout << "compute *cloud_src fpfh" << endl;

	pcl::FPFHEstimationOMP<pcl::PointXYZ, pcl::Normal, pcl::FPFHSignature33> fpfh_tgt;
	fpfh_tgt.setInputCloud(cloud_tgt);
	fpfh_tgt.setInputNormals(cloud_tgt_normals);
	pcl::search::KdTree<PointT>::Ptr tree_tgt_fpfh(new pcl::search::KdTree<PointT>);
	fpfh_tgt.setSearchMethod(tree_tgt_fpfh);
	pcl::PointCloud<pcl::FPFHSignature33>::Ptr fpfhs_tgt(new pcl::PointCloud<pcl::FPFHSignature33>());
	//fpfh_tgt.setRadiusSearch(0.05);
	fpfh_tgt.setRadiusSearch(50);
	fpfh_tgt.compute(*fpfhs_tgt);
	std::cout << "compute *cloud_tgt fpfh" << endl;

	//SAC配准/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	clock_t start = clock();
	PointCloud::Ptr sac_result(new PointCloud);
	Eigen::Matrix4f sac_trans;
	pcl::SampleConsensusInitialAlignment<pcl::PointXYZ, pcl::PointXYZ, pcl::FPFHSignature33> scia;
	scia.setInputSource(cloud_src);
	scia.setInputTarget(cloud_tgt);
	scia.setSourceFeatures(fpfhs_src);
	scia.setTargetFeatures(fpfhs_tgt);

	scia.setMinSampleDistance(0.1);//设置样本之间的最小距离
	scia.setNumberOfSamples(20);//设置每次迭代计算中使用的样本数量(可省)，可节省时间
	scia.setCorrespondenceRandomness(100);//在选择随机特征对应时，设置要使用的邻居的数量;
	//也就是计算协方差时选择的近邻点个数，该值越大，协方差越精确，但是计算效率越低.(可省)

	scia.align(*sac_result);
	cout << "sac has converged:" << scia.hasConverged() << "  score: " << scia.getFitnessScore() << endl;
	sac_trans = scia.getFinalTransformation();
	cout << sac_trans << endl;
	string filename2 = filename + "SAC.pcd";
	pcl::io::savePCDFileASCII(filename2, *sac_result);

	clock_t sac_time = clock();

	//icp配准
	PointCloud::Ptr icp_result(new PointCloud);
	pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
	icp.setInputSource(cloud_src);
	icp.setInputTarget(cloud_tgt_o);

	//Set the max correspondence distance to 4cm (e.g., correspondences with higher distances will be ignored)
	//icp.setMaxCorrespondenceDistance(0.5);
	// 最大迭代次数
	icp.setMaximumIterations(30000);
	// 两次变化矩阵之间的差值
	icp.setTransformationEpsilon(1e-12);
	// 均方误差
	icp.setEuclideanFitnessEpsilon(0.00001);
	icp.convergence_criteria_->setAbsoluteMSE(1e-6);
	icp.convergence_criteria_->setMaximumIterationsSimilarTransforms(3);

	icp.align(*icp_result, sac_trans);
	clock_t end = clock();
	cout << "total time: " << (double)(end - start) / (double)CLOCKS_PER_SEC << " s" << endl;
	//我把计算法线和点特征直方图的时间也算在SAC里面了
	cout << "sac time: " << (double)(sac_time - start) / (double)CLOCKS_PER_SEC << " s" << endl;
	cout << "icp time: " << (double)(end - sac_time) / (double)CLOCKS_PER_SEC << " s" << endl;

	std::cout << "ICP has converged:" << icp.hasConverged()
		<< " score: " << icp.getFitnessScore() << std::endl;
	Eigen::Matrix4f icp_trans;
	icp_trans = icp.getFinalTransformation();
	//cout<<"ransformationProbability"<<icp.getTransformationProbability()<<endl;
	std::cout << icp_trans << endl;
	//使用创建的变换对未过滤的输入点云进行变换
	pcl::transformPointCloud(*cloud_src_o, *icp_result, icp_trans);
	//保存转换的输入点云
	filename2 = filename + "ICP.pcd";
	pcl::io::savePCDFileASCII(filename2, *icp_result);

	//存入txt
	ofstream Result(filename + "_Result.txt", ios::trunc);
	Result << "SACscore:	" << scia.getFitnessScore() << endl;
	Result << "Transformation:" << endl;
	Result << sac_trans << endl;
	Result << "ICPscore:	" << icp.getFitnessScore() << endl;
	Result << "Transformation:" << endl;
	Result << icp_trans << endl;
	Result.close();

	//可视化
	visualize_pcd(cloud_src_o, cloud_tgt_o, sac_result, icp_result);
	return (0);
}