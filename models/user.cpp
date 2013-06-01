#include "user.h"
#include "channel.h"

User::User(QString inName, QStandardItem *inMenuItem, Channel *parent) : QObject(parent)
{
    menuItem = inMenuItem;
    this->setName(inName);
}

User::~User()
{

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

QStandardItem* User::getMenuItem()
{
    return menuItem;
}
