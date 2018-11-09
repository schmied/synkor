
#include "window.hpp"

#include "ui_window.h"

window::window(QWidget *parent) :
	QMainWindow(parent),
	ui_(new Ui::window) {

	ui_->setupUi(this);
	ui_->tree_view_src->set_ui(ui_);
	ui_->tree_view_dst->set_ui(ui_);

	auto root {QDir::drives().first().filePath()};

	model_src_list_.setReadOnly(true);
	model_src_list_.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
	model_src_list_.sort(0);
	ui_->list_view_src->setModel(&model_src_list_);
	model_src_list_.setRootPath(root);

	ui_->label_src->setText(root);
	ui_->label_dst->setText(root);
}

window::~window() {
	delete ui_;
}

void window::on_tree_view_src_clicked(const QModelIndex &index) {
	const QString path {ui_->tree_view_src->model()->filePath(index)};

	model_src_list_.setRootPath(path);
	ui_->list_view_src->setRootIndex(model_src_list_.index(path));

	ui_->label_src->setText(QDir().toNativeSeparators(path));
}

void window::on_tree_view_src_doubleClicked(const QModelIndex &index) {
	const QString path {ui_->tree_view_src->model()->filePath(index)};
	ui_->label_src->setText(QDir().toNativeSeparators(path));
}

void window::on_list_view_src_doubleClicked(const QModelIndex &index) {
	const QString path {model_src_list_.filePath(index)};

//	ui->tree_view_src->collapseAll();
	auto current = ui_->tree_view_src->model()->index(path);
	ui_->tree_view_src->selectionModel()->clearSelection();
	ui_->tree_view_src->selectionModel()->select(current, QItemSelectionModel::Select);
	while (current.isValid()) {
		ui_->tree_view_src->expand(current);
		current = current.parent();
	}

	model_src_list_.setRootPath(path);
	ui_->list_view_src->setRootIndex(model_src_list_.index(path));

	ui_->label_src->setText(QDir().toNativeSeparators(path));
}
