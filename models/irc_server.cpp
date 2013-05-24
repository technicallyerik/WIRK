#include "irc_server.h"
#include "message_parser.h"

irc_server::irc_server(QObject *parent) : QObject(parent)
{
    m_parser = new message_parser(this, this);
}

QString irc_server::getHost() {
    return m_host;
}

void irc_server::setHost(QString host) {
    m_host = host;
}

int irc_server::getPort() {
    return m_port;
}

void irc_server::setPort(int port) {
    m_port = port;
}

QString irc_server::getUsername() {
    return m_username;
}

void irc_server::setUsername(QString username) {
    m_username = username;
}

QString irc_server::getNickname() {
    return m_nickname;
}

void irc_server::setNickname(QString nickname) {
    m_nickname = nickname;
}

QString irc_server::getRealname() {
    return m_realname;
}

void irc_server::setRealname(QString realname) {
    m_realname = realname;
}

bool irc_server::isSSL() {
    return m_ssl;
}

void irc_server::setSSL(bool ssl) {
    m_ssl = ssl;
}

QMap<QString, irc_channel*> irc_server::getChannels() {
    return m_channels;
}

void irc_server::setChannels(QMap<QString, irc_channel*> channels) {
    m_channels = channels;
    emit(channelChanged());
}

void irc_server::addChannel(QString channel) {
    irc_channel* newChannel = new irc_channel(this, this);
    newChannel->setName(channel);
    m_channels.insert(channel.toLower(), newChannel);
    emit(channelChanged());
}

void irc_server::removeChannel(QString channel) {
    m_channels.remove(channel.toLower());
    emit(channelChanged());
}

void irc_server::createConnection() {

    m_session = new IrcSession(this);
    m_session->setHost(m_host);
    m_session->setPort(m_port);
    m_session->setUserName(m_username);
    m_session->setNickName(m_nickname);
    m_session->setRealName(m_realname);

    if(m_ssl) {
       QSslSocket* sslSocket = new QSslSocket(this);
        sslSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
        sslSocket->ignoreSslErrors();
        m_session->setSocket(sslSocket);
    } else {
        m_session->setSocket(new QTcpSocket(this));
    }

    connect(m_session, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(processMessage(IrcMessage*)));
    connect(m_session, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(processError(QAbstractSocket::SocketError)));
    connect(m_session, SIGNAL(nickNameChanged(const QString&)), this, SLOT(nickNameChanged(const QString&)));

    m_session->open();
}

QString irc_server::getText() {
    return m_text;
}

void irc_server::setText(QString text) {
    m_text = text;
    emitTextChanged(NULL, m_text);
}


void irc_server::appendText(QString text) {
    m_text += text;
    emitTextChanged(NULL, m_text);
}

void irc_server::emitTextChanged(QString channel, QString text) {
    emit(textChanged(new parsed_message(m_host, channel, text)));
}

void irc_server::sendMessage(QString message) {
    if(message.at(0) == '/') {
        IrcCommand *command = m_parser->parseCommand(message);
        m_session->sendCommand(command);
    } else {
        // TODO: Figure out current channel, and assume the text is a post for it
    }
}

void irc_server::emitUsersChanged(QMap<QString, irc_channel_user*> users) {
    QStringList *usersInChannel = new QStringList();
    QMapIterator<QString, irc_channel_user*> j(users);
    while (j.hasNext()) {
        j.next();
        irc_channel_user *user = j.value();
        usersInChannel->append(user->getName());
    }
   this->emitUsersChanged(*usersInChannel);
}

void irc_server::emitUsersChanged(QStringList users) {
    emit(usersChanged(users));
}

void irc_server::processMessage(IrcMessage *message)
{
    m_parser->parse(message);
}

void irc_server::processError(QAbstractSocket::SocketError error) {
    QString errorCode = QString::number(error);
    m_text += "Socket Error " + errorCode + "\n";
    emitTextChanged(NULL, m_text);
}

void irc_server::nickNameChanged(const QString &name)
{
    this->setNickname(name);
}
