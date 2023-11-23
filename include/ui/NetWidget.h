#ifndef SRC_NET_WIDGET_H
#define SRC_NET_WIDGET_H

#include <QtGui>
#include <QtCore>
#include <QtWidgets>
#include <string>

//namespace FPGAViewer {
using std::string;

const int PROJECT_FILE = 0;
const int ADG_FILE = 1;
const int OPERATION_FILE = 2;
const int DFG_FILE = 3;
const int NETLIST_FILE = 4;
	/*
	class RouteList : public QListWidget
	{
		Q_OBJECT

	public:
		RouteList(QWidget* parent = 0) : QListWidget(parent) {}

		void set_net_list(const QStringList& nets);
		void set_selected_route(int row);

	protected:
		void currentChanged(const QModelIndex& current, const QModelIndex& previous);
		void mouseReleaseEvent(QMouseEvent* event);
		void mousePressEvent(QMouseEvent* event);

	signals:
		void select_changed(string current, string previous);
		void rt_list_drag(QListWidgetItem* item, bool begin);
	};

	class HRouteList : public RouteList
	{
	public:
		HRouteList(QWidget* parent = 0);

		void set_net_list(const QStringList& nets);
		void set_h_net_list(const QStringList& nets);
	};*/

	/*class TileList : public QListWidget
	{
	public:
		TileList(QWidget* parent = 0) : QListWidget(parent) {}
		void set_tile_list(const QStringList& );
	};*/

	class NetListWidget : public QListWidget
	{
	public:
		NetListWidget(QWidget* parent = 0) : QListWidget(parent) {}
		void add_netlist(std::string* , QListWidgetItem*);
		//void clear();
	};

	class DFGListWidget : public QListWidget
	{
	public:
		DFGListWidget(QWidget* parent = 0) : QListWidget(parent) {}
		void add_dfg(std::string*, QListWidgetItem*);
		//void clear();
	};

	class FileListWidget : public QTreeWidget
	{
	public:
		FileListWidget(QWidget* parent = 0);
		void add_file(int, std::string*);
		void delete_file(int file_type, std::string* file_name) {}
		void delete_all_files();
	private:
		QTreeWidgetItem* project_file;
		QTreeWidgetItem* adg_file;
		QTreeWidgetItem* operation_file;
		QTreeWidgetItem* dfg_file;
		QTreeWidgetItem* netlist_file;
	};

	class NetWidget : public QWidget
	{
		Q_OBJECT

	public:
		NetWidget(QWidget *parent = 0);

		void add_file(int file_type, std::string* file_name)	{ _files->add_file(file_type, file_name); }
		void add_file(int file_type, std::string* file_name, QListWidgetItem* item)	{ _files->add_file(file_type, file_name); _dfgs->add_dfg(file_name, item); }
		void delete_file(int file_type, std::string* file_name)	{ _files->delete_file(file_type, file_name); }
		void delete_all_files()	{ _files->delete_all_files(); }

		void add_netlist(std::string* name, QListWidgetItem* item)	{ _netlists->add_netlist(name, item); }
		void delete_netlists()	{ _netlists->clear(); }
		void delete_dfgs()	{ _dfgs->clear(); }
		//QString  current_net()              const;
		//QString  find_net_name()		    const {return _find_net_name; }

	//public slots:
		//void set_h_nets_list(const QStringList &h_nets)	{ _net_highlighted_list->set_net_list(h_nets); }
		//void set_sta_info(QString info)					{ _txt_net_info->setPlainText(info); }
		//void set_selected_route(int row)				{ _net_list->set_selected_route(row); }
		//void find_net(QString net);

	signals:
		void netlist_state_changed(QListWidgetItem*);
		//void select_changed(string current, string previous);
		//void rt_list_drag(QListWidgetItem* item, bool begin);

	private:

		//void		init_connection();

		QTabWidget*	_tabs;

		//RouteList*	_net_list;
		FileListWidget*	_files;
		//QTextEdit*	_txt_tile_info;
		//TileList*		_tile_list;
		NetListWidget*	_netlists;
		DFGListWidget*	_dfgs;

		string		_selected_net_name;

		QString		_find_net_name;
		QList<QListWidgetItem *>	_found_nets;
	};

//}
#endif
