#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QDialog>
#include <QRubberBand>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QScreen>
#include <QKeyEvent>
#include <QPushButton>
#include <QPalette>

#include "ui_w_ScreenShot_Core.h"

class ScreenShotWindow : public QDialog, private Ui::ScreenShot
{
        Q_OBJECT

    public:
        ScreenShotWindow();
        QImage DoScreenShot();
        //
        void mouseMoveEvent(QMouseEvent *e) override;
        void mousePressEvent(QMouseEvent *e) override;
        void mouseReleaseEvent(QMouseEvent *e) override;
        void keyPressEvent(QKeyEvent *e) override;
        void setBackground(int w, int h, float n);


    private slots:
        void on_startBtn_clicked();

    private:
        QImage resultImage;
        //
        QRubberBand *rubber;
        //
        QPoint origin;//鼠标起始位置
        QPoint end;//鼠标结束位置
        QImage bg;//存贮当前桌面截图

        int width, height;
        int pw, ph, px, py;

        void pSize();
};

#endif // SCREENSHOT_H
