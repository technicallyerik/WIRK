#ifndef PARSED_MESSAGE_H
#define PARSED_MESSAGE_H

#include <qstring.h>

class ParsedMessage
{
public:
    ParsedMessage();
    ParsedMessage(QString destination, QString message);

    void setDestination(QString destination);
    QString getDestination();

    void setMessage(QString message);
    QString getMessage();


private:
    QString destination;
    QString message;
};

#endif // PARSED_MESSAGE_H
