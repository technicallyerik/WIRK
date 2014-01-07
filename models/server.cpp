#include "server.h"
#include "session.h"
#include "channel.h"
#include "irccommand.h"
#include "messageparser.h"
#include "commandparser.h"
#include "ircsession.h"
#include "user.h"
#include <QMessageBox>
#include "../channelsettings.h"
#include "../utilities/preferenceshelper.h"

Server::Server(QStandardItem *inMenuItem, Session *parent) : QObject(parent)
{
    messageParser = new MessageParser(this);
    ircSession = new IrcSession(this);
    menuItem = inMenuItem;
    timeoutTimer = new QTimer(this);
    text = QStringList("<span></span>");
    connect(ircSession, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(processMessage(IrcMessage*)));
    connect(ircSession, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(processError(QAbstractSocket::SocketError)));
    connect(ircSession, SIGNAL(nickNameChanged(const QString&)), this, SLOT(nickNameChanged(const QString&)));
    connect(ircSession, SIGNAL(password(QString*)), this, SLOT(passwordRequested(QString*)));
    connect(ircSession, SIGNAL(connecting()), this, SLOT(connecting()));
    connect(ircSession, SIGNAL(connected()), this, SLOT(connected()));
    connect(ircSession, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(serverTimeout()));
    this->disconnected();
}

Server::~Server()
{

}

QString Server::getHost()
{
    return host;
}

void Server::setHost(QString inHost)
{
    host = inHost;
    menuItem->setText(inHost);
    ircSession->setHost(host);
}

int Server::getPort()
{
    return port;
}

void Server::setPort(int inPort)
{
    port = inPort;
    ircSession->setPort(port);
}

QString Server::getUsername()
{
    return username;
}

void Server::setUsername(QString inUsername)
{
    username = inUsername;
    ircSession->setUserName(username);
}

QString Server::getNickname()
{
    return nickname;
}

void Server::setNickname(QString inNickname)
{
    nickname = inNickname;
    ircSession->setNickName(nickname);
}

QRegExp Server::getNicknameRegex()
{
    return QRegExp("([^\\w]|^)" + nickname + "([^\\w]|$)");
}

QString Server::getRealname()
{
    return realname;
}

void Server::setRealname(QString inRealname)
{
    realname = inRealname;
    ircSession->setRealName(realname);
}

QString Server::getPassword()
{
    return password;
}

void Server::setPassword(QString inPassword)
{
    password = inPassword;
}

bool Server::isSSL()
{
    return ssl;
}

void Server::setSSL(bool inSsl)
{
    ssl = inSsl;
    if(ssl) {
        QSslSocket* sslSocket = new QSslSocket(this);
        sslSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
        sslSocket->ignoreSslErrors();
        ircSession->setSocket(sslSocket);
    } else {
        ircSession->setSocket(new QTcpSocket(this));
    }
}

QString Server::getText()
{
    return text.join("");
}

QString Server::getLatestText()
{
    int latestTextCount = 100;
    if(text.length() > latestTextCount) {
        QStringList latestItems = QStringList(text.mid(text.length() - latestTextCount));
        return latestItems.join("");
    } else {
        return getText();
    }
}

void Server::appendText(QString inText)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString currentTimeStr = currentTime.toString("h:mmap");
    QString tableRow;
    tableRow = "<table width=\"100%\"><tr>";
    tableRow += "<th class=\"col-name\" width=\"25\"></th>";
    tableRow += "<td class=\"col-message\"><p class=\"message\">" + inText + "</p></td>";
    tableRow += "<td class=\"col-meta\" width=\"50\"><h6 class=\"metainfo\">" + currentTimeStr +"</h6></td>";
    tableRow += "</tr></table>";
    text.append(tableRow);
    Session *session = this->getSession();
    QStringList emptyList;
    session->emitMessageReceived(this, NULL, tableRow, emptyList);
}

Channel* Server::addChannel(QString inChannel, Channel::ChannelType inType)
{
    QStandardItem *newMenuItem = new QStandardItem();
    Channel *newChannel = new Channel(inChannel, inType, newMenuItem, this);
    newMenuItem->setData(QVariant::fromValue<Channel*>(newChannel), Qt::UserRole);
    menuItem->appendRow(newMenuItem);
    newMenuItem->setFlags(newMenuItem->flags() & ~Qt::ItemIsEditable);
    menuItem->sortChildren(0);
    return newChannel;
}

void Server::removeChannel(QString inChannel)
{
    Channel *channel = getChannel(inChannel);
    if(channel != NULL) {
        QStandardItem *chanMenuItem = channel->getMenuItem();
        int row = chanMenuItem->row();
        menuItem->removeRow(row);
        channel->deleteLater();
    }
}

