#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QSqlDatabase"
#include "QSqlError"
#include "QSqlQuery"
#include "QMessageBox"
#include "QThread"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    buttonCLicked = false;
    QVBoxLayout* layout = new QVBoxLayout(ui->widget_7);
    layout->setAlignment(Qt::AlignTop);
    ui->stackedWidget->setCurrentIndex(0);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("baseDeDatos.db");

    if (!db.open()) {
        qDebug() << "Error al abrir la base de datos:" << db.lastError().text();
    } else {
        qDebug() << "Conexión exitosa con la base de datos.";
    }

    connect(&dialog, &mydialog::categoryAdd, this, &MainWindow::onCategoriaAgregada);
    connect(&dialog, &mydialog::categoryDelete, this, &MainWindow::deleteCategory);
    connect(this, &MainWindow::currentIndex, &dialog, &mydialog::changinIndex);
    connect(this, &MainWindow::modCategoria, &dialog, &mydialog::comboBoxUpdate);

    createButton(NULL);
    inicioComboBox();
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

void MainWindow::inicioComboBox(){

    QSqlQuery query;
    query.prepare("SELECT categoria FROM categorias");

    if (!query.exec()) {
        qWarning() << "Error al ejecutar la consulta:" << query.lastError().text();
        return;
    }


    while(query.next()){

        QString newItem = query.value(0).toString();
        ui->categoriasComboBox->addItem(newItem);
        ui->comb_categoriasFilter->addItem(newItem);
        emit modCategoria(newItem);

    }

}

void MainWindow::mostrarTextoDelBoton(QString titulo){

    // Usar una consulta SELECT para buscar el texto asociado al título en la base de datos
    QSqlQuery query;
    query.prepare("SELECT texto FROM la_tabla WHERE titulo = ?");
    query.addBindValue(titulo);

    if (!query.exec()) {
        qWarning() << "Error al consultar la base de datos:" << query.lastError().text();
        return;
    }

    // Si el título existe en la base de datos, mostrar el texto en el QTextEdit
    if (query.next()) {
        QString texto = query.value(0).toString();
        ui->mostrarTexto_TextEdit->setPlainText(texto);
        ui->label_titulo->setText(titulo);
    } else {
        // Si el título no se encuentra en la base de datos, mostrar un mensaje de error
        qWarning() << "No se encontró el título en la base de datos.";
        ui->mostrarTexto_TextEdit->setPlainText("Error: Título no encontrado.");
    }

    // Cambiar al índice 1 del QStackedWidget para mostrar el QTextEdit
    ui->stackedWidget->setCurrentIndex(1);

}

void MainWindow::on_boton_enviar_clicked()
{
    if(!ui->in_titulo->text().isEmpty() && !ui->textoTextEdit->toPlainText().isEmpty()){

        //Obteniendo valores de los campos
        QString categoria = ui->categoriasComboBox->currentText();
        QString texto = ui->textoTextEdit->toPlainText();
        QString titulo = ui->in_titulo->text();

        //Guardando valores en base de datos
        QSqlQuery query;
        query.prepare("INSERT INTO la_tabla (titulo, categoria, texto) VALUES (?, ?, ?)");
        query.addBindValue(titulo);
        query.addBindValue(categoria);
        query.addBindValue(texto);

        if (!query.exec()) {
            qWarning() << "Error al insertar en la base de datos:" << query.lastError().text();
        } else {
            qDebug() << "Registro insertado correctamente!";

            buttonCLicked = true;
            createButton(titulo);

            ui->textoTextEdit->clear();
            ui->in_titulo->clear();
        }

    }
}

int MainWindow::lastId(){

    QSqlQuery query;
    query.prepare("SELECT MAX(ID) FROM la_tabla");

    if (query.exec() && query.next()) {
        int lastid = query.value(0).toInt();
        qDebug()<<"Ultimo id"<<lastid;
        return lastid;

    } else {
        qWarning() << "Error al obtener el último ID de la base de datos:" << query.lastError().text();
        return 0;
    }

}

