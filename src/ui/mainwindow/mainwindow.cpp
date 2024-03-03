//
// Created by csger on 2024. 02. 26..
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include <QFileDialog>
#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "../databaseConnectionDialog/databaseconnectiondialog.h"
#include "../../utils/helpers.h"
#include <QClipboard>
#include <QStringListModel>
#include <QNetworkReply>
#include <QHttpMultiPart>


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow), db(nullptr), manager(new QNetworkAccessManager()) {
    ui->setupUi(this);

    QObject::connect(this->manager, &QNetworkAccessManager::finished,
                     this, [this](QNetworkReply *reply) {
                if (this->isPrinting) {
                    this->sendGCode();
                }
            }
    );
}

MainWindow::~MainWindow() {
    this->gCodeParser.closeFile();
    delete ui;
}

void MainWindow::onPrintButtonClicked() {
    this->sendGCode();
}

void MainWindow::onOpenClicked() {
    auto file = QFileDialog::getOpenFileName(this, tr("Open GCode File"), DEFAULT_GCODE_PATH, tr(
            "gCode (*.gcode)")).toStdString();

    gCodeParser.openFile(file);
    gCodeParser.parseFile();

    file = file.substr(file.find_last_of("/\\") + 1);

    if (this->db) {
        pqxx::work worker(*this->db);
        pqxx::result r = worker.exec_prepared("insert", file.substr(0, file.find_last_of('.')),
                                              this->gCodeParser.getGCodeDump());
        worker.commit();

        this->updateList();
    }

    this->ui->fileName->setText(QString::fromStdString(file));
}

void MainWindow::onCloseClicked() {
    gCodeParser.closeFile();
}

void MainWindow::onParseClicked() {
    auto clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(this->gCodeParser.getGCodeDump()));
}

void MainWindow::onConnectToPrinterClicked() {
    auto dialog = new databaseConnectionDialog(this);
    connect(dialog, &databaseConnectionDialog::dialogFinished, this, &MainWindow::onDatabaseConnectionDialogFinished);
    dialog->show();

}

void MainWindow::onDisconnectFromPrinterClicked() {
    if (this->db) {
        this->db->close();

        auto model = new QStringListModel(this);
        this->ui->listView->setModel(model);

        QStringList items;
        model->setStringList(items);
    }
}

void MainWindow::onDatabaseConnectionDialogFinished(const std::string &name, const std::string &password,
                                                    const std::string &database,
                                                    const std::string &host) {
    if (name.empty() || database.empty() || host.empty()) {
        return;
    }

    std::string connString =
            "postgresql://" + name + (password.empty() ? "" : ":" + password) + "@" + host + "/" + database;
    this->db = std::make_unique<pqxx::connection>(
            "postgresql://" + name + ":" + password + "@" + host + "/" + database);

    this->db->prepare("find", "SELECT gcode FROM prints WHERE id = $1");
    this->db->prepare("insert", "INSERT INTO prints (printname, gcode) VALUES ($1, $2)");

    this->updateList();
}

void MainWindow::onItemSelected(const QModelIndex &index) {
    qDebug() << "Item selected: " << index.data().toString();

    auto data = index.data().toString().toStdString();

    if (this->gCodes.find(data) == this->gCodes.end()) {
        std::vector<std::string> tokens = Helpers::split(data, " ");

        pqxx::work worker(*this->db);
        pqxx::result r = worker.exec_prepared("find", tokens[0]);

        for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
            this->gCodes[data] = c[0].as<std::string>();
            qDebug() << QString::fromStdString(c[0].as<std::string>());
        }
    }

    this->gCodeParser.jsonToGCode(this->gCodes[data]);
}

void MainWindow::updateList() {
    pqxx::work worker2(*this->db);

    auto r = worker2.exec("SELECT id,printname FROM prints");
    auto model = new QStringListModel(this);
    this->ui->listView->setModel(model);

    QStringList items;
    model->setStringList(items);

    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        items << QString::fromStdString(c[0].as<std::string>() + " " + c[1].as<std::string>());
    }

    model->setStringList(items);
}

void MainWindow::sendGCode() {
    this->isPrinting = true;

    auto data = this->gCodeParser.getGCode();

    if (data.empty()) {
        this->isPrinting = false;
        return;
    }

    auto http = new QHttpMultiPart();

    QHttpPart receiptPart;
    receiptPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"data\""));
    receiptPart.setBody(data.c_str());

    http->append(receiptPart);

    this->manager->post(QNetworkRequest(QUrl(ESP_IP "/addCode")), http);
}
