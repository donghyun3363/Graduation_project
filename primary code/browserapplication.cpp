/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/
//#include "boost/lambda/lambda.hpp"
//#include "boost/graph/graphviz.hpp"



#include "browserapplication.h"

#include "bookmarks.h"
#include "browsermainwindow.h"
#include "cookiejar.h"
#include "downloadmanager.h"
#include "history.h"
#include "networkaccessmanager.h"
#include "tabwidget.h"
#include "webview.h"

#include <QtCore/QBuffer>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>

#include <QtGui/QDesktopServices>
#include <QtGui/QFileOpenEvent>
#include <QtWidgets/QMessageBox>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QSslSocket>

#include <QWebSettings>

#include <QtCore/QDebug>


//typedef std::pair<int, int> Edge;


Graph *BrowserApplication::s_g_head = 0;
DownloadManager *BrowserApplication::s_downloadManager = 0;
HistoryManager *BrowserApplication::s_historyManager = 0;
NetworkAccessManager *BrowserApplication::s_networkAccessManager = 0;
BookmarksManager *BrowserApplication::s_bookmarksManager = 0;

static void showHelp(QCommandLineParser &parser, const QString errorMessage = QString())
{
    QString text;
    QTextStream str(&text);
    str << "<html><head/><body>";
    if (!errorMessage.isEmpty())
        str << errorMessage;
    str << "<pre>" << parser.helpText() << "</pre></body></html>";
    QMessageBox box(errorMessage.isEmpty() ? QMessageBox::Information : QMessageBox::Warning,
        QGuiApplication::applicationDisplayName(), text, QMessageBox::Ok);
    box.setTextInteractionFlags(Qt::TextBrowserInteraction);
    box.exec();
}

BrowserApplication::BrowserApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_localServer(0)
    , m_initialUrl(QString())
    , m_correctlyInitialized(false)
{
    QCoreApplication::setOrganizationName(QLatin1String("Qt"));
    QCoreApplication::setApplicationName(QLatin1String("demobrowser"));
    QCoreApplication::setApplicationVersion(QLatin1String("0.1"));

    QCommandLineParser commandLineParser;
    commandLineParser.addPositionalArgument(QStringLiteral("url"),
        QStringLiteral("The url to be loaded in the browser window."));

    if (!commandLineParser.parse(QCoreApplication::arguments())) {
        showHelp(commandLineParser, QStringLiteral("<p>Invalid argument</p>"));
        return;
    }

    QStringList args = commandLineParser.positionalArguments();
    if (args.count() > 1) {
        showHelp(commandLineParser, QStringLiteral("<p>Too many arguments.</p>"));
        return;
    } else if (args.count() == 1) {
        m_initialUrl = args.at(0);
    }
    if (!m_initialUrl.isEmpty() && !QUrl::fromUserInput(m_initialUrl).isValid()) {
        showHelp(commandLineParser, QString("<p>%1 is not a valid url</p>").arg(m_initialUrl));
        return;
    }

    m_correctlyInitialized = true;

    QString serverName = QCoreApplication::applicationName();
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        QTextStream stream(&socket);
        stream << m_initialUrl;
        stream.flush();
        socket.waitForBytesWritten();
        return;
    }

#if defined(Q_OS_OSX)
    QApplication::setQuitOnLastWindowClosed(false);
#else
    QApplication::setQuitOnLastWindowClosed(true);
#endif

    m_localServer = new QLocalServer(this);
    connect(m_localServer, SIGNAL(newConnection()),
            this, SLOT(newLocalSocketConnection()));
    if (!m_localServer->listen(serverName)) {
        if (m_localServer->serverError() == QAbstractSocket::AddressInUseError
            && QFile::exists(m_localServer->serverName())) {
            QFile::remove(m_localServer->serverName());
            m_localServer->listen(serverName);
        }
    }

#ifndef QT_NO_OPENSSL
    if (!QSslSocket::supportsSsl()) {
    QMessageBox::information(0, "Demo Browser",
                 "This system does not support OpenSSL. SSL websites will not be available.");
    }
#endif

    QDesktopServices::setUrlHandler(QLatin1String("http"), this, "openUrl");
    QString localSysName = QLocale::system().name();

    installTranslator(QLatin1String("qt_") + localSysName);

    QSettings settings;
    settings.beginGroup(QLatin1String("sessions"));
    m_lastSession = settings.value(QLatin1String("lastSession")).toByteArray();
    settings.endGroup();

#if defined(Q_OS_OSX)
    connect(this, SIGNAL(lastWindowClosed()),
            this, SLOT(lastWindowClosed()));
#endif

    QTimer::singleShot(0, this, SLOT(postLaunch()));
}
 
