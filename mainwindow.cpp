#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "session.h"
#include "server.h"
#include "channel.h"
#include "user.h"
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
#include <QMessageBox>
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebPage>
#include <QtWebKit/QWebElementCollection>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebHistory>
#include <QtWebKit/QWebHistoryItem>
#include <QtWebKit/QWebSettings>
#include "commandparser.h"
#include "irccommand.h"
#include "preferenceshelper.h"
#include "channelsettings.h"
#include <QFile>
#include <QTextStream>

const QString MainWindow::mainCssFileName = "main.css";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QApplication::setApplicationVersion(VER);

    // Setup UI
    ui->setupUi(this);

    // Configure settings
    readWindowSettings();

    // Setup session
    session = new Session(this);
    session->readFromSettings();
    connect(session, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
    connect(session, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex, int, int)));
    connect(session, SIGNAL(messageReceived(Server*,Channel*,QString,QStringList,Channel::MessageType)), this, SLOT(handleMessage(Server*,Channel*,QString,QStringList,Channel::MessageType)));
    connect(session, SIGNAL(serverDisconnected(Server*)), this, SLOT(showDisconnectedMessage(Server*)));

    // Setup tree view
    connect(ui->treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(treeItemClicked(const QModelIndex&)));
    ui->treeView->setModel(session);
    ui->treeView->setFocusPolicy(Qt::NoFocus);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(generateContextMenu(const QPoint &)));

    // Setup sending text
    connect(ui->sendText, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
    connect(ui->sendText, SIGNAL(channelCycle(bool)), this, SLOT(cycleChannel(bool)));
    ui->sendText->setFocus();

    // Setup user list
    ui->userList->setFocusPolicy(Qt::NoFocus);

    // Setup the main text area
    document = new QTextDocument(ui->mainText);
    document->setDefaultStyleSheet(this->readFile(this->mainCssFileName));
    ui->mainText->setDocument(document);
    connect(ui->mainText, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClicked(QUrl)));

    // Set focus on first server
    QModelIndex modelIndex = session->index(0, 0);
    this->selectItem(modelIndex);

    // Setup command parser
    commandParser = new CommandParser(this);

    // Setup network manager
    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(imageDownloaded(QNetworkReply*)));
    updateCheckerNetworkAccessManager = new QNetworkAccessManager(this);
    connect(updateCheckerNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(versionFileDownloaded(QNetworkReply*)));

    // Setup web view for checking oembed
    webView = new QWebView(this);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    webView->hide();
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(webLoadFinished(bool)));

    // Let's get some styles up in here
    QString controlStyles = "QListView, QTreeView, QLineEdit, QPlainTextEdit, QTextBrowser { background:#333;font-family:\"Lucida Console\",Monaco,monospace;font-size:11px;color:#fff; }";
    ui->userList->setStyleSheet(controlStyles);
    ui->treeView->setStyleSheet(controlStyles);
    ui->mainText->setStyleSheet(controlStyles);
    ui->sendText->setStyleSheet(controlStyles + "QLineEdit { padding:5px; }, QPlainTextEdit { padding:2px; }");
    ui->userList->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->treeView->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->sendText->setAttribute(Qt::WA_MacShowFocusRect, 0);

    // Setup menu items
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(openPreferences()));
    connect(ui->actionNewServer, SIGNAL(triggered()), this, SLOT(newServerWindow()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutInfo()));

    //Check For Updates
    checkForUpdates();
}

MainWindow::~MainWindow()
{    
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    session->writeToSettings();
    saveWindowSettings();
    event->accept();
}

void MainWindow::saveWindowSettings()
{
    QSettings *settings = PreferencesHelper::sharedInstance()->getSettings();
    settings->beginGroup("window");
    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());
    settings->setValue("maximized", isMaximized());
    if(!isMaximized()) {
        settings->setValue("pos", pos());
        settings->setValue("size", size());
    }
    settings->endGroup();
    settings->sync();
}

void MainWindow::readWindowSettings()
{
    QSettings *settings = PreferencesHelper::sharedInstance()->getSettings();
    settings->beginGroup("window");
    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("windowState").toByteArray());
    move(settings->value("pos", pos()).toPoint());
    resize(settings->value("size", size()).toSize());
    if(settings->value("maximized", isMaximized()).toBool()) {
        showMaximized();
    }
    settings->endGroup();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier &&
            (event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab))
    {
        bool cycleDown = event->key() == Qt::Key_Tab;
        cycleChannel(cycleDown);
    }

    ui->sendText->setFocus();
    QMainWindow::keyPressEvent(event);
}

