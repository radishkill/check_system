#include <stdio.h>
#include <string.h>

#include "CKCameraInterface.h"

#include <chrono>
#include <iostream>

using namespace std;


void test_softtrigger()
{
  HANDLE hCamera;
  int sum_time = 0;

  int status = CameraInit(&hCamera, 0);

  status = CameraSetTriggerMode(hCamera, 1);  //soft trigger
  status = CameraSetFrameSpeed(hCamera, 1);
  status = CameraSetAeState(hCamera, FALSE);
  status = CameraSetExposureTime(hCamera, 30000);
  status = CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_RGB8);
  status = CameraSetTriggerDelayTime(hCamera, 0);
  status = CameraSetTriggerFrameCount(hCamera, 1);
  status = CameraSetResolution(hCamera, IMAGEOUT_MODE_1280X720);


  tSdkImageResolution image_res;
  int mode;
  status = CameraSetResolution(hCamera, 4);
  if(status != CAMERA_STATUS_SUCCESS)
  {
    cerr << "camera start failed" << status <<  endl;
    //	CameraUnInit(hCamera);
    return;
  }

  status = CameraPlay(hCamera);
  if(status != CAMERA_STATUS_SUCCESS)
  {
    cerr << "camera start failed" << endl;
    CameraUnInit(hCamera);
    return;
  }

  uint32_t frameNo = 0;
  while (true)
  {
    auto begin_tick = chrono::steady_clock::now();
    status = CameraSoftTrigger(hCamera);
    if(status != CAMERA_STATUS_SUCCESS) {
      cout << "soft trigger failed" << endl;
      break;
    }

    stImageInfo imgInfo;
    BYTE * pImgData = CameraGetImageBufferEx(hCamera, &imgInfo, 1000);
    if (pImgData == NULL) {
      cout << "can't get a frame" << endl;
      frameNo++;
    } else {
      auto end_tick = chrono::steady_clock::now();

      cout << "soft trigger to get image duration " << chrono::duration_cast<chrono::milliseconds>(end_tick - begin_tick).count() << " ms" << endl;
      cout << "get a frame no " << frameNo << endl;
      std::cout << imgInfo.iWidth << " " << imgInfo.iHeight << std::endl;
      std::cout << "total bytes:" << imgInfo.TotalBytes << std::endl;
      frameNo++;
      sum_time += chrono::duration_cast<chrono::milliseconds>(end_tick - begin_tick).count();
      std::cout << "avg time :" << sum_time/frameNo << std::endl;

//      pImgData = CameraGetImageBufferEx(hCamera, &imgInfo, 1000);
//      if(pImgData)
//        cerr << "error: get a frame again" << endl;
    }
  }
  status = CameraPause(hCamera);

  CameraUnInit(hCamera);
}

int main(int argc, char *argv[])
{
  int cameraNum = 0;
    int ret = CameraEnumerateDevice(&cameraNum);
  if(ret != CAMERA_STATUS_SUCCESS || cameraNum == 0)
  {
    cerr << "find camera number " << cameraNum << endl;
    return 0;
  }

  test_softtrigger();

  return 0;

}


