#include "ui/CDE.h"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

	SettingsDialog::SettingsDialog(int* timeout_ms, int* max_iters, bool* objOpt, bool* dumpCallFunc, bool* dumpMappedViz)
	{
		p_timeout_ms = timeout_ms;
		p_max_iters = max_iters;
		p_objOpt = objOpt;
		p_dumpCallFunc = dumpCallFunc;
		p_dumpMappedViz = dumpMappedViz;

		setWindowTitle("CGRA Map Settings");
		setWindowModality(Qt::ApplicationModal);

		s1.setRange(1, 100000000);
		s1.setValue(*timeout_ms);
		s1.setSuffix(QString("ms"));

		s2.setRange(1, 100000000);
		s2.setValue(*max_iters);

		c1.setText("Optimize Mapping Objective");
		c1.setChecked(*objOpt);

		c2.setText("Dump CGRA Execution Call Function");
		c2.setChecked(*dumpCallFunc);

		c3.setText("Dump Mapped Visual Graph");
		c3.setChecked(*dumpMappedViz);

		b.setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(&b, SIGNAL(accepted()), this, SLOT(accept()));
		connect(&b, SIGNAL(rejected()), this, SLOT(reject()));

		layout.addRow("Map Timeout: ", &s1);
		layout.addRow("Max Iteration Number: ", &s2);
		layout.addRow(&c1);
		layout.addRow(&c2);
		layout.addRow(&c3);
		layout.addRow(&b);
		setLayout(&layout);
	}

	void SettingsDialog::accept()
	{
		*p_timeout_ms = s1.value();
		*p_max_iters = s2.value();

		*p_objOpt = c1.isChecked();
		*p_dumpCallFunc = c2.isChecked();
		*p_dumpMappedViz = c3.isChecked();

		QDialog::accept();
	}

	CDE::CDE(QWidget *parent) : QMainWindow(parent)
	{
		setGeometry(QGuiApplication::primaryScreen()->availableGeometry());
		showMaximized();
		setWindowState(Qt::WindowMaximized);
		setWindowTitle("CGRA Development Environment");
		setWindowIcon(QIcon(":images/CDE.bmp"));

		InitAction();
		InitMenu();
		InitToolBar();
		InitStatusBar();

		initView();
		initDock();

		InitConnect();
	}

	void CDE::initView()
	{
		_viewWidget = new ViewWidget(this);
		setCentralWidget(_viewWidget);
		_viewWidget->setWindowState(_viewWidget->windowState() | Qt::WindowFullScreen);
	
		_subArchWidget = new SubArchWidget;
	}

	void CDE::initDock()
	{
		_netWidget = new NetWidget(this);
		_netDock = new QDockWidget(tr("Circuit Information"), this);
		_netDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		_netDock->setWidget(_netWidget);
		addDockWidget(Qt::LeftDockWidgetArea, _netDock);

		_birdWidget = new BirdWidget(this);
		_birdDock = new QDockWidget(tr("Bird View"), this);
		_birdDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		_birdDock->setWidget(_birdWidget);
		addDockWidget(Qt::LeftDockWidgetArea, _birdDock);

		_logText = new QTextEdit(this);
		_logText->setReadOnly(true);
		_logDock = new QDockWidget(tr("Output and Logs"), this);
		_logDock->setWidget(_logText);
		_logDock->setAllowedAreas(Qt::AllDockWidgetAreas);
		addDockWidget(Qt::BottomDockWidgetArea, _logDock);
	}

	void CDE::InitAction()
	{
		///////////////////////////////////////////
		// Menu File related
		_actOpenProject     = new QAction(QIcon(""), 				 tr("&Open Project"),   this);
		_actOpenDFG  = new QAction(QIcon(":images/open_design.png"), tr("&Open DFG"),    this);
		_actLoadOperation   = new QAction(QIcon(""),				 tr("&Open Operation File"),    this);
		_actLoadArch = new QAction(QIcon(":images/open_arch.png"),   tr("&Load Arch"),      this);
		_actLoadNetlist     = new QAction(QIcon(""),				 tr("&Open Netlist"),   this);
		_actCloseProject    = new QAction(QIcon(""), 				 tr("&Close Project"),  this);
		_actQuit     = new QAction(QIcon(":images/quit.png"),        tr("&Quit"),           this);
		_actSave     = new QAction(QIcon(":images/save.png"),        tr("&Save"),           this);
		_actSaveAs   = new QAction(QIcon(":images/save_as.png"),     tr("Save As"),         this);
		_actSaveProject	    = new QAction(QIcon(""),     			 tr("Save Project"),	this);
		_actSaveArch	    = new QAction(QIcon(""),     			 tr("Save Arch"),		this);
		//_actRTSave   = new QAction(QIcon(":images/route_save.png"),  tr("&Route and Save"), this);
		_actBitGen	 = new QAction(QIcon(":images/bit_gen.png"),     tr("&Generate Bit Stream"), this);

		_actOpenDFG->setShortcut(QKeySequence::Open);
		_actQuit->setShortcut(QKeySequence::Close);
		_actSave->setShortcut(QKeySequence::Save);
		_actSaveAs->setShortcut(QKeySequence::SaveAs);
		_actLoadArch->setShortcut(QKeySequence(tr("Ctrl+L")));

		///////////////////////////////////////////
		// Menu Operation related
		_actMapDesign= new QAction(QIcon("images/CPUz_dtu.ico"),	tr("&Map Design"),		this);
		_actGenArch  = new QAction(QIcon(""),	 					tr("&Generate Arch"),	this);
		_actAddTap	 = new QAction(QIcon(""),                       tr("Add Signal Tap"),	this);
		_actSettings	 = new QAction(QIcon(""),	tr("CGRA Map Settings..."),	this);

		///////////////////////////////////////////
		// Menu View related
		_actZoomIn        = new QAction(QIcon(":images/zoom_in.png"),    tr("Zoom &In"),   this);
		_actZoomOut       = new QAction(QIcon(":images/zoom_out.png"),   tr("Zoom &Out"),  this);
		_actFullScale     = new QAction(QIcon(":images/full_scale.png"), tr("Full &View"), this);
		_actFind		  = new QAction(QIcon(":images/find.png"),       tr("Find Net"),   this);
		_actFind->setShortcut		  (QKeySequence(tr("Ctrl+F")));
		_actZoomIn->setShortcut       (QKeySequence(tr("Ctrl+Z")));
		_actZoomOut->setShortcut      (QKeySequence(tr("Ctrl+X")));
		_actFullScale->setShortcut    (QKeySequence(tr("F12")));

		///////////////////////////////////////////
		// Menu Window related
		_actHideDock = new QAction(tr("Hide &Dock"), this);
		_actHideDock->setShortcut(QKeySequence(tr("F5")));
		_actHideDock->setCheckable(true);
		_actHideDock->setChecked(false);

		///////////////////////////////////////////
		// Menu Assist related
		_actHideUnselectedNet = new QAction(QIcon(":images/hide_unselected.png"), tr("Hide Unselected Net"), this);
		_actHideUnselectedNet->setCheckable(true);
		_actHideUnselectedNet->setChecked(false);

		_actHideUnHLtedNet = new QAction(QIcon(":images/hide_unhighlighted.png"), tr("Hide Unhighlighted Net"), this);
		_actHideUnHLtedNet->setCheckable(true);
		_actHideUnHLtedNet->setChecked(false);

		_actShowPlacementInfo = new QAction(QIcon(":images/show_place_info.png"), tr("Show Placement Information"), this);
		_actShowPlacementInfo->setCheckable(true);
		_actShowPlacementInfo->setChecked(true);

		_actDrawNet = new QAction(QIcon(":images/draw_net.png"), tr("Draw Net"), this);
		_actDrawNet->setCheckable(true);
		_actDrawNet->setChecked(false);

		_actShowTimeInfo = new QAction(QIcon(":images/show_timing_info.png"), tr("Show Timing Information"), this);
		_actShowTimeInfo->setCheckable(true);
		_actShowTimeInfo->setChecked(false);


		///////////////////////////////////////////
		// Menu Help related
		_actAbout = new QAction(QIcon(":images/about.png"), tr("&About"), this);
		_actAbout->setShortcut(QKeySequence::HelpContents);

		_actArgUsage = new QAction(tr("&Arguments Usage"), this);
		_actArgUsage->setShortcut(QKeySequence(tr("F1")));

		///////////////////////////////////////////
		// Menu enable
		_actSave->setEnabled(false);
		_actSaveAs->setEnabled(false);
		_actSaveArch->setEnabled(false);
		_actOpenDFG->setEnabled(false);
		//_actLoadOperation->setEnabled(false);
		//_actCloseProject->setEnabled(false);
		_actMapDesign->setEnabled(false);
		_actLoadNetlist->setEnabled(false);
		_actBitGen->setEnabled(false);
	}

	void CDE::InitMenu()
	{
		_menuFile = new QMenu(tr("&File"), this);
		_menuOpen = new QMenu("Open");
		_menuOpen->addAction(_actOpenProject);
		_menuOpen->addAction(_actLoadArch);
		_menuOpen->addAction(_actOpenDFG);
		_menuOpen->addAction(_actLoadOperation);
		_menuOpen->addAction(_actLoadNetlist);
		_menuFile->addMenu(_menuOpen);
		_menuFile->addAction(_actCloseProject);
		_menuFile->addAction(_actSave);
		_menuFile->addAction(_actSaveAs);
		_menuFile->addAction(_actSaveProject);
		_menuFile->addAction(_actSaveArch);
		//_menuFile->addAction(_actRTSave);
		_menuFile->addAction(_actBitGen);
		_menuFile->addSeparator();
		_menuFile->addAction(_actQuit);

		_menuOperation = new QMenu(tr("&Operation"), this);
		_menuOperation->addAction(_actMapDesign);
		_menuOperation->addAction(_actGenArch);
		_menuOperation->addAction(_actAddTap);
		_menuOperation->addSeparator();
		_menuOperation->addAction(_actSettings);

		_menuView = new QMenu(tr("&View"), this);
		_menuView->addAction(_actFullScale);
		_menuView->addAction(_actZoomIn);
		_menuView->addAction(_actZoomOut);
		_menuView->addAction(_actFind);

		_menuWindow = new QMenu(tr("&Window"), this);
		_menuWindow->addAction(_actHideDock);

		_menuAssist = new QMenu(tr("&Assist"), this);
		_menuAssist->addAction(_actHideUnselectedNet);
		_menuAssist->addAction(_actHideUnHLtedNet);
		_menuAssist->addAction(_actShowPlacementInfo);
		_menuAssist->addAction(_actDrawNet);
		_menuAssist->addAction(_actShowTimeInfo);

		_menuHelp = new QMenu(tr("&Help"), this);
		_menuHelp->addAction(_actAbout);
		_menuHelp->addAction(_actArgUsage);

		menuBar()->addMenu(_menuFile);
		menuBar()->addMenu(_menuOperation);
		menuBar()->addMenu(_menuView);
		menuBar()->addMenu(_menuWindow);
		menuBar()->addMenu(_menuAssist);
		menuBar()->addMenu(_menuHelp);
	}

	void CDE::InitToolBar()
	{
		_fileToolBar = new QToolBar(this);
		_fileToolBar->setAllowedAreas(Qt::AllToolBarAreas);
		_fileToolBar->setOrientation(Qt::Horizontal);
		_fileToolBar->addAction(_actLoadArch);
		_fileToolBar->addAction(_actOpenDFG);
		_fileToolBar->addAction(_actSave);
		_fileToolBar->addAction(_actSaveAs);
		//_fileToolBar->addAction(_actRTSave);
		_fileToolBar->addAction(_actBitGen);
		_fileToolBar->addAction(_actAddTap);

		_viewToolBar = new QToolBar(this);
		_viewToolBar->setAllowedAreas(Qt::AllToolBarAreas);
		_viewToolBar->setOrientation(Qt::Horizontal);
		_viewToolBar->addAction(_actFullScale);
		_viewToolBar->addAction(_actZoomIn);
		_viewToolBar->addAction(_actZoomOut);
		_viewToolBar->addAction(_actFind);

		_assistToolBar = new QToolBar(this);
		_assistToolBar->setAllowedAreas(Qt::AllToolBarAreas);
		_assistToolBar->setOrientation(Qt::Horizontal);
		_assistToolBar->addAction(_actHideUnselectedNet);
		_assistToolBar->addAction(_actHideUnHLtedNet);
		_assistToolBar->addAction(_actDrawNet);
		_assistToolBar->addAction(_actShowPlacementInfo);
		_assistToolBar->addAction(_actShowTimeInfo);

		_helpToolBar = new QToolBar(this);
		_helpToolBar->setAllowedAreas(Qt::AllToolBarAreas);
		_helpToolBar->setOrientation(Qt::Horizontal);
		_helpToolBar->addAction(_actAbout);

		addToolBar(Qt::TopToolBarArea, _fileToolBar);
		addToolBar(Qt::TopToolBarArea, _viewToolBar);
		addToolBar(Qt::TopToolBarArea, _assistToolBar);
		addToolBar(Qt::TopToolBarArea, _helpToolBar);
	}

	void CDE::InitStatusBar()
	{
		QHBoxLayout* layout = new QHBoxLayout(statusBar());
		
		_lbStatus = new QLabel(tr("Ready"));
		_lbStatus->setMinimumWidth(800);

		_lbProgress = new QProgressBar(this);
		_lbProgress->setRange(0, 10);
		_lbProgress->setMaximumWidth(300);

		statusBar()->addWidget(_lbStatus, 1);
		statusBar()->addWidget(_lbProgress, 0);

		show_progressbar();
		hide_progressbar();
	}

	void CDE::InitConnect()
	{
		connect(_actOpenProject,     SIGNAL(triggered()), this, SLOT(OpenProject()));
		connect(_actOpenDFG,         SIGNAL(triggered()), this, SLOT(LoadDesign()));
		connect(_actLoadOperation,   SIGNAL(triggered()), this, SLOT(LoadOperation()));
		connect(_actLoadArch,        SIGNAL(triggered()), this, SLOT(LoadArch()));
		connect(_actLoadNetlist,     SIGNAL(triggered()), this, SLOT(LoadNetlist()));
		connect(_actCloseProject,    SIGNAL(triggered()), this, SLOT(CloseProject()));
		connect(_actSave,            SIGNAL(triggered()), this, SLOT(Save()));
		connect(_actSaveAs,          SIGNAL(triggered()), this, SLOT(SaveAs()));
		connect(_actSaveProject,     SIGNAL(triggered()), this, SLOT(SaveProject()));
		connect(_actSaveArch,     	 SIGNAL(triggered()), this, SLOT(SaveArch()));
		//connect(_actRTSave,			 SIGNAL(triggered()), this, SLOT(RouteAndSave()));
		connect(_actBitGen,			 SIGNAL(triggered()), this, SLOT(BitGen()));
		//connect(_actBitGen,			 SIGNAL(triggered()), _viewWidget, SLOT(clear()));
		connect(_actQuit,            SIGNAL(triggered()), this, SLOT(Quit()));

		connect(_actMapDesign,       SIGNAL(triggered()), this, SLOT(MapDesign()));
		connect(_actGenArch,       	 SIGNAL(triggered()), this, SLOT(GenArch()));
		//connect(_actAddTap,			 SIGNAL(triggered()), _viewWidget, SLOT(addSignalTap()));
		connect(_actSettings,		 SIGNAL(triggered()), this, SLOT(Settings()));

		connect(_actFullScale,       SIGNAL(triggered()), this, SLOT(FullScale()));
		connect(_actFind,			 SIGNAL(triggered()), this, SLOT(FindNet()));
		connect(_actZoomIn,          SIGNAL(triggered()), this, SLOT(zoom_in()));
		connect(_actZoomOut,         SIGNAL(triggered()), this, SLOT(zoom_out()));
		connect(_actAbout,           SIGNAL(triggered()), this, SLOT(About()));
		connect(_actArgUsage,        SIGNAL(triggered()), this, SLOT(ArgUsage()));
		
		connect(_actHideDock,        SIGNAL(toggled(bool)), this, SLOT(HideDock(bool)));

		connect(_actHideUnselectedNet,	SIGNAL(toggled(bool)), this, SLOT(HideUnselectedLine(bool)));
		connect(_actHideUnHLtedNet,		SIGNAL(toggled(bool)), this, SLOT(HideUnhighlightedLine(bool)));
		connect(_actDrawNet,			SIGNAL(toggled(bool)), this, SLOT(DrawRoute(bool)));
		connect(_actShowPlacementInfo,	SIGNAL(toggled(bool)), this, SLOT(ShowPlacementInfo(bool)));
		connect(_actShowTimeInfo,		SIGNAL(toggled(bool)), this, SLOT(ShowTimeInfo(bool)));

		//connect(this, SIGNAL(add_file(int, std::string*)), 	 _netWidget, SLOT(add_file(int, std::string*)));
		//connect(this, SIGNAL(delete_file(int, std::string*)), _netWidget,SLOT(delete_file(int, std::string*)));
		//connect(this, SIGNAL(delete_all_files()), _netWidget,			 SLOT(delete_all_files()));
		//connect(this, SIGNAL(add_netlist(QListWidgetItem*)), _netWidget, SLOT(add_netlist(QListWidgetItem*)));
		//connect(this, SIGNAL(delete_netlists()), _netWidget,			 SLOT(delete_netlists()));
		connect(_netWidget, SIGNAL(netlist_state_changed(QListWidgetItem*)),	this, SLOT(CheckNetlist(QListWidgetItem*)));

		connect(_viewWidget, SIGNAL(free_design_data()),            this, SLOT(FreeDockDesignData()));
		connect(_viewWidget, SIGNAL(design_is_modified(bool)),      this, SLOT(DesignModified(bool)));
		connect(_viewWidget, SIGNAL(design_route_is_modified(bool)),this, SLOT(DesignRouteModified(bool)));
		connect(_viewWidget, SIGNAL(draw_viewport(QRectF)),         this, SLOT(DrawBirdRect(QRectF)));
		connect(_viewWidget, SIGNAL(emit_draw(bool)),               this, SLOT(DrawRoute(bool)));
		
		connect(_viewWidget, SIGNAL(set_current_net(int)), 	 _netWidget, SLOT(set_selected_route(int)));
		connect(_viewWidget, SIGNAL(show_sta_info(QString)), _netWidget, SLOT(set_sta_info(QString)));
		connect(_viewWidget, SIGNAL(list_highlighted_net(QStringList)), _netWidget, SLOT(set_h_nets_list(QStringList)));
		connect(_viewWidget, SIGNAL(deselect_all_nets()),    _netWidget, SLOT(deselect_all_nets()));

		//connect(_viewWidget, SIGNAL(update_tile(const string&)), _subArchWidget, SLOT(updateModule(const string&)));
		connect(_viewWidget, SIGNAL(update_tile(ADGNode*)), _subArchWidget, SLOT(updateModule(ADGNode*)));

		connect(_viewWidget, SIGNAL(process_started()),  this, SLOT(disable_file_act()));
		connect(_viewWidget, SIGNAL(process_finished()), this, SLOT(enable_file_act()));
		connect(_netWidget, SIGNAL(select_changed(string, string)),       _viewWidget, SLOT(select_changed(string, string)));
		connect(_netWidget, SIGNAL(rt_list_drag(QListWidgetItem*, bool)), _viewWidget, SLOT(drag_rt_list(QListWidgetItem*, bool)));

		connect(_viewWidget, SIGNAL(add_progress(const string&)), this, SLOT(addProgressbar(const string&)));
		connect(_viewWidget, SIGNAL(file_is_loaded()),            this, SLOT(hide_progressbar()));
		connect(_viewWidget, SIGNAL(file_is_loading()),           this, SLOT(show_progressbar()));
	}

	void CDE::About()
	{
		QMessageBox::about(this, tr("About CDE"), 
			tr("<h2>CDE 1.10</h2>"
			"<p>This software is open-source."
			"<p>CDE is a software which explores CGRA hardwares and compiles CGRA applications."));
	}

	void CDE::Settings()
	{
		SettingsDialog s(&timeout_ms, &max_iters, &objOpt, &dumpCallFunc, &dumpMappedViz);
		s.exec();
	}

	void CDE::OpenProject()
	{
		QString file_name = QFileDialog::getOpenFileName(this, tr("Open Project file"),
			".", tr(/*"FPGA ARCH file(*.xml)\n"*/
			"CDE Project file(*.cpf)\n"
			"All (*.*)"));
		if (!file_name.isEmpty()) {
			_project_file_name = file_name;
			if ( OpenProject(file_name.toStdString()) ) {
				setStatus(QString("Ready\t-\t%1").arg(_project_file_name));
				_actCloseProject->setEnabled(true);
			}
		}
	}

	void CDE::LoadArch()
	{
		QString file_name = QFileDialog::getOpenFileName(this, tr("Open ARCH file"),
			".", tr(/*"FPGA ARCH file(*.xml)\n"*/
			"CGRA ARCH file(*.json)\n"
			"All (*.*)"));
		if (!file_name.isEmpty()) {
			_arch_file_name = file_name;
			if ( LoadArch(file_name.toStdString()) ) {
				//_actLoadOperation->setEnabled(true);
				_actLoadNetlist->setEnabled(true);
				setStatus(QString("Ready\t-\t%1").arg(_arch_file_name));
			}
		}
	}

	void CDE::LoadDesign()
	{
		QStringList files = QFileDialog::getOpenFileNames(this, tr("Open DFG files"), ".", tr("DFG file(*.json)\n" "All (*.*)"));

		if (!files.isEmpty())
		{
			workDir = path(files.at(0).toStdString()).parent_path().string();
			if (LoadDesign(&files))
			{
				_actMapDesign->setEnabled(true);
				setStatus(QString("Ready\t-\t%1").arg(_arch_file_name));
			}
		}
	}

	void CDE::LoadOperation()
	{
		QString file_name = QFileDialog::getOpenFileName(this, tr("Open Operation file"),
			".", tr("Operation file(*.json)\n"
			"All (*.*)"));

//		_assist_widget->init_check();
		if (!file_name.isEmpty()) {
			_operation_file_name = file_name;
			if ( LoadOperation(file_name.toStdString()) ) {
				_actOpenDFG->setEnabled(true);
				setStatus(QString("Ready\t-\t%1").arg(_arch_file_name));
			}
		}
	}

	void CDE::LoadNetlist()
	{
		QString file_name = QFileDialog::getOpenFileName(this, tr("Open PR file"),
			".", tr("PR file(*.json)\n"
			"All (*.*)"));

//		_assist_widget->init_check();
		if (!file_name.isEmpty()) {
			_netlist_file_name = file_name;
			if ( LoadNetlist(file_name.toStdString()) ) {
				//setWindowTitle("Chip Viewer - " + file_name);
				setStatus(QString("Ready\t-\t%1").arg(_arch_file_name));
			}
		}
	}

	void CDE::CheckNetlist(QListWidgetItem* item)
	{
		//addProgressbar("Checking Netlists...");
		bool checked = item->checkState() == Qt::Checked ? true : false;
		if(netlist_state[item] == true && checked == false)
		{
			_viewWidget->remove_netlist(netlists[item]);
			netlist_state[item] = false;
			drawedNetlistNum--;
		}
		else if(netlist_state[item] == false && checked == true)
		{
			_viewWidget->draw_netlist(netlists[item]);
			netlist_state[item] = true;
			drawedNetlistNum++;
		}
		if(drawedNetlistNum == 1)
		{
			_actSave->setEnabled(true);
			_actSaveAs->setEnabled(true);
		}
		else
		{
			_actSave->setEnabled(false);
			_actSaveAs->setEnabled(false);
		}
	}

	void CDE::CloseProject()
	{
		_viewWidget->clear();
		if(adg != nullptr)
		{
			delete adg;
			adg = nullptr;
			addProgressbar("ADG Closed.");
		}
		if(dfgs.size() > 0)
		{
			for(auto dfg : dfgs)
			{
				delete dfg;
			}
			dfgs.clear();
			dfg_files.clear();
			dfg_list.clear();
			_netWidget->delete_dfgs();
			addProgressbar("DFGs Closed.");
		}
		if(netlists.size() > 0)
		{
			for(auto n : netlists)
			{
				delete n.second;
			}			
			netlists.clear();
			netlist_state.clear();
			drawedNetlistNum = 0;
			_netWidget->delete_netlists();
			addProgressbar("Netlists Closed.");
		}
		disable_file_act();
		_actMapDesign->setEnabled(false);
		//_actCloseProject->setEnabled(false);
		_netWidget->delete_all_files();		
		addProgressbar("Project Closed.");
	}

	void CDE::MapDesign()
	{
		show_progressbar();
		MapperSA mapper(adg, timeout_ms, max_iters, objOpt);

		std::vector<QListWidgetItem*> sort;
		std::vector<QListWidgetItem*>::iterator it;
		std::vector<DFG*> used_dfgs;
		for(const auto& i : dfg_list){
			if(i.first->checkState() == Qt::Checked){
				sort.emplace_back(i.first);
				used_dfgs.emplace_back(i.second);
			}
		}
    	int numDfg = used_dfgs.size();
    	int numSucceed = 0;
    	double PEUsage, avgPEUsage, avgLatency;
    	std::vector<double>storePEusage;
    	std::vector<int>bestLatency;

        addProgressbar("Map Timeout: " + QString::number(timeout_ms) + "ms, Max Iteration Number: " + QString::number(max_iters) + ", Objective Optimization is " + QVariant(objOpt).toString());
        addProgressbar("Configuration File Dump is " + QVariant(dumpCallFunc).toString() + ", Visual Graph Dump is " + QVariant(dumpMappedViz).toString());

		for(int i = 0; i < used_dfgs.size(); i++)
		{
			DFG*& dfg = used_dfgs[i];
			addProgressbar("Start to map the DFG " + QString::number(numSucceed) + " to ADG...");
			mapper.setDFG(dfg);
        	bool succeed = mapper.execute(dumpCallFunc, dumpMappedViz, workDir);
        	if(!succeed)
        	{
            	addProgressbar("Fail to map the DFG!");
            	break;
        	}
        	numSucceed++;
        	PEUsage = 100 * (double)(dfg->nodes().size())/adg->numGpeNodes();
        	storePEusage.push_back(PEUsage);
        	addProgressbar("PE Usage: " + QString::number(PEUsage, 'f', 4) + "%");
        	bestLatency.push_back(mapper.getMaxLat());
        	addProgressbar("DFG Latency: " + QString::number(mapper.getMaxLat()));
			
        	QListWidgetItem* item = new QListWidgetItem;
        	Netlist* netlist = new Netlist(mapper.getResult());
        	netlists[item] = netlist;
			std::string netlist_name = sort[i]->text().toStdString();
        	_netWidget->add_netlist(&netlist_name, item);

			_viewWidget->draw_netlist(netlist);
        	drawedNetlistNum++;
			netlist_state[item] = true;

			if(drawedNetlistNum == 1)
			{
				_actSave->setEnabled(true);
				_actSaveAs->setEnabled(true);
			}
			else
			{
				_actSave->setEnabled(false);
				_actSaveAs->setEnabled(false);
			}
		}
		
		if(numDfg > 0 && numSucceed == numDfg)
		{
        	addProgressbar("Succeed to map all the DFGs, number: " + QString::number(numSucceed));
        	avgPEUsage = std::accumulate(storePEusage.begin(), storePEusage.end(), 0.0) / storePEusage.size();
        	avgLatency = std::accumulate(bestLatency.begin(), bestLatency.end(), 0) / (double)bestLatency.size();
        	addProgressbar("The average usage of PE is: " + QString::number(avgPEUsage, 'f', 4) + "%");
        	addProgressbar("The average best max latency is: " + QString::number(avgLatency));

			_actBitGen->setEnabled(true);
		}
	}

	void CDE::GenArch()
	{
		_viewWidget->clear();
		_netWidget->delete_netlists();
		if(adg != nullptr)
		{
			delete adg;
		}
		std::list<QListWidgetItem*> sort;
		std::list<QListWidgetItem*>::iterator it;
		std::vector<DFG*> used_dfgs;
		DFG* d;
		for(const auto& i : dfg_list){
			if(i.first->checkState() == Qt::Checked){
				d = i.second;
				it = sort.begin();
				while(it != sort.end() && dfg_list[*it]->nodes().size() > d->nodes().size()){
					it++;
				}
				while(it != sort.end() && dfg_list[*it]->nodes().size() == d->nodes().size() && dfg_list[*it]->numInputs() > d->numInputs()){
					it++;
				}
				while(it != sort.end() && dfg_list[*it]->nodes().size() == d->nodes().size() && dfg_list[*it]->numInputs() == d->numInputs() && dfg_list[*it]->numOutputs() > d->numOutputs()){
					it++;
				}
				sort.insert(it, i.first);
			}
		}
		for(it = sort.begin(); it != sort.end(); it++){
			used_dfgs.emplace_back(dfg_list[*it]);
		}
		Generator generator(used_dfgs);
		addProgressbar("Start to generate the ADG from DFGs...");
		auto start = std::chrono::steady_clock::now();
		if(generator.execute(32, 32 , 12, 1, 7, "GIB") == true){
			auto end = std::chrono::steady_clock::now();
    		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			addProgressbar("Succeed to generate the ADG, used time: " + QString::number(duration) + "ms.");
		}
		adg = generator.getADG();
		_viewWidget->draw_chip(adg);
		addProgressbar("Succeed to draw the ADG.");
		it = sort.begin();
		for(int i = 0; i < used_dfgs.size(); i++){
			QListWidgetItem* item = new QListWidgetItem;
			Netlist* netlist = new Netlist(generator.nodeset(i), generator.linkset(i), generator.switchset(i));
			netlists[item] = netlist;
			std::string netlist_name = (*it)->text().toStdString();
			_netWidget->add_netlist(&netlist_name, item);
			it++;

			_viewWidget->draw_netlist(netlist);
        	drawedNetlistNum++;
			netlist_state[item] = true;

			if(drawedNetlistNum == 1)
			{
				_actSave->setEnabled(true);
				_actSaveAs->setEnabled(true);
			}
			else
			{
				_actSave->setEnabled(false);
				_actSaveAs->setEnabled(false);
			}
		}
		addProgressbar("Succeed to draw the netlist(s).");
		_actSaveArch->setEnabled(true);
	}

	bool CDE::OpenProject(const std::string &file_name)
	{
		std::ifstream ifs(file_name);
		std::string project_file;
		if(!ifs)
		{
			addProgressbar("Cannot Open Project File!");
			return false;
		}
		json fileJson;
		ifs >> fileJson;

		path workPath = path(file_name).parent_path();
		workDir = path(file_name).parent_path().string();
		project_file = path(file_name).filename().string();

		//Load ADG
		if(fileJson.contains("adg_file"))
		{
			std::string adg_file = absolute(fileJson["adg_file"].get<std::string>(), workPath).string();
			if (LoadArch(adg_file))
			{
				//_actLoadOperation->setEnabled(true);
				_actLoadNetlist->setEnabled(true);
			}
			else
			{
				addProgressbar("Cannot Open ADG File!");
				return false;
			}			
		}

		//Load Netlist File
		if(fileJson.contains("netlist_files"))
		{
			std::string netlist_file;
			for(auto& netlistJson : fileJson["netlist_files"])
			{
				netlist_file = absolute(netlistJson.get<std::string>(), workPath).string();
				if(LoadNetlist(netlist_file) == false)	{ return false; }
			}
		}

		//Load Operation File
		if(fileJson.contains("operation_file"))
		{
			std::string operation_file = absolute(fileJson["operation_file"].get<std::string>(), workPath).string();
			if (LoadOperation(operation_file))
			{
				_actOpenDFG->setEnabled(true);
			}
			else
			{
				return false;
			}
		}
		else
		{
			_netWidget->add_file(PROJECT_FILE, &project_file);
			return true;
		}

		//Load DFG File
		if(fileJson.contains("dfg_files"))
		{
			QStringList files;
			std::string dfg_file;
			for(auto& dfgJson : fileJson["dfg_files"])
			{
				dfg_file = absolute(dfgJson.get<std::string>(), workPath).string();
				files.append(QString::fromStdString(dfg_file));
			}
			if (LoadDesign(&files))
			{
				_actMapDesign->setEnabled(true);
			}
			else
			{
				addProgressbar("Cannot Open DFG File!");
				return false;
			}
		}
		_netWidget->add_file(PROJECT_FILE, &project_file);
		return true;
	}

	bool CDE::LoadArch(const std::string &file_name)
	{
		show_progressbar();

//		_assist_widget->init_check();
		QString error_msg = "An error occurred during loading arch file.\nDetails:\n";
		setCursor(Qt::WaitCursor);

		_viewWidget->clear();
		if(adg != nullptr)
		{
			delete adg;
		}
//		try
//		{
			addProgressbar("Loading arch file: " + QString::fromStdString(file_name));
			ADGIR adg_ir(file_name);
			adg = adg_ir.getADG();
			addProgressbar("ADG PE Nodes Number: " + QString::number(adg->numGpeNodes()));
			_viewWidget->draw_chip(adg);
//		}
//		catch (std::runtime_error &e)
//		{
//			QMessageBox::warning(this, "Chip Viewer",
//				error_msg + QString::fromStdString(e.what()));
//			unsetCursor();
//			return false;
//		}
//		catch (...)
//		{
//			QMessageBox::warning(this, "Chip Viewer",
//				"Unknown error occurred.");
//			unsetCursor();
//			return false;
//		}
		adg_file = file_name;
		std::string adg_file_name = path(file_name).filename().string();
		addProgressbar("Load arch file complete.");
		_netWidget->add_file(ADG_FILE, &adg_file_name);
		unsetCursor();
		return true;
	}

	bool CDE::LoadOperation(const std::string &file_name)
	{
		//if (!_viewWidget->arch_is_ready()) return false;

		show_progressbar();

		QString error_msg = "An error occurred during loading operation file.\nDetails:\n";
		setCursor(Qt::WaitCursor);
		try
		{
			//addProgressbar("Freeing design...");
			//_viewWidget->free_design();
			
			addProgressbar("Loading operation file...");
    		Operations::Instance(file_name);
		}
		catch (std::runtime_error &e)
		{
			QMessageBox::warning(this, "Chip Viewer",
				error_msg + QString::fromStdString(e.what()));
			unsetCursor();
			return false;
		}
		catch (...)
		{
			QMessageBox::warning(this, "Chip Viewer",
				"Unknown error occurred.");
			unsetCursor();
			return false;
		}
		operation_file = file_name;
		std::string operation_file_name = path(file_name).filename().string();
		addProgressbar("Load operation file complete.");
		_netWidget->add_file(OPERATION_FILE, &operation_file_name);
		unsetCursor();
		return true;
	}

	bool CDE::LoadDesign(QStringList* files)
	{
		//if (!_viewWidget->arch_is_ready()) return false;

		show_progressbar();
		setCursor(Qt::WaitCursor);

		int total;
		std::string file_name;
//		OpTypeCount result;

		for(QStringList::iterator it = files->begin(); it != files->end(); ++it)
		{
			addProgressbar("Parse DFG file: " + *it);

			DFGIR dfg_ir(it->toStdString());
			DFG* dfg = dfg_ir.getDFG();
			dfgs.emplace_back(dfg);
			file_name = path(it->toStdString()).filename().string();

        	QListWidgetItem* item = new QListWidgetItem;
        	dfg_list[item] = dfg;
			_netWidget->add_file(DFG_FILE, &file_name, item);
			/*
			result = dfg_ir->getNumType();
			total = result.numaddsub + result.numlogic + result.nummul + result.numcomp;
			addProgressbar("Operation Count:");
			addProgressbar("ADDandSUB: " + QString::number(result.numaddsub, 'f', 0) + ", " + QString::number(result.numaddsub*100/total, 'f', 4) + "%");
			addProgressbar("MUL: " + QString::number(result.nummul, 'f', 0) + ", " + QString::number(result.nummul*100/total, 'f', 4) + "%");
			addProgressbar("LOGIC: " + QString::number(result.numlogic, 'f', 0) + ", " + QString::number(result.numlogic*100/total, 'f', 4) + "%");
			addProgressbar("COMP: " + QString::number(result.numcomp, 'f', 0) + ", " + QString::number(result.numcomp*100/total, 'f', 4) + "%");*/
		}
		dfg_files += *files;
		addProgressbar("Parse DFG files complete.");
		unsetCursor();
		return true;
	}

	bool CDE::LoadNetlist(const std::string &file_name)
	{
		if (!_viewWidget->arch_is_ready()) return false;

		show_progressbar();

		QString error_msg = "An error occurred during loading netlist file.\nDetails:\n";
		setCursor(Qt::WaitCursor);
		std::string netlist_file = path(file_name).filename().string();
		try
		{
			//addProgressbar("Freeing design...");
			//_viewWidget->free_design();
			
			addProgressbar("Loading netlist file...");
			QListWidgetItem* item = new QListWidgetItem;
			Netlist* netlist = new Netlist(file_name);
			netlists[item] = netlist;
			_netWidget->add_netlist(&netlist_file, item);
			
			addProgressbar("Drawing netlist...");
			_viewWidget->draw_netlist(netlist);
			drawedNetlistNum++;
			netlist_state[item] = true;
			if(drawedNetlistNum == 1)
			{
				_actSave->setEnabled(true);
				_actSaveAs->setEnabled(true);
			}
			else
			{
				_actSave->setEnabled(false);
				_actSaveAs->setEnabled(false);
			}
		}
		catch (std::runtime_error &e)
		{
			QMessageBox::warning(this, "Chip Viewer",
				error_msg + QString::fromStdString(e.what()));
			unsetCursor();
			return false;
		}
		catch (...)
		{
			QMessageBox::warning(this, "Chip Viewer",
				"Unknown error occurred.");
			unsetCursor();
			return false;
		}
		netlist_files.append(QString::fromStdString(file_name));
		addProgressbar("Load netlist file complete.");
		_netWidget->add_file(NETLIST_FILE, &netlist_file);
		unsetCursor();
		return true;
	}

	void CDE::Save()
	{
		std::ofstream os(workDir + "/netlist.json");
		_viewWidget->get_netlist()->saveNetlist(os, adg);
	}

	void CDE::SaveAs()
	{
		QString file_path = QFileDialog::getSaveFileName(this, "Save placement modification", ".", "Netlist files (*.json)");

		if (!file_path.isEmpty())
		{
			std::ofstream os(file_path.toStdString());
			_viewWidget->get_netlist()->saveNetlist(os, adg);
		}
	}

	void CDE::SaveProject()
	{
		QString file_path = QFileDialog::getSaveFileName(this, "Save placement modification", ".", "CDE Project File (*.cpf)");

		if (!file_path.isEmpty())
		{
			std::ofstream os(file_path.toStdString());
			json fileJson, dfgFiles, netlistFiles, dfgFile, netlistFile;
			if(adg_file != "")
			{
				fileJson["adg_file"] = adg_file;
			}
			if(operation_file != "")
			{
				fileJson["operation_file"] = operation_file;
			}
			if(dfg_files.count() > 0)
			{
				for(QStringList::iterator it = dfg_files.begin(); it != dfg_files.end(); ++it)
				{
					dfgFile = it->toStdString();
					dfgFiles.emplace_back(dfgFile);
				}
				fileJson["dfg_files"] = dfgFiles;
			}
			if(netlist_files.count() > 0)
			{
				for(QStringList::iterator it = netlist_files.begin(); it != netlist_files.end(); ++it)
				{
					netlistFile = it->toStdString();
					netlistFiles.emplace_back(netlistFile);
				}
				fileJson["netlist_files"] = netlistFiles;
			}
    		os << fileJson.dump(2) << std::endl;
		}
	}

	void CDE::SaveArch()
	{
		QString file_path = QFileDialog::getSaveFileName(this, "Save placement modification", ".", "Netlist files (*.json)");

		if (!file_path.isEmpty())
		{
			std::ofstream os(file_path.toStdString());
			ADGIR adg_ir(adg);
			adg_ir.saveADG(os);
		}
	}

	void CDE::BitGen()
	{
		/*QString file_path = QFileDialog::getSaveFileName(this,
			"Save Configuration File", ".", "Bitstream files (*.bin)");
		if (!file_path.isEmpty())
		{
			Configuration cfg(MapperSA::getResult());
            std::ofstream ofs(file_path.toStdString());
            cfg.dumpCfgData(ofs);
		}*/
	}

	void CDE::DesignModified(bool modified)
	{
		/*setWindowTitle("Chip Viewer - " + _viewWidget->design_path() + (modified ? "*" : ""));
		_actSave->setEnabled(modified);*/
	}

	void CDE::DesignRouteModified(bool modified)
	{
		/*setWindowTitle("Chip Viewer - " + _viewWidget->design_path() + (modified ? "*" : ""));
		_actRTSave->setEnabled(modified);*/
	}

	void CDE::Quit()
	{
		/*if (_viewWidget->design_modified()) {
			int response = QMessageBox::warning(this, tr("Chip Viewer"), 
				tr("The placement has been modified.\nDo you want to save your changes?"),
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

			if (response == QMessageBox::Cancel) return;
			else if (response == QMessageBox::Yes) Save();
		}*/

		close();
	}

	void CDE::DrawBirdRect(QRectF bird_rect)
	{
		if (_viewWidget->arch_is_ready())
			_birdWidget->draw_viewport_rect(bird_rect);
	}

	void CDE::HideDock(bool hide)
	{
		_birdDock->setVisible(!hide);
		_netDock->setVisible(!hide);
		_logDock->setVisible(!hide);
	}

	void CDE::DrawRoute(bool draw)
	{
		setCursor(Qt::WaitCursor);

		setStatus(QString("%1 routing...").arg(draw ? "Draw": "Delete"));
		_viewWidget->draw_route(draw);
		setStatus(QString("Ready\t-\t%1").arg(_arch_file_name));
//		_netWidget->set_nets_list(_viewWidget->creat_route_list());

		unsetCursor();
	}
	/*
	void CDE::ShowTimeInfo(bool show)
	{
		Route::SHOW_TIME_INFO = show;

		//////////////////////////////////////////////////////////////////////////
		// update STA info show
		_viewWidget->select_route_by_name(_netWidget->current_net().toStdString(), true);
	}*/

	void CDE::FreeDockDesignData()
	{
//		_netWidget->set_nets_list(QStringList{});
//		_netWidget->set_sta_info("");
	}

	void CDE::ShowPlacementInfo(bool show)
	{
		//Route::HIDE_PLACE_INFO = !show;
		_viewWidget->update_view();
	}

	void CDE::addProgressbar(const QString& output, short progress)
	{
		addOutput(output);
		_lbProgress->setValue(_lbProgress->value() + progress);
	}

	void CDE::hide_progressbar()
	{
		_lbProgress->setVisible(false);
	}

	void CDE::show_progressbar()
	{
		_lbProgress->setVisible(true);
		_lbProgress->setValue(0);
	}

	void CDE::disable_file_act()
	{
		_actOpenDFG->setEnabled(false);
		//_actLoadOperation->setEnabled(false);
		_actSave->setEnabled(false);
		_actSaveAs->setEnabled(false);
		_actLoadNetlist->setEnabled(false);
		//_actRTSave->setEnabled(false);
		_actBitGen->setEnabled(false);
	}

	void CDE::enable_file_act()
	{
		_actOpenDFG->setEnabled(true);
		//_actLoadOperation->setEnabled(true);
		_actSave->setEnabled(true);
		_actSaveAs->setEnabled(true);
		_actLoadNetlist->setEnabled(true);
		//_actRTSave->setEnabled(true);
		_actBitGen->setEnabled(true);
	}

	void CDE::ArgUsage()
	{
		QMessageBox::about(this, tr("Arguments Usage"), 
			tr("--help, -h:\tpresent help usage\n"
			   "--arch, -a:\tset arch file path\n"
			   "--design, -d:\tset design file path\n"));
	}
	/*
	void CDE::FindNet()
	{
		bool ok;
		QString text =  QInputDialog::getText(this, tr("QInputDialog::getText()"),
												tr("Find Net:"), QLineEdit::Normal,
												_netWidget->find_net_name(), &ok);
		if (ok && !text.isEmpty())
			_netWidget->find_net(text);
	}*/
//}
