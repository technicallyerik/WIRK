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
#include <QSignalMapper>
#include <QAction>
#include "newserver.h"
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebPage>
#include <QtWebKit/QWebElementCollection>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebHistory>
#include <QtWebKit/QWebHistoryItem>
#include <QtWebKit/QWebSettings>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Setup UI
    ui->setupUi(this);

    // Setup servers
    session = new Session(this);
    session->readFromSettings();
    connect(session, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));

    // Hook up session messages
    connect(session, SIGNAL(messageReceived(Server*,Channel*,QString,QStringList,Channel::MessageType)), this, SLOT(handleMessage(Server*,Channel*,QString,QStringList,Channel::MessageType)));
    connect(session, SIGNAL(selectItem(QModelIndex)), this, SLOT(selectItem(QModelIndex)));

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
    this->selectItem(modelIndex);

    // Setup network manager
    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(imageDownloaded(QNetworkReply*)));

    // Setup web view for checking oembed
    webView = new QWebView(this);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    webView->hide();
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(webLoadFinished(bool)));

    // Let's get some styles up in here
    QString controlStyles = "background:#333;font-family:\"Lucida Console\",Monaco,monospace;font-size:11px;color:#fff;";
    ui->userList->setStyleSheet(controlStyles);
    ui->treeView->setStyleSheet(controlStyles);
    ui->mainText->setStyleSheet(controlStyles);
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
    connect(ui->actionNewServer, SIGNAL(triggered()), this, SLOT(newServerWindow()));

    // Enable right-click on the tree
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(generateContextMenu(const QPoint &)));
}

MainWindow::~MainWindow()
{    
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    session->writeToSettings();
    event->accept();
}

