#include "parsed_message.h"

parsed_message::parsed_message(QObject *parent) : QObject(parent)
{

}

parsed_message::parsed_message(QString server, QString channel, QString message, QObject *parent) : QObject(parent)
{
    this->m_server = server;
    this->m_channel = channel;
    this->m_message = message;
}

void parsed_message::setServer(QString server)
{
    this->m_server = server;
}

QString parsed_message::getServer()
{
    return this->m_server;
}

void parsed_message::setChannel(QString channel)
{
    this->m_channel = channel;
}

QString parsed_message::getChannel()
{
    return this->m_channel;
}

void parsed_message::setMessage(QString message)
{
    this->m_message = message;
}

QString parsed_message::getMessage()
{
    return this->m_message;
}
