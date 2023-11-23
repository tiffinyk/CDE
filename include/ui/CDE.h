#ifndef CDE_H
#define CDE_H

#include <QtGui>
#include <QtCore>
#include <QtWidgets>
#include <QString>
#include <QProgressBar>

#include "ui/ViewWidget.h"
#include "ui/BirdWidget.h"
#include "ui/NetWidget.h"
#include "ui/SubArchWidget.h"

#include "op/operations.h"
#include "ir/adg_ir.h"
#include "ir/dfg_ir.h"
#include "mapper/mapper_sa.h"
#include "generator/generator.h"

//namespace FPGAViewer {

    //using namespace COS;
	//using namespace ARCH;

	struct Config 
	{
		double	tile_space_h;
		double	tile_space_v;
		double	scene_margin;

		double	cluster_side_length;
		double	cluster_offset;
		double	cluster_space;
	};

	class SettingsDialog :public QDialog
	{
		Q_OBJECT

	private:
		int* p_timeout_ms = nullptr;
		int* p_max_iters = nullptr;
		bool* p_objOpt = nullptr;
		bool* p_dumpCallFunc = nullptr;
		bool* p_dumpMappedViz = nullptr;

		QFormLayout layout;

		QSpinBox s1;
		QSpinBox s2;
		QCheckBox c1;
		QCheckBox c2;
		QCheckBox c3;
		QDialogButtonBox b;

	private slots:

		void accept();

	public:
		SettingsDialog(int*, int*, bool*, bool*, bool*);
	};

	class CDE : public QMainWindow
	{
		Q_OBJECT

	public:
		explicit CDE(QWidget* = 0);
		bool OpenProject(const string& file_name);
		bool LoadArch(const string& file_name);
		bool LoadDesign(QStringList* );
		bool LoadOperation(const string& file_name);
		bool LoadNetlist(const string& file_name);

	signals:
		//void add_file(int, std::string*);
		//void delete_file(int, std::string*);
		//void delete_all_files();

		//void add_netlist(QListWidgetItem*);
		//void delete_netlists();

	private:
		void InitAction();
		void InitMenu();
		void InitToolBar();
		void InitStatusBar();
		void InitConnect();

		void initDock();
		void initView();

	private slots:
		void OpenProject();
		void LoadArch();
		void LoadDesign();
		void LoadOperation();
		void CloseProject();
		void LoadNetlist();
		void Quit();
		void About();
		void ArgUsage();
		//void FindNet();

		void MapDesign();
		void GenArch();
		void Settings();

		void CheckNetlist(QListWidgetItem*);

		void FullScale()               { _viewWidget->full_scale(); }
        void zoom_in()                 { _viewWidget->zoom_in(/*_netWidget->current_net()*/); }
        void zoom_out()                { _viewWidget->zoom_out(/*_netWidget->current_net()*/); }

		void Save();
		void SaveAs();
		void SaveProject();
		void SaveArch();
		//void RouteAndSave();
		void BitGen();
		void DesignModified(bool);
		void DesignRouteModified(bool);
		void DrawBirdRect(QRectF);
		void HideDock(bool);
		//void HideUnselectedLine(bool hide)      { _viewWidget->hide_unselected_line(hide); }
		//void HideUnhighlightedLine(bool hide)	{ _viewWidget->hide_unhighlighted_line(hide); }
		void DrawRoute(bool draw);
		void ShowPlacementInfo(bool);
		//void ShowTimeInfo(bool);
		void FreeDockDesignData();

		void addProgressbar(const QString& output, short progress = 1);
		void addOutput(const QString& output)   { _logText->append("\n" + output); }
		void setStatus(const QString& status)	{ _lbStatus->setText(status); }
		void show_progressbar();
		void hide_progressbar();

		void disable_file_act();
		void enable_file_act();
		
	private:

		ViewWidget*		_viewWidget;
		SubArchWidget*	_subArchWidget;

		ADG* adg = nullptr;
		std::vector<DFG*> dfgs;
		std::map<QListWidgetItem*, DFG*> dfg_list;
		std::map<QListWidgetItem*, Netlist*> netlists;
		std::map<QListWidgetItem*, bool> netlist_state;
		//std::vector<Netlist*> netlists;
		//Netlist* netlist = nullptr;

		int drawedNetlistNum = 0;

    	int timeout_ms = 3600000;
    	int max_iters = 2000;
    	bool objOpt = true;
    	bool dumpConfig = false;
    	bool dumpCallFunc = true;
    	bool dumpMappedViz = true;

    	std::string workDir = QDir::currentPath().toStdString();
    	std::string adg_file = "";
    	std::string operation_file = "";

    	QStringList dfg_files;
    	QStringList netlist_files;

    	//std::string netlist_file;

		QString			_project_file_name;
		QString			_arch_file_name;
		QString			_design_file_name;
		QString			_operation_file_name;
		QString			_netlist_file_name;

		QTextEdit*		_logText;
		QLabel*			_lbStatus;
		QProgressBar*	_lbProgress;

		//////////////////////////////////////////////////////////////////////////
		// for assist & bird view
		BirdWidget*		_birdWidget;
		NetWidget*		_netWidget;

		QDockWidget*	_birdDock;
		QDockWidget*	_netDock;
		QDockWidget*	_logDock;
		
		QMenu* _menuFile;
		QMenu* _menuOpen;
		QMenu* _menuOperation;
		QMenu* _menuView;
		QMenu* _menuWindow;
		QMenu* _menuAssist;
		QMenu* _menuHelp;

		QToolBar* _fileToolBar;
		QToolBar* _viewToolBar;
		QToolBar* _windowToolBar;
		QToolBar* _helpToolBar;
		QToolBar* _assistToolBar;

		//actions in menu_F
		QAction* _actOpenProject;
		QAction* _actOpenDFG;
		QAction* _actLoadOperation;
		QAction* _actLoadArch;
		QAction* _actLoadNetlist;
		QAction* _actCloseProject;
		QAction* _actSave;
		QAction* _actSaveAs;
		QAction* _actSaveProject;
		QAction* _actSaveArch;
		//QAction* _actRTSave;
		QAction* _actBitGen;
		QAction* _actQuit;

		//actions in menu_O
		QAction* _actMapDesign;
		QAction* _actGenArch;
		QAction* _actAddTap;
		QAction* _actSettings;

		//actions in menu_V
		QAction* _actZoomIn;
		QAction* _actZoomOut;
		QAction* _actFullScale;
		QAction* _actFind;

		//actions in menu_W
		QAction* _actHideDock;

		//action in menu_A
		QAction* _actHideUnselectedNet;
		QAction* _actHideUnHLtedNet;
		QAction* _actDrawNet;
		QAction* _actShowPlacementInfo;
		QAction* _actShowTimeInfo;

		//action in menu_H
		QAction* _actAbout;
		QAction* _actArgUsage;

		Config	_conf;
	};

//}
#endif // VIEWER_H
