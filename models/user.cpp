#include "user.h"
#include "channel.h"
#include <QSet>
#include <QDebug>

User::User(QString inName, QChar inMode, QStandardItem *inMenuItem, Channel *parent) : QObject(parent)
{
    menuItem = inMenuItem;
    this->setName(inName);
    this->addMode(inMode);
}

User::~User()
{

}

QString User::getName() {
    return name;
}

void User::setName(QString inName) {
    name = inName;
    QString mode = this->getModeDisplayString();
    menuItem->setText(mode + name);
    menuItem->setData(name, UserDataName);
    this->refreshUserDisplay();
}

QSet<QChar> User::getModes()
{
    return modes;
}

void User::refreshUserDisplay()
{
    QString mode = this->getModeDisplayString();
    menuItem->setText(mode + name);

    QString sortString = getSortString();
    menuItem->setData(sortString, UserDataSort);
    Channel *channel = this->getChannel();
    channel->sortUsers();
}

QChar User::getModeDisplayString()
{
    QChar mode = char();
    if (modes.contains('q'))
        mode = USER_MODE_OWNER;
    else if (modes.contains('a'))
        mode = USER_MODE_ADMIN;
    else if (modes.contains('o'))
        mode = USER_MODE_OPERATOR;
    else if (modes.contains('h'))
        mode = USER_MODE_HALF_OP;
    else if (modes.contains('v'))
        mode = USER_MODE_VOICED;

    return mode;
}

void User::addMode(QChar mode)
{
    if (mode == USER_MODE_OWNER)
        mode = 'q';
    else if (mode == USER_MODE_ADMIN)
        mode = 'a';
    else if (mode == USER_MODE_OPERATOR)
        mode = 'o';
    else if (mode == USER_MODE_HALF_OP)
        mode = 'h';
    else if (mode == USER_MODE_VOICED)
        mode = 'v';

    modes.insert(mode);
    this->refreshUserDisplay();
}

void User::removeMode(QChar mode)
{
    modes.remove(mode);
    this->refreshUserDisplay();
}

QString User::getSortString()
{
    int sortNumberPrefix;
    QChar mode = this->getModeDisplayString();

    if (mode == USER_MODE_OWNER)
        sortNumberPrefix = 1;
    else if (mode == USER_MODE_ADMIN)
        sortNumberPrefix = 2;
    else if (mode == USER_MODE_OPERATOR)
        sortNumberPrefix = 3;
    else if (mode == USER_MODE_HALF_OP)
        sortNumberPrefix = 4;
    else if (mode == USER_MODE_VOICED)
        sortNumberPrefix = 5;\
    else
        sortNumberPrefix = 6;

    return sortNumberPrefix + name.toLower();
}

Channel* User::getChannel() {
    return qobject_cast<Channel*>(this->parent());
}

QStandardItem* User::getMenuItem()
{
    return menuItem;
}
