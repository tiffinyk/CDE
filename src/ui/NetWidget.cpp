#include "ui/NetWidget.h"

//namespace FPGAViewer {
	/*
	void RouteList::set_net_list(const QStringList& nets)
	{
		clear(); // clear all first
		// set a empty list will delete the route list

		for (QString net : nets)
			QListWidgetItem* item = new QListWidgetItem(net, this);
	}

	void RouteList::currentChanged(const QModelIndex& current, const QModelIndex& previous)
	{
		QListWidget::currentChanged(current, previous);
		string cur(current.data().toString().toStdString());
		string pre(previous.data().toString().toStdString());
		emit select_changed(cur, pre);
	}

	void RouteList::mouseReleaseEvent(QMouseEvent* event)
	{
		QListWidget::mouseReleaseEvent(event);
		emit rt_list_drag(0, false);
		//string cur(currentItem()->text().toStdString());
		//emit select_changed(cur, "");
	}

	void RouteList::mousePressEvent(QMouseEvent* event)
	{
		QListWidget::mousePressEvent(event);
		if (event->button() == Qt::LeftButton)
		{
			QListWidgetItem* item = currentItem();
			if (!item) return;
			emit rt_list_drag(item, true);

			QDrag* drag = new QDrag(this);
			QMimeData* mimeData = new QMimeData;

			mimeData->setText(item->text());
			drag->setMimeData(mimeData);

			Qt::DropAction dropAction = drag->exec();
		}
	}

	void RouteList::set_selected_route(int row)
	{
		setCurrentRow(row);
	}

	HRouteList::HRouteList(QWidget* parent) : RouteList(parent) {}

	void HRouteList::set_net_list(const QStringList& nets)
	{
		clear(); // clear all first
		// set a empty list will delete the route list

		for (QString net : nets)
			QListWidgetItem* item = new QListWidgetItem(net, this);
	}*/

	/*void TileList::set_tile_list(const QStringList& a)
	{
		clear();
		for (QString s : a)
		{
			QListWidgetItem* item = new QListWidgetItem(s, this);
		}
	}*/

	void NetListWidget::add_netlist(std::string* name, QListWidgetItem* item)
	{
		item->setText(QString::fromStdString(*name));
		addItem(item);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(Qt::Checked);
		//netlist_state.emplace_back(true);
	}

	void DFGListWidget::add_dfg(std::string* name, QListWidgetItem* item)
	{
		item->setText(QString::fromStdString(*name));
		addItem(item);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(Qt::Checked);
		//netlist_state.emplace_back(true);
	}

	/*void NetListWidget::clear()
	{
		netlist_state.clear();
		QListWidget::clear();
	}*/

	FileListWidget::FileListWidget(QWidget* parent) : QTreeWidget(parent)
	{
		//setFrameStyle(QFrame::NoFrame);
		setHeaderHidden(true);
		project_file = new QTreeWidgetItem(this);
		project_file->setHidden(true);
		project_file->setText(0, "Project File");
		addTopLevelItem(project_file);

		adg_file = new QTreeWidgetItem(project_file);
		adg_file->setHidden(true);
		adg_file->setText(0, "ADG File");

		operation_file = new QTreeWidgetItem(project_file);
		operation_file->setHidden(true);
		operation_file->setText(0, "Operation File");

		dfg_file = new QTreeWidgetItem(project_file);
		dfg_file->setHidden(true);
		dfg_file->setText(0, "DFG File");

		netlist_file = new QTreeWidgetItem(project_file);
		netlist_file->setHidden(true);
		netlist_file->setText(0, "Netlist File");
	}
	
	void FileListWidget::add_file(int file_type, std::string* file_name)
	{
		QTreeWidgetItem* _file;
		switch(file_type)
		{
			case ADG_FILE:
			{
			_file = new QTreeWidgetItem(adg_file);
			adg_file->setHidden(false);
			break;
			}
			case OPERATION_FILE:
			{
			_file = new QTreeWidgetItem(operation_file);
			operation_file->setHidden(false);
			break;
			}
			case DFG_FILE:
			{
			_file = new QTreeWidgetItem(dfg_file);
			dfg_file->setHidden(false);
			break;
			}
			case NETLIST_FILE:
			{
			_file = new QTreeWidgetItem(netlist_file);
			netlist_file->setHidden(false);
			break;
			}
			default:
			{
			_file = new QTreeWidgetItem;
			project_file->insertChild(0, _file);
			break;
			}
		}
		_file->setText(0, QString::fromStdString(*file_name));
		if(project_file->isHidden())	{ project_file->setHidden(false); }
	}

	void FileListWidget::delete_all_files()
	{
		QTreeWidgetItem *child, *grandchild;
		int i = 0;
		while(i < project_file->childCount())
		{
			child = project_file->child(i);
			if(child == adg_file || child == operation_file || child == dfg_file || child == netlist_file)
			{
				while(child->childCount() > 0)
				{
					grandchild = child->child(0);
					child->removeChild(grandchild);
					delete grandchild;
				}
				i++;
			}
			else
			{
				project_file->removeChild(child);
				delete child;
			}
		}
		project_file->setHidden(true);
		adg_file->setHidden(true);
		operation_file->setHidden(true);
		dfg_file->setHidden(true);
		netlist_file->setHidden(true);
	}

	NetWidget::NetWidget(QWidget *parent)
	: QWidget(parent)
	{
		//_txt_tile_info            = new QTextEdit(this);
		//_net_list                = new RouteList(this);
		//_net_highlighted_list	 = new HRouteList(this);
		_netlists = new NetListWidget(this);
		_files = new FileListWidget(this);
		_dfgs = new DFGListWidget(this);

		//_txt_tile_info->setReadOnly(true);

		QVBoxLayout *layout = new QVBoxLayout;
		_tabs = new QTabWidget;
		_tabs->addTab(_files, "Files");
		_tabs->addTab(_netlists, "Netlists");
		_tabs->addTab(_dfgs, "DFGs");
		layout->addWidget(_tabs);
		//layout->addWidget(_txt_tile_info);
		setLayout(layout);

		connect(_netlists, SIGNAL(itemClicked(QListWidgetItem*)), this, SIGNAL(netlist_state_changed(QListWidgetItem*)));
		//init_connection();
	}

	/*QString NetWidget::current_net() const
	{
		QListWidgetItem *item = _net_list->currentItem();
		return item == 0 ? "" : item->text();
	}

	void NetWidget::find_net(QString net)
	{
		if (net == _find_net_name) {
			if (_found_nets.isEmpty())
				_found_nets = _net_list->findItems(net, Qt::MatchContains);
			else {
				if (!_found_nets.isEmpty())
					_net_list->setCurrentItem(_found_nets.takeFirst());
			}
		}
		else {
			_find_net_name = net;
			_found_nets = _net_list->findItems(net, Qt::MatchContains);
			if (!_found_nets.isEmpty())
				_net_list->setCurrentItem(_found_nets.takeFirst());
			else {
				QMessageBox msg;
				msg.setText("No net found.");
				msg.exec();
			}
		}
	}
}*/
