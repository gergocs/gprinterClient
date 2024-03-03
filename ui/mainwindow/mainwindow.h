//
// Created by csger on 2024. 02. 26..
//

#ifndef GPRINTERCLIENT_MAINWINDOW_H
#define GPRINTERCLIENT_MAINWINDOW_H

#include <QMainWindow>
#include "../../gCode/GCodeParser.h"
#include <pqxx/pqxx>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define ESP_IP "192.168.0.0"

class MainWindow : public QMainWindow {
Q_OBJECT

public slots:

    void onParseClicked();

    void onPrintButtonClicked();

    void onOpenClicked();

    void onCloseClicked();

    void onConnectToPrinterClicked();

    void onDisconnectFromPrinterClicked();

    void onDatabaseConnectionDialogFinished(const std::string &name, const std::string &password,
                                            const std::string &database, const std::string &host);

    void onItemSelected(const QModelIndex &index);

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    void updateList();

    void sendGCode();

    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    std::unique_ptr<pqxx::connection> db;
    GCodeParser gCodeParser = GCodeParser();
    std::unordered_map<std::string, std::string> gCodes;
    bool isPrinting = false;
};


#endif //GPRINTERCLIENT_MAINWINDOW_H
