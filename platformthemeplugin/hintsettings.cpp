#include "hintsettings.h"

#include <QDebug>
#include <QDir>
#include <QString>
#include <QFileInfo>
#include <QToolBar>
#include <QPalette>
#include <QToolButton>
#include <QMainWindow>
#include <QApplication>
#include <QGuiApplication>
#include <QDialogButtonBox>
#include <QScreen>
#include <QStandardPaths>
#include <QTemporaryFile>

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>

static const QByteArray s_systemFontName = QByteArrayLiteral("Font");
static const QByteArray s_systemFixedFontName = QByteArrayLiteral("FixedFont");
static const QByteArray s_systemPointFontSize = QByteArrayLiteral("FontSize");
static const QByteArray s_darkModeName = QByteArrayLiteral("DarkMode");
static const QByteArray s_lightIconName = QByteArrayLiteral("elementary-xfce");
static const QByteArray s_darkIconName = QByteArrayLiteral("elementary-xfce-dark");

HintsSettings::HintsSettings(QObject *parent)
    : QObject(parent),
      m_settings(new QSettings(QSettings::UserScope, "panda", "theme"))
{
    m_hints[QPlatformTheme::SystemIconThemeName] = darkMode() ? s_darkIconName : s_lightIconName;
    m_hints[QPlatformTheme::StyleNames] = "panda";
    m_hints[QPlatformTheme::SystemIconFallbackThemeName] = QStringLiteral("hicolor");
    m_hints[QPlatformTheme::IconThemeSearchPaths] = xdgIconThemePaths();
    m_hints[QPlatformTheme::UseFullScreenForPopupMenu] = false;

    // probono: Default button on the right-hand side, no icons in buttons
    m_hints[QPlatformTheme::DialogButtonBoxLayout] = QDialogButtonBox::MacLayout;
    m_hints[QPlatformTheme::DialogButtonBoxButtonsHaveIcons] = false;

    // probono: Mac-like shortcuts
    m_hints[QPlatformTheme::KeyboardScheme] = QPlatformTheme::MacKeyboardScheme;

    // NOTE: Should it be neccessary to swap CTRL and ALT, then see
    // https://code.qt.io/cgit/qt/qtbase.git/tree/src/gui/kernel/qplatformtheme.cpp?h=5.13&id=7db9e02ad11c391c1d616defd11e7deb2718d60a#n610

    // probono: Display shortcut key sequences in context menus
    m_hints[QPlatformTheme::ShowShortcutsInContextMenus] = true;

    m_settingsFile = m_settings->fileName();

    QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

HintsSettings::~HintsSettings()
{
}

void HintsSettings::lazyInit()
{
}

QStringList HintsSettings::xdgIconThemePaths() const
{
    QStringList paths;

    // make sure we have ~/.local/share/icons in paths if it exists
    paths << QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("icons"), QStandardPaths::LocateDirectory);

    const QFileInfo homeIconDir(QDir::homePath() + QStringLiteral("/.icons"));
    if (homeIconDir.isDir()) {
        paths << homeIconDir.absoluteFilePath();
    }

    return paths;
}

QString HintsSettings::systemFont() const
{
    return m_settings->value(s_systemFontName, "Nimbus Sans").toString();
}

QString HintsSettings::systemFixedFont() const
{
    return m_settings->value(s_systemFixedFontName, "Monospace").toString();
}

qreal HintsSettings::systemFontPointSize() const
{
    return m_settings->value(s_systemPointFontSize, 10).toDouble(); // was 10.5
}

bool HintsSettings::darkMode()
{
    return m_settings->value(s_darkModeName, false).toBool();
}