void MainWindow::cycleChannel(bool cycleDown)
{
    QModelIndex selectedChannelIndex = ui->treeView->selectionModel()->currentIndex();
    // going to assume if it's not selected, they're at the first one
    if (!selectedChannelIndex.isValid())
    {
        selectedChannelIndex = ui->treeView->model()->index(0,0);
    }
    QModelIndex nextChannel;

    if (cycleDown)
    {
        nextChannel = ui->treeView->indexBelow(selectedChannelIndex);
        if (!nextChannel.isValid())
        {
            nextChannel = ui->treeView->model()->index(0,0);
        }
    }
    else
    {
        nextChannel = ui->treeView->indexAbove(selectedChannelIndex);
        if (!nextChannel.isValid())
        {
            int lastRowIndex = ui->treeView->model()->rowCount() - 1;
            QModelIndex lastServer = ui->treeView->model()->index(lastRowIndex,0);
            if (lastServer.model()->hasChildren())
            {
                int lastChildRowIndex = lastServer.model()->rowCount(lastServer) - 1;
                nextChannel = lastServer.child(lastChildRowIndex, 0);
            }
            else
            {
                nextChannel = lastServer;
            }
        }
    }

    this->selectItem(nextChannel);
    ui->treeView->setCurrentIndex(nextChannel);
}