BrowserApplication::~BrowserApplication()
{
/////////////////// intelligent bowser start
///히스토리 파일에 그래프 쓰기 
   ifstream fin;
   ifstream temp_fin;
   char r_line[250]={0,};
   int line=0;
   int over_node=0;
   int window_size = 10000;
   

   timer.start();
   ofstream fout;
   ofstream temp_fout;
    Node* history = new Node;
    my_queue* temp_queue = new my_queue;

    fout.open("history/history.txt",ios::app);
    while(BrowserApplication::gethead()->G_queue->Count !=0)
    {
        temp_queue =BrowserApplication::gethead()->G_queue->Dequeue(BrowserApplication::gethead()->G_queue);
        history=temp_queue->get_node();
        fout<<"url* " <<history->url <<endl; 
        fout<< "time* " << ctime(&temp_queue->cur_time)<<endl; 
    }
    fout.close();
    fin.open("history/history.txt");
    
    while(!fin.eof()){
	line++;
        fin.getline(r_line,250);
	
    }
    fin.close();
    line/=3;
        if(line+BrowserApplication::gethead()->G_queue->Count > window_size){
	temp_fin.open("history/history.txt");
        over_node = (line+BrowserApplication::gethead()->G_queue->Count - window_size)*3;
     
        temp_fout.open("history/temp_history.txt");
        while(!temp_fin.eof())
        {
            temp_fin.getline(r_line,250);
            if(over_node<=0)
            {
	       if(over_node!=0)
	       	temp_fout<<endl;
               temp_fout<<r_line;
            }
            over_node--;
        }
        temp_fin.close();
        temp_fout.close();
	
        line=rename("history/temp_history.txt","history/history.txt"); 
	}
///// 히스토리 파일에 그래프 쓰기 끝
/////////////////// intelligent bowser end
    delete s_downloadManager;
    for (int i = 0; i < m_mainWindows.size(); ++i) {
        BrowserMainWindow *window = m_mainWindows.at(i);
        delete window;
    }
    delete s_networkAccessManager;
    delete s_bookmarksManager;
}

#if defined(Q_OS_OSX)
void BrowserApplication::lastWindowClosed()
{
    clean();
    BrowserMainWindow *mw = new BrowserMainWindow;
    mw->slotHome();
    m_mainWindows.prepend(mw);
}
#endif

BrowserApplication *BrowserApplication::instance()
{
    return (static_cast<BrowserApplication *>(QCoreApplication::instance()));
}

#if defined(Q_OS_OSX)
#include <QtWidgets/QMessageBox>
void BrowserApplication::quitBrowser()
{
    clean();
    int tabCount = 0;
    for (int i = 0; i < m_mainWindows.count(); ++i) {
        tabCount =+ m_mainWindows.at(i)->tabWidget()->count();
    }

    if (tabCount > 1) {
        int ret = QMessageBox::warning(mainWindow(), QString(),
                           tr("There are %1 windows and %2 tabs open\n"
                              "Do you want to quit anyway?").arg(m_mainWindows.count()).arg(tabCount),
                           QMessageBox::Yes | QMessageBox::No,
                           QMessageBox::No);
        if (ret == QMessageBox::No)
            return;
    }

    exit(0);
}
#endif

/*!
    Any actions that can be delayed until the window is visible
 */
