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
#include <unistd.h>
#include <sys/types.h>

#include <atomic>

#include "CKCameraInterface.h"

using namespace std;

class CameraTest
{
public:
	CameraTest(int cameraIndex):
		m_cameraIndex(cameraIndex)
		, m_hCamera(NULL)
		, m_pImgBuf(NULL)
		, m_imgBufLen(0)
        , m_pThread(NULL)
        , m_isExit(false)
	{
	}

	~CameraTest()
	{
		Stop();
	}

	int Start()
	{
		Stop();
		
		CameraSdkStatus status = CameraInit(&m_hCamera, m_cameraIndex);
		if(status != CAMERA_STATUS_SUCCESS)
		{
			printf("Camera init failed\n");
			return -1;
		}
        CameraSetFrameSpeed(m_hCamera, 2);
        CameraSetAeState(m_hCamera, FALSE);
        CameraSetExposureTime(m_hCamera, 10 * 1000);

        m_isExit = false;
        m_pThread = new thread(&CameraTest::run, this);

        return 0;
	}

    void Stop()
    {
        if(m_pThread)
        {
            // wait thread exit.
            m_isExit = true;
            m_pThread->join();
            delete m_pThread;
            m_pThread = NULL;
        }

        if(m_hCamera)
        {
            CameraUnInit(m_hCamera);
            m_hCamera = NULL;
        }

        if(m_pImgBuf)
        {
            delete[] m_pImgBuf;
            m_pImgBuf = NULL;
        }
        m_imgBufLen = 0;
    }

    void run()
    {
        CameraSdkStatus status;
        stImageInfo imgInfo;
        HANDLE hRawBuf;
        BYTE *pRawBuf;
        chrono::time_point<chrono::steady_clock> last = chrono::steady_clock::now();
        int displayCnt = 0;
        FrameStatistic last_frameStatistic, now_frameStatistic;

        CameraPlay(m_hCamera);
        CameraGetFrameStatistic(m_hCamera, &last_frameStatistic);
        while(!m_isExit)
        {
            // get RAW image buffer handler
            status = CameraGetRawImageBuffer(m_hCamera, &hRawBuf, 1000);
			if(status != CAMERA_STATUS_SUCCESS)
			{
				this_thread::sleep_for(chrono::milliseconds(10));
				continue;
			}

            // get RAW image buffer address
            pRawBuf = CameraGetImageInfo(m_hCamera, hRawBuf, &imgInfo);
            if(m_pImgBuf == NULL || m_imgBufLen < imgInfo.iWidth * imgInfo.iHeight * 4)
            {
                if(m_pImgBuf)
                    delete[] m_pImgBuf;
                m_imgBufLen = imgInfo.iWidth * imgInfo.iHeight * 4;
                m_pImgBuf = new BYTE[m_imgBufLen];
            }
            printf("Camera %d: image width %u height %u\n", m_cameraIndex, imgInfo.iWidth, imgInfo.iHeight);
            displayCnt++;
            auto now = chrono::steady_clock::now();
            if(now - last >= chrono::seconds(1))
            {
                last = now;
                CameraGetFrameStatistic(m_hCamera, &now_frameStatistic);
                printf("Camera %d: capture frame rate %d\n", m_cameraIndex, now_frameStatistic.iCapture - last_frameStatistic.iCapture);
                memcpy(&last_frameStatistic, &now_frameStatistic, sizeof(FrameStatistic));

                printf("Camera %d: display frame rate %d\n", m_cameraIndex, displayCnt);
                displayCnt = 0;

            }

            // process RAW image buffer to ISP out format
            CameraGetOutImageBuffer(m_hCamera, &imgInfo, pRawBuf, m_pImgBuf);
            // release RAW buffer handle
            CameraReleaseFrameHandle(m_hCamera, hRawBuf);
        }
        CameraPause(m_hCamera);
        printf("camera thread exit\n");
    }

private:
	int m_cameraIndex;
	HANDLE m_hCamera;
	BYTE *m_pImgBuf;
	int m_imgBufLen;
	thread *m_pThread;
	atomic_bool m_isExit;
};

static atomic_bool do_exit;

static void sighandler(int signum)
{
	(void)signum;

	do_exit = true;
}

int main(int argc, char *argv[])
{
	uid_t uid = getuid();
	if(uid != 0)
	{
		printf("Please run this application by root!!!\n");
		exit(-1);
	}

    int numCamera = 1;
    if(argc > 1)
    {
        numCamera = atoi(argv[1]);
    }

    do_exit = false;

	struct sigaction sigact;
	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);

    int num;
    CameraSdkStatus status;
    status = CameraEnumerateDevice(&num);
    if(status != CAMERA_STATUS_SUCCESS)
    {
        printf("enumerate camera failed\r\n");
        return -1;
    }
    printf("enumerate camera num %d\n", num);
    if(num == 0)
        return -1;
    if(numCamera > num)
        numCamera = num;

    CameraTest **pCamTest = new CameraTest*[numCamera];

    // start test camera
    for(int i = 0; i < numCamera; i++)
    {
        pCamTest[i] = new CameraTest(i);
        pCamTest[i]->Start();
    }

    while(!do_exit)
    {
        this_thread::sleep_for(chrono::seconds(1));
    }

    // stop test camera
    for(int i = 0; i < numCamera; i++)
    {
        pCamTest[i]->Stop();
        delete pCamTest[i];
    }

    delete[] pCamTest;

    return 0;
}