void Server::partAllChannels()
{
    int totalMenuItems = menuItem->rowCount()-1;
    for(int i = totalMenuItems; i >= 0; i--) {
        QStandardItem *channelMenuItem = this->menuItem->child(i);
        QVariant data = channelMenuItem->data(Qt::UserRole);
        Channel* channel = data.value<Channel*>();
        channel->part();
    }
}

QStandardItem* Server::getChannelMenuItem(QString inChannel)
{
    QStandardItem *firstChild = menuItem->child(0);
    if(firstChild) {
        QModelIndex startIndex = firstChild->index();
        Session *session = this->getSession();
        QModelIndexList foundChannels = session->match(startIndex, Qt::DisplayRole, inChannel, -1, Qt::MatchExactly);
        if(foundChannels.count() == 1) {
            QStandardItem *channel = session->itemFromIndex(foundChannels.at(0));
            return channel;
        }
    }
    return NULL;
}

Channel* Server::getChannel(QString inChannel)
{
    QStandardItem *channel = getChannelMenuItem(inChannel);
    if(channel != NULL) {
        QVariant data = channel->data(Qt::UserRole);
        return data.value<Channel*>();
    }
    return NULL;
}

void Server::removeUserFromAllChannels(QString username, QString reason)
{
    int totalMenuItems = menuItem->rowCount()-1;
    for(int i = totalMenuItems; i >= 0; i--) {
        QStandardItem *channelMenuItem = this->menuItem->child(i);
        QVariant data = channelMenuItem->data(Qt::UserRole);
        Channel* channel = data.value<Channel*>();
        User *user = channel->getUser(username);
        if(user) {
            channel->removeUser(username, reason);
        }
    }
}

void Server::renameUserInAllChannels(QString oldName, QString newName)
{
    int totalMenuItems = menuItem->rowCount()-1;
    for(int i = totalMenuItems; i >= 0; i--) {
        QStandardItem *channelMenuItem = this->menuItem->child(i);
        QVariant data = channelMenuItem->data(Qt::UserRole);
        Channel* channel = data.value<Channel*>();
        User *user = channel->getUser(oldName);
        if(user) {
            user->setName(newName);
        }
    }
}

Session* Server::getSession() {
    return qobject_cast<Session *>(this->parent());
}

QStandardItem* Server::getMenuItem()
{
    return menuItem;
}

MessageParser* Server::getMessageParser()
{
    return messageParser;
}

void Server::connecting()
{
    this->appendText("Connecting...");
}

void Server::connected()
{
    this->appendText("Connected.");
    menuItem->setForeground(QBrush((QColor(255,255,255))));
    for(int c = 0; c < menuItem->rowCount(); c++) {
        QStandardItem *channelMenuItem = getMenuItem()->child(c);
        QVariant channelData = channelMenuItem->data(Qt::UserRole);
        Channel *channel = channelData.value<Channel*>();
        if (PreferencesHelper::sharedInstance()->getShouldJoinOnConnect(channel->getName(), this->getHost()))
        {
            channel->join();
        }
    }

    resetTimer();
}

void Server::disconnected()
{
    this->appendText("Disconnected.");
    menuItem->setForeground(QBrush((QColor(125,125,125))));
    int totalMenuItems = menuItem->rowCount()-1;
    for(int i = totalMenuItems; i >= 0; i--) {
        QStandardItem *channelMenuItem = this->menuItem->child(i);
        QVariant data = channelMenuItem->data(Qt::UserRole);
        Channel* channel = data.value<Channel*>();
        channel->setIsJoined(false);
    }
}

bool Server::getIsConnected()
{
    return ircSession->isConnected();
}

void Server::openConnection()
{
    ircSession->open();
}

void Server::closeConnection()
{
    ircSession->close();
    timeoutTimer->stop();
}

void Server::serverTimeout()
{
    closeConnection();
    getSession()->emitServerDisconnected(this);
}

void Server::resetTimer()
{
    timeoutTimer->start(MaxTimeoutLength);
}

void Server::sendCommand(IrcCommand *command)
{
    if (this->getIsConnected())
    {
        ircSession->sendCommand(command);
        this->resetTimer();
    }
}

void Server::processMessage(IrcMessage *message) {
    messageParser->parse(message);
}

void Server::processError(QAbstractSocket::SocketError error) {
    QString errorCode = QString::number(error);
    QMessageBox msgBox;
    msgBox.setText("Socket Error " + errorCode + "\n");
    msgBox.exec();
}

void Server::passwordRequested(QString *outPassword) {
    *outPassword = password;
}

void Server::nickNameChanged(const QString &name)
{
    nickname = name;
}
