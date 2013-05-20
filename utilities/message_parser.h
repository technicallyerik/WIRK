#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

#include <QObject>
#include "ircmessage.h"
#include "irccommand.h"
#include "parsed_message.h"

class message_parser : public QObject
{
    Q_OBJECT
public:
    explicit message_parser(QObject *parent = 0);
    
    ParsedMessage* parse(IrcMessage *message);

    IrcCommand* parseCommand(QString commandStr);

private:
    QString parsenumeric(IrcNumericMessage *message);
    QString handlePrivateMessage(IrcPrivateMessage* privateMessage);

signals:
    
private slots:

};

#endif // MESSAGE_PARSER_H
