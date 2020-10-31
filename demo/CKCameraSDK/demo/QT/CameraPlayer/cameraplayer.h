#ifndef CAMERAPLAYER_H
#define CAMERAPLAYER_H

#include <QDialog>
#include <QThread>
#include <QGraphicsItem>

#include "CKCameraInterface.h"

namespace Ui {
class CameraPlayer;
}

//////////////////////////////////////////////////
// 视频帧获取并显示线程
class CVideoThread : public QThread
{
    Q_OBJECT
public:
    explicit CVideoThread(HANDLE hCamera, QObject *parent = 0);

signals:
    void updateFrameRate(float capFrameRate, float disFrameRate);
    void acquireFrame(QImage img);

public:
    void run();
    void stream();
    void pause();
    void resume();
    void stop();
private:
    volatile bool m_bQuit;
    HANDLE m_hCamera;
};

class CameraPlayer : public QDialog
{
    Q_OBJECT

public:
    explicit CameraPlayer(QWidget *parent = 0);
    ~CameraPlayer();
private:
    void InitGui();
    void RefreshDevice();
private slots:
void CameraPlay();
void CameraStop();
void CameraSetting();
void CameraRefresh();
void UpdateFrameRate(float capFrameRate, float disFrameRate);
void AcquireFrame(QImage img);

private:
    Ui::CameraPlayer *ui;
    HANDLE m_hCamera;
    CVideoThread *m_pVideoThread;
    DWORD m_dwWidth;
    DWORD m_dwHeight;
    QGraphicsScene *m_pGraphScene;
    QGraphicsPixmapItem *m_pGraphPixmapItem;
};

#endif // CAMERAPLAYER_H
