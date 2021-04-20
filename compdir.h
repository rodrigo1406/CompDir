#ifndef COMPDIR_H
#define COMPDIR_H

#include <QMainWindow>
#include <QFileDialog>
#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include <vector>

struct t_dirSz {
	QString nome;
	unsigned long long tam;
	bool operator<(const t_dirSz& a) const {
		return tam > a.tam;
	}
};

typedef std::vector<t_dirSz> t_dirSize;

typedef std::vector<QString> t_dirName;

struct t_arq {
	char tipo; // 'd' (diretório) ou 'a' (arquivo)
	std::vector<t_arq> dir; // conteúdo (só para diretórios)
	QString esp;
	QString nome;
	QString path;
	unsigned long long tam;
};

namespace Ui {
	class CompDir;
}

class CompDir : public QMainWindow
{
	Q_OBJECT

public:
	explicit CompDir(QWidget *parent = nullptr);
	~CompDir();

private slots:
	void on_btnFind_clicked();
	void on_actionListar_triggered();
	void on_listDir1_itemSelectionChanged();
	void on_btnRemove_clicked();
	void on_btnInsert_clicked();
	void on_listDir2_itemSelectionChanged();
	void on_actionAnalisar_triggered();
	void on_listResults_itemSelectionChanged();

private:
	Ui::CompDir *ui;
	QString nomeDir;
	QString TAB;
	unsigned long long tamMin;
	t_arq readDir(QString d, QString esp);
	t_dirName listaDir(QString d);
	t_dirSize printDir(t_arq a,t_arq raiz);
	t_dirSize procuraParecidos(t_arq a,t_arq orig);
	QString sepMilhar(unsigned long long v);
};

#endif // COMPDIR_H
