#ifndef USER_H
#define USER_H

#include <QStandardItem>

class Channel;

class User : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)

public:
    explicit User(QString inName, QStandardItem *inMenuItem, Channel *parent = 0);
    
    QString getName();
    void setName(QString name);

    Channel* getChannel();
    QStandardItem* getMenuItem();
    
private:
    QString name;
    QStandardItem *menuItem;
};

#endif // USER_H
