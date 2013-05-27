#include "channel.h"
#include "server.h"
#include "user.h"
#include "session.h"

Channel::Channel(QString inName, QStandardItem *inMenuItem, Server *parent) : QObject(parent)
{
    text = "";
    users = new QStandardItemModel(this);
    menuItem = inMenuItem;
    this->setName(inName);
}

QString Channel::getName() {
    return name;
}

void Channel::setName(QString inName) {
    name = inName;
    menuItem->setText(inName);
}

QString Channel::getText() {
    return text;
}

void Channel::appendText(QString inText) {
    text += inText + "<br />";
    QString channelName = this->getName();
    Server *server = this->getServer();
    QString serverName = server->getHost();
    Session *session = server->getSession();
    session->emitMessageRecieved(serverName, channelName, inText);
}

QStandardItemModel* Channel::getUsers() {
    return users;
}

void Channel::addUsers(QStringList inUsers) {
    for (int i = 0; i < inUsers.count(); i++) {
        QString newUserName = inUsers[i];
        this->addUser(newUserName);
    }
}

void Channel::addUser(QString inUser) {
    QStandardItem *newMenuItem = new QStandardItem();
    User *newUser = new User(inUser.toLower(), newMenuItem, this);
    newMenuItem->setData(QVariant::fromValue<User*>(newUser), Qt::UserRole);
    users->appendRow(newMenuItem);
}

void Channel::removeUser(QString inUser) {
    QList<QStandardItem*> foundUsers = users->findItems(inUser.toLower());
    if(foundUsers.count() == 1) {
        QStandardItem *user = foundUsers[0];
        int row = user->row();
        users->removeRow(row);
    }
}

User* Channel::getUser(QString inUser) {
    QList<QStandardItem*> foundUsers = users->findItems(inUser.toLower());
    if(foundUsers.count() == 1) {
        QStandardItem *user = foundUsers[0];
        QVariant data = user->data(Qt::UserRole);
        return data.value<User*>();
    }
    return NULL;
}

Server* Channel::getServer() {
    return qobject_cast<Server *>(this->parent());
}
