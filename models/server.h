#ifndef SERVER_H
#define SERVER_H

#include <QStandardItem>
#include <QSslSocket>
#include "channel.h"

class Session;
class IrcSession;
class MessageParser;
class CommandParser;
class IrcMessage;
class IrcCommand;

class Server : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString host READ getHost WRITE setHost)
    Q_PROPERTY(int port READ getPort WRITE setPort)
    Q_PROPERTY(QString username READ getUsername WRITE setUsername)
    Q_PROPERTY(QString nickname READ getNickname WRITE setNickname)
    Q_PROPERTY(QString realname READ getRealname WRITE setRealname)
    Q_PROPERTY(QString password READ getPassword WRITE setPassword)
    Q_PROPERTY(bool ssl READ isSSL WRITE setSSL)
    Q_PROPERTY(QString text READ getText)

public:
    explicit Server(QStandardItem *inMenuItem, Session *parent = 0);
    virtual ~Server();

    QString getHost();
    void setHost(QString host);

    int getPort();
    void setPort(int port);

    QString getUsername();
    void setUsername(QString username);

    QString getNickname();
    void setNickname(QString nickname);

    QString getRealname();
    void setRealname(QString realname);

    QString getPassword();
    void setPassword(QString password);

    bool isSSL();
    void setSSL(bool ssl);

    QString getText();
    QString getLatestText();
    void appendText(QString text);

    Channel* addChannel(QString inChannel, Channel::ChannelType inType);
    void partAllChannels();
    QStandardItem* getChannelMenuItem(QString inChannel);
    Channel* getChannel(QString inChannel);

    void removeUserFromAllChannels(QString username, QString reason);
    void renameUserInAllChannels(QString oldName, QString newName);

    Session* getSession();
    QStandardItem* getMenuItem();

    bool getIsConnected();

    void sendMessage(QString command);
    void sendChannelMessage(QString channel, QString message);
    void sendCommand(IrcCommand *command);

public slots:
    void removeChannel(QString inChannel);
    void openConnection();
    void closeConnection();

private slots:
    void connecting();
    void connected();
    void disconnected();
    void processMessage(IrcMessage *message);
    void processError(QAbstractSocket::SocketError error);
    void nickNameChanged(const QString &name);
    void passwordRequested(QString *outPassword);
    
private:
    QString host;
    int port;
    QString username;
    QString nickname;
    QString realname;
    QString password;
    bool ssl;
    QStringList text;

    bool isConnected;

    IrcSession *ircSession;
    MessageParser *messageParser;
    CommandParser *commandParser;
    QStandardItem *menuItem;
};

#endif // SERVER_H
