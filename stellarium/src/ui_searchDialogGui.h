/********************************************************************************
** Form generated from reading UI file 'searchDialogGui.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEARCHDIALOGGUI_H
#define UI_SEARCHDIALOGGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "AngleSpinBox.hpp"
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
    QWidget *simbadTab;
    QGridLayout *gridLayout_4;
    QPushButton *simbadCooQueryButton;
    QTextBrowser *simbadCooResultsTextBrowser;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *allIDsCheckBox;
    QCheckBox *typesCheckBox;
    QCheckBox *spectralClassCheckBox;
    QCheckBox *morphoCheckBox;
    QCheckBox *dimsCheckBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *searchRadiusLabel;
    QSpinBox *searchRadiusSpinBox;
    QLabel *arcsecLabel;
    QSpacerItem *horizontalSpacer;
    QLabel *maxResultsLabel;
    QSpinBox *resultsSpinBox;
    QLabel *simbadCooStatusLabel;
    QWidget *tabPosition;
    QVBoxLayout *verticalLayout_3;
    QFrame *j2000Frame;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QVBoxLayout *AxisYLayout;
    QLabel *AxisYLabel;
    AngleSpinBox *AxisYSpinBox;
    QHBoxLayout *coordinateSystemLayout;
    QLabel *coordinateSystemLabel;
    QComboBox *coordinateSystemComboBox;
    QVBoxLayout *AxisXLayout;
    QLabel *AxisXLabel;
    AngleSpinBox *AxisXSpinBox;
    QSpacerItem *verticalSpacer_2;
    QWidget *tabLists;
    QGridLayout *gridLayout_3;
    QComboBox *objectTypeComboBox;
    QCheckBox *searchInEnglishCheckBox;
    QLineEdit *searchInListLineEdit;
    QListView *objectsListView;
    QWidget *tabOptions;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *simbadGroupBox;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelServer;
    QComboBox *serverListComboBox;
    QGroupBox *searchOptions;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *checkBoxUseStartOfWords;
    QCheckBox *checkBoxLockPosition;
    QCheckBox *checkBoxFOVCenterMarker;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *searchDialogForm)
    {
        if (searchDialogForm->objectName().isEmpty())
            searchDialogForm->setObjectName(QString::fromUtf8("searchDialogForm"));
        searchDialogForm->resize(507, 401);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(searchDialogForm->sizePolicy().hasHeightForWidth());
        searchDialogForm->setSizePolicy(sizePolicy);
        searchDialogForm->setMinimumSize(QSize(390, 0));
        searchDialogForm->setFocusPolicy(Qt::ClickFocus);
        searchDialogForm->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(searchDialogForm);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        TitleBar = new BarFrame(searchDialogForm);
        TitleBar->setObjectName(QString::fromUtf8("TitleBar"));
        TitleBar->setMinimumSize(QSize(16, 25));
        TitleBar->setMaximumSize(QSize(16777215, 25));
        TitleBar->setFocusPolicy(Qt::NoFocus);
        TitleBar->setAutoFillBackground(false);
        TitleBar->setFrameShape(QFrame::StyledPanel);
        hboxLayout = new QHBoxLayout(TitleBar);
        hboxLayout->setSpacing(0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 4, 0);
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        stelWindowTitle = new QLabel(TitleBar);
        stelWindowTitle->setObjectName(QString::fromUtf8("stelWindowTitle"));

        hboxLayout->addWidget(stelWindowTitle);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem1);

        closeStelWindow = new QPushButton(TitleBar);
        closeStelWindow->setObjectName(QString::fromUtf8("closeStelWindow"));
        closeStelWindow->setMinimumSize(QSize(16, 16));
        closeStelWindow->setMaximumSize(QSize(16, 16));
        closeStelWindow->setFocusPolicy(Qt::NoFocus);

        hboxLayout->addWidget(closeStelWindow);


        verticalLayout->addWidget(TitleBar);

        tabWidget = new QTabWidget(searchDialogForm);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        objectTab = new QWidget();
        objectTab->setObjectName(QString::fromUtf8("objectTab"));
        verticalLayout_2 = new QVBoxLayout(objectTab);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSearchLayout = new QFrame(objectTab);
        horizontalSearchLayout->setObjectName(QString::fromUtf8("horizontalSearchLayout"));
        horizontalSearchLayout->setMinimumSize(QSize(0, 0));
        horizontalSearchLayout->setFrameShape(QFrame::StyledPanel);
        horizontalLayout = new QHBoxLayout(horizontalSearchLayout);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        lineEditSearchSkyObject = new QLineEdit(horizontalSearchLayout);
        lineEditSearchSkyObject->setObjectName(QString::fromUtf8("lineEditSearchSkyObject"));
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
        pushButtonGotoSearchSkyObject->setObjectName(QString::fromUtf8("pushButtonGotoSearchSkyObject"));
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
        completionLabel->setObjectName(QString::fromUtf8("completionLabel"));
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
        simbadStatusLabel->setObjectName(QString::fromUtf8("simbadStatusLabel"));
        simbadStatusLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(simbadStatusLabel);

        greekLettersFrame = new QFrame(objectTab);
        greekLettersFrame->setObjectName(QString::fromUtf8("greekLettersFrame"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(greekLettersFrame->sizePolicy().hasHeightForWidth());
        greekLettersFrame->setSizePolicy(sizePolicy4);
        gridLayout = new QGridLayout(greekLettersFrame);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        iotaPushButton = new QPushButton(greekLettersFrame);
        iotaPushButton->setObjectName(QString::fromUtf8("iotaPushButton"));
        iotaPushButton->setFocusPolicy(Qt::NoFocus);
        iotaPushButton->setText(QString::fromUtf8("\316\271"));

        gridLayout->addWidget(iotaPushButton, 1, 8, 1, 1);

        alphaPushButton = new QPushButton(greekLettersFrame);
        alphaPushButton->setObjectName(QString::fromUtf8("alphaPushButton"));
        alphaPushButton->setFocusPolicy(Qt::NoFocus);
        alphaPushButton->setText(QString::fromUtf8("\316\261"));

        gridLayout->addWidget(alphaPushButton, 1, 0, 1, 1);

        betaPushButton = new QPushButton(greekLettersFrame);
        betaPushButton->setObjectName(QString::fromUtf8("betaPushButton"));
        betaPushButton->setFocusPolicy(Qt::NoFocus);
        betaPushButton->setText(QString::fromUtf8("\316\262"));

        gridLayout->addWidget(betaPushButton, 1, 1, 1, 1);

        gammaPushButton = new QPushButton(greekLettersFrame);
        gammaPushButton->setObjectName(QString::fromUtf8("gammaPushButton"));
        gammaPushButton->setFocusPolicy(Qt::NoFocus);
        gammaPushButton->setText(QString::fromUtf8("\316\263"));

        gridLayout->addWidget(gammaPushButton, 1, 2, 1, 1);

        deltaPushButton = new QPushButton(greekLettersFrame);
        deltaPushButton->setObjectName(QString::fromUtf8("deltaPushButton"));
        deltaPushButton->setFocusPolicy(Qt::NoFocus);
        deltaPushButton->setText(QString::fromUtf8("\316\264"));

        gridLayout->addWidget(deltaPushButton, 1, 3, 1, 1);

        epsilonPushButton = new QPushButton(greekLettersFrame);
        epsilonPushButton->setObjectName(QString::fromUtf8("epsilonPushButton"));
        epsilonPushButton->setFocusPolicy(Qt::NoFocus);
        epsilonPushButton->setText(QString::fromUtf8("\316\265"));

        gridLayout->addWidget(epsilonPushButton, 1, 4, 1, 1);

        zetaPushButton = new QPushButton(greekLettersFrame);
        zetaPushButton->setObjectName(QString::fromUtf8("zetaPushButton"));
        zetaPushButton->setFocusPolicy(Qt::NoFocus);
        zetaPushButton->setText(QString::fromUtf8("\316\266"));

        gridLayout->addWidget(zetaPushButton, 1, 5, 1, 1);

        etaPushButton = new QPushButton(greekLettersFrame);
        etaPushButton->setObjectName(QString::fromUtf8("etaPushButton"));
        etaPushButton->setFocusPolicy(Qt::NoFocus);
        etaPushButton->setText(QString::fromUtf8("\316\267"));

        gridLayout->addWidget(etaPushButton, 1, 6, 1, 1);

        thetaPushButton = new QPushButton(greekLettersFrame);
        thetaPushButton->setObjectName(QString::fromUtf8("thetaPushButton"));
        thetaPushButton->setFocusPolicy(Qt::NoFocus);
        thetaPushButton->setText(QString::fromUtf8("\316\270"));

        gridLayout->addWidget(thetaPushButton, 1, 7, 1, 1);

        kappaPushButton = new QPushButton(greekLettersFrame);
        kappaPushButton->setObjectName(QString::fromUtf8("kappaPushButton"));
        kappaPushButton->setFocusPolicy(Qt::NoFocus);
        kappaPushButton->setText(QString::fromUtf8("\316\272"));

        gridLayout->addWidget(kappaPushButton, 1, 9, 1, 1);

        lambdaPushButton = new QPushButton(greekLettersFrame);
        lambdaPushButton->setObjectName(QString::fromUtf8("lambdaPushButton"));
        lambdaPushButton->setFocusPolicy(Qt::NoFocus);
        lambdaPushButton->setText(QString::fromUtf8("\316\273"));

        gridLayout->addWidget(lambdaPushButton, 1, 10, 1, 1);

        muPushButton = new QPushButton(greekLettersFrame);
        muPushButton->setObjectName(QString::fromUtf8("muPushButton"));
        muPushButton->setFocusPolicy(Qt::NoFocus);
        muPushButton->setText(QString::fromUtf8("\316\274"));

        gridLayout->addWidget(muPushButton, 1, 11, 1, 1);

        nuPushButton = new QPushButton(greekLettersFrame);
        nuPushButton->setObjectName(QString::fromUtf8("nuPushButton"));
        nuPushButton->setFocusPolicy(Qt::NoFocus);
        nuPushButton->setText(QString::fromUtf8("\316\275"));

        gridLayout->addWidget(nuPushButton, 2, 0, 1, 1);

        xiPushButton = new QPushButton(greekLettersFrame);
        xiPushButton->setObjectName(QString::fromUtf8("xiPushButton"));
        xiPushButton->setFocusPolicy(Qt::NoFocus);
        xiPushButton->setText(QString::fromUtf8("\316\276"));

        gridLayout->addWidget(xiPushButton, 2, 1, 1, 1);

        omicronPushButton = new QPushButton(greekLettersFrame);
        omicronPushButton->setObjectName(QString::fromUtf8("omicronPushButton"));
        omicronPushButton->setFocusPolicy(Qt::NoFocus);
        omicronPushButton->setText(QString::fromUtf8("\316\277"));

        gridLayout->addWidget(omicronPushButton, 2, 2, 1, 1);

        piPushButton = new QPushButton(greekLettersFrame);
        piPushButton->setObjectName(QString::fromUtf8("piPushButton"));
        piPushButton->setFocusPolicy(Qt::NoFocus);
        piPushButton->setText(QString::fromUtf8("\317\200"));

        gridLayout->addWidget(piPushButton, 2, 3, 1, 1);

        rhoPushButton = new QPushButton(greekLettersFrame);
        rhoPushButton->setObjectName(QString::fromUtf8("rhoPushButton"));
        rhoPushButton->setFocusPolicy(Qt::NoFocus);
        rhoPushButton->setText(QString::fromUtf8("\317\201"));

        gridLayout->addWidget(rhoPushButton, 2, 4, 1, 1);

        sigmaPushButton = new QPushButton(greekLettersFrame);
        sigmaPushButton->setObjectName(QString::fromUtf8("sigmaPushButton"));
        sigmaPushButton->setFocusPolicy(Qt::NoFocus);
        sigmaPushButton->setText(QString::fromUtf8("\317\203"));

        gridLayout->addWidget(sigmaPushButton, 2, 5, 1, 1);

        tauPushButton = new QPushButton(greekLettersFrame);
        tauPushButton->setObjectName(QString::fromUtf8("tauPushButton"));
        tauPushButton->setFocusPolicy(Qt::NoFocus);
        tauPushButton->setText(QString::fromUtf8("\317\204"));

        gridLayout->addWidget(tauPushButton, 2, 6, 1, 1);

        upsilonPushButton = new QPushButton(greekLettersFrame);
        upsilonPushButton->setObjectName(QString::fromUtf8("upsilonPushButton"));
        upsilonPushButton->setFocusPolicy(Qt::NoFocus);
        upsilonPushButton->setText(QString::fromUtf8("\317\205"));

        gridLayout->addWidget(upsilonPushButton, 2, 7, 1, 1);

        phiPushButton = new QPushButton(greekLettersFrame);
        phiPushButton->setObjectName(QString::fromUtf8("phiPushButton"));
        phiPushButton->setFocusPolicy(Qt::NoFocus);
        phiPushButton->setText(QString::fromUtf8("\317\206"));

        gridLayout->addWidget(phiPushButton, 2, 8, 1, 1);

        chiPushButton = new QPushButton(greekLettersFrame);
        chiPushButton->setObjectName(QString::fromUtf8("chiPushButton"));
        chiPushButton->setFocusPolicy(Qt::NoFocus);
        chiPushButton->setText(QString::fromUtf8("\317\207"));

        gridLayout->addWidget(chiPushButton, 2, 9, 1, 1);

        psiPushButton = new QPushButton(greekLettersFrame);
        psiPushButton->setObjectName(QString::fromUtf8("psiPushButton"));
        psiPushButton->setFocusPolicy(Qt::NoFocus);
        psiPushButton->setText(QString::fromUtf8("\317\210"));

        gridLayout->addWidget(psiPushButton, 2, 10, 1, 1);

        omegaPushButton = new QPushButton(greekLettersFrame);
        omegaPushButton->setObjectName(QString::fromUtf8("omegaPushButton"));
        omegaPushButton->setFocusPolicy(Qt::NoFocus);
        omegaPushButton->setText(QString::fromUtf8("\317\211"));

        gridLayout->addWidget(omegaPushButton, 2, 11, 1, 1);

        labelGreekLetterTitle = new QLabel(greekLettersFrame);
        labelGreekLetterTitle->setObjectName(QString::fromUtf8("labelGreekLetterTitle"));
        labelGreekLetterTitle->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(labelGreekLetterTitle, 0, 0, 1, 12);


        verticalLayout_2->addWidget(greekLettersFrame);

        tabWidget->addTab(objectTab, QString());
        simbadTab = new QWidget();
        simbadTab->setObjectName(QString::fromUtf8("simbadTab"));
        sizePolicy4.setHeightForWidth(simbadTab->sizePolicy().hasHeightForWidth());
        simbadTab->setSizePolicy(sizePolicy4);
        gridLayout_4 = new QGridLayout(simbadTab);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        simbadCooQueryButton = new QPushButton(simbadTab);
        simbadCooQueryButton->setObjectName(QString::fromUtf8("simbadCooQueryButton"));
        sizePolicy1.setHeightForWidth(simbadCooQueryButton->sizePolicy().hasHeightForWidth());
        simbadCooQueryButton->setSizePolicy(sizePolicy1);

        gridLayout_4->addWidget(simbadCooQueryButton, 2, 0, 1, 1);

        simbadCooResultsTextBrowser = new QTextBrowser(simbadTab);
        simbadCooResultsTextBrowser->setObjectName(QString::fromUtf8("simbadCooResultsTextBrowser"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(35);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(simbadCooResultsTextBrowser->sizePolicy().hasHeightForWidth());
        simbadCooResultsTextBrowser->setSizePolicy(sizePolicy5);
        simbadCooResultsTextBrowser->setMinimumSize(QSize(0, 4));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Courier"));
        simbadCooResultsTextBrowser->setFont(font1);
        simbadCooResultsTextBrowser->setAcceptRichText(false);

        gridLayout_4->addWidget(simbadCooResultsTextBrowser, 3, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        allIDsCheckBox = new QCheckBox(simbadTab);
        allIDsCheckBox->setObjectName(QString::fromUtf8("allIDsCheckBox"));
        sizePolicy1.setHeightForWidth(allIDsCheckBox->sizePolicy().hasHeightForWidth());
        allIDsCheckBox->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(allIDsCheckBox);

        typesCheckBox = new QCheckBox(simbadTab);
        typesCheckBox->setObjectName(QString::fromUtf8("typesCheckBox"));
        sizePolicy1.setHeightForWidth(typesCheckBox->sizePolicy().hasHeightForWidth());
        typesCheckBox->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(typesCheckBox);

        spectralClassCheckBox = new QCheckBox(simbadTab);
        spectralClassCheckBox->setObjectName(QString::fromUtf8("spectralClassCheckBox"));
        sizePolicy1.setHeightForWidth(spectralClassCheckBox->sizePolicy().hasHeightForWidth());
        spectralClassCheckBox->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(spectralClassCheckBox);

        morphoCheckBox = new QCheckBox(simbadTab);
        morphoCheckBox->setObjectName(QString::fromUtf8("morphoCheckBox"));
        sizePolicy1.setHeightForWidth(morphoCheckBox->sizePolicy().hasHeightForWidth());
        morphoCheckBox->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(morphoCheckBox);

        dimsCheckBox = new QCheckBox(simbadTab);
        dimsCheckBox->setObjectName(QString::fromUtf8("dimsCheckBox"));
        sizePolicy1.setHeightForWidth(dimsCheckBox->sizePolicy().hasHeightForWidth());
        dimsCheckBox->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(dimsCheckBox);


        gridLayout_4->addLayout(horizontalLayout_3, 1, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, -1, -1);
        searchRadiusLabel = new QLabel(simbadTab);
        searchRadiusLabel->setObjectName(QString::fromUtf8("searchRadiusLabel"));
        QSizePolicy sizePolicy6(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(searchRadiusLabel->sizePolicy().hasHeightForWidth());
        searchRadiusLabel->setSizePolicy(sizePolicy6);

        horizontalLayout_4->addWidget(searchRadiusLabel);

        searchRadiusSpinBox = new QSpinBox(simbadTab);
        searchRadiusSpinBox->setObjectName(QString::fromUtf8("searchRadiusSpinBox"));
        sizePolicy2.setHeightForWidth(searchRadiusSpinBox->sizePolicy().hasHeightForWidth());
        searchRadiusSpinBox->setSizePolicy(sizePolicy2);
        searchRadiusSpinBox->setMinimum(1);
        searchRadiusSpinBox->setMaximum(150);
        searchRadiusSpinBox->setValue(30);

        horizontalLayout_4->addWidget(searchRadiusSpinBox);

        arcsecLabel = new QLabel(simbadTab);
        arcsecLabel->setObjectName(QString::fromUtf8("arcsecLabel"));
        sizePolicy6.setHeightForWidth(arcsecLabel->sizePolicy().hasHeightForWidth());
        arcsecLabel->setSizePolicy(sizePolicy6);

        horizontalLayout_4->addWidget(arcsecLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        maxResultsLabel = new QLabel(simbadTab);
        maxResultsLabel->setObjectName(QString::fromUtf8("maxResultsLabel"));
        QSizePolicy sizePolicy7(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(maxResultsLabel->sizePolicy().hasHeightForWidth());
        maxResultsLabel->setSizePolicy(sizePolicy7);
        maxResultsLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(maxResultsLabel);

        resultsSpinBox = new QSpinBox(simbadTab);
        resultsSpinBox->setObjectName(QString::fromUtf8("resultsSpinBox"));
        sizePolicy2.setHeightForWidth(resultsSpinBox->sizePolicy().hasHeightForWidth());
        resultsSpinBox->setSizePolicy(sizePolicy2);
        resultsSpinBox->setMinimum(1);
        resultsSpinBox->setMaximum(10);
        resultsSpinBox->setValue(3);

        horizontalLayout_4->addWidget(resultsSpinBox);


        gridLayout_4->addLayout(horizontalLayout_4, 0, 0, 1, 1);

        simbadCooStatusLabel = new QLabel(simbadTab);
        simbadCooStatusLabel->setObjectName(QString::fromUtf8("simbadCooStatusLabel"));
        simbadCooStatusLabel->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(simbadCooStatusLabel, 4, 0, 1, 1);

        tabWidget->addTab(simbadTab, QString());
        tabWidget->setTabText(tabWidget->indexOf(simbadTab), QString::fromUtf8("SIMBAD"));
        tabPosition = new QWidget();
        tabPosition->setObjectName(QString::fromUtf8("tabPosition"));
        verticalLayout_3 = new QVBoxLayout(tabPosition);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        j2000Frame = new QFrame(tabPosition);
        j2000Frame->setObjectName(QString::fromUtf8("j2000Frame"));
        gridLayout_2 = new QGridLayout(j2000Frame);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(j2000Frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);

        gridLayout_2->addWidget(label, 2, 0, 1, 2);

        AxisYLayout = new QVBoxLayout();
        AxisYLayout->setObjectName(QString::fromUtf8("AxisYLayout"));
        AxisYLabel = new QLabel(j2000Frame);
        AxisYLabel->setObjectName(QString::fromUtf8("AxisYLabel"));

        AxisYLayout->addWidget(AxisYLabel);

        AxisYSpinBox = new AngleSpinBox(j2000Frame);
        AxisYSpinBox->setObjectName(QString::fromUtf8("AxisYSpinBox"));
        sizePolicy1.setHeightForWidth(AxisYSpinBox->sizePolicy().hasHeightForWidth());
        AxisYSpinBox->setSizePolicy(sizePolicy1);

        AxisYLayout->addWidget(AxisYSpinBox);


        gridLayout_2->addLayout(AxisYLayout, 1, 1, 1, 1);

        coordinateSystemLayout = new QHBoxLayout();
        coordinateSystemLayout->setObjectName(QString::fromUtf8("coordinateSystemLayout"));
        coordinateSystemLabel = new QLabel(j2000Frame);
        coordinateSystemLabel->setObjectName(QString::fromUtf8("coordinateSystemLabel"));

        coordinateSystemLayout->addWidget(coordinateSystemLabel);

        coordinateSystemComboBox = new QComboBox(j2000Frame);
        coordinateSystemComboBox->setObjectName(QString::fromUtf8("coordinateSystemComboBox"));

        coordinateSystemLayout->addWidget(coordinateSystemComboBox);


        gridLayout_2->addLayout(coordinateSystemLayout, 0, 0, 1, 2);

        AxisXLayout = new QVBoxLayout();
        AxisXLayout->setObjectName(QString::fromUtf8("AxisXLayout"));
        AxisXLabel = new QLabel(j2000Frame);
        AxisXLabel->setObjectName(QString::fromUtf8("AxisXLabel"));

        AxisXLayout->addWidget(AxisXLabel);

        AxisXSpinBox = new AngleSpinBox(j2000Frame);
        AxisXSpinBox->setObjectName(QString::fromUtf8("AxisXSpinBox"));
        sizePolicy1.setHeightForWidth(AxisXSpinBox->sizePolicy().hasHeightForWidth());
        AxisXSpinBox->setSizePolicy(sizePolicy1);

        AxisXLayout->addWidget(AxisXSpinBox);


        gridLayout_2->addLayout(AxisXLayout, 1, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 3, 0, 1, 2);


        verticalLayout_3->addWidget(j2000Frame);

        tabWidget->addTab(tabPosition, QString());
        tabLists = new QWidget();
        tabLists->setObjectName(QString::fromUtf8("tabLists"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Ubuntu"));
        tabLists->setFont(font2);
        gridLayout_3 = new QGridLayout(tabLists);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        objectTypeComboBox = new QComboBox(tabLists);
        objectTypeComboBox->setObjectName(QString::fromUtf8("objectTypeComboBox"));
        QSizePolicy sizePolicy8(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(objectTypeComboBox->sizePolicy().hasHeightForWidth());
        objectTypeComboBox->setSizePolicy(sizePolicy8);
        objectTypeComboBox->setEditable(true);
        objectTypeComboBox->setInsertPolicy(QComboBox::NoInsert);

        gridLayout_3->addWidget(objectTypeComboBox, 0, 0, 1, 2);

        searchInEnglishCheckBox = new QCheckBox(tabLists);
        searchInEnglishCheckBox->setObjectName(QString::fromUtf8("searchInEnglishCheckBox"));

        gridLayout_3->addWidget(searchInEnglishCheckBox, 0, 2, 1, 1);

        searchInListLineEdit = new QLineEdit(tabLists);
        searchInListLineEdit->setObjectName(QString::fromUtf8("searchInListLineEdit"));

        gridLayout_3->addWidget(searchInListLineEdit, 1, 0, 1, 3);

        objectsListView = new QListView(tabLists);
        objectsListView->setObjectName(QString::fromUtf8("objectsListView"));

        gridLayout_3->addWidget(objectsListView, 2, 0, 1, 3);

        tabWidget->addTab(tabLists, QString());
        tabOptions = new QWidget();
        tabOptions->setObjectName(QString::fromUtf8("tabOptions"));
        verticalLayout_4 = new QVBoxLayout(tabOptions);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        simbadGroupBox = new QGroupBox(tabOptions);
        simbadGroupBox->setObjectName(QString::fromUtf8("simbadGroupBox"));
        simbadGroupBox->setFlat(true);
        simbadGroupBox->setCheckable(true);
        simbadGroupBox->setChecked(false);
        verticalLayout_5 = new QVBoxLayout(simbadGroupBox);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelServer = new QLabel(simbadGroupBox);
        labelServer->setObjectName(QString::fromUtf8("labelServer"));

        horizontalLayout_2->addWidget(labelServer);

        serverListComboBox = new QComboBox(simbadGroupBox);
        serverListComboBox->setObjectName(QString::fromUtf8("serverListComboBox"));
        sizePolicy4.setHeightForWidth(serverListComboBox->sizePolicy().hasHeightForWidth());
        serverListComboBox->setSizePolicy(sizePolicy4);
        serverListComboBox->setMinimumSize(QSize(0, 24));
        serverListComboBox->setMaximumSize(QSize(16777215, 24));
        serverListComboBox->setEditable(false);

        horizontalLayout_2->addWidget(serverListComboBox);


        verticalLayout_5->addLayout(horizontalLayout_2);


        verticalLayout_4->addWidget(simbadGroupBox);

        searchOptions = new QGroupBox(tabOptions);
        searchOptions->setObjectName(QString::fromUtf8("searchOptions"));
        verticalLayout_6 = new QVBoxLayout(searchOptions);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        checkBoxUseStartOfWords = new QCheckBox(searchOptions);
        checkBoxUseStartOfWords->setObjectName(QString::fromUtf8("checkBoxUseStartOfWords"));

        verticalLayout_6->addWidget(checkBoxUseStartOfWords);

        checkBoxLockPosition = new QCheckBox(searchOptions);
        checkBoxLockPosition->setObjectName(QString::fromUtf8("checkBoxLockPosition"));

        verticalLayout_6->addWidget(checkBoxLockPosition);

        checkBoxFOVCenterMarker = new QCheckBox(searchOptions);
        checkBoxFOVCenterMarker->setObjectName(QString::fromUtf8("checkBoxFOVCenterMarker"));

        verticalLayout_6->addWidget(checkBoxFOVCenterMarker);


        verticalLayout_4->addWidget(searchOptions);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        tabWidget->addTab(tabOptions, QString());

        verticalLayout->addWidget(tabWidget);

        QWidget::setTabOrder(tabWidget, lineEditSearchSkyObject);
        QWidget::setTabOrder(lineEditSearchSkyObject, AxisXSpinBox);
        QWidget::setTabOrder(AxisXSpinBox, AxisYSpinBox);

        retranslateUi(searchDialogForm);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(searchDialogForm);
    } // setupUi

    void retranslateUi(QWidget *searchDialogForm)
    {
        searchDialogForm->setWindowTitle(QCoreApplication::translate("searchDialogForm", "Find Object", nullptr));
        stelWindowTitle->setText(QCoreApplication::translate("searchDialogForm", "Search window", nullptr));
        closeStelWindow->setText(QString());
#if QT_CONFIG(tooltip)
        lineEditSearchSkyObject->setToolTip(QCoreApplication::translate("searchDialogForm", "Use tab key for select of found items", nullptr));
#endif // QT_CONFIG(tooltip)
        lineEditSearchSkyObject->setText(QString());
#if QT_CONFIG(tooltip)
        completionLabel->setToolTip(QCoreApplication::translate("searchDialogForm", "Use tab key for select of found items", nullptr));
#endif // QT_CONFIG(tooltip)
        completionLabel->setText(QString());
        simbadStatusLabel->setText(QString());
#if QT_CONFIG(tooltip)
        iotaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "iota", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        alphaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "alpha", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        betaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "beta", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        gammaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "gamma", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        deltaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "delta", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        epsilonPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "epsilon", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        zetaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "zeta", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        etaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "eta", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        thetaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "theta", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        kappaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "kappa", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        lambdaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "lambda", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        muPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "mu", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        nuPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "nu", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        xiPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "xi", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        omicronPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "omicron", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        piPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "pi", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        rhoPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "rho", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        sigmaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "sigma", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        tauPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "tau", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        upsilonPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "upsilon", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        phiPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "phi", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        chiPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "chi", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        psiPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "psi", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        omegaPushButton->setToolTip(QCoreApplication::translate("searchDialogForm", "omega", nullptr));
#endif // QT_CONFIG(tooltip)
        labelGreekLetterTitle->setText(QCoreApplication::translate("searchDialogForm", "Greek letters for Bayer designations", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(objectTab), QCoreApplication::translate("searchDialogForm", "Object", nullptr));
        simbadCooQueryButton->setText(QCoreApplication::translate("searchDialogForm", "Query SIMBAD about selected object", nullptr));
        allIDsCheckBox->setText(QCoreApplication::translate("searchDialogForm", "All IDs", nullptr));
        typesCheckBox->setText(QCoreApplication::translate("searchDialogForm", "Types", nullptr));
        spectralClassCheckBox->setText(QCoreApplication::translate("searchDialogForm", "Spectral Class", nullptr));
#if QT_CONFIG(tooltip)
        morphoCheckBox->setToolTip(QCoreApplication::translate("searchDialogForm", "Morphological description", nullptr));
#endif // QT_CONFIG(tooltip)
        morphoCheckBox->setText(QCoreApplication::translate("searchDialogForm", "Morph. Descr.", nullptr));
        dimsCheckBox->setText(QCoreApplication::translate("searchDialogForm", "Dimensions", nullptr));
        searchRadiusLabel->setText(QCoreApplication::translate("searchDialogForm", "Max. search radius", nullptr));
        arcsecLabel->setText(QCoreApplication::translate("searchDialogForm", "arcseconds", nullptr));
        maxResultsLabel->setText(QCoreApplication::translate("searchDialogForm", "Max. results ", nullptr));
        simbadCooStatusLabel->setText(QString());
        label->setText(QCoreApplication::translate("searchDialogForm", "Note: this tool doesn't apply the refraction correction for coordinates.", nullptr));
        AxisYLabel->setText(QString());
        coordinateSystemLabel->setText(QCoreApplication::translate("searchDialogForm", "Coordinate system:", nullptr));
        AxisXLabel->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabPosition), QCoreApplication::translate("searchDialogForm", "Position", nullptr));
#if QT_CONFIG(tooltip)
        objectTypeComboBox->setToolTip(QCoreApplication::translate("searchDialogForm", "Some objects may be found after activation respective plug-ins", nullptr));
#endif // QT_CONFIG(tooltip)
        searchInEnglishCheckBox->setText(QCoreApplication::translate("searchDialogForm", "names in English", nullptr));
        searchInListLineEdit->setPlaceholderText(QCoreApplication::translate("searchDialogForm", "Search in list...", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLists), QCoreApplication::translate("searchDialogForm", "Lists", nullptr));
#if QT_CONFIG(tooltip)
        simbadGroupBox->setToolTip(QCoreApplication::translate("searchDialogForm", "Use on-line astronomical database SIMBAD", nullptr));
#endif // QT_CONFIG(tooltip)
        simbadGroupBox->setTitle(QCoreApplication::translate("searchDialogForm", "Use SIMBAD", nullptr));
        labelServer->setText(QCoreApplication::translate("searchDialogForm", "Server:", nullptr));
        searchOptions->setTitle(QCoreApplication::translate("searchDialogForm", "Search options", nullptr));
        checkBoxUseStartOfWords->setText(QCoreApplication::translate("searchDialogForm", "Use autofill only from the beginning of words", nullptr));
        checkBoxLockPosition->setText(QCoreApplication::translate("searchDialogForm", "Lock position when coordinates are used", nullptr));
        checkBoxFOVCenterMarker->setText(QCoreApplication::translate("searchDialogForm", "Show FOV center marker when position is search", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabOptions), QCoreApplication::translate("searchDialogForm", "Options", nullptr));
    } // retranslateUi

};

namespace Ui {
    class searchDialogForm: public Ui_searchDialogForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEARCHDIALOGGUI_H
