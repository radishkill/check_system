/********************************************************************************
** Form generated from reading UI file 'cameraplayer.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERAPLAYER_H
#define UI_CAMERAPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CameraPlayer
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox_deviceList;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_play;
    QPushButton *pushButton_stop;
    QPushButton *pushButton_setting;
    QPushButton *pushButton_refresh;
    QGraphicsView *graphicsView;
    QLabel *label_FrameRate;

    void setupUi(QDialog *CameraPlayer)
    {
        if (CameraPlayer->objectName().isEmpty())
            CameraPlayer->setObjectName(QString::fromUtf8("CameraPlayer"));
        CameraPlayer->resize(683, 630);
        verticalLayoutWidget = new QWidget(CameraPlayer);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 661, 611));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        comboBox_deviceList = new QComboBox(verticalLayoutWidget);
        comboBox_deviceList->setObjectName(QString::fromUtf8("comboBox_deviceList"));

        horizontalLayout->addWidget(comboBox_deviceList);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButton_play = new QPushButton(verticalLayoutWidget);
        pushButton_play->setObjectName(QString::fromUtf8("pushButton_play"));

        horizontalLayout_2->addWidget(pushButton_play);

        pushButton_stop = new QPushButton(verticalLayoutWidget);
        pushButton_stop->setObjectName(QString::fromUtf8("pushButton_stop"));

        horizontalLayout_2->addWidget(pushButton_stop);

        pushButton_setting = new QPushButton(verticalLayoutWidget);
        pushButton_setting->setObjectName(QString::fromUtf8("pushButton_setting"));

        horizontalLayout_2->addWidget(pushButton_setting);

        pushButton_refresh = new QPushButton(verticalLayoutWidget);
        pushButton_refresh->setObjectName(QString::fromUtf8("pushButton_refresh"));

        horizontalLayout_2->addWidget(pushButton_refresh);


        verticalLayout->addLayout(horizontalLayout_2);

        graphicsView = new QGraphicsView(verticalLayoutWidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));

        verticalLayout->addWidget(graphicsView);

        label_FrameRate = new QLabel(verticalLayoutWidget);
        label_FrameRate->setObjectName(QString::fromUtf8("label_FrameRate"));

        verticalLayout->addWidget(label_FrameRate);


        retranslateUi(CameraPlayer);

        QMetaObject::connectSlotsByName(CameraPlayer);
    } // setupUi

    void retranslateUi(QDialog *CameraPlayer)
    {
        CameraPlayer->setWindowTitle(QCoreApplication::translate("CameraPlayer", "CameraPlayer", nullptr));
        label->setText(QCoreApplication::translate("CameraPlayer", "\350\256\276\345\244\207\345\210\227\350\241\250\357\274\232", nullptr));
        pushButton_play->setText(QCoreApplication::translate("CameraPlayer", "\346\222\255\346\224\276", nullptr));
        pushButton_stop->setText(QCoreApplication::translate("CameraPlayer", "\345\201\234\346\255\242", nullptr));
        pushButton_setting->setText(QCoreApplication::translate("CameraPlayer", "\350\256\276\347\275\256", nullptr));
        pushButton_refresh->setText(QCoreApplication::translate("CameraPlayer", "\345\210\267\346\226\260", nullptr));
        label_FrameRate->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CameraPlayer: public Ui_CameraPlayer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERAPLAYER_H
