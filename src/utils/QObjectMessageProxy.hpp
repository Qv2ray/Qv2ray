#ifndef QVQOBJECTPROXY_H
#define QVQOBJECTPROXY_H
#include <QObject>
namespace Qv2ray
{
    namespace Utils
    {
        typedef void (action)();
        class QObjectMessageProxy : public QObject
        {
                Q_OBJECT
            public:
                explicit QObjectMessageProxy(action *function);
                ~QObjectMessageProxy();
            public slots:
                void processMessage();
            private:
                action *_function;
        };
    }
}
#endif // QVQOBJECTPROXY_H
