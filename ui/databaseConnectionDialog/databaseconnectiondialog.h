//
// Created by csger on 2024. 02. 28..
//

#ifndef GPRINTERCLIENT_DATABASECONNECTIONDIALOG_H
#define GPRINTERCLIENT_DATABASECONNECTIONDIALOG_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class databaseConnectionDialog; }
QT_END_NAMESPACE

class databaseConnectionDialog : public QDialog {
Q_OBJECT

public:
    explicit databaseConnectionDialog(QWidget *parent = nullptr);

    void accept() override;

    void reject() override;

    ~databaseConnectionDialog() override;

signals:
    void dialogFinished(std::string name, std::string password, std::string database, std::string host);
private:
    Ui::databaseConnectionDialog *ui;
};


#endif //GPRINTERCLIENT_DATABASECONNECTIONDIALOG_H
