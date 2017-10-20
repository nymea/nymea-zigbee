#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <unistd.h>

#include "core.h"
#include "loggingcategory.h"

static const char *const normal = "\033[0m";
static const char *const warning = "\e[33m";
static const char *const error = "\e[31m";

static QHash<QString, bool> s_loggingFilters;

static void loggingCategoryFilter(QLoggingCategory *category)
{
    // If this is a known category
    if (s_loggingFilters.contains(category->categoryName())) {
        category->setEnabled(QtDebugMsg, s_loggingFilters.value(category->categoryName()));
        category->setEnabled(QtWarningMsg, true);
        category->setEnabled(QtCriticalMsg, true);
        category->setEnabled(QtFatalMsg, true);
    } else {
        //Disable default debug messages, print only >= warnings
        category->setEnabled(QtDebugMsg, false);
        category->setEnabled(QtWarningMsg, true);
        category->setEnabled(QtCriticalMsg, true);
        category->setEnabled(QtFatalMsg, true);
    }
}

static void consoleLogHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    switch (type) {
    case QtInfoMsg:
        fprintf(stdout, " I | %s: %s\n", context.category, message.toUtf8().data());
        break;
    case QtDebugMsg:
        fprintf(stdout, " I | %s: %s\n", context.category, message.toUtf8().data());
        break;
    case QtWarningMsg:
        fprintf(stdout, "%s W | %s: %s%s\n", warning, context.category, message.toUtf8().data(), normal);
        break;
    case QtCriticalMsg:
        fprintf(stdout, "%s C | %s: %s%s\n", error, context.category, message.toUtf8().data(), normal);
        break;
    case QtFatalMsg:
        fprintf(stdout, "%s F | %s: %s%s\n", error, context.category, message.toUtf8().data(), normal);
        break;
    }
    fflush(stdout);
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(consoleLogHandler);

    QCoreApplication application(argc, argv);

    // Command line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(QString("\nDaemon for the zigbee NXP uart bridge.\n\nCopyright %1 2016 Simon Stürz <simon.stuerz@guh.io>\nAll rights reserved.").arg(QChar(0xA9)));

    QCommandLineOption debugLevelOption(QStringList() << "d" << "debug-level", "Set debug level [1-4].");
    debugLevelOption.setDefaultValue("1");
    debugLevelOption.setValueName("level");
    parser.addOption(debugLevelOption);

    parser.process(application);

    bool debugLevelValueOk = false;
    int debugLevel = parser.value(debugLevelOption).toInt(&debugLevelValueOk);

    if (debugLevel < 1 || debugLevel > 4 || !debugLevelValueOk) {
        qWarning() << "Invalid debug level passed:" << parser.value(debugLevelOption);
        debugLevel = 1;
    }

    s_loggingFilters.insert("Zigbee", true);
    s_loggingFilters.insert("ZigbeeController", (debugLevel > 1));
    s_loggingFilters.insert("ZigbeeInterface", (debugLevel > 2));
    s_loggingFilters.insert("ZigbeeInterfaceTraffic", (debugLevel > 3));

    QLoggingCategory::installFilter(loggingCategoryFilter);

    Core core;

    return application.exec();
}
