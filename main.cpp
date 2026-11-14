#include <QApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyle>
#include <QTabBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <QWidget>
#include <QDebug>

const QString bstyle = R"(
    QMainWindow { background-color: #f3f3f3; }
    #top_tab_bar_container { background-color: #f3f3f3; border-bottom: 1px solid #e0e0e0; padding: 4px 6px 0px 6px; }
    #header_bar { background-color: #ffffff; border-bottom: 1px solid #dcdcdc; padding: 6px 12px; }
    QPushButton { background: transparent; color: #3c4043; border: none; border-radius: 6px; padding: 6px 10px; font-size: 16px; }
    QPushButton:hover { background: #e8eaed; }
    QPushButton:pressed { background: #d2d6d9; }
    #clear_cache_btn { font-size: 14px; font-weight: 500; color: #1a73e8; background: #e8f0fe; padding: 6px 12px; border-radius: 6px; }
    #clear_cache_btn:hover { background: #d2e3fc; }
    QLineEdit { background-color: #f1f3f4; color: #202124; border: 1px solid transparent; border-radius: 18px; padding: 6px 16px; font-size: 14px; }
    QLineEdit:hover { background-color: #e8eaed; }
    QLineEdit:focus { background-color: #ffffff; border: 1px solid #1a73e8; }
    QTabBar { background: transparent; qproperty-drawBase: 0; }
    QTabBar::tab { background: #dee1e6; color: #5f6368; border: none; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 2px; padding: 6px 12px; font-size: 12px; min-width: 120px; max-width: 200px; height: 32px; }
    QTabBar::tab:selected { background: #ffffff; color: #202124; font-weight: 500; }
    QTabBar::tab:hover:!selected { background: #e4e7ea; }
)";

class WebBrowser : public QMainWindow {
public:
    WebBrowser(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Pir browser");
        setStyleSheet(bstyle);

        defaultIcon = style()->standardIcon(QStyle::SP_ComputerIcon);
        homeUrl = "https://github.com/alexbennett2007/pir";

        auto *mainWidget = new QWidget(this);
        setCentralWidget(mainWidget);
        auto *mainLayout = new QVBoxLayout(mainWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        tabBar = new QTabBar();
        tabBar->setMovable(true);
        tabBar->setTabsClosable(true);
        connect(tabBar, &QTabBar::currentChanged, this, &WebBrowser::switchTab);
        connect(tabBar, &QTabBar::tabCloseRequested, this, &WebBrowser::closeTabByIndex);

        auto *topContainer = new QWidget();
        topContainer->setObjectName("top_tab_bar_container");
        auto *topLayout = new QHBoxLayout(topContainer);
        topLayout->setContentsMargins(4, 0, 4, 0);
        topLayout->setSpacing(0);
        topLayout->addWidget(tabBar);
        topLayout->addStretch(1);

        auto *headerBar = new QWidget();
        headerBar->setObjectName("header_bar");
        auto *headerLayout = new QHBoxLayout(headerBar);
        headerLayout->setContentsMargins(8, 4, 8, 4);
        headerLayout->setSpacing(6);

        backBtn = new QPushButton("←");
        forwardBtn = new QPushButton("→");
        reloadBtn = new QPushButton("↻");

        urlBar = new QLineEdit();
        urlBar->setFont(QFont("Sans Serif", 10));
        urlBar->setPlaceholderText("Search or enter address");

        connect(backBtn, &QPushButton::clicked, this, [this]() {
            if (auto *web = qobject_cast<QWebEngineView*>(container->currentWidget())) web->back();
        });
        connect(forwardBtn, &QPushButton::clicked, this, [this]() {
            if (auto *web = qobject_cast<QWebEngineView*>(container->currentWidget())) web->forward();
        });
        connect(reloadBtn, &QPushButton::clicked, this, [this]() {
            if (auto *web = qobject_cast<QWebEngineView*>(container->currentWidget())) web->reload();
        });
        connect(urlBar, &QLineEdit::returnPressed, this, &WebBrowser::navigateToUrl);

        auto *addTabBtn = new QPushButton("+");
        addTabBtn->setCursor(Qt::PointingHandCursor);
        connect(addTabBtn, &QPushButton::clicked, this, [this]() { addNewTab(); });

        auto *clearCacheBtn = new QPushButton("Clear cache");
        clearCacheBtn->setObjectName("clear_cache_btn");
        clearCacheBtn->setCursor(Qt::PointingHandCursor);
        connect(clearCacheBtn, &QPushButton::clicked, this, &WebBrowser::clearBrowserCache);

        headerLayout->addWidget(backBtn);
        headerLayout->addWidget(forwardBtn);
        headerLayout->addWidget(reloadBtn);
        headerLayout->addWidget(urlBar);
        headerLayout->addWidget(addTabBtn);
        headerLayout->addWidget(clearCacheBtn);

        container = new QStackedWidget();
        mainLayout->addWidget(topContainer);
        mainLayout->addWidget(headerBar);
        mainLayout->addWidget(container);

        addNewTab(QUrl(homeUrl), "New Tab");
        showMaximized();
    }

private:
    QTabBar *tabBar;
    QLineEdit *urlBar;
    QPushButton *backBtn;
    QPushButton *forwardBtn;
    QPushButton *reloadBtn;
    QStackedWidget *container;
    QIcon defaultIcon;
    QString homeUrl;

    void addNewTab(const QUrl &qurl = QUrl(), const QString &label = "New Tab") {
        auto *browser = new QWebEngineView();
        browser->setUrl(qurl.isEmpty() ? QUrl(homeUrl) : qurl);

        int idx = container->addWidget(browser);
        tabBar->addTab(defaultIcon, label);
        tabBar->setCurrentIndex(idx);
        container->setCurrentIndex(idx);

        connect(browser, &QWebEngineView::urlChanged, this, [this, browser](const QUrl &q) {
            if (container->currentWidget() == browser) {
                urlBar->setText(q.toString());
            }
        });

        connect(browser, &QWebEngineView::titleChanged, this, [this, browser](const QString &title) {
            int tabIdx = container->indexOf(browser);
            QString shortTitle = title.length() > 16 ? title.left(14) + "..." : (title.isEmpty() ? "New Tab" : title);
            tabBar->setTabText(tabIdx, shortTitle);
        });

        connect(browser, &QWebEngineView::iconChanged, this, [this, browser](const QIcon &icon) {
            int tabIdx = container->indexOf(browser);
            tabBar->setTabIcon(tabIdx, icon.isNull() ? defaultIcon : icon);
        });
    }

    void switchTab(int index) {
        if (index != -1) {
            container->setCurrentIndex(index);
            if (auto *currentWeb = qobject_cast<QWebEngineView*>(container->widget(index))) {
                urlBar->setText(currentWeb->url().toString());
            }
        }
    }

    void closeTabByIndex(int index) {
        if (container->count() > 1) {
            QWidget *browserWidget = container->widget(index);
            tabBar->removeTab(index);
            container->removeWidget(browserWidget);
            browserWidget->deleteLater();
        }
    }

    void navigateToUrl() {
        QString text = urlBar->text().trimmed();
        if (!text.isEmpty()) {
            if (auto *currentWeb = qobject_cast<QWebEngineView*>(container->currentWidget())) {
                if (!text.startsWith("http://") && !text.startsWith("https://")) {
                    text = "https://duckduckgo.com/?q=" + text;
                }
                currentWeb->setUrl(QUrl(text));
            }
        }
    }

    void clearBrowserCache() {
        auto *profile = QWebEngineProfile::defaultProfile();
        profile->clearHttpCache();
        profile->cookieStore()->deleteAllCookies();
        qDebug() << "finished";
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    WebBrowser window;
    return app.exec();
}

