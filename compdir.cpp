#include "compdir.h"
#include "ui_compdir.h"

CompDir::CompDir(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CompDir)
{
	ui->setupUi(this);
	TAB = "    ";
	tamMin = 1e6;
}

CompDir::~CompDir() {
	delete ui;
}

t_arq CompDir::readDir(QString d, QString esp) {
	DIR *dir;
	t_arq a;
	if ((dir = opendir(d.toUtf8().constData())) != nullptr) {
		struct dirent *ent; // https://stackoverflow.com/a/612176/1086511
		struct stat buf; // https://linux.die.net/man/2/stat
		unsigned long long tam = 0; // tamanho total em bytes
		t_arq arq;
		while ((ent = readdir(dir)) != nullptr) {
			if (strcmp(ent->d_name,"..") != 0 && strcmp(ent->d_name,".") != 0) { // não é . nem ..
				QString dName;
				try {
					dName = QString::fromUtf8(ent->d_name);
				}
				catch (std::exception& e) {
					std::wcout << "Erro: " << e.what() << " (em “" << ent->d_name << "”)\n";
				}
				if (ent->d_type == DT_DIR) { // diretório
					arq = readDir(d+"/"+dName,esp+TAB);
					if (arq.nome.compare("") != 0) {
						tam += arq.tam;
						if (arq.tam > tamMin) {
							a.dir.push_back(arq);
						}
					}
				} else { // arquivo
					if (stat((d+"/"+dName).toUtf8().constData(),&buf) == 0) {
						tam += static_cast<unsigned long long>(buf.st_size); // seguro ??
					}
				}
			}
		}
		closedir(dir);
		int pos = d.lastIndexOf("/");
		a.esp = esp;
		a.nome = d.mid(pos+1);
		a.path = d.mid(0,pos);
		a.tam = tam;
		a.tipo = 'd';
	} else {
		std::wcout << "Não foi possível abrir o diretório “" << d.toUtf8().constData() << "”.\n";
	}
	return a;
}

t_dirName CompDir::listaDir(QString d) {
	DIR *dir;
	struct dirent *ent; // https://stackoverflow.com/a/612176/1086511
	t_dirName td;
	if ((dir = opendir(d.toUtf8().constData())) != nullptr) {
		while ((ent = readdir(dir)) != nullptr) {
			if (strcmp(ent->d_name,"..") != 0 && strcmp(ent->d_name,".") != 0) { // não é . nem ..
				QString dName;
				try {
					dName = QString::fromUtf8(ent->d_name);
				}
				catch (std::exception& e) {
					std::wcout << "Erro: " << e.what() << " (em “" << ent->d_name << "”)\n";
				}
				if (ent->d_type == DT_DIR) { // diretório
					td.push_back(dName);
				}
			}
		}
		closedir(dir);
	} else {
		std::cout << "Não foi possível abrir o diretório “" << d.toUtf8().constData() << "”.\n";
	}
	return td;
}

t_dirSize CompDir::procuraParecidos(t_arq a,t_arq orig) {
	t_dirSize dir;
	t_dirSize dir2;
	t_dirSz dsz;
	QString ws;
	if (a.tipo == 'd') {
		unsigned long long tamMax;
		for (const auto& d : a.dir) {
			if (d.path.compare(orig.path) != 0 && d.nome.compare(orig.nome) == 0 &&
					orig.tam > 0 && d.tam/orig.tam > 0.1 && d.tam/orig.tam < 10) {
				tamMax = std::max(d.tam,orig.tam);
				if (tamMax == d.tam) {
					ws = d.path+"/"+d.nome+" ("+sepMilhar(d.tam)+") x "+
							orig.path+"/"+orig.nome+" ("+sepMilhar(orig.tam)+")";
				} else {
					ws = orig.path+"/"+orig.nome+" ("+sepMilhar(orig.tam)+") x "+
							d.path+"/"+d.nome+" ("+sepMilhar(d.tam)+")";
				}
				dsz.nome = ws;
				dsz.tam = tamMax;
				dir.push_back(dsz);
			}
			dir2 = procuraParecidos(d,orig);
			if (dir2.size() > 0) {
				dir.insert(dir.end(),dir2.begin(),dir2.end());
			}
		}
	}
	return dir;
}

