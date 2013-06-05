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
#include <QStandardItem>
#include <QTimer>
#include "preferences.h"
#include <QDebug>
#include <QThread>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <QScrollBar>

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
    connect(session, SIGNAL(messageReceived(Server*,Channel*,QString,Channel::MessageType)), this, SLOT(handleMessage(Server*,Channel*,QString,Channel::MessageType)));

    // Hook up user interactions
    connect(ui->sendText, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
    connect(ui->treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(treeItemClicked(const QModelIndex&)));

    // Setup Tree
    ui->treeView->setModel(session);

    // Setup the main text area
    document = new QTextDocument(ui->mainText);
    // TODO: create a function to return this as a string from the actual CSS file instead of putting in a minified string
    document->setDefaultStyleSheet("body{background:#333;margin:0}table{font-family:\"Lucida Console\",Monaco,monospace;font-size:11px;line-height:1.25;margin:0}th,td{padding:2px 10px;vertical-align:top;color:#fff}h1,h2,h3,h4,h5,h6{margin:0}p{margin:0}a{color:#ddd;text-decoration:underline}.user{color:#aaa;font-size:11px;font-weight:bold}.metainfo{color:#999;font-size:8px}.col-name{text-align:right}.col-meta{padding-top:4px}.msg-mentioned{background:#736500}.msg-mentioned .user{color:#ddd}.msg-mentioned .message{color:#ffe100}.msg-mentioned a{color:#d6bd00}.msg-info .message{font-style:italic;color:#999}.msg-topic{background:#555}.msg-topic .user{color:#fff}.msg-topic .message{font-style:italic}.msg-emote{background:#73005e}.msg-emote .message{font-style:italic;color:#ff00d1}.msg-emote a{color:#cc00a7}");
    ui->mainText->setDocument(document);
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

    // Let's get some styles up in here
    QString controlStyles = "background:#333;font-family:\"Lucida Console\",Monaco,monospace;font-size:11px;color:#fff;";
    ui->userList->setStyleSheet(controlStyles);
    ui->treeView->setStyleSheet(controlStyles);
    ui->sendText->setStyleSheet(controlStyles + "padding:5px;");
    ui->userList->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->treeView->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->sendText->setAttribute(Qt::WA_MacShowFocusRect, 0);

    // Setup redraw timer
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(refreshImages()));
    timer->start(150);

    // Setup menu items
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(openPreferences()));
}

MainWindow::~MainWindow()
{    
    delete ui;
}

void MainWindow::handleMessage(Server *inServer, Channel *inChannel, QString inMessage, Channel::MessageType type)
{
    // Download any image assets included in the message
    QRegExp imageRegex(".*(src=\"(([^>]+)\\.(jpg|png|gif))\").*");
    int pos = 0;
    while ((pos = imageRegex.indexIn(inMessage, pos)) != -1) {
        QString urlString = imageRegex.cap(2);
        QVariant existingResource = document->resource(QTextDocument::ImageResource, urlString);
        if(existingResource.isNull()) {
            QUrl url(urlString);
            QNetworkRequest request(url);
            networkAccessManager->get(request);
        }
        pos += imageRegex.matchedLength();
    }

    // Determine highlight type if this server/channel isn't enabled
    ChannelHighlightType ht;
    if(inMessage.contains(inServer->getUsername(), Qt::CaseInsensitive)) {
        ht = ChannelHighlightTypeMention;
    } else {
        ht = ChannelHighlightTypeNew;
    }

    // Determine if the text should be added to the text area or if
    // a tree view item should be highlighted
    QModelIndexList selectedItems = ui->treeView->selectionModel()->selectedIndexes();
    if(selectedItems.count() == 1) {
        QModelIndex selectedItem = selectedItems[0];
        QVariant data = selectedItem.data(Qt::UserRole);
        if(data.canConvert<Channel*>()) {
            Channel *selectedChannel = data.value<Channel*>();
            QString selectedChannelName = selectedChannel->getName();
            Server *selectedServer = selectedChannel->getServer();
            QString selectedServerName = selectedServer->getHost();
            if(selectedServerName.compare(inServer->getHost(), Qt::CaseInsensitive) == 0 &&
               inChannel != NULL &&
               selectedChannelName.compare(inChannel->getName(), Qt::CaseInsensitive) == 0) {
                // Channel message with channel selected
                ui->mainText->append(inMessage);
                scrollToBottom();
            } else {
                if(inChannel == NULL) {
                    highlightServer(inServer, ht);
                } else {
                    highlightChannel(inChannel, ht, type);
                }
            }
        } else if(data.canConvert<Server*>()) {
            Server *selectedServer = data.value<Server*>();
            QString selectedServerName = selectedServer->getHost();
            if(selectedServerName.compare(inServer->getHost(), Qt::CaseInsensitive) == 0 &&
               inChannel == NULL) {
                // Server message with server selected
                ui->mainText->append(inMessage);
                scrollToBottom();
            } else {
                if(inChannel == NULL) {
                    highlightServer(inServer, ht);
                } else {
                    highlightChannel(inChannel, ht, type);
                }
            }
        }
    }
}

