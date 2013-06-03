#include "user.h"
#include "channel.h"
#include <QDebug>

User::User(QString inName, QChar inMode, QStandardItem *inMenuItem, Channel *parent) : QObject(parent)
{
    menuItem = inMenuItem;
    this->setName(inName);
    this->setMode(inMode);
}

User::~User()
{

}

QString User::getName() {
    return name;
}

void User::setName(QString inName) {
    name = inName;
    menuItem->setText(mode + name);
    menuItem->setData(name, UserDataName);
}

QChar User::getMode()
{
    return mode;
}

void User::setMode(QChar inMode)
{
    mode = inMode;
    menuItem->setText(mode + name);
    QString sortString = getSortString();
    menuItem->setData(sortString, UserDataSort);
}

QString User::getSortString()
{
    int sortNumberPrefix;
    if(mode == USER_MODE_OWNER) {
        sortNumberPrefix = 0;
    } else if(mode == USER_MODE_ADMIN) {
        sortNumberPrefix = 1;
    } else if(mode == USER_MODE_OPERATOR) {
        sortNumberPrefix = 2;
    } else if(mode == USER_MODE_HALF_OP) {
        sortNumberPrefix = 3;
    } else if(mode == USER_MODE_VOICED) {
        sortNumberPrefix = 4;
    } else {
        sortNumberPrefix = 5;
    }
    return sortNumberPrefix + name;
}

Channel* User::getChannel() {
    return qobject_cast<Channel*>(this->parent());
}

QStandardItem* User::getMenuItem()
{
    return menuItem;
}
