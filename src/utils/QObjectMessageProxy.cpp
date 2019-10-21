#include "QObjectMessageProxy.h"
namespace Qv2ray
{
    namespace Utils
    {
        QObjectMessageProxy::QObjectMessageProxy(action *function) : QObject()
        {
            _function = function;
        }
        QObjectMessageProxy::~QObjectMessageProxy()
        {
            // Destructor
        }
        void QObjectMessageProxy::processMessage()
        {
            _function();
        }
    }
}
