#include "window.hpp"
#include <QApplication>

#include <QFileSystemModel>
#include <QTreeView>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	window w;
	w.show();

/*
	QFileSystemModel modelSrc;
	modelSrc.setRootPath("C:/");
	QTreeView *treeViewSrc = w.findChild<QTreeView*>("treeViewSrc");
	treeViewSrc->setModel(&modelSrc);
//	treeViewSrc->setAnimated(false);
//	treeViewSrc->setSortingEnabled(true);

	QFileSystemModel modelDst;
	modelDst.setRootPath("C:/");
	QTreeView *treeViewDst = w.findChild<QTreeView*>("treeViewDst");
	treeViewDst->setModel(&modelDst);
//	treeViewDst->setAnimated(false);
//	treeViewDst->setSortingEnabled(true);
*/
	return a.exec();
}
