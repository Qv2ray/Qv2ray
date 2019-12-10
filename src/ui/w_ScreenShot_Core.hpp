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

    protected:
        bool event(QEvent *e) override;

    private slots:
        void on_startBtn_clicked();

    private:
        QRubberBand *rubber;
        // Desktop Image
        QPixmap desktopImage;
        QImage windowBg;
        QImage resultImage;
        //
        QPoint origin;
        QPoint end;
        int imgW, imgH, imgX, imgY;

        void pSize();
};

#endif // SCREENSHOT_H
