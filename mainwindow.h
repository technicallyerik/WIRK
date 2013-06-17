#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QPixmap>
#include <QtWebKitWidgets/QWebView>
#include "channel.h"

namespace Ui {
    class MainWindow;
}

class Session;
class Server;
class Channel;
class QNetworkAccessManager;
class QNetworkReply;
class QTextDocument;
class AnimationViewModel;
class CommandParser;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static const int HighlightType = 64;

    enum ChannelHighlightType
    {
        ChannelHighlightTypeNone,
        ChannelHighlightTypeNew,
        ChannelHighlightTypeMention
    };

    void readSettings();
    
private:
    Ui::MainWindow *ui;
    Session *session;
    QNetworkAccessManager *networkAccessManager;
    QWebView *webView;
    QTextDocument *document;
    QList<AnimationViewModel *> animations;
    QTimer *timer;
    QHash<QString, QString> imagePageMap;
    CommandParser *commandParser;

    void changeToServer(Server *newServer);
    void changeToChannel(Channel *newChannel);
    void scrollToBottom();

    void highlightServer(Server *server, ChannelHighlightType highlight);
    void highlightChannel(Channel *channel, ChannelHighlightType highlight, Channel::MessageType type);
    void highlightMenuItem(QStandardItem *menuItem, ChannelHighlightType highlight);

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void sendMessage();
    void handleMessage(Server *server, Channel *channel, QString message, QStringList images, Channel::MessageType type);

    void treeItemClicked(const QModelIndex& index);
    void selectItem(QModelIndex index);

    void webLoadFinished(bool ok);
    void imageDownloaded(QNetworkReply *networkReply);
    void movieChanged(QPixmap pixels, QUrl url);

    void generateContextMenu(const QPoint &point);

    void rowsRemoved(const QModelIndex &modelIndex, int start, int end);
    void rowsInserted(const QModelIndex &modelIndex, int start, int end);

    void anchorClicked(QUrl url);

    void openPreferences();
    void newServerWindow();
    void showAboutInfo();
};

#endif // MAINWINDOW_H
