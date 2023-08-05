#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>

namespace Ui {
class mydialog;
}

class mydialog : public QDialog
{
    Q_OBJECT

public:
    explicit mydialog(QWidget *parent = nullptr);
    ~mydialog();

signals:
    void categoryAdd(const QString&);
    void categoryDelete(const QString&);

private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_2_accepted();

    void on_buttonBox_2_rejected();

public slots:
    void changinIndex(const int&);
    void comboBoxUpdate(const QString&);
private:
    Ui::mydialog *ui;
    QString nuevaCategoria;
};

#endif // MYDIALOG_H
