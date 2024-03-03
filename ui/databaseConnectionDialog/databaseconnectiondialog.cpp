//
// Created by csger on 2024. 02. 28..
//

// You may need to build the project (run Qt uic code generator) to get "ui_databaseConnectionDialog.h" resolved

#include "databaseconnectiondialog.h"
#include "ui_databaseConnectionDialog.h"


databaseConnectionDialog::databaseConnectionDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::databaseConnectionDialog) {
    ui->setupUi(this);
    ui->username->setText("csger");
    ui->database->setText("gprinter");
    ui->host->setText("localhost");
}

databaseConnectionDialog::~databaseConnectionDialog() {
    delete ui;
}

void databaseConnectionDialog::accept() {
    emit dialogFinished(ui->username->text().toStdString(), ui->password->text().toStdString(),
                        ui->database->text().toStdString(), ui->host->text().toStdString());
    QDialog::accept();
}

void databaseConnectionDialog::reject() {
    emit dialogFinished("", "", "", "");
    QDialog::reject();
}
