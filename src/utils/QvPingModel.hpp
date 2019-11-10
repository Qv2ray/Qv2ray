#ifndef PINGMODEL_H
#define PINGMODEL_H

#include <QObject>
#include <QProcess>

namespace Qv2ray
{
    namespace Utils
    {
        class QvPingModel : public QObject
        {
                Q_OBJECT
            public:
                explicit QvPingModel(QObject *parent = nullptr);
                ~QvPingModel();

                void start_command();
                bool is_running();
                bool finished();

            public slots:
                void verifyStatus();
                void readResult();

            private:
                QProcess *pingworker;
                bool running;
        };
    }
}
#endif // PINGMODEL_H
