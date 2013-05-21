#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMapIterator>
#include "irc_channel.h"

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
    m_servers.append(server);

    // Hook up the sending text box
    connect(ui->sendText, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

    // Setup Tree
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setModel(this->generateTree());
}

MainWindow::~MainWindow()
{    
    delete ui;
}

QStandardItemModel* MainWindow::generateTree() {
    QStandardItemModel *treeModel = new QStandardItemModel();
    for(int i = 0; i < m_servers.count(); i++) {
        irc_server *server = m_servers[i];
        QStandardItem *server_node = new QStandardItem();
        server_node->setText(server->getHost());
        server_node->setData(server);
        treeModel->setItem(i, server_node);
        QMapIterator<QString, irc_channel*> j(server->getChannels());
        while (j.hasNext()) {
            j.next();
            irc_channel *channel = j.value();
            QStandardItem *channel_node = new QStandardItem();
            channel_node->setText(channel->getName());
            channel_node->setData(channel);
            server_node->setChild(0, channel_node);
        }
    }
    return treeModel;
}

void MainWindow::displayMessage(QString message)
{
    // TODO:  Only display text from currently selected tree item
    //        Highlight the item in the tree if it's not currently selected
    ui->mainText->setHtml(message);

    // This scrolls the main text to the bottom
    QTextCursor c = ui->mainText->textCursor();
    c.movePosition(QTextCursor::End);
    ui->mainText->setTextCursor(c);
}

void MainWindow::sendMessage()
{
    m_servers[0]->sendMessage(ui->sendText->text()); // TODO: Send to selected server in tree
    ui->sendText->setText("");
}
