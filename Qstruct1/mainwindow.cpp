// mainwindow.cpp
#include "mainwindow.h"
#include <QFile>
#include <QTextStream>

const QString MainWindow::DATA_LOCATION = "C:\\Users\\ASUS\\Desktop\\data.txt";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    recommendButton = new QPushButton("推荐", this);
        searchButton = new QPushButton("搜索", this);
        sortLabel = new QLabel("排序关键字:", this);
        interestLabel = new QLabel("兴趣类型:", this);
        sortLineEdit = new QLineEdit(this);
        interestLineEdit = new QLineEdit(this);
        outputTextEdit = new QTextEdit(this);


    // 设置控件位置
        recommendButton->setGeometry(300, 100, 200, 50);
        searchButton->setGeometry(300, 160, 200, 50);
        sortLabel->setGeometry(200, 250, 100, 30);
        interestLabel->setGeometry(200, 300, 100, 30);
        sortLineEdit->setGeometry(300, 250, 200, 30);
        interestLineEdit->setGeometry(300, 300, 200, 30);
        outputTextEdit->setGeometry(100, 350, 600, 400);

    // 连接槽函数
//    connect(recommendButton, &QPushButton::clicked, this, &MainWindow::recommendButtonClicked);
    connect(recommendButton, SIGNAL(clicked()),
            this, SIGNAL(recommendButtonClicked()));
//    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchButtonClicked);
    connect(searchButton, SIGNAL(clicked), this, SLOT(searchButtonClicked()));



    resize(800, 600);
}

MainWindow::~MainWindow()
{
}

void MainWindow::recommendButtonClicked()
{
    int sortKey = sortLineEdit->text().toInt();
    int interestType = interestLineEdit->text().toInt();
    recommendPlaces(places, sortKey, interestType);
}

void MainWindow::searchButtonClicked()
{
    QString input = outputTextEdit->toPlainText();
    int sortKey = sortLineEdit->text().toInt();
    searchAndSortPlaces(input, sortKey);
}
int MainWindow::mapInputToType(const QString &input) {
    if (input == "休闲娱乐") return 4;
    if (input == "名胜古迹") return 5;
    if (input == "踏青公园") return 6;
    if (input == "博物馆") return 7;
    if (input == "高校") return 2;
    if (input == "景点") return 1; // 特殊标记，代表搜索1、2、3、4类型的景点
    return -1; // 没有匹配的类别，返回-1
}
void MainWindow::loadPlacesFromFile()
{
    QFile file(DATA_LOCATION);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file" << DATA_LOCATION;
        return;
    }

    QTextStream in(&file);
    QString line;
    while (!in.atEnd())
    {
        line = in.readLine();
        QStringList fields = line.split(" ");
        if (fields.size() >= 4)
        {
            Place place;
            strncpy(place.name, fields[0].toStdString().c_str(), 99);
            place.popularity = fields[1].toFloat();
            place.review = fields[2].toFloat();
            place.type = fields[3].toInt();
            places.append(place);
        }
    }
    file.close();
}

void MainWindow::recommendPlaces(const QVector<Place> &places, int sortKey, int interestType)
{
    // 清空之前的推荐结果
    outputTextEdit->clear();

    // 加载景点信息
    loadPlacesFromFile();

    // 过滤和排序
    QVector<Place> filteredPlaces;
    for (const auto &place : places)
    {
        // 根据兴趣类型过滤
        if (place.type == interestType)
        {
            filteredPlaces.append(place);
        }
    }

    // 排序
    if (sortKey == 1)
    {
        std::sort(filteredPlaces.begin(), filteredPlaces.end(), [](const Place &a, const Place &b) {
            return a.popularity > b.popularity;
        });
    }
    else if (sortKey == 2)
    {
        std::sort(filteredPlaces.begin(), filteredPlaces.end(), [](const Place &a, const Place &b) {
            return a.review > b.review;
        });
    }

    // 打印推荐结果
    for (int i = 0; i < qMin(filteredPlaces.size(), 10); ++i)
    {
        outputTextEdit->append(QString("%1: 热度: %2, 评价: %3, 种类: %4")
                               .arg(filteredPlaces[i].name)
                               .arg(filteredPlaces[i].popularity, 0, 'f', 1)
                               .arg(filteredPlaces[i].review, 0, 'f', 1)
                               .arg(filteredPlaces[i].type));
    }
}


void MainWindow::searchAndSortPlaces(const QString &input, int sortKey)
{
    // 清空之前的搜索结果
    outputTextEdit->clear();

    // 加载景点信息
    loadPlacesFromFile();

    int type = mapInputToType(input.toStdString().c_str()); // 将输入映射到类型

    // 过滤和搜索逻辑
    QVector<int> filteredIndexes; // 存储过滤后景点的索引
    for (int i = 0; i < places.size(); i++)
    {
        bool matches = (type == -1 && QString(places[i].name).contains(input)) ||
                       (type == 1 && (places[i].type >= 1 && places[i].type <= 4)) ||
                       (places[i].type == type - 3 || (type == 2) && (places[i].type == 5));
        if (matches)
        {
            filteredIndexes.append(i);
        }
    }

    if (filteredIndexes.isEmpty())
    {
        outputTextEdit->append("没有找到匹配的景点。");
        return;
    }

    // 让用户选择排序方式
    qDebug() << "请输入1个数字,1:按热度排序; 2:按评价排序";
    if (sortKey == 1)
    {
        // 使用快速选择找到前k个元素
        for (int i = 0; i < filteredIndexes.size() - 1; i++)
        {
            for (int j = 0; j < filteredIndexes.size() - i - 1; j++)
            {
                Place *p1 = &places[filteredIndexes[j]], *p2 = &places[filteredIndexes[j + 1]];
                bool swapCondition = (p1->popularity < p2->popularity);
                if (swapCondition)
                {
                    int temp = filteredIndexes[j];
                    filteredIndexes[j] = filteredIndexes[j + 1];
                    filteredIndexes[j + 1] = temp;
                }
            }
        }
    }
    else if (sortKey == 2)
    {
        // 使用快速选择找到前k个元素
        for (int i = 0; i < filteredIndexes.size() - 1; i++)
        {
            for (int j = 0; j < filteredIndexes.size() - i - 1; j++)
            {
                Place *p1 = &places[filteredIndexes[j]], *p2 = &places[filteredIndexes[j + 1]];
                bool swapCondition = (p1->review < p2->review);
                if (swapCondition)
                {
                    int temp = filteredIndexes[j];
                    filteredIndexes[j] = filteredIndexes[j + 1];
                    filteredIndexes[j + 1] = temp;
                }
            }
        }
    }

    // 打印排序后的结果
    for (int i = 0; i < filteredIndexes.size(); i++)
    {
        Place *place = &places[filteredIndexes[i]];
        outputTextEdit->append(QString("%1: 热度: %2, 评价: %3, 种类: %4")
                               .arg(place->name)
                               .arg(place->popularity, 0, 'f', 1)
                               .arg(place->review, 0, 'f', 1)
                               .arg(place->type));
    }
}
