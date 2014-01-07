#ifndef CHANNEL_H
#define CHANNEL_H

#include <QStandardItem>
#include <QChar>

class Server;
class User;
class ChannelSettings;

class Channel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(QString text READ getText)
    Q_PROPERTY(QStandardItemModel* users READ getUsers)

public:
    enum ChannelType
    {
        ChannelTypeNormal,
        ChannelTypeUser
    };

    enum MessageType
    {
        MessageTypeDefault,
        MessageTypeTopic,
        MessageTypeEmote,
        MessageTypeInfo
    };

    explicit Channel(QString inName, ChannelType type, QStandardItem *inMenuItem, Server *parent = 0);
    virtual ~Channel();
    
    QString getName();
    void setName(QString name);

    QString getText();
    QString getLatestText();
    void appendText(QString text);
    void appendText(QString sender, QString text, MessageType type = Channel::MessageTypeDefault);

    ChannelType getType();
    void setType(ChannelType type);

    bool getIsJoined();
    void setIsJoined(bool isJoined);

    QStandardItemModel* getUsers();
    void addUsers(QStringList inUsers);
    User* addUser(QString inUser, QChar prefix);
    void removeUser(QString inUser, QString reason);
    void sortUsers();
    QStandardItem* getUserMenuItem(QString inUser);
    User* getUser(QString inUser);

    QStringList findUsersByPrefix(QString searchStr);

    void sendMessage(QString message);

    Server* getServer();
    QStandardItem* getMenuItem();

    static bool isChannel(QString name);

public slots:
    void join();
    void part();

private:
    QString name;
    QStringList text;
    ChannelType type;
    QStandardItemModel *users;
    QStandardItem *menuItem;
    bool isJoined;

    QString getStyledUserString(QString user);
};

#endif // CHANNEL_H
