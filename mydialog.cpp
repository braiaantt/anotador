#include "mydialog.h"
#include "ui_mydialog.h"
#include "QMessageBox"


mydialog::mydialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mydialog)
{
    ui->setupUi(this);
}

mydialog::~mydialog()
{
    delete ui;
}

void mydialog::changinIndex(const int& indice){

    if(indice == 0){

        ui->stackedWidget->setCurrentIndex(0);

    } else {

        ui->stackedWidget->setCurrentIndex(1);

    }

}

void mydialog::on_buttonBox_accepted()
{
  // Obtener el valor del QLineEdit
    QString nuevaCategoria = ui->newCatLineEdit->text();

    // Comprobar si la categoría no está vacía
    if (!nuevaCategoria.isEmpty()) {
        // Emitir la señal
        emit categoryAdd(nuevaCategoria);
        accept(); // Cerrar el QDialog y devolver el resultado aceptado
    } else {
        // Mostrar un mensaje de error o indicar que el campo está vacío
        QMessageBox::warning(this, "Error", "La categoría no puede estar vacía.");
    }
}

void mydialog::comboBoxUpdate(const QString& cat){

    ui->comboBox->addItem(cat);

}



void mydialog::on_buttonBox_2_accepted()
{

    QString categoria = ui->comboBox->currentText();

    int index = ui->comboBox->findText(categoria);
    ui->comboBox->removeItem(index);

    emit categoryDelete(categoria);
    accept();
}


void mydialog::on_buttonBox_2_rejected()
{
    reject();
}

