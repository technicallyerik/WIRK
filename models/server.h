#ifndef SERVER_H
#define SERVER_H

#include <QStandardItem>
#include <QSslSocket>
#include <irccommand.h>

class Session;
class IrcSession;
class MessageParser;
class CommandParser;
class IrcMessage;

class Channel;

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
    void appendText(QString text);

    void addChannel(QString inChannel);
    void removeChannel(QString inChannel);
    void partAllChannels();
    QStandardItem* getChannelMenuItem(QString inChannel);
    Channel* getChannel(QString inChannel);

    void removeUserFromAllChannels(QString username);

    Session* getSession();
    QStandardItem* getMenuItem();

    void openConnection();
    void sendMessage(QString command);
    void sendChannelMessage(QString channel, QString message);
    void sendCommand(IrcCommand *command);

private slots:
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
    QString text;

    IrcSession *ircSession;
    MessageParser *messageParser;
    CommandParser *commandParser;
    QStandardItem *menuItem;
};

#endif // SERVER_H