void MainWindow::handleMessage(Server *inServer, Channel *inChannel, QString inMessage, QStringList images, Channel::MessageType type)
{
    QRegExp imageUrlRegex("https?:\/\/.*\.(?:jpe?g|gif|png)((?=\\?)\\S*)?");

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
            int imageRegexPosition = imageUrlRegex.indexIn(image);
            if(imageRegexPosition != -1)
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
    QRegExp usernameRX = inServer->getNicknameRegex();
    if(inMessage.contains(usernameRX)) {
        ht = ChannelHighlightTypeMention;
        QApplication::alert(this);
    } else {
        ht = ChannelHighlightTypeNew;
    }

    // Determine if we should scroll once the text is appended
    QScrollBar *verticalBar = ui->mainText->verticalScrollBar();
    bool saveUserPosition = verticalBar->value() != verticalBar->maximum();

    // Determine if the text should be added to the text area or if
    // a tree view item should be highlighted
    QModelIndexList selectedItems = ui->treeView->selectionModel()->selectedIndexes();
    if(selectedItems.count() == 1) {
        QModelIndex selectedItem = selectedItems[0];
        QVariant data = selectedItem.data(Qt::UserRole);
        if(data.canConvert<Channel*>()) {
            Channel *selectedChannel = data.value<Channel*>();
            Server *selectedServer = selectedChannel->getServer();
            if(selectedServer == inServer &&
               inChannel != NULL && selectedChannel == inChannel) {
                // Channel message with channel selected
                ui->mainText->append(inMessage);
                if(!saveUserPosition) {
                    scrollToBottom();
                }
            } else {
                if(inChannel == NULL) {
                    highlightServer(inServer, ht);
                } else {
                    highlightChannel(inChannel, ht, type);
                }
            }
        } else if(data.canConvert<Server*>()) {
            Server *selectedServer = data.value<Server*>();
            if(selectedServer == inServer && inChannel == NULL) {
                // Server message with server selected
                ui->mainText->append(inMessage);
                if(!saveUserPosition) {
                    scrollToBottom();
                }
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
    if(type != Channel::MessageTypeInfo) {
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
    QString text = ui->sendText->toPlainText();
    if(selectedItems.count() == 1 && text.length() > 0) {
        QModelIndex selectedItem = selectedItems[0];
        QVariant data = selectedItem.data(Qt::UserRole);
        if(data.canConvert<Channel*>()) {
            Channel *channel = data.value<Channel*>();
            Server *server = channel->getServer();
            if(text.at(0) == '/') {
                // User entered command
                IrcCommand *command = commandParser->parse(text, server, channel);
                server->sendCommand(command);
            } else {
                // User entered channel message
                QString channelName = channel->getName();
                QStringList messages = text.split("\n", QString::SkipEmptyParts);
                foreach (QString message, messages)
                {
                    do
                    {
                        QString messageChunk = message.left(MaxMessageLength);
                        channel->sendMessage(messageChunk);
                        message = message.mid(messageChunk.length());
                    } while (message != NULL);
                }
            }
        } else if(data.canConvert<Server*>()) {
            Server *server = data.value<Server*>();
            if(text.at(0) == '/') {
                // User entered command
                IrcCommand *command = commandParser->parse(text, server, NULL);
                server->sendCommand(command);
            } else {
                // User entered channel message without channel selected
                // Do nothing
            }
        }
        ui->sendText->setPlainText("");
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

void MainWindow::rowsInserted(const QModelIndex &modelIndex, int start, int end)
{
    QModelIndex newItemIndex = modelIndex.child(start, 0);
    this->selectItem(newItemIndex);
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

        QSignalMapper *channelMapper = new QSignalMapper(this);
        QAction *editChannelSettingsAction = menu.addAction("Settings");
        channelMapper->setMapping(editChannelSettingsAction, QString("%1/%2").arg(channel->getServer()->getHost(),channel->getName()));
        connect(editChannelSettingsAction, SIGNAL(triggered()), channelMapper, SLOT(map()));
        connect(channelMapper, SIGNAL(mapped(const QString &)), this, SLOT(openChannelSettings(const QString &)));

    } else if(data.canConvert<Server*>()) {

        Server *server = data.value<Server*>();

        if(server->getIsConnected()) {
            menu.addAction("Disconnect", server, SLOT(closeConnection()));
        } else {
            menu.addAction("Connect", server, SLOT(openConnection()));

            QAction *removeAction = menu.addAction("Remove");
            QSignalMapper *removeMapper = new QSignalMapper(this);
            removeMapper->setMapping(removeAction, server);
            connect(removeAction, SIGNAL(triggered()), removeMapper, SLOT(map()));
            connect(removeMapper, SIGNAL(mapped(QObject *)), session, SLOT(removeServer(QObject *)));
        }

    } else {

        menu.addAction("Add Server...", this, SLOT(newServerWindow()));

    }

    menu.exec(ui->treeView->viewport()->mapToGlobal(point));
}

void MainWindow::scrollToBottom()
{
    // First move cursor to the bottom because it's a smoother animation
    QTextCursor c = ui->mainText->textCursor();
    c.movePosition(QTextCursor::End);
    ui->mainText->setTextCursor(c);
    // Set the vertical scroll bar to the max because cursor position doesn't quite get us there
    QScrollBar *verticalBar = ui->mainText->verticalScrollBar();
    verticalBar->setValue(verticalBar->maximum());
}

void MainWindow::changeToServer(Server *newServer)
{
    ui->mainText->setHtml(newServer->getLatestText());
    ui->userList->setModel(NULL);
    highlightServer(newServer, ChannelHighlightTypeNone);
    scrollToBottom();
    ui->sendText->setFocus();
}

void MainWindow::changeToChannel(Channel *newChannel)
{
    ui->mainText->setHtml(newChannel->getLatestText());
    QStandardItemModel *users = newChannel->getUsers();

    // Refreshing name colors in case the preference changed
    // ***Do not add back until performance problems are addressed
    //    for (int i = 0; i < users->rowCount(); i++)
    //    {
    //        QStandardItem *row = users->item(i);
    //        QVariant data = row->data(Qt::UserRole);
    //        if (data.canConvert<User*>())
    //        {
    //            User *userItem = data.value<User*>();
    //            userItem->refreshUserDisplay();
    //        }
    //    }

    ui->userList->setModel(users);
    highlightChannel(newChannel, ChannelHighlightTypeNone, Channel::MessageTypeDefault);
    scrollToBottom();
    ui->sendText->setFocus();
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
        QString name = metaTag.attribute("name");
        QString content = metaTag.attribute("content");
        if (property.compare("og:image", Qt::CaseInsensitive) == 0 ||
            name.compare("twitter:image", Qt::CaseInsensitive) == 0)
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

    QBuffer buffer(&bytes);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer);
    bool imageIsAnimated = reader.supportsAnimation() && reader.imageCount() > 1;

    QImage image = reader.read();

    int imageWidth = image.width();
    int imageHeight = image.height();
    int maxWidth = 400;
    int newHeight = qMin(150, imageHeight);
    double resizeRatio = (double)newHeight / (double)imageHeight;
    if((resizeRatio * imageWidth) > maxWidth) {
        newHeight = ((double)maxWidth / (double)imageWidth) * (double)imageHeight;
    }
    image = image.scaledToHeight(newHeight, Qt::SmoothTransformation);

    if(imageIsAnimated) {
        AnimationViewModel *avm = new AnimationViewModel(bytes, mappedUrl, document, this);
        connect(avm, SIGNAL(movieChanged(QPixmap, QUrl)), this, SLOT(movieChanged(QPixmap, QUrl)), Qt::QueuedConnection);
        avm->start();
        animations.append(avm);
    }

    QScrollBar *verticalBar = ui->mainText->verticalScrollBar();
    bool saveUserPosition = verticalBar->value() != verticalBar->maximum();
    document->addResource(QTextDocument::ImageResource, mappedUrl, image);
    document->markContentsDirty(0, document->characterCount());
    if(!saveUserPosition) {
        scrollToBottom();
    }
}

void MainWindow::movieChanged(QPixmap pixels, QUrl url)
{
    document->addResource(QTextDocument::ImageResource, url, pixels);

    if(QApplication::activeWindow() != 0) {
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
                    qDebug() << QTime::currentTime().toString() << " " << url.toString();
                }
            }
        }
    }
}

void MainWindow::anchorClicked(QUrl url)
{
    if(url.toString().startsWith("channel:")) {
        QRegExp channelNameRegExp("channel:(.*)");
        int pos = channelNameRegExp.indexIn(url.toString());
        if(pos != -1) {
            Server *server = getCurrentServer();
            QString channelName = channelNameRegExp.cap(1);
            Channel* channel = server->getChannel(channelName);
            if(channel == NULL) {
                IrcCommand *command = IrcCommand::createJoin(channelName, NULL);
                server->sendCommand(command);
            } else {
                QStandardItem *channelMenuItem = channel->getMenuItem();
                QModelIndex channelIndex = channelMenuItem->index();
                this->selectItem(channelIndex);
            }
        }
    } else {
        if(!url.toString().startsWith("http", Qt::CaseInsensitive)) {
            url.setUrl("http://" + url.toString());
        }
        QDesktopServices::openUrl(url);
    }
}

void MainWindow::openPreferences()
{
    Preferences dialog(this);
    dialog.exec();
}

///
/// \brief MainWindow::openChannelSettings
/// \param serverChannel expecting format "server/channel"
///
void MainWindow::openChannelSettings(QString serverChannel)
{
    QStringList settingParts = serverChannel.split("/");
    QString channel = settingParts.at(1);
    QString server = settingParts.at(0);
    ChannelSettings dialog(channel, server, this);
    dialog.exec();
}

void MainWindow::newServerWindow()
{
    NewServer dialog(*session, this);
    dialog.exec();
}

void MainWindow::showAboutInfo()
{
    QMessageBox msgBox;
    msgBox.setText("About");
    msgBox.setInformativeText(QString("Version: %1").arg(QApplication::applicationVersion()));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

Server* MainWindow::getCurrentServer()
{
    QModelIndexList selectedItems = ui->treeView->selectionModel()->selectedIndexes();
    if(selectedItems.count() == 1) {
        QModelIndex selectedItem = selectedItems[0];
        QVariant data = selectedItem.data(Qt::UserRole);
        if(data.canConvert<Channel*>()) {
            Channel *selectedChannel = data.value<Channel*>();
            QString selectedChannelName = selectedChannel->getName();
            return selectedChannel->getServer();
        } else if(data.canConvert<Server*>()) {
            return data.value<Server*>();
        }
    }
    return NULL;
}

void MainWindow::showDisconnectedMessage(Server *server)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Connection Timeout");
    msgBox.setText(QString("%1 has timed out.").arg(server->getHost()));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::checkForUpdates()
{
    QUrl versionFileUrl(QString("http://technicallyerik.github.io/WIRK/version.txt"));
    QNetworkRequest request(versionFileUrl);

    updateCheckerNetworkAccessManager->get(request);
}

void MainWindow::versionFileDownloaded(QNetworkReply *networkReply)
{
    // now use pMyReply to get the data, which is a subclass of IODevice!
    // don't know if the device is already open, if not, use ->open()
    // then read
    QByteArray byteData = networkReply->readAll();
    // and convert to string!
    // be careful, take care of encoding here:
    QTextStream versionFile(byteData);
    QString latestVersionNumber = versionFile.readLine();

    QString versionPrefix("WIRK");
    // This verifies we're getting the actual number and not some sort of 404 page
    if (!latestVersionNumber.startsWith(versionPrefix))
    {
        return;
    }
    latestVersionNumber = latestVersionNumber.mid(versionPrefix.length());

    bool updateAvailable = false;

    //compare version numbers
    if (latestVersionNumber != QApplication::applicationVersion())
    {
        QStringList latestVersionList(latestVersionNumber.split('.'));
        QStringList applicationVersionList(QApplication::applicationVersion().split('.'));
        for (int i = 0; i < latestVersionList.length(); i++)
        {
            bool *versionNumberIsNumber = new bool;
            bool *applicationNumberIsNumber = new bool;

            int latestNumber = latestVersionList[i].toInt(versionNumberIsNumber);
            int applicationNumber = applicationVersionList[i].toInt(applicationNumberIsNumber);

            if (versionNumberIsNumber && applicationNumberIsNumber)
            {
                if (latestNumber > applicationNumber)
                {
                    updateAvailable = true;
                    delete versionNumberIsNumber;
                    delete applicationNumberIsNumber;
                    break;
                }
            }
            delete versionNumberIsNumber;
            delete applicationNumberIsNumber;
        }
    }

    //show updateBox
    if (updateAvailable)
    {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Update Available");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText("A new version is available.<br><a href=\"http://technicallyerik.github.io/WIRK/\">Update Here</a>");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }

    // and clean up
    delete networkReply;
}

QString MainWindow::readFile(QString fileName)
{
    QString path = QString("%1/%2").arg(QCoreApplication::applicationDirPath(),fileName);
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    QTextStream stream(&file);
    return stream.readAll().simplified();
}
