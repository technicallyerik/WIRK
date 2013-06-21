#include "newserver.h"
#include "ui_newserver.h"
#include "session.h"
#include "server.h"
#include <QMessageBox>

NewServer::NewServer(Session &inSession, QWidget *parent) : QDialog(parent), ui(new Ui::NewServer)
{
    ui->setupUi(this);
    session = &inSession;
}

NewServer::~NewServer()
{
    delete ui;
}

void NewServer::accept()
{
    QString address = ui->server->text();
    int port = ui->port->text().toInt();
    QString realName = ui->realName->text();
    QString nickname = ui->nickname->text();
    QString username = ui->username->text();
    QString password = ui->password->text();
    bool useSSL = ui->useSSL->isChecked();
    Server *newServer = session->addServer(address, port, username, nickname, realName, password, useSSL);
    newServer->openConnection();
    this->close();
}
