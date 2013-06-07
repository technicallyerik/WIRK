#include "server.h"
#include "session.h"
#include "channel.h"
#include "irccommand.h"
#include "messageparser.h"
#include "commandparser.h"
#include "ircsession.h"
#include <QMessageBox>

Server::Server(QStandardItem *inMenuItem, Session *parent) : QObject(parent)
{
    messageParser = new MessageParser(this);
    commandParser = new CommandParser(this);
    ircSession = new IrcSession(this);
    menuItem = inMenuItem;
    text = "<body>";
    connect(ircSession, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(processMessage(IrcMessage*)));
    connect(ircSession, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(processError(QAbstractSocket::SocketError)));
    connect(ircSession, SIGNAL(nickNameChanged(const QString&)), this, SLOT(nickNameChanged(const QString&)));
    connect(ircSession, SIGNAL(password(QString*)), this, SLOT(passwordRequested(QString*)));
    connect(ircSession, SIGNAL(connecting()), this, SLOT(connecting()));
    connect(ircSession, SIGNAL(connected()), this, SLOT(connected()));
    connect(ircSession, SIGNAL(disconnected()), this, SLOT(disconnected()));
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
    return text;
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
    text += tableRow;
    Session *session = this->getSession();
    session->emitMessageReceived(this, NULL, tableRow);
}

Channel* Server::addChannel(QString inChannel)
{
    QStandardItem *newMenuItem = new QStandardItem();
    Channel *newChannel = new Channel(inChannel, newMenuItem, this);
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
        QString channelName = channel->getName();
        IrcCommand *partCommand = IrcCommand::createPart(channelName, NULL);
        ircSession->sendCommand(partCommand);
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
            channel->removeUser(username);
            QString channelName = channel->getName();
            QString partMessage = QString("%1 has left %2").arg(username, channelName);
            if (!reason.trimmed().isEmpty())
            {
                partMessage.append(QString(" (Reason: %3)").arg(reason));
            }
            channel->appendText(partMessage);
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

void Server::connecting()
{
    this->appendText("Connecting...");
}

void Server::connected()
{
    this->appendText("Connected.");
    menuItem->setForeground(QBrush((QColor(255,255,255))));

    Session* session = this->getSession();
    session->selectItem(this->getHost());

    for(int c = 0; c < menuItem->rowCount(); c++) {
        QStandardItem *channelMenuItem = getMenuItem()->child(c);
        QVariant channelData = channelMenuItem->data(Qt::UserRole);
        Channel *channel = channelData.value<Channel*>();
        channel->join();
    }
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
}

void Server::sendMessage(QString message) {
    IrcCommand *command = commandParser->parse(message);
    ircSession->sendCommand(command);
}

void Server::sendChannelMessage(QString channel, QString message) {
    IrcCommand *command = IrcCommand::createMessage(channel, message);
    Channel* sendChannel = this->getChannel(channel);
    QString nickname = this->getNickname();
    QString styledString = messageParser->styleString(message);
    sendChannel->appendText(nickname, styledString);
    ircSession->sendCommand(command);
}

void Server::sendCommand(IrcCommand *command)
{
    ircSession->sendCommand(command);
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