void BrowserApplication::postLaunch()
{
    QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    QWebSettings::setIconDatabasePath(directory);
    QWebSettings::setOfflineStoragePath(directory);
    setWindowIcon(QIcon(QLatin1String(":browser.svg")));
    loadSettings();
/////////////////// intelligent bowser start 
//히스토리 파일 읽어서 그래프 생성
//////////////////////////////make Graph
	int i=0;
    	int found_url = 0;
    	char url_data[250] = { 0, };
    	char* temp_token;

	Node* temp_node2 = NULL;
	Node* temp_node3 = NULL;

	timer.start();
	ifstream fin;
	fin.open("history/history.txt");
   	while(!fin.eof())
    	{
		fin.getline(url_data,250);
		if (strlen(url_data) == 0) continue;
		temp_token=strtok(url_data,"* \n");

		if (strcmp(temp_token, "url") == 0)
		{
			i++;
			temp_token = strtok(NULL, " ");

            		BrowserApplication::gethead()->set_temp_node(BrowserApplication::gethead()->count(temp_node2, temp_token));
            		temp_node2= BrowserApplication::gethead()->get_temp_node();
			
		}
     	}//make graph	
	fin.close();
///////////////////////////////////////////////////////////////////////////////////
boost_graph g;
boost_graph::vertex_descriptor v[11] ={1,2,3,4,5,6,7,8,9,10,11};
for(int i=0 ; i<10; i++){
	v[i] = add_vertex(g); 
}
//boost_graph::vertex_descriptor v0 = add_vertex(g);  //index
//boost_graph::vertex_descriptor v1 = add_vertex(g);
//boost_graph::vertex_descriptor v2 = add_vertex(g);

g[v[0]].url = new char[100];
strcpy(g[v[0]].url,"fafasdfsad");

g[v[1]].url = new char[100];
strcpy(g[v[1]].url,"fuck");

add_edge(v[0],v[1],g);
add_edge(v[1],v[2],g);

g[v[3]].url = new char[100];
strcpy(g[v[3]].url,"fuck33333");


/////노드 만들기
vertex(v[10],g);
g[v[10]].url = new char[100];
strcpy(g[v[10]].url,"fuck");
//////////////////////////

for(int i=0 ; i<10; i++){
//	cout<< i <<"   "   <<endl<<endl;
	if(g[v[i]].url==NULL) continue;
	if( strcmp(g[v[i]].url,g[v[10]].url)==0){
		strcpy(g[v[i]].url,"23");;
	}
}


ofstream outf("min.dot");
dynamic_properties dp;
dp.property("label", get(&Vertex::url, g) );
dp.property("lwert2", get(&Vertex::url, g) );
dp.property("node_id", get(&Vertex::url, g));
dp.property("label",  get(&Edge::label,g));
write_graphviz_dp(outf, g, dp);

////////////////////////////////////////////////////////////////////////////////////////
	long msecs = timer.elapsed();
	long nsecs = timer.nsecsElapsed();
printf(" ---------------------------------------------------------------\n");
	    printf("\nfile read time : %ld msecs %ld nsecs \n\n",msecs , nsecs);
printf(" ---------------------------------------------------------------\n");



/////////////////// intelligent bowser end



    if (m_mainWindows.count() > 0) {
        if (!m_initialUrl.isEmpty()){
            mainWindow()->loadPage(m_initialUrl);
	}
        else
            {
		mainWindow()->slotHome();
	    }
    }
    BrowserApplication::historyManager();
}
Graph* BrowserApplication::gethead(){
         if (!s_g_head) {
        	s_g_head = new Graph;
	}
	return s_g_head;
}
void BrowserApplication::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("websettings"));

    QWebSettings *defaultSettings = QWebSettings::globalSettings();
    QString standardFontFamily = defaultSettings->fontFamily(QWebSettings::StandardFont);
    int standardFontSize = defaultSettings->fontSize(QWebSettings::DefaultFontSize);
    QFont standardFont = QFont(standardFontFamily, standardFontSize);
    standardFont = qvariant_cast<QFont>(settings.value(QLatin1String("standardFont"), standardFont));
    defaultSettings->setFontFamily(QWebSettings::StandardFont, standardFont.family());
    defaultSettings->setFontSize(QWebSettings::DefaultFontSize, standardFont.pointSize());

    QString fixedFontFamily = defaultSettings->fontFamily(QWebSettings::FixedFont);
    int fixedFontSize = defaultSettings->fontSize(QWebSettings::DefaultFixedFontSize);
    QFont fixedFont = QFont(fixedFontFamily, fixedFontSize);
    fixedFont = qvariant_cast<QFont>(settings.value(QLatin1String("fixedFont"), fixedFont));
    defaultSettings->setFontFamily(QWebSettings::FixedFont, fixedFont.family());
    defaultSettings->setFontSize(QWebSettings::DefaultFixedFontSize, fixedFont.pointSize());

    defaultSettings->setAttribute(QWebSettings::JavascriptEnabled, settings.value(QLatin1String("enableJavascript"), true).toBool());
    defaultSettings->setAttribute(QWebSettings::PluginsEnabled, settings.value(QLatin1String("enablePlugins"), true).toBool());

    QUrl url = settings.value(QLatin1String("userStyleSheet")).toUrl();
    defaultSettings->setUserStyleSheetUrl(url);

    defaultSettings->setAttribute(QWebSettings::DnsPrefetchEnabled, true);

    settings.endGroup();
}

QList<BrowserMainWindow*> BrowserApplication::mainWindows()
{
    clean();
    QList<BrowserMainWindow*> list;
    for (int i = 0; i < m_mainWindows.count(); ++i)
        list.append(m_mainWindows.at(i));
    return list;
}

void BrowserApplication::clean()
{
    // cleanup any deleted main windows first
    for (int i = m_mainWindows.count() - 1; i >= 0; --i)
        if (m_mainWindows.at(i).isNull())
            m_mainWindows.removeAt(i);
}

void BrowserApplication::saveSession()
{
    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        return;

    clean();

    QSettings settings;
    settings.beginGroup(QLatin1String("sessions"));

    QByteArray data;
    QBuffer buffer(&data);
    QDataStream stream(&buffer);
    buffer.open(QIODevice::ReadWrite);

    stream << m_mainWindows.count();
    for (int i = 0; i < m_mainWindows.count(); ++i)
        stream << m_mainWindows.at(i)->saveState();
    settings.setValue(QLatin1String("lastSession"), data);
    settings.endGroup();
}

