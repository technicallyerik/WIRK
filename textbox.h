#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <QLineEdit>

class MessageHistory;

class TextBox : public QLineEdit
{
    Q_OBJECT
public:
    explicit TextBox(QWidget *parent = 0);
    virtual ~TextBox();
    void setUsernames(QStringList users);

protected:
    void keyPressEvent(QKeyEvent *event);
    bool event(QEvent *e);
    
signals:
    
public slots:

private:
    MessageHistory *messageHistory;
    QStringList usernames, searchingUsernames;
    QString currentFullMessage;
    int userSearchIndex;

    QString getLastArgument();

    
};

#endif // TEXTBOX_H
