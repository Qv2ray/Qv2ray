#ifndef FORMWIDGET_H
#define FORMWIDGET_H

#include <QWidget>

namespace Ui
{
    class Formwidget;
}

class Formwidget : public QWidget
{
        Q_OBJECT

    public:
        explicit Formwidget(int id, QWidget *parent = nullptr);
        ~Formwidget();

    private slots:
        void on_pushButton_clicked();

    private:
        int _id;
        Ui::Formwidget *ui;
};

#endif // FORMWIDGET_H
