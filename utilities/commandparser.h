#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QObject>

class Server;
class IrcCommand;
class Channel;

class CommandParser : public QObject
{
    Q_OBJECT
public:
    explicit CommandParser(QObject *parent = 0);

    IrcCommand* parse(QString commandStr, Server* currentServer, Channel* currentChannel);
};

#endif // COMMANDPARSER_H
