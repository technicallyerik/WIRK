#ifndef IRC_CHANNEL_H
#define IRC_CHANNEL_H

#include <QObject>
#include "irc_channel_user.h"
#include <QMap>

typedef QMap<QString, irc_channel_user*> IrcChannelUserMap;

class irc_channel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged)
    Q_PROPERTY(IrcChannelUserMap users READ getUsers WRITE setUsers)

public:
    explicit irc_channel(QObject *parent = 0);

    QString getName();
    void setName(QString name);

    QString getText();
    void setText(QString text);

    QMap<QString, irc_channel_user*> getUsers();
    void setUsers(QMap<QString, irc_channel_user*> users);

signals:
    void textChanged(QString text);
    
public slots:
    
private:
    QString m_name;
    QString m_text;
    QMap<QString, irc_channel_user*> m_users;

};

#endif // IRC_CHANNEL_H
