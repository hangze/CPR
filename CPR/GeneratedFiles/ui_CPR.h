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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CPRClass
{
public:
    QWidget *centralWidget;
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
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 823, 587));
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
        label->setMinimumSize(QSize(640, 480));
        label->setLayoutDirection(Qt::LeftToRight);

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
        tabWidget->addTab(tab_2, QString());
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

        tabWidget->setCurrentIndex(0);


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
        menu->setTitle(QApplication::translate("CPRClass", "\346\226\207\344\273\266", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CPRClass: public Ui_CPRClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CPR_H