void MainWindow::handleMessage(Server *inServer, Channel *inChannel, QString inMessage, QStringList images, Channel::MessageType type)
{
    // Get images for the links
    foreach(QString image, images)
    {
        QVariant existingResource = document->resource(QTextDocument::ImageResource, image);
        if(existingResource.isNull()) {
            QImage emptyImage(1, 1, QImage::Format_ARGB32);
            emptyImage.fill(Qt::transparent);
            document->addResource(QTextDocument::ImageResource, image, emptyImage); // placeholder
            QUrl url(image);
            QNetworkRequest request(url);
            if(image.endsWith(".gif") || image.endsWith(".jpg") || image.endsWith(".png"))
            {
                // Start downloading the image
                imagePageMap.insert(image, image);
                networkAccessManager->get(request);
            } else {
                // Parse the image markup for an open graph image
                webView->load(request);
            }
        }
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
    QStandardItem* menuItem = server->getMenuItem();
    highlightMenuItem(menuItem, highlight);
}

void MainWindow::highlightChannel(Channel *channel, ChannelHighlightType highlight, Channel::MessageType type)
{
    if(type != Channel::Info) {
        QStandardItem* menuItem = channel->getMenuItem();
        highlightMenuItem(menuItem, highlight);
    }
}

void MainWindow::highlightMenuItem(QStandardItem *menuItem, ChannelHighlightType highlight)
{
    QVariant data = menuItem->data(HighlightType);
    int currentHighlight = data.value<int>();
    int enumValue = highlight;
    if(enumValue == 0 || enumValue > currentHighlight) {
        QBrush color;
        switch(highlight) {
            case ChannelHighlightTypeMention:
                color = QBrush((QColor(41,167,33)));
                break;
            case ChannelHighlightTypeNew:
                color = QBrush((QColor(177,44,51)));
                break;
            case ChannelHighlightTypeNone:
            default:
                color = QBrush((QColor(0,0,0)), Qt::NoBrush);
        }

        menuItem->setBackground(color);
        menuItem->setData(highlight, HighlightType);
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

void MainWindow::selectItem(QModelIndex index)
{
    ui->treeView->expandAll();
    ui->treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    this->treeItemClicked(index);
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
    } else {
        ui->mainText->setHtml("");
        ui->userList->setModel(NULL);
    }
}

void MainWindow::rowsRemoved(const QModelIndex &modelIndex, int start, int end)
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    this->selectItem(index);
}

void MainWindow::generateContextMenu(const QPoint &point)
{
    QModelIndex modelIndex = ui->treeView->indexAt(point);
    QVariant data = modelIndex.data(Qt::UserRole);
    QMenu menu;

    if(data.canConvert<Channel*>()) {

        Channel *channel = data.value<Channel*>();
        Server *server = channel->getServer();

        if(channel->getIsJoined() && channel->getType() == Channel::ChannelTypeNormal) {
            menu.addAction("Part", channel, SLOT(part()));
        }

        if(!channel->getIsJoined() && channel->getType() == Channel::ChannelTypeNormal) {
                menu.addAction("Join", channel, SLOT(join()));
        }

        if(!channel->getIsJoined() || channel->getType() == Channel::ChannelTypeUser) {
            QAction *removeAction = menu.addAction("Remove");
            QSignalMapper *removeMapper = new QSignalMapper(this);
            removeMapper->setMapping(removeAction, channel->getName());
            connect(removeAction, SIGNAL(triggered()), removeMapper, SLOT(map()));
            connect(removeMapper, SIGNAL(mapped(const QString &)), server, SLOT(removeChannel(const QString &)));
        }

    } else if(data.canConvert<Server*>()) {

        Server *server = data.value<Server*>();

        if(server->getIsConnected()) {
            menu.addAction("Disconnect", server, SLOT(closeConnection()));
        } else {
            menu.addAction("Connect", server, SLOT(openConnection()));

            QAction *removeAction = menu.addAction("Remove");
            QSignalMapper *removeMapper = new QSignalMapper(this);
            removeMapper->setMapping(removeAction, server->getHost());
            connect(removeAction, SIGNAL(triggered()), removeMapper, SLOT(map()));
            connect(removeMapper, SIGNAL(mapped(const QString &)), session, SLOT(removeServer(const QString &)));
        }

    } else {

        menu.addAction("Add Server...", this, SLOT(newServerWindow()));

    }

    menu.exec(ui->treeView->viewport()->mapToGlobal(point));
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

    ui->sendText->setChannel(*newChannel);
}

void MainWindow::webLoadFinished(bool ok)
{
    webView->triggerPageAction(QWebPage::Stop); // stop loading sub frames
    webView->triggerPageAction(QWebPage::StopScheduledPageRefresh);

    QUrl requestUrl = webView->url();
    QString requestUrlString = requestUrl.toString();

    QWebPage *page = webView->page();
    QWebFrame *mainFrame = page->mainFrame();
    QWebElementCollection metaTags = mainFrame->findAllElements("meta");

    foreach (QWebElement metaTag, metaTags)
    {
        QString property = metaTag.attribute("property");
        QString content = metaTag.attribute("content");
        if (property.compare("og:image", Qt::CaseInsensitive) == 0)
        {
            imagePageMap.insert(content, requestUrlString);
            QUrl url(content);
            QNetworkRequest request(url);
            networkAccessManager->get(request);
            return;
        }
    }
}

void MainWindow::imageDownloaded(QNetworkReply* networkReply)
{
    QByteArray bytes = networkReply->readAll();
    QUrl url = networkReply->url();
    QUrl mappedUrl = QUrl(imagePageMap.value(url.toString()));
    QImage image;
    image.loadFromData(bytes);
    image = image.scaledToHeight(150, Qt::SmoothTransformation);

    if(url.toString().endsWith(".gif")) {
        AnimationViewModel *avm = new AnimationViewModel(bytes, mappedUrl, document, this);
        connect(avm, SIGNAL(movieChanged(QPixmap, QUrl)), this, SLOT(movieChanged(QPixmap, QUrl)), Qt::QueuedConnection);
        avm->start();
        animations.append(avm);
    }

    document->addResource(QTextDocument::ImageResource, mappedUrl, image);
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

void MainWindow::newServerWindow()
{
    NewServer dialog(*session, this);
    dialog.exec();
}
