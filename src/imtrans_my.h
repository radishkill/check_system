#pragma once
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


#include <vector>
#include <iterator>
#include <algorithm> //输出vector所有值

#include<numeric>//计算方差和均值

#include <opencv2/calib3d.hpp>
using namespace cv;
using namespace cv::xfeatures2d;
using std::cout;
using std::endl;


int TransformPic(Mat& img1, Mat& img2, Mat rI2) {
    Mat rI1;
   
    if (img1.empty() || img2.empty()) {
        cout << "Could not open or find the image!\n" << endl;

        return -1;
    }

    //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
    int minHessian = 400;
    Ptr<SURF> detector = SURF::create(minHessian);
    std::vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);

    cout << "key1 key2  " << keypoints1.size() << "   " << keypoints2.size() << endl;

    //-- Step 2: Matching descriptor vectors with a brute force matcher
    // Since SURF is a floating-point descriptor NORM_L2 is used
    /*Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);
    std::vector< DMatch > matches;
    matcher->match(descriptors1, descriptors2, matches);*/


    /*KNN匹配方法*/
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector< std::vector<DMatch> > knn_matches;
    matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);
    cout << "KNN_matches; " << knn_matches.size() << endl;
    /*对应matlab中的MaxRatio*/
    const float ratio_thresh = 0.6f;
    std::vector<DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
        }
    }
    cout << "ratio_thresh good_matches; " << good_matches.size() << endl;
    /*对应matlab中的MatchThreshold*/
    //sort函数对数据进行升序排列
    //筛选匹配点，根据match里面特征对的距离从小到大排序
    //筛选出最优的knn_matches.size() * MatchThreshold个匹配点
    float MatchThreshold = 0.1;
    sort(knn_matches.begin(), knn_matches.end());
    float matche_distant = knn_matches[floor(knn_matches.size() * MatchThreshold)][0].distance;//位于knn_matches.size() * MatchThreshold处的distance
    std::vector< DMatch > good_good_matches;
    for (int i = 0; i < good_matches.size() * MatchThreshold; i++)
    {
        if (good_matches[i].distance < matche_distant)
            good_good_matches.push_back(good_matches[i]);
    }
    cout << "MatchThreshold good good_matches; " << good_good_matches.size() << endl;

    //-- Draw matches
    std::vector<Point2f> keypoints01;
    std::vector<Point2f> keypoints02;
    for (size_t i = 0; i < good_good_matches.size(); i++)
    {
        //-- Get the keypoints from the good matches
        keypoints01.push_back(keypoints1[good_good_matches[i].queryIdx].pt);
        keypoints02.push_back(keypoints2[good_good_matches[i].trainIdx].pt);
        circle(img1, keypoints01[i], 10, Scalar(0, 255, 0));
    }
    cout << "  1 的特征点的坐标" << keypoints01 << endl;
    cout << " 2 的特征点的坐标" << keypoints02 << endl;


    Mat img_matches;

    drawMatches(img1, keypoints1, img2, keypoints2, good_good_matches, img_matches, cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255));
    bitwise_not(img1, img1);