bool BrowserApplication::canRestoreSession() const
{
    return !m_lastSession.isEmpty();
}

void BrowserApplication::restoreLastSession()
{
    QList<QByteArray> windows;
    QBuffer buffer(&m_lastSession);
    QDataStream stream(&buffer);
    buffer.open(QIODevice::ReadOnly);
    int windowCount;
    stream >> windowCount;
    for (int i = 0; i < windowCount; ++i) {
        QByteArray windowState;
        stream >> windowState;
        windows.append(windowState);
    }
    for (int i = 0; i < windows.count(); ++i) {
        BrowserMainWindow *newWindow = 0;
        if (m_mainWindows.count() == 1
            && mainWindow()->tabWidget()->count() == 1
            && mainWindow()->currentTab()->url() == QUrl()) {
            newWindow = mainWindow();
        } else {
            newWindow = newMainWindow();
        }
        newWindow->restoreState(windows.at(i));
    }
}

bool BrowserApplication::isTheOnlyBrowser() const
{
    return (m_localServer != 0);
}

bool BrowserApplication::isCorrectlyInitialized() const
{
    return m_correctlyInitialized;
}

void BrowserApplication::installTranslator(const QString &name)
{
    QTranslator *translator = new QTranslator(this);
    translator->load(name, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::installTranslator(translator);
}

#if defined(Q_OS_OSX)
bool BrowserApplication::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::ApplicationActivate: {
        clean();
        if (!m_mainWindows.isEmpty()) {
            BrowserMainWindow *mw = mainWindow();
            if (mw && !mw->isMinimized()) {
                mainWindow()->show();
            }
            return true;
        }
    }
    case QEvent::FileOpen:
        if (!m_mainWindows.isEmpty()) {
            mainWindow()->loadPage(static_cast<QFileOpenEvent *>(event)->file());
            return true;
        }
    default:
        break;
    }
    return QApplication::event(event);
}
#endif

void BrowserApplication::openUrl(const QUrl &url)
{
    mainWindow()->loadPage(url.toString());
}

BrowserMainWindow *BrowserApplication::newMainWindow()
{
    BrowserMainWindow *browser = new BrowserMainWindow();
    m_mainWindows.prepend(browser);
    browser->show();
    return browser;
}

BrowserMainWindow *BrowserApplication::mainWindow()
{
    clean();
    if (m_mainWindows.isEmpty())
        newMainWindow();
    return m_mainWindows[0];
}

void BrowserApplication::newLocalSocketConnection()
{
    QLocalSocket *socket = m_localServer->nextPendingConnection();
    if (!socket)
        return;
    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    QString url;
    stream >> url;
    if (!url.isEmpty()) {
        QSettings settings;
        settings.beginGroup(QLatin1String("general"));
        int openLinksIn = settings.value(QLatin1String("openLinksIn"), 0).toInt();
        settings.endGroup();
        if (openLinksIn == 1)
            newMainWindow();
        else
            mainWindow()->tabWidget()->newTab();
        openUrl(url);
    }
    delete socket;
    mainWindow()->raise();
    mainWindow()->activateWindow();
}

CookieJar *BrowserApplication::cookieJar()
{
    return (CookieJar*)networkAccessManager()->cookieJar();
}

DownloadManager *BrowserApplication::downloadManager()
{
    if (!s_downloadManager) {
        s_downloadManager = new DownloadManager();
    }
    return s_downloadManager;
}

NetworkAccessManager *BrowserApplication::networkAccessManager()
{
    if (!s_networkAccessManager) {
        s_networkAccessManager = new NetworkAccessManager();
        s_networkAccessManager->setCookieJar(new CookieJar);
    }
    return s_networkAccessManager;
}

HistoryManager *BrowserApplication::historyManager()
{
    if (!s_historyManager) {
        s_historyManager = new HistoryManager();
        QWebHistoryInterface::setDefaultInterface(s_historyManager);
    }
    return s_historyManager;
}

BookmarksManager *BrowserApplication::bookmarksManager()
{
    if (!s_bookmarksManager) {
        s_bookmarksManager = new BookmarksManager;
    }
    return s_bookmarksManager;
}

QIcon BrowserApplication::icon(const QUrl &url) const
{
    QIcon icon = QWebSettings::iconForUrl(url);
    if (!icon.isNull())
        return icon.pixmap(16, 16);
    if (m_defaultIcon.isNull())
        m_defaultIcon = QIcon(QLatin1String(":defaulticon.png"));
    return m_defaultIcon.pixmap(16, 16);
}

