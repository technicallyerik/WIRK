#include "session.h"
#include "server.h"
#include "channel.h"
#include "preferenceshelper.h"
#include <QSettings>
#include <QStandardItem>

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
    QSettings *settings = PreferencesHelper::sharedInstance()->getSettings();
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
            newServer->addChannel(channelName, Channel::ChannelTypeNormal);
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
    QSettings *settings = PreferencesHelper::sharedInstance()->getSettings();
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
        int ci = 0;
        for(int c = 0; c < server->getMenuItem()->rowCount(); c++) {
            QStandardItem *channelMenuItem = server->getMenuItem()->child(c);
            QVariant channelData = channelMenuItem->data(Qt::UserRole);
            Channel *channel = channelData.value<Channel*>();
            if(channel->getType() == Channel::ChannelTypeNormal) {
                settings->setArrayIndex(ci);
                settings->beginGroup("channel");
                settings->setValue("name", channel->getName());
                settings->endGroup();
                ci++;
            }
        }
        settings->endArray();
        settings->endGroup();
    }
    settings->endArray();
}

void Session::emitMessageReceived(Server *server, Channel *channel, QString message, QStringList images, Channel::MessageType type) {
    emit(messageReceived(server, channel, message, images, type));
}
