#pragma once
#include "base/Qv2rayBase.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "ui/common/QvMessageBus.hpp"
#include "ui/widgets/widgets/StreamSettingsWidget.hpp"
#include "ui_w_OutboundEditor.h"

#include <QDialog>
#include <QtCore>

class OutboundEditor
    : public QDialog
    , private Ui::OutboundEditor
{
    Q_OBJECT
  public:
    explicit OutboundEditor(const OUTBOUND &outboundEntry, QWidget *parent = nullptr);
    ~OutboundEditor();
    OUTBOUND OpenEditor();
    QString GetFriendlyName();

  private:
    explicit OutboundEditor(QWidget *parent = nullptr);
    QvMessageBusSlotDecl;

  private slots:
    void on_buttonBox_accepted();
    void on_ipLineEdit_textEdited(const QString &arg1);
    void on_muxConcurrencyTxt_valueChanged(int arg1);
    void on_muxEnabledCB_stateChanged(int arg1);
    void on_outBoundTypeCombo_currentIndexChanged(int index);
    void on_portLineEdit_textEdited(const QString &arg1);
    void on_tagTxt_textEdited(const QString &arg1);
    void on_useFPCB_stateChanged(int arg1);

  private:
    QString tag;
    void reloadGUI();
    bool useForwardProxy;
    OUTBOUND generateConnectionJson();
    OUTBOUND originalConfig;
    OUTBOUND resultConfig;
    QJsonObject muxConfig;
    QString serverAddress;
    int serverPort;
    //
    // Connection Configs
    QString outboundType;
    //
    StreamSettingsWidget *streamSettingsWidget;
    //
    QMap<QString, QvPluginEditor *> pluginWidgets;
};
