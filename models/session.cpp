#include "session.h"
#include "server.h"

Session::Session(QObject *parent) : QStandardItemModel(parent)
{

}

void Session::addServer(QString host, int port, QString username, QString nickname, QString realname, QString password, bool isSSL)
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
}

QStandardItem* Session::getServerStandardItem(QString inServer)
{
    QList<QStandardItem*> foundServers = this->findItems(inServer.toLower(), Qt::MatchExactly);
    if(foundServers.count() == 1) {
        QStandardItem *server = foundServers[0];
        return server;
    }
    return NULL;
}

Server* Session::getServer(QString inServer)
{
    QStandardItem *server = getServerStandardItem(inServer);
    if(server != NULL) {
        QVariant data = server->data(Qt::UserRole);
        return data.value<Server*>();
    }
}

void Session::emitMessageReceived(QString server, QString channel, QString message) {
    emit(messageReceived(server, channel, message));
}
