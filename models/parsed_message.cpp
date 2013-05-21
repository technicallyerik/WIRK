#include "parsed_message.h"

parsed_message::parsed_message(QObject *parent) : QObject(parent)
{

}

parsed_message::parsed_message(QString destination, QString message, QObject *parent) : QObject(parent)
{
    this->m_destination = destination;
    this->m_message = message;
}

void parsed_message::setDestination(QString destination)
{
    this->m_destination = destination;
}

QString parsed_message::getDestination()
{
    return this->m_destination;
}

void parsed_message::setMessage(QString message)
{
    this->m_message = message;
}

QString parsed_message::getMessage()
{
    return this->m_message;
}
