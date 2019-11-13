#include "w_ScreenShot_Core.hpp"
#include "QvUtils.hpp"
#include <QMessageBox>
#include <QThread>

ScreenShotWindow::ScreenShotWindow() : QDialog(), rubber(new QRubberBand(QRubberBand::Rectangle, this))
{
    setupUi(this);
    QRect deskRect = qApp->screens().first()->geometry();
    width = deskRect.width();
    height = deskRect.height();
    //
    setMouseTracking(true);
    resize(width, height);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowType::Dialog);
    //
    setBackground(width, height, 0.6f);
    label->setAttribute(Qt::WA_TranslucentBackground);
    startBtn->setAttribute(Qt::WA_TranslucentBackground);
    //
    QPalette pal;
    pal.setColor(QPalette::WindowText, Qt::white);
    label->setPalette(pal);
    startBtn->setPalette(pal);
    //
    label->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    startBtn->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
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

void ScreenShotWindow::pSize() //获取截图位置坐标
{
    pw = abs(end.x() - origin.x());
    ph = abs(end.y() - origin.y());
    px = origin.x() < end.x() ? origin.x() : end.x();
    py = origin.y() < end.y() ? origin.y() : end.y();
}


void ScreenShotWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        this->close();
    }
}

void ScreenShotWindow::mousePressEvent(QMouseEvent *e) //鼠标按下
{
    origin = e->pos();
    LOG(MODULE_UI, "Start capturing mouse")
    rubber->setGeometry(origin.x(), origin.y(), 0, 0);
    rubber->show();
    label->show();
    startBtn->show();
}

void ScreenShotWindow::mouseMoveEvent(QMouseEvent *e) //鼠标移动
{
    if (e->buttons() & Qt::LeftButton) {
        end = e->pos();
        pSize();
        rubber->setGeometry(px, py, pw, ph);
        //
        QString size = QString("%1x%2").arg(pw).arg(ph);
        label->setText(size);
        //
        QRect labelRect(label->contentsRect());
        QRect btnRect(startBtn->contentsRect());

        if (py > labelRect.height()) {
            label->move(QPoint(px, py - labelRect.height()));
        } else {
            label->move(QPoint(px, py));
        }

        if (height - py - ph > btnRect.height()) {
            startBtn->move(QPoint(px + pw - btnRect.width(), py + ph));
        }        else {
            startBtn->move(QPoint(px + pw - btnRect.width(), py + ph - btnRect.height()));
        }

        label->show();
        startBtn->show();
    }
}


void ScreenShotWindow::mouseReleaseEvent(QMouseEvent *e) //鼠标松开
{
    if (e->button() == Qt::RightButton) {
        close();
    }
}

void ScreenShotWindow::setBackground(int w, int h, float n) //定格当前屏幕
{
    QScreen *screen = QGuiApplication::primaryScreen();
    bg = screen->grabWindow(0).toImage();
    QImage bg_grey(w, h, QImage::Format_RGB32);
    //
    int r, g, b;

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            //
            r = static_cast<int>(qRed(bg.pixel(i, j)) * n);
            g = static_cast<int>(qGreen(bg.pixel(i, j)) * n);
            b = static_cast<int>(qBlue(bg.pixel(i, j)) * n);
            //
            bg_grey.setPixel(i, j, qRgb(r, g, b));
        }
    }

    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(bg_grey));
    this->setPalette(palette);
    this->showFullScreen();
}

void ScreenShotWindow::on_startBtn_clicked()
{
    resultImage = bg.copy(px, py, pw, ph);
    rubber->hide();
    close();
}
