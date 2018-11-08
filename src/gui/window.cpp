#include "window.hpp"
#include "ui_window.h"

#include <QFileSystemModel>
#include <QTreeView>

window::window(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::window) {

	ui->setupUi(this);

	auto root {QDir::drives().first().filePath()};

	model_src_tree.setReadOnly(true);
	model_src_tree.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
	model_src_tree.sort(0);
	model_src_tree.setRootPath(root);
	ui->tree_view_src->setColumnHidden(2, true);
	ui->tree_view_src->setColumnHidden(3, true);
	ui->tree_view_src->setModel(&model_src_tree);
	ui->tree_view_src->setColumnHidden(2, true);
	ui->tree_view_src->setColumnHidden(3, true);

	model_src_list.setRootPath(root);
	ui->list_view_src->setModel(&model_src_list);

	model_dst.setReadOnly(true);
	model_dst.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
	model_dst.sort(0);
	model_dst.setRootPath(root);
	ui->tree_view_dst->setModel(&model_dst);

	ui->label_src->setText(root);
	ui->label_dst->setText(root);
}

window::~window() {
	delete ui;
}

void window::on_tree_view_src_clicked(const QModelIndex &index) {
	const QString path {model_src_tree.filePath(index)};

	model_src_list.setRootPath(path);
	ui->list_view_src->setRootIndex(model_src_list.index(path));

	ui->label_src->setText(QDir().toNativeSeparators(path));
}

void window::on_tree_view_src_doubleClicked(const QModelIndex &index) {
	const QString path {model_src_tree.filePath(index)};

//	ui->tree_view_src->collapseAll();
//	auto current = model_src_tree.index(path);
//	while (current.isValid()) {
//		ui->tree_view_src->expand(current);
//		current = current.parent();
//	}

//	model_src_list.setRootPath(path);
//	ui->list_view_src->setRootIndex(model_src_list.index(path));

	ui->label_src->setText(QDir().toNativeSeparators(path));
}

void window::on_list_view_src_doubleClicked(const QModelIndex &index) {
	const QString path {model_src_list.filePath(index)};

//	ui->tree_view_src->collapseAll();
	auto current = model_src_tree.index(path);
	while (current.isValid()) {
		ui->tree_view_src->expand(current);
		current = current.parent();
	}

	model_src_list.setRootPath(path);
	ui->list_view_src->setRootIndex(model_src_list.index(path));

	ui->label_src->setText(QDir().toNativeSeparators(path));
}
