
#include "window.hpp"

#include "base_view.hpp"

#include "ui_window.h"

window::window(QWidget *parent) :
	QMainWindow(parent),
	ui_(new Ui::window) {

	ui_->setupUi(this);

	base_view_ = new base_view {ui_->tree_view_src, ui_->list_view_src, ui_->label_src, ui_->status_bar};
	ui_->tree_view_src->set_base_view(base_view_);
	ui_->list_view_src->set_base_view(base_view_);
}

window::~window() {
	delete base_view_;
	delete ui_;
}
