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

protected:
    void keyPressEvent(QKeyEvent *event);
    
signals:
    
public slots:

private:
    MessageHistory *messageHistory;
    
};

#endif // TEXTBOX_H