void MainWindow::highlightServer(Server *server, ChannelHighlightType highlight)
{
    QBrush color = getColorForHighlightType(highlight);
    QStandardItem* menuItem = server->getMenuItem();
    menuItem->setBackground(color);
}

void MainWindow::highlightChannel(Channel *channel, ChannelHighlightType highlight, Channel::MessageType type)
{
    if(type != Channel::Info) {
        QBrush color = getColorForHighlightType(highlight);
        QStandardItem* menuItem = channel->getMenuItem();
        menuItem->setBackground(color);
    }
}

QBrush MainWindow::getColorForHighlightType(ChannelHighlightType ht)
{
    switch(ht) {
        case ChannelHighlightTypeMention:
            return QBrush((QColor(41,167,33)));
        case ChannelHighlightTypeNew:
            return QBrush((QColor(177,44,51)));
        case ChannelHighlightTypeNone:
        default:
            return QBrush((QColor(0,0,0)), Qt::NoBrush);
    }
}

void MainWindow::sendMessage()
{
    QModelIndexList selectedItems = ui->treeView->selectionModel()->selectedIndexes();
    QString text = ui->sendText->text();
    if(selectedItems.count() == 1 && text.length() > 0) {
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
    highlightServer(newServer, ChannelHighlightTypeNone);
    scrollToBottom();
}

void MainWindow::changeToChannel(Channel *newChannel)
{
    ui->mainText->setHtml(newChannel->getText());
    QStandardItemModel *users = newChannel->getUsers();
    ui->userList->setModel(users);
    highlightChannel(newChannel, ChannelHighlightTypeNone, Channel::Default);
    scrollToBottom();
}

void MainWindow::imageDownloaded(QNetworkReply* networkReply)
{
    QByteArray bytes = networkReply->readAll();
    QUrl url = networkReply->url();
    QImage image;
    image.loadFromData(bytes);
    image = image.scaledToHeight(150, Qt::SmoothTransformation);

    if(networkReply->url().toString().endsWith(".gif")) {
        AnimationViewModel *avm = new AnimationViewModel(bytes, url, document, this);
        connect(avm, SIGNAL(movieChanged(QPixmap, QUrl)), this, SLOT(movieChanged(QPixmap, QUrl)), Qt::QueuedConnection);
        avm->start();
        animations.append(avm);
    }

    document->addResource(QTextDocument::ImageResource, url, image);
    document->markContentsDirty(0, document->characterCount());
    scrollToBottom();
}

void MainWindow::refreshImages()
{
    // I think it's more efficient to change the block of the movie
    // that animated instead of consistently refresh the whole document.
    // See:  movieChanged()
    //document->markContentsDirty(0, document->characterCount());
}

void MainWindow::movieChanged(QPixmap pixels, QUrl url)
{
    document->addResource(QTextDocument::ImageResource, url, pixels);

    // Find the location of the URL in the text
    QTextCursor cursor = document->find(url.toString());
    int cursorPosition = cursor.position();
    if(cursorPosition > -1) {
        // Find the location and size of the block containing the url
        QTextBlock block = document->findBlock(cursorPosition);
        int blockStart = block.position();
        int blockLength = block.length();
        int blockEnd = blockStart + blockLength;
        if(blockStart > -1 && blockLength > 0) {
            // Make sure the image is scrolled into view
            QRect viewport = ui->mainText->viewport()->visibleRegion().boundingRect();
            QPoint scrollStartPoint = viewport.topLeft();
            QPoint scrollEndPoint = viewport.bottomRight();
            int scrollStartCursorPosition = ui->mainText->cursorForPosition(scrollStartPoint).position();
            int scrollEndCursorPosition = ui->mainText->cursorForPosition(scrollEndPoint).position();
            if((blockEnd > scrollStartCursorPosition) &&
               (blockStart < scrollEndCursorPosition)) {
                document->markContentsDirty(blockStart, blockLength);
            }
        }
    }
}

void MainWindow::anchorClicked(QUrl url)
{
    if(!url.toString().startsWith("http://", Qt::CaseInsensitive)) {
        url.setUrl("http://" + url.toString());
    }
    QDesktopServices::openUrl(url);
}

void MainWindow::openPreferences()
{
    Preferences dialog(this);
    dialog.exec();
}
