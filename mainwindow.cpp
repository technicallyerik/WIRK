#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "irc_server.h"
#include <QStandardItemModel>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO:  Get this from settings
    irc_server *server = new irc_server(this);
    server->setHost("chat.freenode.net");
    server->setPort(7070);
    server->setUsername("testing1234567");
    server->setNickname("testing1234567");
    server->setRealname("Testing 1234567");
    connect(server, SIGNAL(textChanged(QString)), this, SLOT(displayMessage(QString)));
    server->setSSL(true);
    server->createConnection();

    // Hook up the sending text box
    connect(ui->sendText, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

    // Setup Tree
    // TODO: Generate this from list of irc_servers
    QStandardItemModel *treeModel = new QStandardItemModel();
    QStandardItem *server_node = new QStandardItem();
    server_node->setText("Test Server");
    treeModel->setItem(0, server_node);
    QStandardItem *channel_node = new QStandardItem();
    channel_node->setText("#testchannel");
    server_node->setChild(0, channel_node);
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setModel(treeModel);
}

MainWindow::~MainWindow()
{    
    delete ui;
}

void MainWindow::displayMessage(QString message)
{
    // TODO:  Only display text from currently selected tree item
    ui->mainText->setText(message);
}


void MainWindow::sendMessage()
{
    qDebug() << "User wants to send: " + ui->sendText->text();
}
