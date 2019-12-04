#include "w_ScreenShot_Core.hpp"
#include "QvUtils.hpp"
#include <QMessageBox>
#include <QThread>
#include <QStyleFactory>

#define QV2RAY_SCREENSHOT_DIM_RATIO 0.6f

ScreenShotWindow::ScreenShotWindow() : QDialog(), rubber(new QRubberBand(QRubberBand::Rectangle, this))
{
    setupUi(this);
    // Fusion prevents the KDE Plasma Breeze's "Move window when dragging in the empty area" issue
    this->setStyle(QStyleFactory::create("Fusion"));
    //
    LOG(MODULE_IMPORT, "We currently only support the primary screen.")
    desktopImage = QGuiApplication::primaryScreen()->grabWindow(0);
    //
    int w = desktopImage.width();
    int h = desktopImage.height();
    QImage bg_grey(w, h, QImage::Format_RGB32);
    //
    int r, g, b;
    auto _xdesktopImg = desktopImage.toImage();

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            r = static_cast<int>(qRed(_xdesktopImg.pixel(i, j)) * QV2RAY_SCREENSHOT_DIM_RATIO);
            g = static_cast<int>(qGreen(_xdesktopImg.pixel(i, j)) * QV2RAY_SCREENSHOT_DIM_RATIO);
            b = static_cast<int>(qBlue(_xdesktopImg.pixel(i, j)) * QV2RAY_SCREENSHOT_DIM_RATIO);
            bg_grey.setPixel(i, j, qRgb(r, g, b));
        }
    }

    //bg_grey = bg_grey.scaled(this->size(), Qt::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);
    auto p = this->palette();
    p.setBrush(QPalette::Background, bg_grey);
    setPalette(p);
    //
    setWindowState(Qt::WindowState::WindowFullScreen);
    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->showFullScreen();
    label->setAttribute(Qt::WA_TranslucentBackground);
    startBtn->setAttribute(Qt::WA_TranslucentBackground);
    //
    QPalette pal;
    pal.setColor(QPalette::WindowText, Qt::white);
    label->setPalette(pal);
    startBtn->setPalette(pal);
    //
    label->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    startBtn->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    //
    label->hide();
    startBtn->hide();
    this->show();
}

QImage ScreenShotWindow::DoScreenShot()
{
    this->exec();
    return resultImage;
}

void ScreenShotWindow::pSize()
{
    imgW = abs(end.x() - origin.x());
    imgH = abs(end.y() - origin.y());
    imgX = origin.x() < end.x() ? origin.x() : end.x();
    imgY = origin.y() < end.y() ? origin.y() : end.y();
    DEBUG("Capture Mouse Position", to_string(imgW)  + " " + to_string(imgH)  + " " + to_string(imgX) + " " + to_string(imgY))
    fg->setGeometry(imgX, imgY, imgW, imgH);
    fg->setPixmap(desktopImage.copy(imgX, imgY, imgW, imgH).scaled(fg->size() * devicePixelRatio()));
    rubber->setGeometry(imgX, imgY, imgW, imgH);
}


void ScreenShotWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        reject();
    } else if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        on_startBtn_clicked();
    }
}

void ScreenShotWindow::mousePressEvent(QMouseEvent *e)
{
    origin = e->pos();
    LOG(MODULE_UI, "Start capturing mouse")
    rubber->setGeometry(origin.x(), origin.y(), 0, 0);
    rubber->show();
    rubber->raise();
    label->hide();
    startBtn->hide();
}

void ScreenShotWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        end = e->pos();
        pSize();
        //
        label->setText(QString("%1x%2").arg(imgW).arg(imgH));
        //
        //
        QRect labelRect(label->contentsRect());
        QRect btnRect(startBtn->contentsRect());

        if (imgY > labelRect.height()) {
            label->move(imgX, imgY - labelRect.height());
        } else {
            label->move(imgX, imgY);
        }

        if (height() - imgY - imgH > btnRect.height()) {
            startBtn->move(imgX + imgW - btnRect.width(), imgY + imgH);
        } else {
            startBtn->move(imgX + imgW - btnRect.width(), imgY + imgH - btnRect.height());
        }

        label->show();
        startBtn->show();
    }
}


void ScreenShotWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton) {
        reject();
    }
}


void ScreenShotWindow::on_startBtn_clicked()
{
    resultImage = desktopImage.copy(imgX, imgY, imgW, imgH).toImage();
    rubber->hide();
    accept();
}