void MainWindow::createButton(const QString titulo){

    if(buttonCLicked){

        int lastID = lastId();

        //Creando botones
        QPushButton *nuevoboton = new QPushButton(this);
        nuevoboton->setText(titulo);
        nuevoboton->setObjectName("boton_" + QString::number(lastID));
        nuevoboton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        QPushButton *botonEliminar = new QPushButton(this);
        botonEliminar->setText("-");
        botonEliminar->setFixedSize(25,25);
        botonEliminar->setObjectName(QString::number(lastID));
        botonEliminar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QPushButton *editButton = new QPushButton(this);
        editButton->setText("...");
        editButton->setFixedSize(25,25);
        editButton->setObjectName("editButton_" + QString::number(lastID));
        editButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(nuevoboton);
        buttonLayout->addWidget(editButton);
        buttonLayout->addWidget(botonEliminar);
        buttonLayout->setObjectName("groupButtons_" + QString::number(lastID));

        QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->widget_7->layout());
        mainLayout->addLayout(buttonLayout);


        //Dandole funcion al boton creado
        connect(nuevoboton, &QPushButton::clicked, this, [=] (){


            QString ttitulo = nuevoboton->text();

            mostrarTextoDelBoton(ttitulo);

        });

        connect(botonEliminar, &QPushButton::clicked, this, [=] (){

            qDebug()<<"Inicio de eliminacion";

            QString titulo = nuevoboton->text();
            QMessageBox::StandardButton respuesta = QMessageBox::question(this, "Eliminar", "Estás seguro que quieres eliminar " + titulo, QMessageBox::Yes | QMessageBox::No);

            if(respuesta == QMessageBox::Yes){

                QSqlQuery query;
                query.prepare("DELETE FROM la_tabla WHERE titulo = ?");
                query.addBindValue(titulo);


                if (query.exec()) {

                    qDebug()<<"Eliminación exitosa!";

                    delete nuevoboton;
                    delete botonEliminar;
                    delete editButton;

                } else {
                    qWarning() << "Error al eliminar de la base de datos:" << query.lastError().text();
                }

            }

        });

        connect(editButton, &QPushButton::clicked, this, [=] (){

            ui->stackedWidget->setCurrentIndex(2);

            QString objectname = nuevoboton->objectName();

            int ID = findId(objectname);

            QSqlQuery query;
            query.prepare("SELECT titulo, categoria FROM la_tabla WHERE ID = ?");
            query.addBindValue(ID);

            if (query.exec() && query.next()) {
                QString titulo = query.value(0).toString();
                QString categoria = query.value(1).toString();

                ui->lineEditNewTitle->setText(titulo);
                ui->lineEditNewCat->setText(categoria);

                ui->updateButton_->setObjectName("updateButton_" + QString::number(ID));

            } else {
                qDebug() << "Error al ejecutar la consulta o no se encontró una fila con el ID:" << ID;
            }

        });

    } else {

        QSqlQuery query("SELECT ID, titulo FROM la_tabla");
        while(query.next()){

            int ID = query.value(0).toInt();

            QString titulo = query.value(1).toString();
            QPushButton *nuevoboton = new QPushButton(this);
            QPushButton *botonEliminar = new QPushButton(this);
            QPushButton *editButton = new QPushButton(this);

            botonEliminar->setText("-");
            botonEliminar->setObjectName(QString::number(ID));
            botonEliminar->setFixedSize(25,25);
            botonEliminar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            nuevoboton->setText(titulo);
            nuevoboton->setObjectName("boton_" + QString::number(ID));
            nuevoboton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            editButton->setText("...");
            editButton->setObjectName("editButton_" + QString::number(ID));
            editButton->setFixedSize(25,25);
            editButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


            QHBoxLayout* buttonLayout = new QHBoxLayout();
            buttonLayout->addWidget(nuevoboton);
            buttonLayout->addWidget(editButton);
            buttonLayout->addWidget(botonEliminar);
            buttonLayout->setObjectName("groupButtons_" + QString::number(ID));


            QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->widget_7->layout());
            mainLayout->addLayout(buttonLayout);


            connect(nuevoboton, &QPushButton::clicked, this, [=] (){

                mostrarTextoDelBoton(titulo);

            });

            connect(botonEliminar, &QPushButton::clicked, this, [=] (){

                QString titulo = nuevoboton->text();
                QMessageBox::StandardButton respuesta = QMessageBox::question(this, "Eliminar", "Estás seguro que quieres eliminar " + titulo, QMessageBox::Yes | QMessageBox::No);

                    if(respuesta == QMessageBox::Yes){

                    QSqlQuery query;
                    query.prepare("DELETE FROM la_tabla WHERE titulo = ?");
                    query.addBindValue(titulo);
                    if (query.exec()) {
                        //Eliminación exitosa, también debes eliminar los botones de la interfaz

                        delete nuevoboton;
                        delete botonEliminar;
                        delete editButton;


                    } else {
                        qWarning() << "Error al eliminar de la base de datos:" << query.lastError().text();
                    }

                }

            });

            connect(editButton, &QPushButton::clicked, this, [=] (){

                ui->stackedWidget->setCurrentIndex(2);

                QString objectname = nuevoboton->objectName();

                int ID = findId(objectname);

                QSqlQuery query;
                query.prepare("SELECT titulo, categoria FROM la_tabla WHERE ID = ?");
                query.addBindValue(ID);

                if (query.exec() && query.next()) {
                    QString titulo = query.value(0).toString();
                    QString categoria = query.value(1).toString();

                    ui->lineEditNewTitle->setText(titulo);
                    ui->lineEditNewCat->setText(categoria);

                    ui->updateButton_->setObjectName("updateButton_" + QString::number(ID));

                } else {
                    qDebug() << "Error al ejecutar la consulta o no se encontró una fila con el ID:" << ID;
                }
            });

        }

    }

}

int MainWindow::findId(QString name){

    QString ID;
    for(int i = 0; i< name.length();i++){

        QChar caracter = name.at(i);

        if(caracter.isDigit()){

            ID.append(caracter);

        }

    }

    qDebug()<<ID.toInt();
    return ID.toInt();

}

