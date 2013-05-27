#include "user.h"
#include "channel.h"

User::User(QString inName, QStandardItem *inMenuItem, Channel *parent) : QObject(parent)
{
    menuItem = inMenuItem;
    this->setName(inName);
}

QString User::getName() {
    return name;
}

void User::setName(QString inName) {
    name = inName;
    menuItem->setText(inName);
}

Channel* User::getChannel() {
    return qobject_cast<Channel*>(this->parent());
}
