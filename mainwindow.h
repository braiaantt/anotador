#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mydialog.h"
#include "QSqlDatabase"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_boton_enviar_clicked();

    void on_backButton_clicked();

    void mostrarTextoDelBoton(QString);

    void createButton(QString);

    void on_updateButton_clicked();

    void on_comb_categoriasFilter_activated(int);

    void on_addCatButton_clicked();

    void onCategoriaAgregada(const QString&);
    void deleteCategory(const QString&);

    void inicioComboBox();
    void on_deleteCatButton_clicked();

    int lastId();

    void on_pushButton_clicked();

    int findId(QString);

    void on_updateButton__clicked();

signals:
    void currentIndex(const int&);
    void modCategoria(const QString&);
private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    bool buttonCLicked;
    mydialog dialog;
};
#endif // MAINWINDOW_H
