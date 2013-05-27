#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "session.h"
#include "server.h"
#include "channel.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Setup UI
    ui->setupUi(this);

    // Setup servers
    session = new Session(this);
    // TODO:  Get from settings
    session->addServer("irc.freenode.net",
                       7000,
                       "testing1234567",
                       "testing1234567",
                       "Testing",
                       "password",
                       true);

    // Hook up session messages
    connect(session, SIGNAL(messageRecieved(QString,QString,QString)), this, SLOT(handleMessage(QString,QString,QString)));

    // Hook up user interactions
    connect(ui->sendText, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
    connect(ui->treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(treeItemClicked(const QModelIndex&)));

    // Setup Tree
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setModel(session);

    // Set focus on first server
    QModelIndex modelIndex = session->index(0, 0);
    ui->treeView->selectionModel()->select(modelIndex, QItemSelectionModel::ClearAndSelect);

}

MainWindow::~MainWindow()
{    
    delete ui;
}

void MainWindow::handleMessage(QString inServer, QString inChannel, QString inMessage)
{
    QModelIndexList selectedItems = ui->treeView->selectionModel()->selectedIndexes();
    if(selectedItems.count() == 1) {
        QModelIndex selectedItem = selectedItems[0];
        QVariant data = selectedItem.data(Qt::UserRole);
        if(data.canConvert<Channel*>()) {
            Channel *channel = data.value<Channel*>();
            QString channelName = channel->getName();
            Server *server = channel->getServer();
            QString serverName = server->getHost();
            if(serverName.compare(inServer, Qt::CaseInsensitive) == 0 &&
               channelName.compare(inChannel, Qt::CaseInsensitive) == 0) {
                // Channel message with channel selected
                ui->mainText->append(inMessage);
            } else {
                if(inChannel.isEmpty()) {
                    // TODO:  Highlight server
                } else {
                    // TODO:  Highlight channel
                }
            }
        } else if(data.canConvert<Server*>()) {
            Server *server = data.value<Server*>();
            QString serverName = server->getHost();
            if(serverName.compare(inServer, Qt::CaseInsensitive) == 0) {
                // Server message with server selected
                ui->mainText->append(inMessage);
            } else {
                if(inChannel.isEmpty()) {
                    // TODO:  Highlight server
                } else {
                    // TODO:  Highlight channel
                }
            }
        }
        QTextCursor c = ui->mainText->textCursor();
        c.movePosition(QTextCursor::End);
        ui->mainText->setTextCursor(c);
    }
}

void MainWindow::sendMessage()
{
    QModelIndexList selectedItems = ui->treeView->selectionModel()->selectedIndexes();
    QString text = ui->sendText->text();
    if(selectedItems.count() == 1) {
        QModelIndex selectedItem = selectedItems[0];
        QVariant data = selectedItem.data(Qt::UserRole);
        if(data.canConvert<Channel*>()) {
            Channel *channel = data.value<Channel*>();
            Server *server = channel->getServer();
            if(text.at(0) == '/') {
                // User entered command
                server->sendMessage(text);
            } else {
                // User entered channel message
                QString channelName = channel->getName();
                server->sendChannelMessage(channelName, text);
            }
        } else if(data.canConvert<Server*>()) {
            Server *server = data.value<Server*>();
            if(text.at(0) == '/') {
                // User entered command
                server->sendMessage(text);
            } else {
                // User entered channel message without channel selected
                // Do nothing
            }
        }
        ui->sendText->setText("");
    }
}

void MainWindow::treeItemClicked(const QModelIndex& index)
{
    QVariant data = index.data(Qt::UserRole);
    if(data.canConvert<Channel*>()) {
        Channel *channel = data.value<Channel*>();
        this->changeToChannel(channel);
    } else if(data.canConvert<Server*>()) {
        Server *server = data.value<Server*>();
        this->changeToServer(server);
    }
}

void MainWindow::changeToServer(Server *newServer)
{
    ui->mainText->setHtml(newServer->getText());
    ui->userList->setModel(NULL);
}

void MainWindow::changeToChannel(Channel *newChannel)
{
    ui->mainText->setHtml(newChannel->getText());
    QStandardItemModel *users = newChannel->getUsers();
    ui->userList->setModel(users);
}
