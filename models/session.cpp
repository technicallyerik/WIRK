#include "session.h"
#include "server.h"
#include "channel.h"
#include <QSettings>
#include <QStandardItem>

const QString Session::settingsFileName = "session.ini";

Session::Session(QObject *parent) : QStandardItemModel(parent)
{
    settings = new QSettings(settingsFileName, QSettings::IniFormat, this);
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

void Session::readFromSettings()
{
    int serverSize = settings->beginReadArray("servers");
    for (int s = 0; s < serverSize; s++) {
         settings->setArrayIndex(s);
         settings->beginGroup("server");
         QString address = settings->value("address").toString();
         int port = settings->value("port").toInt();
         QString realName = settings->value("realname").toString();
         QString nickname = settings->value("nickname").toString();
         QString username = settings->value("username").toString();
         QString password = settings->value("password").toString();
         bool useSSL = settings->value("usessl", false).toBool();
         Server *newServer = addServer(address,
                                        port,
                                        username,
                                        nickname,
                                        realName,
                                        password,
                                        useSSL);
         int channelSize = settings->beginReadArray("channels");
         for(int c = 0; c < channelSize; c++) {
            settings->setArrayIndex(c);
            settings->beginGroup("channel");
            QString channelName = settings->value("name").toString();
            newServer->addChannel(channelName);
            settings->endGroup();
         }
         newServer->openConnection();
         settings->endArray();
         settings->endGroup();
     }
    settings->endArray();
}

void Session::writeToSettings()
{
    settings->beginWriteArray("servers");
    for (int s = 0; s < this->rowCount(); s++) {
        settings->setArrayIndex(s);
        QStandardItem *serverMenuItem = this->item(s);
        QVariant serverData = serverMenuItem->data(Qt::UserRole);
        Server *server = serverData.value<Server*>();
        settings->beginGroup("server");
        settings->setValue("address", server->getHost());
        settings->setValue("port", server->getPort());
        settings->setValue("realname", server->getRealname());
        settings->setValue("nickname", server->getNickname());
        settings->setValue("username", server->getUsername());
        settings->setValue("password", server->getPassword());
        settings->setValue("usessl", server->isSSL());
        settings->beginWriteArray("channels");
        for(int c = 0; c < server->getMenuItem()->rowCount(); c++) {
            settings->setArrayIndex(c);
            settings->beginGroup("channel");
            QStandardItem *channelMenuItem = server->getMenuItem()->child(c);
            QVariant channelData = channelMenuItem->data(Qt::UserRole);
            Channel *channel = channelData.value<Channel*>();
            settings->setValue("name", channel->getName());
            settings->endGroup();
        }
        settings->endArray();
        settings->endGroup();
    }
    settings->endArray();
}

// TODO:  Need to account for multiple servers with same channel
void Session::selectItem(QString string)
{
    QList<QStandardItem*> foundItems = this->findItems(string, Qt::MatchExactly | Qt::MatchRecursive);
    if(foundItems.count() == 1) {
        QStandardItem *item = foundItems[0];
        QModelIndex index = item->index();
        emit(selectItem(index));
    }
}

void Session::emitMessageReceived(Server *server, Channel *channel, QString message, Channel::MessageType type) {
    emit(messageReceived(server, channel, message, type));
}
