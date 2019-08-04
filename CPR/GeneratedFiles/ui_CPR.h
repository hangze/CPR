/********************************************************************************
** Form generated from reading UI file 'CPR.ui'
**
** Created by: Qt User Interface Compiler version 5.9.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CPR_H
#define UI_CPR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CPRClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *readImgBtn;
    QPushButton *nextBtn;
    QPushButton *batchTestBtn;
    QWidget *tab_2;
    QWidget *tab_3;
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_2;
    QLabel *label_9;
    QLineEdit *chooseModelPath;
    QPushButton *startTrainBtn;
    QToolButton *negaPathBtn;
    QToolButton *posPathBtn;
    QLabel *label_6;
    QLabel *label_8;
    QToolButton *saveModelPathBtn;
    QLabel *label_7;
    QLineEdit *saveModelPath;
    QLabel *label_10;
    QToolButton *chooseModelBtn;
    QToolButton *testSamplePathBtn;
    QLineEdit *testSamplePath;
    QPushButton *modelTestBtn;
    QLineEdit *posiPath;
    QLineEdit *negaPath;
    QLabel *label_11;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer;
    QMenuBar *menuBar;
    QMenu *menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CPRClass)
    {
        if (CPRClass->objectName().isEmpty())
            CPRClass->setObjectName(QStringLiteral("CPRClass"));
        CPRClass->resize(870, 783);
        CPRClass->setMinimumSize(QSize(833, 666));
        centralWidget = new QWidget(CPRClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_3 = new QGridLayout(centralWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout = new QGridLayout(tab);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(tab);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMinimumSize(QSize(0, 0));
        label->setLayoutDirection(Qt::LeftToRight);
        label->setScaledContents(true);

        horizontalLayout->addWidget(label);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(136, 36));

        verticalLayout->addWidget(label_2);

        label_3 = new QLabel(tab);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);

        label_4 = new QLabel(tab);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout->addWidget(label_4);

        label_5 = new QLabel(tab);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout->addWidget(label_5);


        horizontalLayout->addLayout(verticalLayout);

        horizontalLayout->setStretch(0, 4);
        horizontalLayout->setStretch(1, 1);

        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        readImgBtn = new QPushButton(tab);
        readImgBtn->setObjectName(QStringLiteral("readImgBtn"));

        horizontalLayout_2->addWidget(readImgBtn);

        nextBtn = new QPushButton(tab);
        nextBtn->setObjectName(QStringLiteral("nextBtn"));

        horizontalLayout_2->addWidget(nextBtn);

        batchTestBtn = new QPushButton(tab);
        batchTestBtn->setObjectName(QStringLiteral("batchTestBtn"));

        horizontalLayout_2->addWidget(batchTestBtn);


        verticalLayout_2->addLayout(horizontalLayout_2);


        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tab_2->sizePolicy().hasHeightForWidth());
        tab_2->setSizePolicy(sizePolicy1);
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout_4 = new QGridLayout(tab_3);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_9 = new QLabel(tab_3);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout_2->addWidget(label_9, 5, 0, 1, 1);

        chooseModelPath = new QLineEdit(tab_3);
        chooseModelPath->setObjectName(QStringLiteral("chooseModelPath"));

        gridLayout_2->addWidget(chooseModelPath, 5, 1, 1, 1);

        startTrainBtn = new QPushButton(tab_3);
        startTrainBtn->setObjectName(QStringLiteral("startTrainBtn"));

        gridLayout_2->addWidget(startTrainBtn, 3, 3, 1, 1);

        negaPathBtn = new QToolButton(tab_3);
        negaPathBtn->setObjectName(QStringLiteral("negaPathBtn"));

        gridLayout_2->addWidget(negaPathBtn, 2, 2, 1, 1);

        posPathBtn = new QToolButton(tab_3);
        posPathBtn->setObjectName(QStringLiteral("posPathBtn"));

        gridLayout_2->addWidget(posPathBtn, 1, 2, 1, 1);

        label_6 = new QLabel(tab_3);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_2->addWidget(label_6, 1, 0, 1, 1);

        label_8 = new QLabel(tab_3);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_2->addWidget(label_8, 6, 0, 1, 1);

        saveModelPathBtn = new QToolButton(tab_3);
        saveModelPathBtn->setObjectName(QStringLiteral("saveModelPathBtn"));

        gridLayout_2->addWidget(saveModelPathBtn, 3, 2, 1, 1);

        label_7 = new QLabel(tab_3);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_2->addWidget(label_7, 2, 0, 1, 1);

        saveModelPath = new QLineEdit(tab_3);
        saveModelPath->setObjectName(QStringLiteral("saveModelPath"));

        gridLayout_2->addWidget(saveModelPath, 3, 1, 1, 1);

        label_10 = new QLabel(tab_3);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout_2->addWidget(label_10, 3, 0, 1, 1);

        chooseModelBtn = new QToolButton(tab_3);
        chooseModelBtn->setObjectName(QStringLiteral("chooseModelBtn"));

        gridLayout_2->addWidget(chooseModelBtn, 5, 2, 1, 1);

        testSamplePathBtn = new QToolButton(tab_3);
        testSamplePathBtn->setObjectName(QStringLiteral("testSamplePathBtn"));

        gridLayout_2->addWidget(testSamplePathBtn, 6, 2, 1, 1);

        testSamplePath = new QLineEdit(tab_3);
        testSamplePath->setObjectName(QStringLiteral("testSamplePath"));

        gridLayout_2->addWidget(testSamplePath, 6, 1, 1, 1);

        modelTestBtn = new QPushButton(tab_3);
        modelTestBtn->setObjectName(QStringLiteral("modelTestBtn"));

        gridLayout_2->addWidget(modelTestBtn, 6, 3, 1, 1);

        posiPath = new QLineEdit(tab_3);
        posiPath->setObjectName(QStringLiteral("posiPath"));

        gridLayout_2->addWidget(posiPath, 1, 1, 1, 1);

        negaPath = new QLineEdit(tab_3);
        negaPath->setObjectName(QStringLiteral("negaPath"));

        gridLayout_2->addWidget(negaPath, 2, 1, 1, 1);

        label_11 = new QLabel(tab_3);
        label_11->setObjectName(QStringLiteral("label_11"));

        gridLayout_2->addWidget(label_11, 0, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Maximum);

        gridLayout_2->addItem(verticalSpacer_2, 4, 1, 1, 1);


        gridLayout_4->addLayout(gridLayout_2, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer, 1, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());

        gridLayout_3->addWidget(tabWidget, 0, 0, 1, 1);

        CPRClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(CPRClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 870, 30));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        CPRClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(CPRClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        CPRClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(CPRClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        CPRClass->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());

        retranslateUi(CPRClass);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(CPRClass);
    } // setupUi

    void retranslateUi(QMainWindow *CPRClass)
    {
        CPRClass->setWindowTitle(QApplication::translate("CPRClass", "CPR", Q_NULLPTR));
        label->setText(QApplication::translate("CPRClass", "\345\216\237\345\233\276", Q_NULLPTR));
        label_2->setText(QApplication::translate("CPRClass", "\345\256\232\344\275\215\350\275\246\347\211\2141", Q_NULLPTR));
        label_3->setText(QApplication::translate("CPRClass", "\350\275\246\347\211\214\345\255\227\347\254\2461", Q_NULLPTR));
        label_4->setText(QApplication::translate("CPRClass", "\350\275\246\347\211\214\345\233\276\345\203\2172", Q_NULLPTR));
        label_5->setText(QApplication::translate("CPRClass", "\350\275\246\347\211\214\345\255\227\347\254\2462", Q_NULLPTR));
        readImgBtn->setText(QApplication::translate("CPRClass", "\350\257\273\345\217\226\345\233\276\347\211\207", Q_NULLPTR));
        nextBtn->setText(QApplication::translate("CPRClass", "\344\270\213\344\270\200\345\274\240", Q_NULLPTR));
        batchTestBtn->setText(QApplication::translate("CPRClass", "\346\211\271\351\207\217\346\265\213\350\257\225", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("CPRClass", "\344\270\273\351\241\265\351\235\242", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("CPRClass", "\345\217\202\346\225\260\351\205\215\347\275\256", Q_NULLPTR));
        label_9->setText(QApplication::translate("CPRClass", "\344\275\277\347\224\250\346\250\241\345\236\213", Q_NULLPTR));
        startTrainBtn->setText(QApplication::translate("CPRClass", "\345\274\200\345\247\213\350\256\255\347\273\203", Q_NULLPTR));
        negaPathBtn->setText(QApplication::translate("CPRClass", "...", Q_NULLPTR));
        posPathBtn->setText(QApplication::translate("CPRClass", "...", Q_NULLPTR));
        label_6->setText(QApplication::translate("CPRClass", "\346\255\243\346\240\267\346\234\254\350\267\257\345\276\204", Q_NULLPTR));
        label_8->setText(QApplication::translate("CPRClass", "\346\265\213\350\257\225\346\240\267\346\234\254\350\267\257\345\276\204", Q_NULLPTR));
        saveModelPathBtn->setText(QApplication::translate("CPRClass", "...", Q_NULLPTR));
        label_7->setText(QApplication::translate("CPRClass", "\350\264\237\346\240\267\346\234\254\350\267\257\345\276\204", Q_NULLPTR));
        label_10->setText(QApplication::translate("CPRClass", "\346\250\241\345\236\213\344\277\235\345\255\230\350\267\257\345\276\204", Q_NULLPTR));
        chooseModelBtn->setText(QApplication::translate("CPRClass", "...", Q_NULLPTR));
        testSamplePathBtn->setText(QApplication::translate("CPRClass", "...", Q_NULLPTR));
        modelTestBtn->setText(QApplication::translate("CPRClass", "\346\250\241\345\236\213\346\265\213\350\257\225", Q_NULLPTR));
        label_11->setText(QApplication::translate("CPRClass", "SVM\346\250\241\345\236\213", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("CPRClass", "\346\250\241\345\236\213\350\256\255\347\273\203", Q_NULLPTR));
        menu->setTitle(QApplication::translate("CPRClass", "\346\226\207\344\273\266", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CPRClass: public Ui_CPRClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CPR_H