//    imshow("img1", img1);


    //-- Localize the object
    /*得到成对的特征点的坐标，因为特征点1 和2  的数量不相同直接使用 .pt 的话，得到的可能不是成对的两个点*/
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for (size_t i = 0; i < good_good_matches.size(); i++)
    {
        //-- Get the keypoints from the good matches
        obj.push_back(keypoints1[good_good_matches[i].queryIdx].pt);
        scene.push_back(keypoints2[good_good_matches[i].trainIdx].pt);
    }

    cout << "筛选之前的数量" << good_good_matches.size() << endl;

    //TODO
    std::vector<Point2f>x1;
    std::vector<Point2f>::iterator i;
    std::vector<Point2f>::iterator j;
    std::vector<float> distant;  //半径过大异常

    int ii = 0;
    int test_count = 0;//距离大于阈值的个数  测试删除部分是否正确
    for (
        i = obj.begin(), j = scene.begin();
        //ii<786;
        (i != obj.end()) && (j != scene.end());
        ii++, i++, j++
        )
    {


        distant.push_back(powf(((*i).x - (*j).x), 2) + powf(((*i).y - (*j).y), 2));
        if (distant[ii] > 2000) {
            test_count++;
        }

    }

    /*DMatch的distance（在0-1之间）  和手动计算的distance*/
    /*for (int a = 0; a < 768; a++) {
        cout<<matches[a].distance<<endl;
    }
    cout << "---------------------";
    std::copy(distant.begin(), distant.end(), std::ostream_iterator<int>(cout, "\n"));*/

    for (int a = 0; a < distant.size(); ) {///distant.size()  是变化的，值为最开始的size-count（删除了多少个）,因为做了删除之后值会变小
        if (distant[a] > 2000) {
            cout << " 删除此点 位置 距离" << obj[a] << "  " << scene[a] << " " << distant[a] << endl;
            obj.begin() + a = obj.erase(obj.begin() + a);//删除
            scene.begin() + a = scene.erase(scene.begin() + a);
            distant.begin() + a = distant.erase(distant.begin() + a);

        }
        else {

            a++;
        }

    }
    cout << "删除半径过大的 " << obj.size() << "  " << distant.size() << endl;

    /*经过测试，删除成功*/
    /*cout << endl << "删除之后的 理论值" << matches.size() -test_count << endl;
    cout << "实际值 "<<obj.size()<<"  "<<distant.size()<<endl;*/


    /*消除半径异常的*/
    /*  tip  在matlab 代码中没有判断删除完半径过大 是否为空*/
    for (int aa = 0; aa < 3; aa++) {
        if (distant.size() != 0) {
            float sum = std::accumulate(distant.begin(), distant.end(), 0.0);
            float mean = sum / distant.size(); //均值
            float accum = 0.0;
            std::for_each(distant.begin(), distant.end(), [&](const double d) {
                accum += (d - mean) * (d - mean);
                });

            double stdev = sqrt(accum / (distant.size() - 1)); //方差  标准差

            cout << "avg " << mean << "std  " << stdev << endl;
            for (int a = 0; a < distant.size(); ) {

                if ((distant[a] > (mean + 3 * stdev)) || (distant[a] < (mean - 3 * stdev))) {
                    cout << " 删除此点 位置 距离" << obj[a] << "  " << scene[a] << " " << distant[a] << endl;
                    obj.begin() + a = obj.erase(obj.begin() + a);//删除
                    scene.begin() + a = scene.erase(scene.begin() + a);
                    distant.begin() + a = distant.erase(distant.begin() + a);
                }
                else {
                    a++;
                }

            }

        }
        else continue;

    }
    cout << "/*消除半径异常的*/ " << obj.size() << "  " << distant.size() << endl;

    /*消除方向异常的*/
    std::vector<float> x_div_y_of_delt_distant;//方向异常
    /*x_div_y_of_delt_distant 不能和distance在上面的 for 中push
    因为上面做了删除操作，keypoints长度变小，
    x_div_y_of_delt_distant如果在上面初始化，长度会比做了半径删除操作之后的keypoints的长度长，溢出*/
    for (
        i = obj.begin(), j = scene.begin();
        (i != obj.end()) && (j != scene.end());
        i++, j++
        )
    {
        x_div_y_of_delt_distant.push_back(Point2f(i - j).x / Point2f(i - j).y);

    }

    cout << "删除方向异常前 " << x_div_y_of_delt_distant.size();
    for (int aa = 0; aa < 3; aa++) {
        float sum = std::accumulate(x_div_y_of_delt_distant.begin(), x_div_y_of_delt_distant.end(), 0.0);
        float mean = sum / x_div_y_of_delt_distant.size(); //均值
        float accum = 0.0;
        std::for_each(x_div_y_of_delt_distant.begin(), x_div_y_of_delt_distant.end(), [&](const double d) {
            accum += (d - mean) * (d - mean);
            });

        double stdev = sqrt(accum / (x_div_y_of_delt_distant.size() - 1)); //方差  标准差

        //cout << "avg " << mean << "std  " << stdev << endl;
        //int b=0;
        if (x_div_y_of_delt_distant.size() != 0) {

            for (int a = 0; a < x_div_y_of_delt_distant.size(); ) {

                if ((x_div_y_of_delt_distant[a] > (mean + 3 * stdev)) || (x_div_y_of_delt_distant[a] < (mean - 3 * stdev))) {
                    cout << " 删除此点 位置 方向" << obj[a] << "  " << scene[a] << " " << x_div_y_of_delt_distant[a] << endl;
                    obj.begin() + a = obj.erase(obj.begin() + a);//删除
                    scene.begin() + a = scene.erase(scene.begin() + a);
                    x_div_y_of_delt_distant.begin() + a = x_div_y_of_delt_distant.erase(x_div_y_of_delt_distant.begin() + a);
                    //cout <<" x "<< b<<endl;
                }
                else {
                    a++;
                    //cout << " y " << b << endl;
                }
                //b++;
            }
        }
        else continue;

    }
    cout << "/*消除方向异常的*/ " << x_div_y_of_delt_distant.size() << "   " << obj.size() << endl;

    Rect rect = Rect(Point(50, 50), Point(img2.cols - 50, img2.rows - 50));

   
    if (obj.size() != 0) {

        if (obj.size() >= 8) {
            /*投射变换，旋转*/

            /*变换核 需要变换的，参考的 （类型：特征点的坐标）*/
            Mat HH = findHomography(scene, obj, RANSAC);

            /*做变换的图片，输出，变换核，大小*/
            cv::warpPerspective(img2, rI2, HH, img2.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
                       
            
        }

        else if ((obj.size() < 8) && (obj.size() > 0)) {

            Point2f dx;
            if (obj.size() == 1) {
                dx = Point2f(floor((obj[0] - obj[0]).x), floor((obj[0] - obj[0]).y));
            }
            else
            {
                /*求均值*/
                float avgx = 0;
                float sumx = 0;
                float avgy = 0;
                float sumy = 0;
                for (int aa = 0; aa < obj.size(); aa++) {
                    sumx += (obj[aa] - scene[aa]).x;
                    sumy += (obj[aa] - scene[aa]).y;
                }
                dx = Point2f(floor(sumx / obj.size()), floor(sumy / obj.size()));
            }

            /*平移变换*/
            cv::Size dst_sz = img2.size();

            //定义平移矩阵
            cv::Mat t_mat = cv::Mat::zeros(2, 3, CV_32FC1);

            t_mat.at<float>(0, 0) = 1;
            t_mat.at<float>(0, 2) = dx.x; //水平平移量
            t_mat.at<float>(1, 1) = 1;
            t_mat.at<float>(1, 2) = dx.y; //竖直平移量

            //根据平移矩阵进行仿射变换
            cv::warpAffine(img2, rI2, t_mat, dst_sz);
            rI2 = Mat(rI2, rect);

        }

        else {
            rI2 = Mat(img2, rect);
        }
    }
    else {
        rI2 = Mat(img2, rect);
    }
    
    rI1 = Mat(img1, rect);

    ////-- Show detected matches
//    imshow("Matches", img_matches);
//    imshow("rI2", rI2);
//    waitKey();
    return 0;

}

