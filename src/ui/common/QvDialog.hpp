#pragma once
#include <QDialog>

class QvDialog : public QDialog
{
    Q_OBJECT
  public:
    explicit QvDialog(QWidget *parent) : QDialog(parent){};
    virtual void processCommands(QString command, QStringList commands, QMap<QString, QString> args) = 0;
    virtual void updateColorScheme() = 0;
};
