#include "cameraplayer.h"
#include "ui_cameraplayer.h"

#include <QTime>
#include <QMessageBox>

CameraPlayer::CameraPlayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraPlayer)
  , m_hCamera(NULL)
  , m_pVideoThread(NULL)
  , m_pGraphScene(NULL)
  , m_pGraphPixmapItem(NULL)
{
    ui->setupUi(this);

    InitGui();
}

CameraPlayer::~CameraPlayer()
{
    CameraStop();

    if (m_pGraphPixmapItem)
    {
        m_pGraphScene->removeItem(m_pGraphPixmapItem);
        delete m_pGraphPixmapItem;
    }
    if (m_pGraphScene)
    {
        delete m_pGraphScene;
    }

    delete ui;
}


void CameraPlayer::InitGui()
{
    m_pGraphScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(m_pGraphScene);


    connect(ui->pushButton_play, SIGNAL(clicked()), this, SLOT(CameraPlay()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(CameraStop()));
    connect(ui->pushButton_setting, SIGNAL(clicked()), this, SLOT(CameraSetting()));
    connect(ui->pushButton_refresh, SIGNAL(clicked()), this, SLOT(CameraRefresh()));

    RefreshDevice();
}

// 刷新设备列表，将设备显示在combo控件中
void CameraPlayer::RefreshDevice()
{
    ui->comboBox_deviceList->clear();

    // 枚举设备， 并将设备增加到控件中
    INT cameraNum = 0;
    CameraEnumerateDevice(&cameraNum);
    for (int i = 0; i < cameraNum; i++)
    {
        tDevEnumInfo devAllInfo;
        CameraSdkStatus status = CameraGetEnumIndexInfo(i, &devAllInfo);
        if (status != CAMERA_STATUS_SUCCESS)
            continue;

        QString strText;
        strText.sprintf("Dev: %d %s", i, devAllInfo.DevAttribute.acFriendlyName);
        ui->comboBox_deviceList->insertItem(i, strText);
    }
}

// 播放combo控件中选择的设备
void CameraPlayer::CameraPlay()
{
    // 先关闭当前的播放，再打开摄像头
    CameraStop();

    int idx = ui->comboBox_deviceList->currentIndex();
    CameraSdkStatus ret = CameraInit(&m_hCamera, idx);
    if (ret != CAMERA_STATUS_SUCCESS)
    {
        QMessageBox::critical(this, QString("error"), QString("open camera error"));
        m_hCamera = NULL;
        return;
    }


    CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_RGB8);

    ret = CameraSetResolution(m_hCamera, IMAGEOUT_MODE_1280X960);
    CameraGetOutImageSize(m_hCamera, &m_dwWidth, &m_dwHeight);


    m_pVideoThread = new CVideoThread(m_hCamera, this);
    connect(m_pVideoThread, SIGNAL(updateFrameRate(float,float)), this, SLOT(UpdateFrameRate(float,float)));
    connect(m_pVideoThread, SIGNAL(acquireFrame(QImage)), this, SLOT(AcquireFrame(QImage)), Qt::BlockingQueuedConnection);
    m_pVideoThread->start();
}

// 停止当前的设备
void CameraPlayer::CameraStop()
{
    if (m_pVideoThread)
    {
        m_pVideoThread->stop();
        while (!m_pVideoThread->wait(100))
            QCoreApplication::processEvents();
        delete m_pVideoThread;
        m_pVideoThread = NULL;
    }
    if (m_hCamera != NULL)
    {
        CameraUnInit(m_hCamera);
        m_hCamera = NULL;
    }
    ui->label_FrameRate->setText(QString("设备已经关闭"));
}

// 打开配置对话框
void CameraPlayer::CameraSetting()
{
    if (m_hCamera != NULL)
    {
        if (CameraSetActivePage(m_hCamera, SETTING_PAGE_ALL, 0) != CAMERA_STATUS_SUCCESS)
            return;
        if (CameraCreateSettingPageEx(m_hCamera) != CAMERA_STATUS_SUCCESS)
            return;
        CameraShowSettingPage(m_hCamera, TRUE);
    }
}

// 刷新设备
void CameraPlayer::CameraRefresh()
{
    RefreshDevice();
}

// 更新帧率
void CameraPlayer::UpdateFrameRate(float capFrameRate, float disFrameRate)
{
    QString strFrameRate;
    strFrameRate.sprintf("分辨率: %ux%u | 获取帧率: %.1f | 显示帧率: %.1f", m_dwWidth, m_dwHeight, capFrameRate, disFrameRate);
    ui->label_FrameRate->setText(strFrameRate);
}

// 显示图像
void CameraPlayer::AcquireFrame(QImage img)
{

    if (m_pGraphPixmapItem)
    {
        m_pGraphScene->removeItem(m_pGraphPixmapItem);
        delete m_pGraphPixmapItem;
        m_pGraphPixmapItem = NULL;
    }

    m_pGraphPixmapItem = m_pGraphScene->addPixmap(QPixmap::fromImage(img));

    m_pGraphScene->setSceneRect(0, 0, img.width(), img.height());
}


//////////////////////////////////////////////////////////////////
CVideoThread::CVideoThread(HANDLE hCamera, QObject *parent)
    : QThread(parent)
    , m_bQuit(false)
    , m_hCamera(hCamera)
{

}


void CVideoThread::run()
{
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
    QTime time;

    CameraPlay(m_hCamera);
    time.start();
    // 获取统计信息来统计帧率
    CameraGetFrameStatistic(m_hCamera, &lastFS);
    while (!m_bQuit)
    {
        int elapsed = time.elapsed();
        if (elapsed >= 1000)
        {
            // 获取统计信息来统计帧率
            CameraGetFrameStatistic(m_hCamera, &curFS);
            capFrameRate = (float)(((float)curFS.iCapture - (float)lastFS.iCapture)*1000.0) / elapsed;
            disFrameRate = (float)disFrameCnt * 1000.0 / elapsed;
            emit updateFrameRate(capFrameRate, disFrameRate);
            lastFS = curFS;

            disFrameCnt = 0;
            time.restart();
        }

        // 获取raw image data
        status = CameraGetRawImageBuffer(m_hCamera, &hBuf, 1000);
        if (status != CAMERA_STATUS_SUCCESS)
        {
            msleep(5);
            continue;
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

            QImage img(pRBGBuffer, imageInfo.iWidth, imageInfo.iHeight, QImage::Format_RGB888);
            emit acquireFrame(img);
            disFrameCnt++;
        }
    }
    CameraPause(m_hCamera);
    if (pRBGBuffer)
        delete[]pRBGBuffer;
}

void CVideoThread::stream()
{

}

void CVideoThread::pause()
{

}

void CVideoThread::resume()
{

}

void CVideoThread::stop()
{
    m_bQuit = true;
}


