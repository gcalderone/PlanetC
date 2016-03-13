/********************************************************************************
** Form generated from reading UI file 'searchDialogGui.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEARCHDIALOGGUI_H
#define UI_SEARCHDIALOGGUI_H

#include <AngleSpinBox.hpp>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "Dialog.hpp"
#include "SearchDialog.hpp"

QT_BEGIN_NAMESPACE

class Ui_searchDialogForm
{
public:
    QVBoxLayout *verticalLayout;
    BarFrame *TitleBar;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QLabel *stelWindowTitle;
    QSpacerItem *spacerItem1;
    QPushButton *closeStelWindow;
    QTabWidget *tabWidget;
    QWidget *objectTab;
    QVBoxLayout *verticalLayout_2;
    QFrame *horizontalSearchLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEditSearchSkyObject;
    QPushButton *pushButtonGotoSearchSkyObject;
    CompletionLabel *completionLabel;
    QLabel *simbadStatusLabel;
    QFrame *greekLettersFrame;
    QGridLayout *gridLayout;
    QPushButton *iotaPushButton;
    QPushButton *alphaPushButton;
    QPushButton *betaPushButton;
    QPushButton *gammaPushButton;
    QPushButton *deltaPushButton;
    QPushButton *epsilonPushButton;
    QPushButton *zetaPushButton;
    QPushButton *etaPushButton;
    QPushButton *thetaPushButton;
    QPushButton *kappaPushButton;
    QPushButton *lambdaPushButton;
    QPushButton *muPushButton;
    QPushButton *nuPushButton;
    QPushButton *xiPushButton;
    QPushButton *omicronPushButton;
    QPushButton *piPushButton;
    QPushButton *rhoPushButton;
    QPushButton *sigmaPushButton;
    QPushButton *tauPushButton;
    QPushButton *upsilonPushButton;
    QPushButton *phiPushButton;
    QPushButton *chiPushButton;
    QPushButton *psiPushButton;
    QPushButton *omegaPushButton;
    QLabel *labelGreekLetterTitle;
    QWidget *tabPosition;
    QVBoxLayout *verticalLayout_3;
    QFrame *j2000Frame;
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *AxisYLayout;
    QLabel *AxisYLabel;
    AngleSpinBox *AxisYSpinBox;
    QHBoxLayout *coordinateSystemLayout;
    QLabel *coordinateSystemLabel;
    QComboBox *coordinateSystemComboBox;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *AxisXLayout;
    QLabel *AxisXLabel;
    AngleSpinBox *AxisXSpinBox;
    QLabel *noteLabel;
    QWidget *tabLists;
    QGridLayout *gridLayout_3;
    QComboBox *objectTypeComboBox;
    QListWidget *objectsListWidget;
    QCheckBox *searchInEnglishCheckBox;
    QLineEdit *searchInListLineEdit;
    QWidget *tabOptions;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *simbadGroupBox;
    QVBoxLayout *verticalLayout_5;
    QCheckBox *checkBoxUseSimbad;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelServer;
    QComboBox *serverListComboBox;
    QGroupBox *searchOptions;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *checkBoxUseStartOfWords;

    void setupUi(QWidget *searchDialogForm)
    {
        if (searchDialogForm->objectName().isEmpty())
            searchDialogForm->setObjectName(QStringLiteral("searchDialogForm"));
        searchDialogForm->resize(507, 213);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(searchDialogForm->sizePolicy().hasHeightForWidth());
        searchDialogForm->setSizePolicy(sizePolicy);
        searchDialogForm->setMinimumSize(QSize(390, 0));
        searchDialogForm->setFocusPolicy(Qt::ClickFocus);
        searchDialogForm->setStyleSheet(QStringLiteral(""));
        verticalLayout = new QVBoxLayout(searchDialogForm);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        TitleBar = new BarFrame(searchDialogForm);
        TitleBar->setObjectName(QStringLiteral("TitleBar"));
        TitleBar->setMinimumSize(QSize(16, 25));
        TitleBar->setMaximumSize(QSize(16777215, 25));
        TitleBar->setFocusPolicy(Qt::NoFocus);
        TitleBar->setAutoFillBackground(false);
        TitleBar->setFrameShape(QFrame::StyledPanel);
        hboxLayout = new QHBoxLayout(TitleBar);
        hboxLayout->setSpacing(0);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 4, 0);
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        stelWindowTitle = new QLabel(TitleBar);
        stelWindowTitle->setObjectName(QStringLiteral("stelWindowTitle"));

        hboxLayout->addWidget(stelWindowTitle);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem1);

        closeStelWindow = new QPushButton(TitleBar);
        closeStelWindow->setObjectName(QStringLiteral("closeStelWindow"));
        closeStelWindow->setMinimumSize(QSize(16, 16));
        closeStelWindow->setMaximumSize(QSize(16, 16));
        closeStelWindow->setFocusPolicy(Qt::NoFocus);

        hboxLayout->addWidget(closeStelWindow);


        verticalLayout->addWidget(TitleBar);

        tabWidget = new QTabWidget(searchDialogForm);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        objectTab = new QWidget();
        objectTab->setObjectName(QStringLiteral("objectTab"));
        verticalLayout_2 = new QVBoxLayout(objectTab);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSearchLayout = new QFrame(objectTab);
        horizontalSearchLayout->setObjectName(QStringLiteral("horizontalSearchLayout"));
        horizontalSearchLayout->setMinimumSize(QSize(0, 0));
        horizontalSearchLayout->setFrameShape(QFrame::StyledPanel);
        horizontalLayout = new QHBoxLayout(horizontalSearchLayout);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        lineEditSearchSkyObject = new QLineEdit(horizontalSearchLayout);
        lineEditSearchSkyObject->setObjectName(QStringLiteral("lineEditSearchSkyObject"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lineEditSearchSkyObject->sizePolicy().hasHeightForWidth());
        lineEditSearchSkyObject->setSizePolicy(sizePolicy1);
        QFont font;
        lineEditSearchSkyObject->setFont(font);
        lineEditSearchSkyObject->setFocusPolicy(Qt::StrongFocus);
        lineEditSearchSkyObject->setContextMenuPolicy(Qt::CustomContextMenu);
        lineEditSearchSkyObject->setAutoFillBackground(false);

        horizontalLayout->addWidget(lineEditSearchSkyObject);

        pushButtonGotoSearchSkyObject = new QPushButton(horizontalSearchLayout);
        pushButtonGotoSearchSkyObject->setObjectName(QStringLiteral("pushButtonGotoSearchSkyObject"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButtonGotoSearchSkyObject->sizePolicy().hasHeightForWidth());
        pushButtonGotoSearchSkyObject->setSizePolicy(sizePolicy2);
        pushButtonGotoSearchSkyObject->setMinimumSize(QSize(45, 30));
        pushButtonGotoSearchSkyObject->setMaximumSize(QSize(16777215, 30));
        pushButtonGotoSearchSkyObject->setFocusPolicy(Qt::NoFocus);
        pushButtonGotoSearchSkyObject->setIconSize(QSize(24, 24));
        pushButtonGotoSearchSkyObject->setCheckable(false);

        horizontalLayout->addWidget(pushButtonGotoSearchSkyObject);


        verticalLayout_2->addWidget(horizontalSearchLayout);

        completionLabel = new CompletionLabel(objectTab);
        completionLabel->setObjectName(QStringLiteral("completionLabel"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy3.setHorizontalStretch(35);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(completionLabel->sizePolicy().hasHeightForWidth());
        completionLabel->setSizePolicy(sizePolicy3);
        completionLabel->setMinimumSize(QSize(390, 0));
        completionLabel->setFrameShape(QFrame::NoFrame);
        completionLabel->setLineWidth(1);
        completionLabel->setScaledContents(false);
        completionLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        completionLabel->setWordWrap(true);
        completionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

        verticalLayout_2->addWidget(completionLabel);

        simbadStatusLabel = new QLabel(objectTab);
        simbadStatusLabel->setObjectName(QStringLiteral("simbadStatusLabel"));
        simbadStatusLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(simbadStatusLabel);

        greekLettersFrame = new QFrame(objectTab);
        greekLettersFrame->setObjectName(QStringLiteral("greekLettersFrame"));
        sizePolicy.setHeightForWidth(greekLettersFrame->sizePolicy().hasHeightForWidth());
        greekLettersFrame->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(greekLettersFrame);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        iotaPushButton = new QPushButton(greekLettersFrame);
        iotaPushButton->setObjectName(QStringLiteral("iotaPushButton"));
        iotaPushButton->setFocusPolicy(Qt::NoFocus);
        iotaPushButton->setText(QString::fromUtf8("\316\271"));

        gridLayout->addWidget(iotaPushButton, 1, 8, 1, 1);

        alphaPushButton = new QPushButton(greekLettersFrame);
        alphaPushButton->setObjectName(QStringLiteral("alphaPushButton"));
        alphaPushButton->setFocusPolicy(Qt::NoFocus);
        alphaPushButton->setText(QString::fromUtf8("\316\261"));

        gridLayout->addWidget(alphaPushButton, 1, 0, 1, 1);

        betaPushButton = new QPushButton(greekLettersFrame);
        betaPushButton->setObjectName(QStringLiteral("betaPushButton"));
        betaPushButton->setFocusPolicy(Qt::NoFocus);
        betaPushButton->setText(QString::fromUtf8("\316\262"));

        gridLayout->addWidget(betaPushButton, 1, 1, 1, 1);

        gammaPushButton = new QPushButton(greekLettersFrame);
        gammaPushButton->setObjectName(QStringLiteral("gammaPushButton"));
        gammaPushButton->setFocusPolicy(Qt::NoFocus);
        gammaPushButton->setText(QString::fromUtf8("\316\263"));

        gridLayout->addWidget(gammaPushButton, 1, 2, 1, 1);

        deltaPushButton = new QPushButton(greekLettersFrame);
        deltaPushButton->setObjectName(QStringLiteral("deltaPushButton"));
        deltaPushButton->setFocusPolicy(Qt::NoFocus);
        deltaPushButton->setText(QString::fromUtf8("\316\264"));

        gridLayout->addWidget(deltaPushButton, 1, 3, 1, 1);

        epsilonPushButton = new QPushButton(greekLettersFrame);
        epsilonPushButton->setObjectName(QStringLiteral("epsilonPushButton"));
        epsilonPushButton->setFocusPolicy(Qt::NoFocus);
        epsilonPushButton->setText(QString::fromUtf8("\316\265"));

        gridLayout->addWidget(epsilonPushButton, 1, 4, 1, 1);

        zetaPushButton = new QPushButton(greekLettersFrame);
        zetaPushButton->setObjectName(QStringLiteral("zetaPushButton"));
        zetaPushButton->setFocusPolicy(Qt::NoFocus);
        zetaPushButton->setText(QString::fromUtf8("\316\266"));

        gridLayout->addWidget(zetaPushButton, 1, 5, 1, 1);

        etaPushButton = new QPushButton(greekLettersFrame);
        etaPushButton->setObjectName(QStringLiteral("etaPushButton"));
        etaPushButton->setFocusPolicy(Qt::NoFocus);
        etaPushButton->setText(QString::fromUtf8("\316\267"));

        gridLayout->addWidget(etaPushButton, 1, 6, 1, 1);

        thetaPushButton = new QPushButton(greekLettersFrame);
        thetaPushButton->setObjectName(QStringLiteral("thetaPushButton"));
        thetaPushButton->setFocusPolicy(Qt::NoFocus);
        thetaPushButton->setText(QString::fromUtf8("\316\270"));

        gridLayout->addWidget(thetaPushButton, 1, 7, 1, 1);

        kappaPushButton = new QPushButton(greekLettersFrame);
        kappaPushButton->setObjectName(QStringLiteral("kappaPushButton"));
        kappaPushButton->setFocusPolicy(Qt::NoFocus);
        kappaPushButton->setText(QString::fromUtf8("\316\272"));

        gridLayout->addWidget(kappaPushButton, 1, 9, 1, 1);

        lambdaPushButton = new QPushButton(greekLettersFrame);
        lambdaPushButton->setObjectName(QStringLiteral("lambdaPushButton"));
        lambdaPushButton->setFocusPolicy(Qt::NoFocus);
        lambdaPushButton->setText(QString::fromUtf8("\316\273"));

        gridLayout->addWidget(lambdaPushButton, 1, 10, 1, 1);

        muPushButton = new QPushButton(greekLettersFrame);
        muPushButton->setObjectName(QStringLiteral("muPushButton"));
        muPushButton->setFocusPolicy(Qt::NoFocus);
        muPushButton->setText(QString::fromUtf8("\316\274"));

        gridLayout->addWidget(muPushButton, 1, 11, 1, 1);

        nuPushButton = new QPushButton(greekLettersFrame);
        nuPushButton->setObjectName(QStringLiteral("nuPushButton"));
        nuPushButton->setFocusPolicy(Qt::NoFocus);
        nuPushButton->setText(QString::fromUtf8("\316\275"));

        gridLayout->addWidget(nuPushButton, 2, 0, 1, 1);

        xiPushButton = new QPushButton(greekLettersFrame);
        xiPushButton->setObjectName(QStringLiteral("xiPushButton"));
        xiPushButton->setFocusPolicy(Qt::NoFocus);
        xiPushButton->setText(QString::fromUtf8("\316\276"));

        gridLayout->addWidget(xiPushButton, 2, 1, 1, 1);

        omicronPushButton = new QPushButton(greekLettersFrame);
        omicronPushButton->setObjectName(QStringLiteral("omicronPushButton"));
        omicronPushButton->setFocusPolicy(Qt::NoFocus);
        omicronPushButton->setText(QString::fromUtf8("\316\277"));

        gridLayout->addWidget(omicronPushButton, 2, 2, 1, 1);

        piPushButton = new QPushButton(greekLettersFrame);
        piPushButton->setObjectName(QStringLiteral("piPushButton"));
        piPushButton->setFocusPolicy(Qt::NoFocus);
        piPushButton->setText(QString::fromUtf8("\317\200"));

        gridLayout->addWidget(piPushButton, 2, 3, 1, 1);

        rhoPushButton = new QPushButton(greekLettersFrame);
        rhoPushButton->setObjectName(QStringLiteral("rhoPushButton"));
        rhoPushButton->setFocusPolicy(Qt::NoFocus);
        rhoPushButton->setText(QString::fromUtf8("\317\201"));

        gridLayout->addWidget(rhoPushButton, 2, 4, 1, 1);

        sigmaPushButton = new QPushButton(greekLettersFrame);
        sigmaPushButton->setObjectName(QStringLiteral("sigmaPushButton"));
        sigmaPushButton->setFocusPolicy(Qt::NoFocus);
        sigmaPushButton->setText(QString::fromUtf8("\317\203"));

        gridLayout->addWidget(sigmaPushButton, 2, 5, 1, 1);

        tauPushButton = new QPushButton(greekLettersFrame);
        tauPushButton->setObjectName(QStringLiteral("tauPushButton"));
        tauPushButton->setFocusPolicy(Qt::NoFocus);
        tauPushButton->setText(QString::fromUtf8("\317\204"));

        gridLayout->addWidget(tauPushButton, 2, 6, 1, 1);

        upsilonPushButton = new QPushButton(greekLettersFrame);
        upsilonPushButton->setObjectName(QStringLiteral("upsilonPushButton"));
        upsilonPushButton->setFocusPolicy(Qt::NoFocus);
        upsilonPushButton->setText(QString::fromUtf8("\317\205"));

        gridLayout->addWidget(upsilonPushButton, 2, 7, 1, 1);

        phiPushButton = new QPushButton(greekLettersFrame);
        phiPushButton->setObjectName(QStringLiteral("phiPushButton"));
        phiPushButton->setFocusPolicy(Qt::NoFocus);
        phiPushButton->setText(QString::fromUtf8("\317\206"));

        gridLayout->addWidget(phiPushButton, 2, 8, 1, 1);

        chiPushButton = new QPushButton(greekLettersFrame);
        chiPushButton->setObjectName(QStringLiteral("chiPushButton"));
        chiPushButton->setFocusPolicy(Qt::NoFocus);
        chiPushButton->setText(QString::fromUtf8("\317\207"));

        gridLayout->addWidget(chiPushButton, 2, 9, 1, 1);

        psiPushButton = new QPushButton(greekLettersFrame);
        psiPushButton->setObjectName(QStringLiteral("psiPushButton"));
        psiPushButton->setFocusPolicy(Qt::NoFocus);
        psiPushButton->setText(QString::fromUtf8("\317\210"));

        gridLayout->addWidget(psiPushButton, 2, 10, 1, 1);

        omegaPushButton = new QPushButton(greekLettersFrame);
        omegaPushButton->setObjectName(QStringLiteral("omegaPushButton"));
        omegaPushButton->setFocusPolicy(Qt::NoFocus);
        omegaPushButton->setText(QString::fromUtf8("\317\211"));

        gridLayout->addWidget(omegaPushButton, 2, 11, 1, 1);

        labelGreekLetterTitle = new QLabel(greekLettersFrame);
        labelGreekLetterTitle->setObjectName(QStringLiteral("labelGreekLetterTitle"));
        labelGreekLetterTitle->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(labelGreekLetterTitle, 0, 0, 1, 12);


        verticalLayout_2->addWidget(greekLettersFrame);

        tabWidget->addTab(objectTab, QString());
        tabPosition = new QWidget();
        tabPosition->setObjectName(QStringLiteral("tabPosition"));
        verticalLayout_3 = new QVBoxLayout(tabPosition);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        j2000Frame = new QFrame(tabPosition);
        j2000Frame->setObjectName(QStringLiteral("j2000Frame"));
        gridLayout_2 = new QGridLayout(j2000Frame);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 0, 0, 1, 1);

        AxisYLayout = new QVBoxLayout();
        AxisYLayout->setObjectName(QStringLiteral("AxisYLayout"));
        AxisYLabel = new QLabel(j2000Frame);
        AxisYLabel->setObjectName(QStringLiteral("AxisYLabel"));

        AxisYLayout->addWidget(AxisYLabel);

        AxisYSpinBox = new AngleSpinBox(j2000Frame);
        AxisYSpinBox->setObjectName(QStringLiteral("AxisYSpinBox"));
        sizePolicy1.setHeightForWidth(AxisYSpinBox->sizePolicy().hasHeightForWidth());
        AxisYSpinBox->setSizePolicy(sizePolicy1);

        AxisYLayout->addWidget(AxisYSpinBox);


        gridLayout_2->addLayout(AxisYLayout, 2, 1, 1, 1);

        coordinateSystemLayout = new QHBoxLayout();
        coordinateSystemLayout->setObjectName(QStringLiteral("coordinateSystemLayout"));
        coordinateSystemLabel = new QLabel(j2000Frame);
        coordinateSystemLabel->setObjectName(QStringLiteral("coordinateSystemLabel"));

        coordinateSystemLayout->addWidget(coordinateSystemLabel);

        coordinateSystemComboBox = new QComboBox(j2000Frame);
        coordinateSystemComboBox->setObjectName(QStringLiteral("coordinateSystemComboBox"));

        coordinateSystemLayout->addWidget(coordinateSystemComboBox);


        gridLayout_2->addLayout(coordinateSystemLayout, 1, 0, 1, 2);

        verticalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 4, 0, 1, 1);

        AxisXLayout = new QVBoxLayout();
        AxisXLayout->setObjectName(QStringLiteral("AxisXLayout"));
        AxisXLabel = new QLabel(j2000Frame);
        AxisXLabel->setObjectName(QStringLiteral("AxisXLabel"));

        AxisXLayout->addWidget(AxisXLabel);

        AxisXSpinBox = new AngleSpinBox(j2000Frame);
        AxisXSpinBox->setObjectName(QStringLiteral("AxisXSpinBox"));
        sizePolicy1.setHeightForWidth(AxisXSpinBox->sizePolicy().hasHeightForWidth());
        AxisXSpinBox->setSizePolicy(sizePolicy1);

        AxisXLayout->addWidget(AxisXSpinBox);


        gridLayout_2->addLayout(AxisXLayout, 2, 0, 1, 1);

        noteLabel = new QLabel(j2000Frame);
        noteLabel->setObjectName(QStringLiteral("noteLabel"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(noteLabel->sizePolicy().hasHeightForWidth());
        noteLabel->setSizePolicy(sizePolicy4);
        noteLabel->setMinimumSize(QSize(380, 0));
        noteLabel->setMaximumSize(QSize(1000, 16777215));
        noteLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        noteLabel->setWordWrap(true);

        gridLayout_2->addWidget(noteLabel, 3, 0, 1, 2, Qt::AlignHCenter);


        verticalLayout_3->addWidget(j2000Frame);

        tabWidget->addTab(tabPosition, QString());
        tabLists = new QWidget();
        tabLists->setObjectName(QStringLiteral("tabLists"));
        QFont font1;
        font1.setFamily(QStringLiteral("Ubuntu"));
        tabLists->setFont(font1);
        gridLayout_3 = new QGridLayout(tabLists);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        objectTypeComboBox = new QComboBox(tabLists);
        objectTypeComboBox->setObjectName(QStringLiteral("objectTypeComboBox"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(objectTypeComboBox->sizePolicy().hasHeightForWidth());
        objectTypeComboBox->setSizePolicy(sizePolicy5);
        objectTypeComboBox->setEditable(true);
        objectTypeComboBox->setInsertPolicy(QComboBox::NoInsert);

        gridLayout_3->addWidget(objectTypeComboBox, 0, 0, 1, 2);

        objectsListWidget = new QListWidget(tabLists);
        objectsListWidget->setObjectName(QStringLiteral("objectsListWidget"));

        gridLayout_3->addWidget(objectsListWidget, 2, 0, 1, 3);

        searchInEnglishCheckBox = new QCheckBox(tabLists);
        searchInEnglishCheckBox->setObjectName(QStringLiteral("searchInEnglishCheckBox"));

        gridLayout_3->addWidget(searchInEnglishCheckBox, 0, 2, 1, 1);

        searchInListLineEdit = new QLineEdit(tabLists);
        searchInListLineEdit->setObjectName(QStringLiteral("searchInListLineEdit"));

        gridLayout_3->addWidget(searchInListLineEdit, 1, 0, 1, 3);

        tabWidget->addTab(tabLists, QString());
        tabOptions = new QWidget();
        tabOptions->setObjectName(QStringLiteral("tabOptions"));
        verticalLayout_4 = new QVBoxLayout(tabOptions);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        simbadGroupBox = new QGroupBox(tabOptions);
        simbadGroupBox->setObjectName(QStringLiteral("simbadGroupBox"));
        verticalLayout_5 = new QVBoxLayout(simbadGroupBox);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        checkBoxUseSimbad = new QCheckBox(simbadGroupBox);
        checkBoxUseSimbad->setObjectName(QStringLiteral("checkBoxUseSimbad"));

        verticalLayout_5->addWidget(checkBoxUseSimbad);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        labelServer = new QLabel(simbadGroupBox);
        labelServer->setObjectName(QStringLiteral("labelServer"));

        horizontalLayout_2->addWidget(labelServer);

        serverListComboBox = new QComboBox(simbadGroupBox);
        serverListComboBox->setObjectName(QStringLiteral("serverListComboBox"));
        sizePolicy.setHeightForWidth(serverListComboBox->sizePolicy().hasHeightForWidth());
        serverListComboBox->setSizePolicy(sizePolicy);
        serverListComboBox->setMinimumSize(QSize(0, 24));
        serverListComboBox->setMaximumSize(QSize(16777215, 24));
        serverListComboBox->setEditable(false);

        horizontalLayout_2->addWidget(serverListComboBox);


        verticalLayout_5->addLayout(horizontalLayout_2);


        verticalLayout_4->addWidget(simbadGroupBox);

        searchOptions = new QGroupBox(tabOptions);
        searchOptions->setObjectName(QStringLiteral("searchOptions"));
        verticalLayout_6 = new QVBoxLayout(searchOptions);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        checkBoxUseStartOfWords = new QCheckBox(searchOptions);
        checkBoxUseStartOfWords->setObjectName(QStringLiteral("checkBoxUseStartOfWords"));

        verticalLayout_6->addWidget(checkBoxUseStartOfWords);


        verticalLayout_4->addWidget(searchOptions);

        tabWidget->addTab(tabOptions, QString());

        verticalLayout->addWidget(tabWidget);

        QWidget::setTabOrder(tabWidget, lineEditSearchSkyObject);

        retranslateUi(searchDialogForm);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(searchDialogForm);
    } // setupUi

    void retranslateUi(QWidget *searchDialogForm)
    {
        searchDialogForm->setWindowTitle(QApplication::translate("searchDialogForm", "Find Object", 0));
        stelWindowTitle->setText(QApplication::translate("searchDialogForm", "Search window", 0));
        closeStelWindow->setText(QString());
#ifndef QT_NO_TOOLTIP
        lineEditSearchSkyObject->setToolTip(QApplication::translate("searchDialogForm", "Use tab key for select of found items", 0));
#endif // QT_NO_TOOLTIP
        lineEditSearchSkyObject->setText(QString());
#ifndef QT_NO_TOOLTIP
        completionLabel->setToolTip(QApplication::translate("searchDialogForm", "Use tab key for select of found items", 0));
#endif // QT_NO_TOOLTIP
        completionLabel->setText(QString());
        simbadStatusLabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        iotaPushButton->setToolTip(QApplication::translate("searchDialogForm", "iota", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        alphaPushButton->setToolTip(QApplication::translate("searchDialogForm", "alpha", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        betaPushButton->setToolTip(QApplication::translate("searchDialogForm", "beta", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        gammaPushButton->setToolTip(QApplication::translate("searchDialogForm", "gamma", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        deltaPushButton->setToolTip(QApplication::translate("searchDialogForm", "delta", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        epsilonPushButton->setToolTip(QApplication::translate("searchDialogForm", "epsilon", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        zetaPushButton->setToolTip(QApplication::translate("searchDialogForm", "zeta", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        etaPushButton->setToolTip(QApplication::translate("searchDialogForm", "eta", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        thetaPushButton->setToolTip(QApplication::translate("searchDialogForm", "theta", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        kappaPushButton->setToolTip(QApplication::translate("searchDialogForm", "kappa", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        lambdaPushButton->setToolTip(QApplication::translate("searchDialogForm", "lambda", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        muPushButton->setToolTip(QApplication::translate("searchDialogForm", "mu", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        nuPushButton->setToolTip(QApplication::translate("searchDialogForm", "nu", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        xiPushButton->setToolTip(QApplication::translate("searchDialogForm", "xi", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        omicronPushButton->setToolTip(QApplication::translate("searchDialogForm", "omicron", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        piPushButton->setToolTip(QApplication::translate("searchDialogForm", "pi", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        rhoPushButton->setToolTip(QApplication::translate("searchDialogForm", "rho", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        sigmaPushButton->setToolTip(QApplication::translate("searchDialogForm", "sigma", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        tauPushButton->setToolTip(QApplication::translate("searchDialogForm", "tau", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        upsilonPushButton->setToolTip(QApplication::translate("searchDialogForm", "upsilon", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        phiPushButton->setToolTip(QApplication::translate("searchDialogForm", "phi", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        chiPushButton->setToolTip(QApplication::translate("searchDialogForm", "chi", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        psiPushButton->setToolTip(QApplication::translate("searchDialogForm", "psi", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        omegaPushButton->setToolTip(QApplication::translate("searchDialogForm", "omega", 0));
#endif // QT_NO_TOOLTIP
        labelGreekLetterTitle->setText(QApplication::translate("searchDialogForm", "Greek letters for Bayer designations", 0));
        tabWidget->setTabText(tabWidget->indexOf(objectTab), QApplication::translate("searchDialogForm", "Object", 0));
        AxisYLabel->setText(QString());
        coordinateSystemLabel->setText(QApplication::translate("searchDialogForm", "Coordinate system:", 0));
        AxisXLabel->setText(QString());
        noteLabel->setText(QApplication::translate("searchDialogForm", "Note: this tool doesn't apply the refraction correction for coordinates.", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabPosition), QApplication::translate("searchDialogForm", "Position", 0));
#ifndef QT_NO_TOOLTIP
        objectTypeComboBox->setToolTip(QApplication::translate("searchDialogForm", "Some objects may be found after activation respective plug-ins", 0));
#endif // QT_NO_TOOLTIP
        searchInEnglishCheckBox->setText(QApplication::translate("searchDialogForm", "names in English", 0));
        searchInListLineEdit->setPlaceholderText(QApplication::translate("searchDialogForm", "Search in list...", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabLists), QApplication::translate("searchDialogForm", "Lists", 0));
        simbadGroupBox->setTitle(QApplication::translate("searchDialogForm", "On-line astronomical database SIMBAD", 0));
        checkBoxUseSimbad->setText(QApplication::translate("searchDialogForm", "Extend search with SIMBAD", 0));
        labelServer->setText(QApplication::translate("searchDialogForm", "Server:", 0));
        searchOptions->setTitle(QApplication::translate("searchDialogForm", "Search options", 0));
        checkBoxUseStartOfWords->setText(QApplication::translate("searchDialogForm", "Use autofill only from the beginning of words", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabOptions), QApplication::translate("searchDialogForm", "Options", 0));
    } // retranslateUi

};

namespace Ui {
    class searchDialogForm: public Ui_searchDialogForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEARCHDIALOGGUI_H
