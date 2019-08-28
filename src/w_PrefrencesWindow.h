#ifndef HVCONF_H
#define HVCONF_H

#include <QDialog>
#include <ui_w_PrefrencesWindow.h>
#include "Qv2rayBase.h"

namespace Ui
{
    class PrefrencesWindow;
}

class PrefrencesWindow : public QDialog
{
        Q_OBJECT

    public:
        explicit PrefrencesWindow(QWidget *parent = nullptr);
        ~PrefrencesWindow();
    signals:
        void s_reload_config(bool need_restart);
    private slots:
        void on_buttonBox_accepted();
        void on_httpCB_stateChanged(int arg1);
        void on_socksCB_stateChanged(int arg1);

        void on_httpAuthCB_stateChanged(int arg1);

        void on_runAsRootCheckBox_stateChanged(int arg1);

        void on_socksAuthCB_stateChanged(int arg1);

        void on_languageComboBox_currentTextChanged(const QString &arg1);

        void on_logLevelComboBox_currentIndexChanged(int index);

        void on_vCoreExePathTxt_textEdited(const QString &arg1);

        void on_vCoreAssetsPathTxt_textEdited(const QString &arg1);

        void on_muxEnabledCB_stateChanged(int arg1);

        void on_muxConcurrencyTxt_valueChanged(int arg1);

        void on_listenIPTxt_textEdited(const QString &arg1);

        void on_socksPortLE_textEdited(const QString &arg1);

        void on_httpPortLE_textEdited(const QString &arg1);

        void on_httpAuthUsernameTxt_textEdited(const QString &arg1);

        void on_httpAuthPasswordTxt_textEdited(const QString &arg1);

        void on_socksAuthUsernameTxt_textEdited(const QString &arg1);

        void on_socksAuthPasswordTxt_textEdited(const QString &arg1);

        void on_proxyCNCb_stateChanged(int arg1);

        void on_proxyDefaultCb_stateChanged(int arg1);

        void on_localDNSCb_stateChanged(int arg1);

        void on_selectVCoreBtn_clicked();

        void on_selectVAssetBtn_clicked();

        void on_DNSListTxt_textChanged();

        void on_autoStartCombo_currentTextChanged(const QString &arg1);

        void on_aboutQt_clicked();

        void on_cancelIgnoreVersionBtn_clicked();

    private:
        bool IsConnectionPropertyChanged = false;
        bool finishedLoading = false;
        Qv2ray::QvConfigModels::Qv2rayConfig CurrentConfig;
        Ui::PrefrencesWindow *ui;
};
#endif // HVCONF_H
