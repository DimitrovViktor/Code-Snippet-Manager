#ifndef csm_H
#define csm_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <string>
#include <vector>

#include "sqlite/sqlite3.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CSM; }
QT_END_NAMESPACE

struct Snippet {
    int id;
    std::string code;
    std::string language;
    std::string tags;
};

class CSM : public QMainWindow {
    Q_OBJECT

public:
    explicit CSM(QWidget *parent = nullptr);
    ~CSM();

private slots:
    void onAdd();
    void onEdit();
    void onDelete();
    void onCopy();
    void onSearch(const QString &text);
    void onFilterChanged(int index);
    void onSnippetSelected(QListWidgetItem *item);

private:
    Ui::CSM *ui;
    sqlite3 *m_db = nullptr;
    std::vector<Snippet> m_snippets;

    void initDb();
    void loadSnippets();
    void refreshList();
    void filterSnippets(const std::string &term, int filterMode);
    void showSnippet(const Snippet &snippet);
    void clearPreview();
    int selectedSnippetId();
    void setStatus(const QString &message);
};

#endif
