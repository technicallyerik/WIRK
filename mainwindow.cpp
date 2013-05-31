#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "session.h"
#include "server.h"
#include "channel.h"
#include "animationviewmodel.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QMovie>
#include <QBuffer>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Setup UI
    ui->setupUi(this);

    // Setup servers
    session = new Session(this);
    // TODO:  Get from settings
    session->addServer("irc.freenode.net",  // host
                       7000,                // port
                       "wirktest1234",       // username
                       "wirktest1234",       // nickname
                       "WIRK Test",         // real name
                       "",                  // password
                       true);               // is ssl

    // Hook up session messages
    connect(session, SIGNAL(messageReceived(QString,QString,QString)), this, SLOT(handleMessage(QString,QString,QString)));

    // Hook up user interactions
    connect(ui->sendText, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
    connect(ui->treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(treeItemClicked(const QModelIndex&)));

    // Setup Tree
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setModel(session);

    // Setup the main text area
    document = new QTextDocument(ui->mainText);
    // TODO: create a function to return this as a string from the actual CSS file instead of putting in a minified string
    document->setDefaultStyleSheet("body{background:#333;margin:0}table{font-family:\"Lucida Console\",Monaco,monospace;font-size:11px;line-height:1.25;margin:0}th,td{padding:2px 10px;vertical-align:top;color:#fff}h1,h2,h3,h4,h5,h6{margin:0}p{margin:0}a{color:#ddd;text-decoration:underline}.user{color:#aaa;font-size:11px;font-weight:bold}.metainfo{color:#999;font-size:8px}.col-name{text-align:right}.col-meta{padding-top:4px}.msg-mentioned{background:#736500}.msg-mentioned .user{color:#ddd}.msg-mentioned .message{color:#ffe100}.msg-mentioned a{color:#d6bd00}.msg-info .message{font-style:italic;color:#999}.msg-topic{background:#555}.msg-topic .user{color:#fff}.msg-topic .message{font-style:italic}.msg-emote{background:#73005e}.msg-emote .message{font-style:italic;color:#ff00d1}.msg-emote a{color:#cc00a7}");
    ui->mainText->setDocument(document);
    ui->mainText->setOpenLinks(false);
    connect(ui->mainText, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClicked(QUrl)));

    // Set focus on first server
    QModelIndex modelIndex = session->index(0, 0);
    ui->treeView->selectionModel()->select(modelIndex, QItemSelectionModel::ClearAndSelect);
    QVariant data = modelIndex.data(Qt::UserRole);
    Server *server = data.value<Server*>();
    this->changeToServer(server);

    // Setup network manager
    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(imageDownloaded(QNetworkReply*)));
}

MainWindow::~MainWindow()
{    
    delete ui;
}

void MainWindow::handleMessage(QString inServer, QString inChannel, QString inMessage)
{
    // Download any image assets included in the message
    QRegExp imageRegex(".*(src=\"(([^>]+)\\.(jpg|png|gif))\").*");
    int pos = 0;
    while ((pos = imageRegex.indexIn(inMessage, pos)) != -1) {
        QUrl url = QUrl(imageRegex.cap(2));
        QNetworkRequest request = QNetworkRequest(url);
        networkAccessManager->get(request);
        pos += imageRegex.matchedLength();
    }

    // Determine if the text should be added to the text area or if
    // a tree view item should be highlighted
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
            if(serverName.compare(inServer, Qt::CaseInsensitive) == 0 &&
               inChannel.isEmpty()) {
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

void MainWindow::scrollToBottom()
{
    QTextCursor c = ui->mainText->textCursor();
    c.movePosition(QTextCursor::End);
    ui->mainText->setTextCursor(c);
}

void MainWindow::changeToServer(Server *newServer)
{
    ui->mainText->setHtml(newServer->getText());
    ui->userList->setModel(NULL);
    scrollToBottom();
}

void MainWindow::changeToChannel(Channel *newChannel)
{
    ui->mainText->setHtml(newChannel->getText());
    QStandardItemModel *users = newChannel->getUsers();
    ui->userList->setModel(users);
    scrollToBottom();
}

void MainWindow::imageDownloaded(QNetworkReply* networkReply)
{
    QByteArray bytes = networkReply->readAll();
    QUrl url = networkReply->url();

    if(networkReply->url().toString().endsWith(".gif")) {
        AnimationViewModel *avm = new AnimationViewModel(bytes, url, this);
        connect(avm, SIGNAL(movieAnimated(QPixmap, QUrl)), this, SLOT(movieAnimated(QPixmap, QUrl)));
        animations.append(avm);
    }

    document->addResource(QTextDocument::ImageResource, url, bytes);
    ui->mainText->setLineWrapColumnOrWidth(ui->mainText->lineWrapColumnOrWidth()); // Hack to get the image to redraw
    scrollToBottom();
}

void MainWindow::movieAnimated(QPixmap pixels, QUrl url)
{
    document->addResource(QTextDocument::ImageResource, url, pixels);
    ui->mainText->setLineWrapColumnOrWidth(ui->mainText->lineWrapColumnOrWidth()); // Hack to get the image to redraw
}

void MainWindow::anchorClicked(QUrl url)
{
    if(!url.toString().startsWith("http://", Qt::CaseInsensitive)) {
        url.setUrl("http://" + url.toString());
    }
    QDesktopServices::openUrl(url);
}
