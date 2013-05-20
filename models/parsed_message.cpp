#include "parsed_message.h"

ParsedMessage::ParsedMessage()
{
    this->destination = "";
    this->message = "";
}

ParsedMessage::ParsedMessage(QString destination, QString message)
{
    this->destination = destination;
    this->message = message;
}

void ParsedMessage::setDestination(QString destination)
{
    this->destination = destination;
}

QString ParsedMessage::getDestination()
{
    return this->destination;
}

void ParsedMessage::setMessage(QString message)
{
    this->message = message;
}

QString ParsedMessage::getMessage()
{
    return this->message;
}
