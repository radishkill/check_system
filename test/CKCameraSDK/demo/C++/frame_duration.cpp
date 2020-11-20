#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <wchar.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <iostream>
#include "CKCameraInterface.h"


using namespace std;

static volatile  bool g_is_quit = 0;

int frame_duration()
{
	INT deviceNum = 0;
	CameraEnumerateDevice(&deviceNum);
	if(deviceNum == 0)
	{
		cerr << "can't find camera\n" << endl;
		return -1;
	}

	HANDLE hCamera;
	CameraSdkStatus status = CameraInit(&hCamera, 0);
	if(status != CAMERA_STATUS_SUCCESS)
	{
		cerr << "camera init failed\n" << endl;
		return -1;
	}

	CameraSetAeState(hCamera, FALSE);
	CameraSetExposureTime(hCamera, 100 * 1000); //100ms
	CameraSetAnalogGain(hCamera, 1000);  // analog gain 1.0
	CameraSetTriggerMode(hCamera, 0);  //continous mode
	CameraSetFrameSpeed(hCamera, 2);   // high speed

	UINT64 pre_timestamp = 0;
	struct timeval pre_tv = {0};
	gettimeofday(&pre_tv, NULL);
	CameraPlay(hCamera);
	while(!g_is_quit)
	{
		HANDLE hRawBuf;
		status = CameraGetRawImageBuffer(hCamera, &hRawBuf, 1000);
		if(status != CAMERA_STATUS_SUCCESS)
			continue;

		struct timeval cur_tv;
		gettimeofday(&cur_tv, NULL);
		cout << "frame interval time by system time(us) " << (cur_tv.tv_sec * 1000000 + cur_tv.tv_usec) - (pre_tv.tv_sec * 1000000 + pre_tv.tv_usec) << endl;
		memcpy(&pre_tv, &cur_tv, sizeof(pre_tv));
		
		UINT64 timestamp = 0;
		CameraGetImageTimestamp(hCamera, hRawBuf, &timestamp);
		cout << "frame interval time(us) " << timestamp - pre_timestamp << endl;
		pre_timestamp = timestamp;
		CameraReleaseFrameHandle(hCamera, hRawBuf);
	}
	CameraPause(hCamera);
	CameraUnInit(hCamera);

	return 0;
}

static void sighandler(int signum)
{
	(void)signum;

	g_is_quit = 1;
}

int main(int argc, char *argv[])
{
	struct sigaction sigact;
	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);

	frame_duration();
	
	return 0;
}





