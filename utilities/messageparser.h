#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <QObject>

class Server;
class Channel;
class IrcMessage;
class IrcNumericMessage;

class MessageParser : public QObject
{
    Q_OBJECT
public:
    explicit MessageParser(Server *parent = 0);

    void parse(IrcMessage *message);
    QString parseNumeric(IrcNumericMessage *message);
    QString styleString(QString fullMessage);

    Server* getServer();
    Channel* getChannel(QString name);
};

#endif // MESSAGEPARSER_H
