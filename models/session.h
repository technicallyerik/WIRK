#ifndef SESSION_H
#define SESSION_H

#include <QStandardItemModel>

class Server;

class Session : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit Session(QObject *parent = 0);
    
    void addServer(QString host, int port, QString username, QString nickname, QString realname, QString password, bool isSSL);
    Server* getServer(QString inServer);

    void emitMessageRecieved(QString server, QString channel, QString message);

signals:
    void messageRecieved(QString server, QString channel, QString message);

};

#endif // SESSION_H
