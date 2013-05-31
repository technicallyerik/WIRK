#ifndef CHANNEL_H
#define CHANNEL_H

#include <QStandardItem>

class Server;
class User;

class Channel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(QString text READ getText)
    Q_PROPERTY(QStandardItemModel* users READ getUsers)

public:
    explicit Channel(QString inName, QStandardItem *inMenuItem, Server *parent = 0);

    enum MessageType
    {
        Default,
        Topic,
        Emote,
        Info
    };
    
    QString getName();
    void setName(QString name);

    QString getText();
    void appendText(QString text);
    void appendText(QString sender, QString text, MessageType type = Channel::Default);

    QStandardItemModel* getUsers();
    void addUsers(QStringList inUsers);
    void addUser(QString inUser);
    void removeUser(QString inUser);
    QStandardItem* getUserMenuItem(QString inUser);
    User* getUser(QString inUser);

    Server* getServer();
    QStandardItem* getMenuItem();

private:
    QString name;
    QString text;
    QStandardItemModel *users;
    QStandardItem *menuItem;
};

#endif // CHANNEL_H
