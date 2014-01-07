#include "user.h"
#include "channel.h"
#include "server.h"
#include <stdlib.h>
#include <QSet>
#include <QBrush>
#include <QDebug>
#include "preferenceshelper.h"
#include "../channelsettings.h"

User::User(QString inName, QChar inMode, QStandardItem *inMenuItem, Channel *parent) : QObject(parent)
{
    menuItem = inMenuItem;
    modes = QSet<QChar>();

    QStringList userColors = PreferencesHelper::sharedInstance()->getUserColors();
    int randomColorNumber = (rand() % userColors.count());
    QString userColor = userColors.at(randomColorNumber);
    this->userColor = QBrush(QColor(userColor));

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
    if(name != NULL && inName.compare(name, Qt::CaseInsensitive) != 0) {
        QString renameMessage = QString("%1 is now known as %2").arg(name, inName);
        Channel *channel = getChannel();
        channel->appendText(renameMessage);
    }
    name = inName;

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
    Channel *channel = this->getChannel();

    bool shouldUseColorUsernames = PreferencesHelper::sharedInstance()->getShouldUseColorUsernames(channel->getName(), channel->getServer()->getHost());
    if(shouldUseColorUsernames) {
        menuItem->setForeground(userColor);
    } else {
        // Completely clear the brush from the data so the list item falls back to it's style sheet
        menuItem->setData(false, Qt::ForegroundRole);
    }

    QString sortString = getSortString();
    menuItem->setData(sortString, UserDataSort);

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

QBrush User::getUserColor()
{
    return userColor;
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
        sortNumberPrefix = 5;
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
