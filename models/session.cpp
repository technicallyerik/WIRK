#include "session.h"
#include "server.h"
#include "channel.h"

Session::Session(QObject *parent) : QStandardItemModel(parent)
{

}

Session::~Session()
{

}

Server* Session::addServer(QString host, int port, QString username, QString nickname, QString realname, QString password, bool isSSL)
{
    QStandardItem *newMenuItem = new QStandardItem();
    Server *server = new Server(newMenuItem, this);
    server->setHost(host);
    server->setPort(port);
    server->setUsername(username);
    server->setNickname(nickname);
    server->setRealname(realname);
    server->setPassword(password);
    server->setSSL(isSSL);
    server->openConnection();
    newMenuItem->setData(QVariant::fromValue<Server*>(server), Qt::UserRole);
    this->appendRow(newMenuItem);
    newMenuItem->setFlags(newMenuItem->flags() & ~Qt::ItemIsEditable);
    this->sort(0);
    return server;
}

void Session::removeServer(QString inServer)
{
    Server *server = getServer(inServer);
    if(server != NULL) {
        QStandardItem *servMeuItem = server->getMenuItem();
        int row = servMeuItem->row();
        this->removeRow(row);
        server->deleteLater();
    }
}

QStandardItem* Session::getServerMenuItem(QString inServer)
{
    QList<QStandardItem*> foundServers = this->findItems(inServer, Qt::MatchExactly);
    if(foundServers.count() == 1) {
        QStandardItem *server = foundServers[0];
        return server;
    }
    return NULL;
}

Server* Session::getServer(QString inServer)
{
    QStandardItem *server = getServerMenuItem(inServer);
    if(server != NULL) {
        QVariant data = server->data(Qt::UserRole);
        return data.value<Server*>();
    }
    return NULL;
}

void Session::emitMessageReceived(Server *server, Channel *channel, QString message, Channel::MessageType type) {
    emit(messageReceived(server, channel, message, type));
}
