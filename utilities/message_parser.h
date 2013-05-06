#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

#include <QObject>
#include "ircmessage.h"

class message_parser : public QObject
{
    Q_OBJECT
public:
    explicit message_parser(QObject *parent = 0);
    
    QString parse(IrcMessage *message);

private:
    QString parsenumeric(IrcNumericMessage *message);

signals:
    
private slots:

};

#endif // MESSAGE_PARSER_H
