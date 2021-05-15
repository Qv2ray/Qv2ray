#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/common/QvMessageBus.hpp"
#include "ui_w_ChainSha256Editor.h"

#include <QDialog>

class ChainSha256Editor
    : public QDialog
    , private Ui::ChainSha256Editor
{
    Q_OBJECT

  private:
    QList<QString> chain;

  private:
    static QList<QString> convertFromString(const QString &&str);
    static std::optional<QString> validateError(const QList<QString> &newChain);

  public:
    explicit ChainSha256Editor(QWidget *parent = nullptr, const QList<QString>& chain = QList<QString>());
    explicit operator QList<QString>()
    {
        return this->chain;
    };

  private slots:
    void on_buttonBox_helpRequested();

  private:
    void accept() override;

  private:
    QvMessageBusSlotDecl;
};
