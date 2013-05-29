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
    ircSession = new IrcSession();
    menuItem = inMenuItem;
    connect(ircSession, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(processMessage(IrcMessage*)));
    connect(ircSession, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(processError(QAbstractSocket::SocketError)));
    connect(ircSession, SIGNAL(nickNameChanged(const QString&)), this, SLOT(nickNameChanged(const QString&)));
}

QString Server::getHost() {
    return host;
}

void Server::setHost(QString inHost) {
    host = inHost;
    menuItem->setText(inHost);
    ircSession->setHost(host);
}

int Server::getPort() {
    return port;
}

void Server::setPort(int inPort) {
    port = inPort;
    ircSession->setPort(port);
}

QString Server::getUsername() {
    return username;
}

void Server::setUsername(QString inUsername) {
    username = inUsername;
    ircSession->setUserName(username);
}

QString Server::getNickname() {
    return nickname;
}

void Server::setNickname(QString inNickname) {
    nickname = inNickname;
    ircSession->setNickName(nickname);
}

QString Server::getRealname() {
    return realname;
}

void Server::setRealname(QString inRealname) {
    realname = inRealname;
    ircSession->setRealName(realname);
}

QString Server::getPassword() {
    return password;
}

void Server::setPassword(QString inPassword) {
    password = inPassword;
}

bool Server::isSSL() {
    return ssl;
}

void Server::setSSL(bool inSsl) {
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

QString Server::getText() {
    return text;
}

void Server::appendText(QString inText) {
    text += inText + "<br />";
    Session *session = this->getSession();
    QString host = this->getHost();
    session->emitMessageReceived(host, NULL, inText);
}

void Server::addChannel(QString inChannel) {
    QStandardItem *newMenuItem = new QStandardItem();
    Channel *newChannel = new Channel(inChannel.toLower(), newMenuItem, this);
    newMenuItem->setData(QVariant::fromValue<Channel*>(newChannel), Qt::UserRole);
    menuItem->appendRow(newMenuItem);
    newMenuItem->setFlags(newMenuItem->flags() & ~Qt::ItemIsEditable);

}

void Server::removeChannel(QString inChannel) {
    Session *session = this->getSession();
    QList<QStandardItem*> foundChannels = session->findItems(inChannel.toLower(), Qt::MatchExactly | Qt::MatchRecursive);
    if(foundChannels.count() == 1) {
        QStandardItem *channel = foundChannels[0];
        int row = channel->row();
        menuItem->removeRow(row);
    }
}

void Server::partAllChannels() {
    int totalMenuItems = this->menuItem->rowCount()-1;
    for(int i = totalMenuItems; i >= 0; i--) {
        QStandardItem *channelMenuItem = this->menuItem->child(i);
        QVariant data = channelMenuItem->data(Qt::UserRole);
        Channel* channel = data.value<Channel*>();
        QString channelName = channel->getName();
        IrcCommand *partCommand = IrcCommand::createPart(channelName, NULL);
        ircSession->sendCommand(partCommand);
    }
}

Channel* Server::getChannel(QString inChannel) {
    Session *session = this->getSession();
    QList<QStandardItem*> foundChannels = session->findItems(inChannel.toLower(), Qt::MatchExactly | Qt::MatchRecursive);
    if(foundChannels.count() == 1) {
        QStandardItem *channel = foundChannels[0];
        QVariant data = channel->data(Qt::UserRole);
        return data.value<Channel*>();
    }
    return NULL;
}

Session* Server::getSession() {
    return qobject_cast<Session *>(this->parent());
}

void Server::openConnection() {
    ircSession->open();
}

void Server::sendMessage(QString message) {
    IrcCommand *command = commandParser->parse(message);
    ircSession->sendCommand(command);
}

void Server::sendChannelMessage(QString channel, QString message) {
    IrcCommand *command = IrcCommand::createMessage(channel, message);
    Channel* sendChannel = this->getChannel(channel);
    sendChannel->appendText(QString("%1: %2").arg(this->getUsername(), message));
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

void Server::nickNameChanged(const QString &name)
{
    nickname = name;
}