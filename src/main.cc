#include <iostream>
#include <thread>
#include <fstream>

#include "CKCameraInterface.h"
#include "usart.h"

HANDLE m_hCamera;
DWORD m_dwWidth;
DWORD m_dwHeight;

std::ofstream ofs;
BYTE* fp;


void CameraThread() {
  CameraSdkStatus status;
  HANDLE hBuf;
  BYTE*			pbyBuffer;
  BYTE*           pRBGBuffer = NULL;
  DWORD           dwRGBBufSize = 0;
  float capFrameRate = 0;
  float disFrameRate = 0;
  UINT disFrameCnt = 0;
  FrameStatistic curFS, lastFS;
  stImageInfo imageInfo;

  std::cout << m_dwHeight << "   " << m_dwWidth << std::endl;
  CameraPlay(m_hCamera);
  // 获取统计信息来统计帧率
  CameraGetFrameStatistic(m_hCamera, &lastFS);
  // 获取raw image data
  status = CameraGetRawImageBuffer(m_hCamera, &hBuf, 1000);
  if (status != CAMERA_STATUS_SUCCESS) {
    std::cout << "CAMERA_STATUS_SUCCESS\n";
 //   continue;
  }
  // 获取图像帧信息
  pbyBuffer = CameraGetImageInfo(m_hCamera, hBuf, &imageInfo);
  // 申请RGB image buffer内存
  if (pRBGBuffer == NULL || imageInfo.iWidth * imageInfo.iHeight * 4 > dwRGBBufSize)
  {
      if (pRBGBuffer)
          delete []pRBGBuffer;
      dwRGBBufSize = imageInfo.iWidth * imageInfo.iHeight * 4;
      pRBGBuffer = new BYTE[dwRGBBufSize];
  }
  status = CameraGetOutImageBuffer(m_hCamera, &imageInfo, pbyBuffer, pRBGBuffer);
  CameraReleaseFrameHandle(m_hCamera, hBuf);
  if (status == CAMERA_STATUS_SUCCESS)
  {
      // 叠加十字线等
      CameraImageOverlay(m_hCamera, pRBGBuffer, &imageInfo);
      std::cout << "image info" << imageInfo.iWidth << " " << imageInfo.iHeight << std::endl;
//      std::cout << pRBGBuffer << std::endl;
      disFrameCnt++;
  }
  fp = pRBGBuffer;
  for (int i = 0; i < dwRGBBufSize; i+=1920) {
    ofs.write((char*)fp, 1920);
    fp+=1920;
  }
  CameraPause(m_hCamera);
  if (pRBGBuffer)
      delete[]pRBGBuffer;
  ofs.close();
}

void TestCamer() {
  ofs.open("test.img");
  // 枚举设备， 并将设备增加到控件中
  INT cameraNum = 0;
  CameraEnumerateDevice(&cameraNum);
  for (int i = 0; i < cameraNum; i++)
  {
    tDevEnumInfo devAllInfo;
    CameraSdkStatus status = CameraGetEnumIndexInfo(i, &devAllInfo);
    if (status != CAMERA_STATUS_SUCCESS)
      continue;
    std::cout << devAllInfo.DevAttribute.acFriendlyName << std::endl;
  }
  CameraSdkStatus ret = CameraInit(&m_hCamera, 0);
  if (ret != CAMERA_STATUS_SUCCESS)
  {
      std::cout << "open camer error\n";
      m_hCamera = NULL;
      return;
  }

  CameraGetOutImageSize(m_hCamera, &m_dwWidth, &m_dwHeight);
  CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_BGR8);
  std::thread th1(CameraThread);
  th1.join();
}
void TestUsart() {
  Usart usart("/dev/ttyUSB0", 9600, 8, 1, 'N', 0);

  usart.SendData("test\n");
  while (1) {
    std::cout << usart.ReadData() << std::endl;
    sleep(1);
  }
}

int main() {
//  TestCamer();
  TestUsart();
  return 0;
}
