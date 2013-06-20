#ifndef SESSION_H
#define SESSION_H

#include <QStandardItemModel>
#include "channel.h"

class Server;
class Channel;
class QSettings;

class Session : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit Session(QSettings *settings, QObject *parent = 0);
    virtual ~Session();
    
    Server* addServer(QString host, int port, QString username, QString nickname, QString realname, QString password, bool isSSL);
    QStandardItem* getServerMenuItem(QString inServer);
    Server* getServer(QString inServer);

    void emitMessageReceived(Server *server, Channel *channel, QString message, QStringList images, Channel::MessageType type = Channel::MessageTypeDefault);
    void emitSelectItem(QModelIndex index);

    void readFromSettings();
    void writeToSettings();

public slots:
    void removeServer(QString inServer);

private:
    QSettings *settings;

signals:
    void messageReceived(Server *server, Channel *channel, QString message, QStringList images, Channel::MessageType type);

};

#endif // SESSION_H
