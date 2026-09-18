#include "csm.h"
#include "ui_CSM.h"

#include <QClipboard>
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>

CSM::CSM(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::CSM) {
    ui->setupUi(this);
    initDb();
    loadSnippets();
    refreshList();

    connect(ui->add_btn, &QPushButton::clicked, this, &CSM::onAdd);
    connect(ui->edit_btn, &QPushButton::clicked, this, &CSM::onEdit);
    connect(ui->delete_btn, &QPushButton::clicked, this, &CSM::onDelete);
    connect(ui->copy_btn, &QPushButton::clicked, this, &CSM::onCopy);
    connect(ui->search_bar, &QLineEdit::textChanged, this, &CSM::onSearch);
    connect(ui->filter_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CSM::onFilterChanged);
    connect(ui->snippet_list, &QListWidget::itemClicked, this, &CSM::onSnippetSelected);
}

CSM::~CSM() {
    if (m_db) sqlite3_close(m_db);
    delete ui;
}

void CSM::setStatus(const QString &message) {
    ui->statusbar->showMessage(message, 5000);
}

void CSM::initDb() {
    sqlite3_open("snippets.db", &m_db);
    const char *sql = "CREATE TABLE IF NOT EXISTS SNIPPETS("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "code TEXT,"
                      "language TEXT,"
                      "tags TEXT);";
    char *err = nullptr;
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        setStatus(QString("DB error: %1").arg(err));
        sqlite3_free(err);
    }
}

void CSM::loadSnippets() {
    m_snippets.clear();
    const char *sql = "SELECT id, code, language, tags FROM SNIPPETS ORDER BY id DESC";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Snippet s;
        s.id = sqlite3_column_int(stmt, 0);
        s.code = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1) ? sqlite3_column_text(stmt, 1) : (const unsigned char *)"");
        s.language = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2) ? sqlite3_column_text(stmt, 2) : (const unsigned char *)"");
        s.tags = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3) ? sqlite3_column_text(stmt, 3) : (const unsigned char *)"");
        m_snippets.push_back(s);
    }
    sqlite3_finalize(stmt);
}

void CSM::refreshList() {
    ui->snippet_list->clear();
    clearPreview();
    std::string searchTerm = ui->search_bar->text().toStdString();
    int filterMode = ui->filter_combo->currentIndex();

    for (const auto &s : m_snippets) {
        bool match = true;
        if (!searchTerm.empty()) {
            std::string lower_term = searchTerm;
            std::transform(lower_term.begin(), lower_term.end(), lower_term.begin(), ::tolower);

            auto containsLower = [&](const std::string &src) {
                std::string lower = src;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                return lower.find(lower_term) != std::string::npos;
            };

            switch (filterMode) {
                case 0: match = containsLower(s.code) || containsLower(s.language) || containsLower(s.tags); break;
                case 1: match = containsLower(s.language); break;
                case 2: match = containsLower(s.tags); break;
                case 3: match = containsLower(s.code); break;
            }
        }

        if (match) {
            QString label = QString("[%1] %2 - %3")
                .arg(s.id)
                .arg(QString::fromStdString(s.language))
                .arg(QString::fromStdString(s.tags));
            QListWidgetItem *item = new QListWidgetItem(label);
            item->setData(Qt::UserRole, s.id);
            ui->snippet_list->addItem(item);
        }
    }
}

void CSM::showSnippet(const Snippet &snippet) {
    ui->preview_lang_label->setText(QString("Language: %1").arg(QString::fromStdString(snippet.language)));
    ui->preview_tags_label->setText(QString("Tags: %1").arg(QString::fromStdString(snippet.tags)));
    ui->preview_code->setPlainText(QString::fromStdString(snippet.code));
}

void CSM::clearPreview() {
    ui->preview_lang_label->setText("Language: ");
    ui->preview_tags_label->setText("Tags: ");
    ui->preview_code->clear();
}

int CSM::selectedSnippetId() {
    QListWidgetItem *item = ui->snippet_list->currentItem();
    if (!item) return -1;
    return item->data(Qt::UserRole).toInt();
}

void CSM::onSnippetSelected(QListWidgetItem *item) {
    int id = item->data(Qt::UserRole).toInt();
    for (const auto &s : m_snippets) {
        if (s.id == id) {
            showSnippet(s);
            return;
        }
    }
}

void CSM::onSearch(const QString &) {
    refreshList();
}

void CSM::onFilterChanged(int) {
    refreshList();
}

void CSM::onAdd() {
    bool ok = false;
    QString code = QInputDialog::getMultiLineText(this, "Add Snippet", "Code:", "", &ok);
    if (!ok || code.isEmpty()) return;

    QString language = QInputDialog::getText(this, "Add Snippet", "Language:", QLineEdit::Normal, "", &ok);
    if (!ok || language.isEmpty()) return;

    QString tags = QInputDialog::getText(this, "Add Snippet", "Tags (comma separated):", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    const char *sql = "INSERT INTO SNIPPETS(code, language, tags) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        setStatus("Failed to add snippet.");
        return;
    }
    sqlite3_bind_text(stmt, 1, code.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, language.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, tags.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    loadSnippets();
    refreshList();
    setStatus("Snippet added.");
}

void CSM::onEdit() {
    int id = selectedSnippetId();
    if (id < 0) {
        setStatus("Select a snippet first.");
        return;
    }

    const Snippet *current = nullptr;
    for (const auto &s : m_snippets) {
        if (s.id == id) { current = &s; break; }
    }
    if (!current) return;

    bool ok = false;
    QString code = QInputDialog::getMultiLineText(this, "Edit Snippet", "Code:", QString::fromStdString(current->code), &ok);
    if (!ok) return;

    QString language = QInputDialog::getText(this, "Edit Snippet", "Language:", QLineEdit::Normal, QString::fromStdString(current->language), &ok);
    if (!ok) return;

    QString tags = QInputDialog::getText(this, "Edit Snippet", "Tags:", QLineEdit::Normal, QString::fromStdString(current->tags), &ok);
    if (!ok) return;

    const char *sql = "UPDATE SNIPPETS SET code=?, language=?, tags=? WHERE id=?";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        setStatus("Failed to edit snippet.");
        return;
    }
    sqlite3_bind_text(stmt, 1, code.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, language.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, tags.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    loadSnippets();
    refreshList();
    setStatus("Snippet updated.");
}

void CSM::onDelete() {
    int id = selectedSnippetId();
    if (id < 0) {
        setStatus("Select a snippet first.");
        return;
    }

    QMessageBox::StandardButton confirm = QMessageBox::question(
        this, "Delete", QString("Delete snippet #%1?").arg(id));
    if (confirm != QMessageBox::Yes) return;

    const char *sql = "DELETE FROM SNIPPETS WHERE id=?";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        setStatus("Failed to delete snippet.");
        return;
    }
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    loadSnippets();
    refreshList();
    setStatus("Snippet deleted.");
}

void CSM::onCopy() {
    int id = selectedSnippetId();
    if (id < 0) {
        setStatus("Select a snippet first.");
        return;
    }

    for (const auto &s : m_snippets) {
        if (s.id == id) {
            QApplication::clipboard()->setText(QString::fromStdString(s.code));
            setStatus("Code copied to clipboard.");
            return;
        }
    }
}
