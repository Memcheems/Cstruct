#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QDebug>
#include <QVector>

typedef struct {
    char name[100];
    float popularity; // 游学热度
    float review; // 评价
    int type; // 景点类型：1代表休闲娱乐，2代表名胜古迹，3代表踏青公园，4代表博物馆，5代表高校
} Place;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void recommendButtonClicked();
    void searchButtonClicked();

private:
    void loadPlacesFromFile();
    void recommendPlaces(const QVector<Place> &places, int sortKey, int interestType);
    void searchAndSortPlaces(const QString &input, int sortKey);
    int mapInputToType(const QString &input);

    QPushButton *recommendButton;
    QPushButton *searchButton;
    QLabel *sortLabel;
    QLabel *interestLabel;
    QLineEdit *sortLineEdit;
    QLineEdit *interestLineEdit;
    QTextEdit *outputTextEdit;

    QVector<Place> places; // 存储景点信息的向量

    static const QString DATA_LOCATION; // 文件路径
};

#endif // MAINWINDOW_H
