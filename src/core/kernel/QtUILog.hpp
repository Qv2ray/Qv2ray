#ifndef QTUILOG_HPP
#define QTUILOG_HPP
#include <QThread>
class Q_DECL_EXPORT QtUILog :public QThread
{
    Q_OBJECT
Q_SIGNALS:
    void OnDataReady(quint64 dataUp, quint64 dataDown);
    void onSSRThreadLog(QString);
};
#endif // QTUILOG_HPP
