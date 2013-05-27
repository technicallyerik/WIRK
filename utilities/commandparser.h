#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QObject>

class Server;
class IrcCommand;

class CommandParser : public QObject
{
    Q_OBJECT
public:
    explicit CommandParser(Server *parent = 0);
    
    IrcCommand* parse(QString commandStr);
    Server* getServer();
};

#endif // COMMANDPARSER_H
