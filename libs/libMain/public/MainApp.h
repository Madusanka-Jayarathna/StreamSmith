#pragma once
#include <iostream>

class MainApp {
public:
    void run();
    
private:
    void showProductTitle();
    int selectedProduct();
};



/*

#pragma once
#include <QMainWindow>
#include <QObject>
#include <QWidget>

class QStackedWidget;
class MainView;
class DownloaderView;

class MainApp : public QMainWindow {
    Q_OBJECT
public:
    explicit MainApp(QWidget* parent = nullptr);
   
private slots:
    void showDownloaderView();
    void showMainView();

private:
    QStackedWidget* m_stackedWidget{nullptr};
    MainView* m_mainView{nullptr};
    DownloaderView* m_downloaderView{nullptr};
};

*/