void MainWindow::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}



void MainWindow::on_updateButton_clicked()
{
    QString titulo = ui->label_titulo->text();
    QString newText = ui->mostrarTexto_TextEdit->toPlainText();

    QSqlQuery query;
    query.prepare("UPDATE la_tabla SET texto = ? WHERE titulo = ?");
    query.addBindValue(newText);
    query.addBindValue(titulo);

    if (query.exec()) {
        qDebug()<<"Base de datos actualizada correctamente";
    } else {
        // Error al ejecutar la consulta
        qWarning() << "Error al actualizar:" << query.lastError().text();
    }

}




void MainWindow::on_comb_categoriasFilter_activated(int index)
{
    QString categoria = ui->comb_categoriasFilter->currentText();

    if(categoria == "todos"){

        QSqlQuery query;
        query.prepare("SELECT id FROM la_tabla");

        if (!query.exec()) {
            qDebug() << "Error al ejecutar la consulta:" << query.lastError().text();
            return;
        }

        while(query.next()){

            QString ID = query.value(0).toString();

            QPushButton* button = findChild<QPushButton*>("boton_" + ID);
            QPushButton* deleteButton = findChild<QPushButton*>(ID);
            QPushButton* editButton = findChild<QPushButton*>("editButton_" + ID);


            if(button && deleteButton){

                button->setHidden(false);
                deleteButton->setHidden(false);
                editButton->setHidden(false);

            } else {

                qDebug()<<"No se encontraron a los botones con el ID: " + ID;

            }

        }

    } else {

        QSqlQuery query("SELECT ID, categoria FROM la_tabla");
        while (query.next()) {

            QString catButton = query.value(1).toString();
            QString ID = query.value(0).toString();

            QPushButton* button = findChild<QPushButton*>("boton_" + ID);
            QPushButton* deleteButton = findChild<QPushButton*>(ID);
            QPushButton* editButton = findChild<QPushButton*>("editButton_" + ID);

            if(categoria != catButton){

                button->setHidden(true);
                deleteButton->setHidden(true);
                editButton->setHidden(true);

            } else {

                button->setHidden(false);
                deleteButton->setHidden(false);
                editButton->setHidden(false);

            }


        }

    }

}


void MainWindow::on_addCatButton_clicked()
{
    dialog.show();
    emit currentIndex(0);
}

void MainWindow::on_deleteCatButton_clicked()
{
    dialog.show();
    emit currentIndex(1);

}

void MainWindow::onCategoriaAgregada(const QString& categoria){

    ui->categoriasComboBox->addItem(categoria);
    ui->comb_categoriasFilter->addItem(categoria);
    emit modCategoria(categoria);
    QSqlQuery query;
    query.prepare("INSERT INTO categorias (categoria) VALUES (?)");
    query.addBindValue(categoria);

    if (query.exec()) {
        qDebug() << "Nueva categoría agregada exitosamente!";
    } else {
        qWarning() << "Error al insertar nueva categoría:" << query.lastError().text();
    }

}

void MainWindow::deleteCategory(const QString& categoria){

    QSqlQuery query;
    query.prepare("DELETE FROM categorias WHERE categoria = ?");
    query.addBindValue(categoria);

    if (query.exec()) {
        qDebug() << "Categoría eliminada exitosamente!";

        int index = ui->categoriasComboBox->findText(categoria);
        ui->categoriasComboBox->removeItem(index);

        ui->comb_categoriasFilter->removeItem(index+1);

    } else {
        qWarning() << "Error al insertar nueva categoría:" << query.lastError().text();
    }

}



void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}



void MainWindow::on_updateButton__clicked()
{
    qDebug()<<ui->updateButton_->objectName();
    QString objectname = ui->updateButton_->objectName();
    int ID = findId(objectname);

    QString nuevoTitulo = ui->lineEditNewTitle->text();
    QString nuevaCategoria = ui->lineEditNewCat->text();

    QSqlQuery query;
    query.prepare("SELECT titulo, categoria FROM la_tabla WHERE ID = ?");
    query.addBindValue(ID);

    if(query.exec() && query.next()){

        QString titulo = query.value(0).toString();
        QString categoria = query.value(1).toString();

        if(nuevoTitulo != titulo){

            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE la_tabla SET titulo = ? WHERE ID = ?");
            updateQuery.addBindValue(nuevoTitulo);
            updateQuery.addBindValue(ID);

            if(updateQuery.exec()){

                qDebug()<<"Nuevo titulo añadido";

                QPushButton* button = findChild<QPushButton*>("boton_" + QString::number(ID));
                button->setText(nuevoTitulo);

                ui->stackedWidget->setCurrentIndex(0);

            }

        }

        if(nuevaCategoria != categoria){

            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE la_tabla SET categoria = ? WHERE ID = ?");
            updateQuery.addBindValue(nuevaCategoria);
            updateQuery.addBindValue(ID);

            if(updateQuery.exec()){

                qDebug()<<"Nueva categoria añadido";
                ui->stackedWidget->setCurrentIndex(0);

            }

        }

    }

}

