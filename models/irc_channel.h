#ifndef IRC_CHANNEL_H
#define IRC_CHANNEL_H

#include <QObject>
#include "irc_channel_user.h"
#include <QMap>
#include "parsed_message.h"

typedef QMap<QString, irc_channel_user*> IrcChannelUserMap;

class irc_server;

class irc_channel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(QString text READ getText WRITE setText)
    Q_PROPERTY(IrcChannelUserMap users READ getUsers WRITE setUsers)

public:
    explicit irc_channel(QObject *parent = 0);
    irc_channel(irc_server *server, QObject *parent = 0);

    QString getName();
    void setName(QString name);

    QString getText();
    void setText(QString text);
    void appendText(QString text);

    QMap<QString, irc_channel_user*> getUsers();
    void setUsers(QMap<QString, irc_channel_user*> users);
    void addUsers(QStringList users);

signals:
    
public slots:
    
private:
    irc_server *m_server;
    QString m_name;
    QString m_text;
    QMap<QString, irc_channel_user*> m_users;

};

#endif // IRC_CHANNEL_H
