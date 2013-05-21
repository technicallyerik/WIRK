#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

#include <QObject>
#include "ircmessage.h"
#include "irccommand.h"
#include "parsed_message.h"

class irc_server;

class message_parser : public QObject
{
    Q_OBJECT

public:
    explicit message_parser(QObject *parent = 0);
    explicit message_parser(irc_server *server, QObject *parent = 0);

    void parse(IrcMessage *message);

    IrcCommand* parseCommand(QString commandStr);

private:
    QString parsenumeric(IrcNumericMessage *message);
    QString handlePrivateMessage(IrcPrivateMessage* privateMessage);
    QString styleString(QString fullMessage);

signals:
    
private slots:

private:
    irc_server *m_server;

};

#endif // MESSAGE_PARSER_H
