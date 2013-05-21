#include "irc_channel_user.h"

irc_channel_user::irc_channel_user(QObject *parent) : QObject(parent)
{

}

QString irc_channel_user::getName() {
    return m_name;
}

void irc_channel_user::setName(QString name) {
    m_name = name;
}
