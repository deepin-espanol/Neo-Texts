#ifndef DOPENAPPLICATION_H
#define DOPENAPPLICATION_H

#include <DApplication>

/**
 * @brief Provides signal handling and open funcs
 */

class DOpenApplication : public Dtk::Widget::DApplication
{
    Q_OBJECT
public:
    explicit DOpenApplication(int &argc, char **argv);
    ~DOpenApplication() override;

    void handleQuitAction() override;
    void handleHelpAction() override;
    void handleAboutAction() override;
};

#endif // DOPENAPPLICATION_H
