#pragma once
#include "ui_w_ScreenShot_Core.h"

#include <QDialog>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPalette>
#include <QPushButton>
#include <QRubberBand>
#include <QScreen>

class ScreenShotWindow
    : public QDialog
    , private Ui::ScreenShot
{
    Q_OBJECT

  public:
    ScreenShotWindow();
    ~ScreenShotWindow();
    QImage DoScreenShot();
    //
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

  private slots:
    void on_startBtn_clicked();

  private:
    double scale;
    QRubberBand rubber;
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