t_dirSize CompDir::printDir(t_arq a,t_arq raiz) {
	t_dirSize dir;
	t_dirSize dir2;
	if (a.tipo == 'd') {
		if (a.nome.compare(raiz.nome) != 0 || a.path.compare(raiz.path) != 0) {
			ui->statusBar->showMessage(a.path+"/"+a.nome);
			ui->statusBar->repaint();
			std::wcout << (a.esp+a.nome+" ("+sepMilhar(a.tam)+")\n").toStdWString();
			dir = procuraParecidos(raiz,a);
		}
		for (const auto& d : a.dir) {
			if (d.tipo == 'd') {
				if (a.nome.compare(raiz.nome) == 0 && a.path.compare(raiz.path) == 0) {
					QList<QListWidgetItem*> L = ui->listDir1->findItems(d.nome,Qt::MatchFixedString|Qt::MatchCaseSensitive);
					if (L.size() > 0) {
						QListWidgetItem* lwi = L.at(0);
						ui->listDir1->clearSelection();
						ui->listDir1->setItemSelected(lwi,true);
						ui->listDir1->scrollToItem(lwi);
						qApp->processEvents();
					}
				}
				dir2 = printDir(d,raiz);
				if (dir2.size() > 0) {
					dir.insert(dir.end(),dir2.begin(),dir2.end());
				}
			}
		}
	}
	return dir;
}

void CompDir::on_btnFind_clicked() {
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setViewMode(QFileDialog::List);
	QStringList fileNames;
	if (dialog.exec()) {
		fileNames = dialog.selectedFiles();
		nomeDir = fileNames[0];
		ui->edtPath->setText(fileNames[0]);
		ui->actionListar->setEnabled(true);
	}
}

void CompDir::on_actionListar_triggered() {
	std::wcout << (QString::fromUtf8("Listando “")+nomeDir+QString::fromUtf8("”\n\n")).toStdWString();
	t_dirName td = listaDir(nomeDir);
	ui->listDir1->clear();
	ui->listDir2->clear();
	ui->listResults->clear();
	for (const auto& d : td) {
		ui->listDir1->addItem(d);
	}
	ui->actionAnalisar->setEnabled(ui->listDir1->count() > 0);
}

void CompDir::on_listDir1_itemSelectionChanged() {
	ui->btnRemove->setEnabled(ui->listDir1->selectedItems().length() > 0);
}

void CompDir::on_btnRemove_clicked() {
	int i=0;
	for (const auto& index : ui->listDir1->selectionModel()->selectedIndexes()) {
		ui->listDir2->insertItem(ui->listDir2->count(),ui->listDir1->takeItem(index.row()-i++));
	}
	ui->actionAnalisar->setEnabled(ui->listDir1->count() > 0);
}

void CompDir::on_btnInsert_clicked() {
	int i=0;
	for (const auto& index : ui->listDir2->selectionModel()->selectedIndexes()) {
		ui->listDir1->insertItem(ui->listDir1->count(),ui->listDir2->takeItem(index.row()-i++));
	}
	ui->actionAnalisar->setEnabled(ui->listDir1->count() > 0);
}

void CompDir::on_listDir2_itemSelectionChanged() {
	ui->btnInsert->setEnabled(ui->listDir2->selectedItems().length() > 0);
}

void CompDir::on_actionAnalisar_triggered() {
	t_arq arq;
	arq.nome = nomeDir;
	arq.tipo = 'd';
	arq.tam = 0;
	t_arq a;
	for (int i=0; i<ui->listDir1->count(); i++) {
		a = readDir(nomeDir+"/"+ui->listDir1->item(i)->text(),TAB);
		arq.dir.push_back(a);
		arq.tam += a.tam;
	}
	std::wcout.imbue(std::locale("pt_BR.UTF-8")); // pra incluir separador de milhar
	std::wcout << (nomeDir+" ("+sepMilhar(arq.tam)+")\n").toStdWString();
	ui->statusBar->showMessage("Analisando estrutura de diretórios...");
	ui->statusBar->repaint();
	t_dirSize dir = printDir(arq,arq); // faz a comparação... demora
	if (dir.size() == 0) {
		ui->statusBar->showMessage(QString::fromUtf8("Nenhum diretório repetido encontrado."));
	} else {
		ui->statusBar->showMessage("Listando diretórios repetidos... ("+sepMilhar(dir.size())+")");
		ui->statusBar->repaint();
		std::sort(dir.begin(),dir.end());
		for (const auto& d : dir) {
			if (ui->listResults->findItems(d.nome,Qt::MatchFixedString|Qt::MatchCaseSensitive).size() == 0) { // não add repetidos
				QListWidgetItem* item = new QListWidgetItem;
				item->setText(d.nome);
				item->setData(Qt::UserRole,QVariant(d.tam));
				ui->listResults->addItem(item);
			}
		}
	}
	ui->statusBar->clearMessage();
}

QString CompDir::sepMilhar(unsigned long long v) {
	std::string s;
	std::string parte;
	do {
		parte = std::to_string(v%1000);
		while (parte.length() < 3)
			parte = '0'+parte;
		s = parte + s;
		if (v > 999)
			s = '.' + s;
		v /= 1000;
	} while (v);
	while (s[0] == '0')
		s = s.substr(1);
	return QString::fromStdString(s);
}

void CompDir::on_listResults_itemSelectionChanged()
{
	unsigned long long tam = ui->listResults->currentItem()->data(Qt::UserRole).toULongLong();
	ui->statusBar->showMessage(sepMilhar(tam));
}
