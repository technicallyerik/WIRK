#ifndef CHANNEL_H
#define CHANNEL_H

#include <QStandardItem>
#include <QChar>

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
    virtual ~Channel();

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

    bool getIsJoined();
    void setIsJoined(bool isJoined);

    QStandardItemModel* getUsers();
    void addUsers(QStringList inUsers);
    User* addUser(QString inUser, QChar prefix);
    void removeUser(QString inUser);
    void sortUsers();
    QStandardItem* getUserMenuItem(QString inUser);
    User* getUser(QString inUser);

    Server* getServer();
    QStandardItem* getMenuItem();

public slots:
    void join();
    void part();

private:
    QString name;
    QString text;
    QStandardItemModel *users;
    QStandardItem *menuItem;
    bool isJoined;
};

#endif // CHANNEL_H
