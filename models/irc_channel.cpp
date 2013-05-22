#include "irc_channel.h"
#include "irc_server.h"

irc_channel::irc_channel(QObject *parent) : QObject(parent)
{

}

irc_channel::irc_channel(irc_server *server, QObject *parent) : QObject(parent)
{
    m_server = server;
}

QString irc_channel::getName() {
    return m_name;
}

void irc_channel::setName(QString name) {
    m_name = name;
}

QString irc_channel::getText() {
    return m_text;
}

void irc_channel::setText(QString text) {
    m_text = text;
    m_server->emitTextChanged(m_name, m_text);
}

void irc_channel::appendText(QString text) {
    m_text += text;
    m_server->emitTextChanged(m_name, m_text);
}

QMap<QString, irc_channel_user*> irc_channel::getUsers() {
    return m_users;
}

void irc_channel::setUsers(QMap<QString, irc_channel_user*> users) {
    m_users = users;
}
