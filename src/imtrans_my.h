#pragma once
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

#include <numeric>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/calib3d.hpp>

using namespace cv;
using namespace cv::xfeatures2d;
using std::cout;
using std::endl;



int transform_my(Mat& img1, Mat& img2, Mat rI2) {
    Mat rI1;
   
    if (img1.empty() || img2.empty())
    {
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


    /*KNNÆ¥Åä·½·¨*/
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector< std::vector<DMatch> > knn_matches;
    matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);
    cout << "KNN_matches; " << knn_matches.size() << endl;
    /*¶ÔÓ¦matlabÖÐµÄMaxRatio*/
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
    /*¶ÔÓ¦matlabÖÐµÄMatchThreshold*/
    //sortº¯Êý¶ÔÊý¾Ý½øÐÐÉýÐòÅÅÁÐ
    //É¸Ñ¡Æ¥Åäµã£¬¸ù¾ÝmatchÀïÃæÌØÕ÷¶ÔµÄ¾àÀë´ÓÐ¡µ½´óÅÅÐò
    //É¸Ñ¡³ö×îÓÅµÄknn_matches.size() * MatchThreshold¸öÆ¥Åäµã
    float MatchThreshold = 0.1;
    sort(knn_matches.begin(), knn_matches.end());
    float matche_distant = knn_matches[floor(knn_matches.size() * MatchThreshold)][0].distance;//Î»ÓÚknn_matches.size() * MatchThreshold´¦µÄdistance
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
    cout << "  1 µÄÌØÕ÷µãµÄ×ø±ê" << keypoints01 << endl;
    cout << " 2 µÄÌØÕ÷µãµÄ×ø±ê" << keypoints02 << endl;


    Mat img_matches;

    drawMatches(img1, keypoints1, img2, keypoints2, good_good_matches, img_matches, cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255));
    bitwise_not(img1, img1);
    imshow("img1", img1);


    //-- Localize the object
    /*µÃµ½³É¶ÔµÄÌØÕ÷µãµÄ×ø±ê£¬ÒòÎªÌØÕ÷µã1 ºÍ2  µÄÊýÁ¿²»ÏàÍ¬Ö±½ÓÊ¹ÓÃ .pt µÄ»°£¬µÃµ½µÄ¿ÉÄÜ²»ÊÇ³É¶ÔµÄÁ½¸öµã*/
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for (size_t i = 0; i < good_good_matches.size(); i++)
    {
        //-- Get the keypoints from the good matches
        obj.push_back(keypoints1[good_good_matches[i].queryIdx].pt);
        scene.push_back(keypoints2[good_good_matches[i].trainIdx].pt);
    }

    cout << "É¸Ñ¡Ö®Ç°µÄÊýÁ¿" << good_good_matches.size() << endl;

    //TODO
    std::vector<Point2f>x1;
    std::vector<Point2f>::iterator i;
    std::vector<Point2f>::iterator j;
    std::vector<float> distant;  //°ë¾¶¹ý´óÒì³£

    int ii = 0;
    int test_count = 0;//¾àÀë´óÓÚãÐÖµµÄ¸öÊý  ²âÊÔÉ¾³ý²¿·ÖÊÇ·ñÕýÈ·
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

    /*DMatchµÄdistance£¨ÔÚ0-1Ö®¼ä£©  ºÍÊÖ¶¯¼ÆËãµÄdistance*/
    /*for (int a = 0; a < 768; a++) {
        cout<<matches[a].distance<<endl;
    }
    cout << "---------------------";
    std::copy(distant.begin(), distant.end(), std::ostream_iterator<int>(cout, "\n"));*/

    for (int a = 0; a < distant.size(); ) {///distant.size()  ÊÇ±ä»¯µÄ£¬ÖµÎª×î¿ªÊ¼µÄsize-count£¨É¾³ýÁË¶àÉÙ¸ö£©,ÒòÎª×öÁËÉ¾³ýÖ®ºóÖµ»á±äÐ¡
        if (distant[a] > 2000) {
            cout << " É¾³ý´Ëµã Î»ÖÃ ¾àÀë" << obj[a] << "  " << scene[a] << " " << distant[a] << endl;
            obj.begin() + a = obj.erase(obj.begin() + a);//É¾³ý
            scene.begin() + a = scene.erase(scene.begin() + a);
            distant.begin() + a = distant.erase(distant.begin() + a);

        }
        else {

            a++;
        }

    }
    cout << "É¾³ý°ë¾¶¹ý´óµÄ " << obj.size() << "  " << distant.size() << endl;

    /*¾­¹ý²âÊÔ£¬É¾³ý³É¹¦*/
    /*cout << endl << "É¾³ýÖ®ºóµÄ ÀíÂÛÖµ" << matches.size() -test_count << endl;
    cout << "Êµ¼ÊÖµ "<<obj.size()<<"  "<<distant.size()<<endl;*/


    /*Ïû³ý°ë¾¶Òì³£µÄ*/
    /*  tip  ÔÚmatlab ´úÂëÖÐÃ»ÓÐÅÐ¶ÏÉ¾³ýÍê°ë¾¶¹ý´ó ÊÇ·ñÎª¿Õ*/
    for (int aa = 0; aa < 3; aa++) {
        if (distant.size() != 0) {
            float sum = std::accumulate(distant.begin(), distant.end(), 0.0);
            float mean = sum / distant.size(); //¾ùÖµ
            float accum = 0.0;
            std::for_each(distant.begin(), distant.end(), [&](const double d) {
                accum += (d - mean) * (d - mean);
                });

            double stdev = sqrt(accum / (distant.size() - 1)); //·½²î  ±ê×¼²î

            cout << "avg " << mean << "std  " << stdev << endl;
            for (int a = 0; a < distant.size(); ) {

                if ((distant[a] > (mean + 3 * stdev)) || (distant[a] < (mean - 3 * stdev))) {
                    cout << " É¾³ý´Ëµã Î»ÖÃ ¾àÀë" << obj[a] << "  " << scene[a] << " " << distant[a] << endl;
                    obj.begin() + a = obj.erase(obj.begin() + a);//É¾³ý
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
    cout << "/*Ïû³ý°ë¾¶Òì³£µÄ*/ " << obj.size() << "  " << distant.size() << endl;

    /*Ïû³ý·½ÏòÒì³£µÄ*/
    std::vector<float> x_div_y_of_delt_distant;//·½ÏòÒì³£
    /*x_div_y_of_delt_distant ²»ÄÜºÍdistanceÔÚÉÏÃæµÄ for ÖÐpush
    ÒòÎªÉÏÃæ×öÁËÉ¾³ý²Ù×÷£¬keypoints³¤¶È±äÐ¡£¬
    x_div_y_of_delt_distantÈç¹ûÔÚÉÏÃæ³õÊ¼»¯£¬³¤¶È»á±È×öÁË°ë¾¶É¾³ý²Ù×÷Ö®ºóµÄkeypointsµÄ³¤¶È³¤£¬Òç³ö*/
    for (
        i = obj.begin(), j = scene.begin();
        (i != obj.end()) && (j != scene.end());
        i++, j++
        )
    {
        x_div_y_of_delt_distant.push_back(Point2f(i - j).x / Point2f(i - j).y);

    }

    cout << "É¾³ý·½ÏòÒì³£Ç° " << x_div_y_of_delt_distant.size();
    for (int aa = 0; aa < 3; aa++) {
        float sum = std::accumulate(x_div_y_of_delt_distant.begin(), x_div_y_of_delt_distant.end(), 0.0);
        float mean = sum / x_div_y_of_delt_distant.size(); //¾ùÖµ
        float accum = 0.0;
        std::for_each(x_div_y_of_delt_distant.begin(), x_div_y_of_delt_distant.end(), [&](const double d) {
            accum += (d - mean) * (d - mean);
            });

        double stdev = sqrt(accum / (x_div_y_of_delt_distant.size() - 1)); //·½²î  ±ê×¼²î

        //cout << "avg " << mean << "std  " << stdev << endl;
        //int b=0;
        if (x_div_y_of_delt_distant.size() != 0) {

            for (int a = 0; a < x_div_y_of_delt_distant.size(); ) {

                if ((x_div_y_of_delt_distant[a] > (mean + 3 * stdev)) || (x_div_y_of_delt_distant[a] < (mean - 3 * stdev))) {
                    cout << " É¾³ý´Ëµã Î»ÖÃ ·½Ïò" << obj[a] << "  " << scene[a] << " " << x_div_y_of_delt_distant[a] << endl;
                    obj.begin() + a = obj.erase(obj.begin() + a);//É¾³ý
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
    cout << "/*Ïû³ý·½ÏòÒì³£µÄ*/ " << x_div_y_of_delt_distant.size() << "   " << obj.size() << endl;

    Rect rect = Rect(Point(50, 50), Point(img2.cols - 50, img2.rows - 50));

   
    if (obj.size() != 0) {

        if (obj.size() >= 8) {
            /*Í¶Éä±ä»»£¬Ðý×ª*/

            /*±ä»»ºË ÐèÒª±ä»»µÄ£¬²Î¿¼µÄ £¨ÀàÐÍ£ºÌØÕ÷µãµÄ×ø±ê£©*/
            Mat HH = findHomography(scene, obj, RANSAC);

            /*×ö±ä»»µÄÍ¼Æ¬£¬Êä³ö£¬±ä»»ºË£¬´óÐ¡*/
            cv::warpPerspective(img2, rI2, HH, img2.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
                       
            
        }

        else if ((obj.size() < 8) && (obj.size() > 0)) {

            Point2f dx;
            if (obj.size() == 1) {
                dx = Point2f(floor((obj[0] - obj[0]).x), floor((obj[0] - obj[0]).y));
            }
            else
            {
                /*Çó¾ùÖµ*/
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

            /*Æ½ÒÆ±ä»»*/
            cv::Size dst_sz = img2.size();

            //¶¨ÒåÆ½ÒÆ¾ØÕó
            cv::Mat t_mat = cv::Mat::zeros(2, 3, CV_32FC1);

            t_mat.at<float>(0, 0) = 1;
            t_mat.at<float>(0, 2) = dx.x; //Ë®Æ½Æ½ÒÆÁ¿
            t_mat.at<float>(1, 1) = 1;
            t_mat.at<float>(1, 2) = dx.y; //ÊúÖ±Æ½ÒÆÁ¿

            //¸ù¾ÝÆ½ÒÆ¾ØÕó½øÐÐ·ÂÉä±ä»»
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
    imshow("Matches", img_matches);
    imshow("rI2", rI2);
    waitKey();
    return 0;

}

