#ifndef DICONLOOKUP_H
#define DICONLOOKUP_H

#include <QObject>

/**
 * @brief Provides icon lookup of libdtk available images/icons
 */

class DIconic : public QObject
{
    Q_OBJECT
public:
    explicit DIconic(QObject *parent = nullptr);
    ~DIconic();

    enum IconScopes {
        All,
        BuiltIn,
        BuiltInIcon,
        Assets,
        Light,
        Dark,
        BuiltInText,
        Chameleon
    };

    /**
     * @brief fromString
     * @param icon_name
     * @param scope
     * @return QIcon* with the icon or nullptr if not found.
     */
    static QIcon *fromString(const QString icon_name, IconScopes scope = IconScopes::All);

    /**
     * @brief list
     * @return QStringList of available icons (and their paths)
     */
    static QStringList list();

    /**
     * @brief listPaths
     * @return QStringList of path used to find icons/images
     */
    static QStringList listPaths();
};

#endif // DICONLOOKUP_H
