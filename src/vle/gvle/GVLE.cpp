/**
 * @file vle/gvle/GVLE.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/About.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/ExecutionBox.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/HostsBox.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/PreferencesBox.hpp>
#include <vle/gvle/ViewOutputBox.hpp>
#include <vle/gvle/View.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/vpz/Vpz.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <boost/filesystem.hpp>
#include <gtkmm/filechooserdialog.h>
#include <glibmm/spawn.h>
#include <glibmm/miscutils.h>
#include <gtkmm/stock.h>

namespace vle { namespace gvle {

const std::string GVLE::WINDOW_TITLE =
    "GVLE  " +
    std::string(VLE_VERSION) +
    std::string(VLE_EXTRA_VERSION);

GVLE::FileTreeView::FileTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject), mDelayTime(0)
{

    mRefTreeModel = Gtk::TreeStore::create(mColumns);
    set_model(mRefTreeModel);
    mColumnName = append_column_editable(_("Files"), mColumns.m_col_name);

    mCellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnName - 1));
    mCellrenderer->property_editable() = true;
    mCellrenderer->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &GVLE::FileTreeView::onEditionStarted) );

    mCellrenderer->signal_edited().connect(
	sigc::mem_fun(*this,
		      &GVLE::FileTreeView::onEdition) );

    mRefTreeSelection = get_selection();
    mIgnoredFilesList.push_front("build");

    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Open with..."),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onOpen)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("New _Directory"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onNewDirectory)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("New _File"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onNewFile)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Rename"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onRename)));
    }
    mMenuPopup.accelerate(*this);
}

GVLE::FileTreeView::~FileTreeView()
{
}

void GVLE::FileTreeView::buildHierarchyDirectory(
    const Gtk::TreeModel::Row& parent, const std::string& dirname)
{
    Glib::Dir dir(dirname);
    std::list<std::string> entries (dir.begin(), dir.end());
    entries.sort();
    std::list <std::string> ::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
	if (((*it)[0] != '.') //Don't show hidden files
	    and (std::find(mIgnoredFilesList.begin(), mIgnoredFilesList.end(), *it)
	         == mIgnoredFilesList.end())) {
	    std::string nextpath = Glib::build_filename(dirname, *it);
	    if (isDirectory(nextpath)) {
		Gtk::TreeModel::Row row = *(mRefTreeModel->append(parent.children()));
		row[mColumns.m_col_name] = *it;
		buildHierarchy(*row, nextpath);
	    }
	}
    }
}

void GVLE::FileTreeView::buildHierarchyFile(
    const Gtk::TreeModel::Row& parent, const std::string& dirname)
{
    Glib::Dir dir(dirname);
    std::list<std::string> entries (dir.begin(), dir.end());
    entries.sort();
    std::list<std::string>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
	if (((*it)[0] != '.') //Don't show hidden files
	    and (std::find(mIgnoredFilesList.begin(), mIgnoredFilesList.end(), *it)
	         == mIgnoredFilesList.end())) {
	    std::string nextpath = Glib::build_filename(dirname, *it);
	    if (not isDirectory(nextpath)) {
		Gtk::TreeModel::Row row = *(mRefTreeModel->append(parent.children()));
		row[mColumns.m_col_name] = *it;
	    }
	}
    }
}


void GVLE::FileTreeView::buildHierarchy(
    const Gtk::TreeModel::Row& parent, const std::string& dirname)
{
    buildHierarchyDirectory(parent, dirname);
    buildHierarchyFile(parent, dirname);
}

void GVLE::FileTreeView::clear()
{
    mRefTreeModel->clear();
}

void GVLE::FileTreeView::build()
{
    if (not mPackage.empty()) {
	Gtk::TreeIter iter = mRefTreeModel->append();
	Gtk::TreeModel::Row row = *iter;
	row[mColumns.m_col_name] = boost::filesystem::basename(mPackage);
	buildHierarchy(*row, mPackage);
	expand_row(Gtk::TreePath(iter), false);
    }
}

bool GVLE::FileTreeView::isDirectory(const std::string& dirname)
{
    return Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR);
}

void GVLE::FileTreeView::on_row_activated(const Gtk::TreeModel::Path&,
					  Gtk::TreeViewColumn*)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
    const Gtk::TreeModel::Row row = *it;
    const std::list<std::string>* lstpath = projectFilePath(row);

    std::string absolute_path =
	Glib::build_filename(mPackage, Glib::build_filename(*lstpath));
    if (not isDirectory(absolute_path)) {
	if (mParent->getEditor()->existTab(absolute_path)) {
	    mParent->getEditor()->focusTab(absolute_path);
	} else {
	    if (boost::filesystem::extension(absolute_path) == ".vpz") {
		mParent->getEditor()->closeVpzTab();
		if (not mParent->getEditor()->existVpzTab())
		    mParent->getEditor()->openTab(absolute_path);
	    } else {
		mParent->getEditor()->openTab(absolute_path);
	    }
	}
    }
    else {
	if (not row_expanded(Gtk::TreePath(it)))
	    expand_row(Gtk::TreePath(it), false);
	else
	    collapse_row(Gtk::TreePath(it));
    }
}

std::list<std::string>* GVLE::FileTreeView::projectFilePath(
    const Gtk::TreeRow& row)
{
    if (row.parent()) {
	std::list<std::string>* lst =
	    projectFilePath(*row.parent());
	lst->push_back(std::string(row.get_value(mColumns.m_col_name)));
	return lst;
    } else {
	return new std::list<std::string>();
    }
}

bool GVLE::FileTreeView::on_button_press_event(GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);
    if (event->type == GDK_BUTTON_PRESS and event->button == 3
	and not vle::utils::Path::path().getPackageDir().empty()) {
	mMenuPopup.popup(event->button, event->time);
    }

    if (event->type == GDK_BUTTON_PRESS) {
	if (mDelayTime + 250 < event->time) {
	    mDelayTime = event->time;
	    mCellrenderer->property_editable() = true;
	} else {
	    mDelayTime = event->time;
	    mCellrenderer->property_editable() = false;
	    Gtk::TreeModel::Path path;
	    Gtk::TreeViewColumn* column;
	    get_cursor(path, column);
	    on_row_activated(path, column);
	}
    }
    return return_value;
}

void GVLE::FileTreeView::onOpen()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  get_selection();
    SimpleTypeBox box(_("Path of the program ?"));
    std::string prg = Glib::find_program_in_path(boost::trim_copy(box.run()));
    if (refSelection and box.valid() and not prg.empty()) {
	Gtk::TreeModel::Row row = *refSelection->get_selected();
	std::list < std::string > argv;
	argv.push_back(prg);
	std::string filepath = Glib::build_filename(
	    mPackage, Glib::build_filename(*projectFilePath(row)));
	argv.push_back(filepath);

	try {
	    Glib::spawn_async(utils::Path::path().getParentPath(filepath),
			     argv,
			     Glib::SpawnFlags(0),
			     sigc::slot < void >());
	} catch(const Glib::SpawnError& e) {
	    Error(_("The program can not be lanched"));
	}
    }
}

void GVLE::FileTreeView::onNewFile()
{
    SimpleTypeBox box(_("Name of the File ?"));
    std::string name = boost::trim_copy(box.run());
    std::string filepath;
    if (box.valid() and not name.empty()) {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	if (refSelection) {
	    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
	    const Gtk::TreeModel::Row row = *it;
	    const std::list<std::string>* lstpath = projectFilePath(row);
	    filepath = Glib::build_filename(
		mPackage, Glib::build_filename(*lstpath));
	    if (not isDirectory(filepath)) {
		boost::filesystem::path path(filepath);
		filepath = boost::lexical_cast<std::string>(path);
	    }
	} else {
	    filepath = mPackage;
	}
	utils::Package::package().addFile(filepath, name);
    }
    mParent->buildPackageHierarchy();
}

void GVLE::FileTreeView::onNewDirectory()
{
    SimpleTypeBox box(_("Name of the Directory ?"));
    std::string name = boost::trim_copy(box.run());
    std::string directorypath;
    if (box.valid() and not name.empty()) {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	if (refSelection) {
	    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
	    const Gtk::TreeModel::Row row = *it;
	    const std::list<std::string>* lstpath = projectFilePath(row);
	    directorypath = Glib::build_filename(
		mPackage, Glib::build_filename(*lstpath));
	    if (not isDirectory(directorypath)) {
		boost::filesystem::path path(directorypath);
		directorypath = boost::lexical_cast<std::string>(path);
	    }
	} else {
	    directorypath = mPackage;
	}
	utils::Package::package().addDirectory(directorypath, name);
    }
    mParent->buildPackageHierarchy();
}


void GVLE::FileTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    if (refSelection) {
	Gtk::TreeModel::const_iterator it = refSelection->get_selected();
	const Gtk::TreeModel::Row row = *it;
	const std::list<std::string>* lstpath = projectFilePath(row);

	if (gvle::Question(_("Do you really want remove this file ?\n")))
	    utils::Package::package().removeFile(Glib::build_filename(*lstpath));
    }
    mParent->buildPackageHierarchy();
}

void GVLE::FileTreeView::onRename()
{
    SimpleTypeBox box(_("Name of the file ?"));
    std::string name = boost::trim_copy(box.run());
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (box.valid() and not name.empty() and refSelection) {
	Gtk::TreeModel::const_iterator it = refSelection->get_selected();
	const Gtk::TreeModel::Row row = *it;
	const std::list<std::string>* lstpath = projectFilePath(row);

	utils::Package::package().renameFile(Glib::build_filename(*lstpath),
					name);
    }
    mParent->buildPackageHierarchy();
}

void GVLE::FileTreeView::onEditionStarted(Gtk::CellEditable* cell_editable,
					  const Glib::ustring& /* path */)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();

    if (iter) {
	Gtk::TreeModel::Row row = *iter;
	const std::list<std::string>* lstpath = projectFilePath(row);

	mOldAbsolutePath = Glib::build_filename(*lstpath);
    }

    if(mValidateRetry) {
	Gtk::CellEditable* celleditable_validated = cell_editable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
	if(pEntry) {
	    pEntry->set_text(mInvalidTextForRetry);
	    mValidateRetry = false;
	    mInvalidTextForRetry.clear();
	}
    }
}

void GVLE::FileTreeView::onEdition(const Glib::ustring& pathString,
				   const Glib::ustring& newName)
{
    std::string name(newName);
    utils::Package::package().renameFile(mOldAbsolutePath, name);
    Gtk::TreeModel::Row row = *(mRefTreeModel->get_iter(pathString));
    row[mColumns.m_col_name] = newName;
}

GVLE::GVLE(BaseObjectType* cobject,
	   const Glib::RefPtr<Gnome::Glade::Xml> xml):
    Gtk::Window(cobject),
    m_modeling(new Modeling(this)),
    m_currentButton(POINTER),
    m_helpbox(0)
{
    mRefXML = xml;
    m_modeling->setGlade(mRefXML);

    mGlobalVpzPrevDirPath = "";

    mConditionsBox = new ConditionsBox(mRefXML, m_modeling);
    mSimulationBox = new LaunchSimulationBox(mRefXML, m_modeling);
    mPreferencesBox = new PreferencesBox(mRefXML, m_modeling);
    mOpenPackageBox = new OpenPackageBox(mRefXML, m_modeling);
    mOpenVpzBox = new OpenVpzBox(mRefXML, m_modeling);
    mNewProjectBox = new NewProjectBox(mRefXML, m_modeling);
    mSaveVpzBox = new SaveVpzBox(mRefXML, m_modeling);
    mQuitBox = new QuitBox(mRefXML, m_modeling);

    mRefXML->get_widget("MenuAndToolbarVbox", mMenuAndToolbarVbox);
    mRefXML->get_widget("StatusBarPackageBrowser", mStatusbar);
    mRefXML->get_widget("TextViewLogPackageBrowser", mLog);
    mRefXML->get_widget_derived("FileTreeViewPackageBrowser", mFileTreeView);
    mFileTreeView->setParent(this);
    mRefXML->get_widget_derived("NotebookPackageBrowser", mEditor);
    mEditor->setParent(this);
    mRefXML->get_widget_derived("TreeViewModel", mModelTreeBox);
    mModelTreeBox->setModeling(m_modeling);
    mRefXML->get_widget_derived("TreeViewClass", mModelClassBox);
    mModelClassBox->createNewModelBox(m_modeling);

    mMenuAndToolbar = new GVLEMenuAndToolbar(this);
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getMenuBar());
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getToolbar());
    mMenuAndToolbar->getToolbar()->set_toolbar_style(Gtk::TOOLBAR_BOTH);

    m_modeling->setModified(false);
    set_title(WINDOW_TITLE);
    resize(900, 550);
    show();
}

GVLE::~GVLE()
{
    delete m_modeling;

    delete mConditionsBox;
    delete mSimulationBox;
    delete mPreferencesBox;
    delete mOpenPackageBox;
    delete mOpenVpzBox;
    delete mNewProjectBox;
    delete mSaveVpzBox;
}

void GVLE::show()
{
    buildPackageHierarchy();
    show_all();
}

void GVLE::setModifiedTitle(const std::string& name)
{
    if (not name.empty() and
	boost::filesystem::extension(name) == ".vpz") {
	Editor::Documents::iterator it =
	    mEditor->getDocumentsList().find(name);
	if (it != mEditor->getDocumentsList().end())
	    it->second->setTitle(Glib::path_get_basename(name),
				 getModeling()->getTopModel(),
				 true);
    }
}

void GVLE::buildPackageHierarchy()
{
    mModelTreeBox->clear();
    mModelClassBox->clear();
    mPackage = vle::utils::Path::path().getPackageDir();
    setTitle();
    mFileTreeView->clear();
    mFileTreeView->setPackage(mPackage);
    mFileTreeView->build();
}

void GVLE::setFileName(std::string name)
{
    if (not name.empty() and
	boost::filesystem::extension(name) == ".vpz") {
	m_modeling->parseXML(name);
	mMenuAndToolbar->onViewMode();
    }
    m_modeling->setModified(false);
}

void GVLE::insertLog(const std::string& text)
{
    mLog->get_buffer()->insert(
	mLog->get_buffer()->end(), text);
}

void GVLE::redrawModelTreeBox()
{
    assert(m_modeling->getTopModel());
    mModelTreeBox->parseModel(m_modeling->getTopModel());
}

void GVLE::redrawModelClassBox()
{
    mModelClassBox->parseClass();
}

void GVLE::clearModelTreeBox()
{
    mModelTreeBox->clear();
}

void GVLE::clearModelClassBox()
{
    mModelClassBox->clear();
}

void GVLE::showRowTreeBox(const std::string& name)
{
    mModelTreeBox->showRow(name);
}

void GVLE::showRowModelClassBox(const std::string& name)
{
    mModelClassBox->showRow(name);
}

bool GVLE::on_delete_event(GdkEventAny* event)
{
    if (event->type == GDK_DELETE) {
	onMenuQuit();
	return true;
    }
    return false;
}

void GVLE::onArrow()
{
    m_currentButton = POINTER;
    m_status.push(_("Selection"));
}

void GVLE::onAddModels()
{
    m_currentButton = ADDMODEL;
    m_status.push(_("Add models"));
}

void GVLE::onAddLinks()
{
    m_currentButton = ADDLINK;
    m_status.push(_("Add links"));
}

void GVLE::onDelete()
{
    m_currentButton = DELETE;
    m_status.push(_("Delete object"));
}

void GVLE::onAddCoupled()
{
    m_currentButton = ADDCOUPLED;
    m_status.push(_("Coupled Model"));
}

void GVLE::onZoom()
{
    m_currentButton = ZOOM;
    m_status.push(_("Zoom"));
}

void GVLE::onQuestion()
{
    m_currentButton = QUESTION;
    m_status.push(_("Question"));
}

void GVLE::newFile()
{
    mEditor->createBlankNewFile();
}

void GVLE::onMenuNew()
{
    if (m_modeling->isModified() == false) {
        m_modeling->delNames();
        m_modeling->start();
        redrawModelTreeBox();
    } else if (gvle::Question(_("Do you really want destroy model ?"))) {
        m_modeling->delNames();
        m_modeling->start();
        redrawModelTreeBox();
    }
}

void GVLE::onMenuNewProject()
{
    mNewProjectBox->show();
}


void GVLE::openFile()
{
    Gtk::FileChooserDialog file(_("Choose a file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    if (file.run() == Gtk::RESPONSE_OK) {
	std::string selected_file = file.get_filename();
	mEditor->openTab(selected_file);
    }
}
void GVLE::onMenuOpenPackage()
{
    mEditor->closeAllTab();
    mOpenPackageBox->show();
    if (not utils::Package::package().name().empty())
	mMenuAndToolbar->onPackageMode();
}

void GVLE::onMenuOpenVpz()
{
    if (m_modeling->isModified() == false or
	gvle::Question(_("Do you really want load a new Model ?\nCurrent"
			 "model will be destroy and not save"))) {
	try {
	    mOpenVpzBox->show();
	    mMenuAndToolbar->onViewMode();
	} catch(utils::InternalError) {
            Error((fmt(_("No experiments in the package '%1%'")) %
                  utils::Package::package().name()).str());
	}
    }
}

void GVLE::onMenuLoad()
{
    if (m_modeling->isModified() == false or
            gvle::Question(_("Do you really want load a new Model ?\nCurrent"
                             "model will be destroy and not save"))) {
        Gtk::FileChooserDialog file("VPZ file", Gtk::FILE_CHOOSER_ACTION_OPEN);
        file.set_transient_for(*this);
        file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
        Gtk::FileFilter filter;
        filter.set_name(_("Vle Project gZipped"));
        filter.add_pattern("*.vpz");
        file.add_filter(filter);
	if (mGlobalVpzPrevDirPath != "") {
	    file.set_current_folder(mGlobalVpzPrevDirPath);
        }

        if (file.run() == Gtk::RESPONSE_OK) {
	    mGlobalVpzPrevDirPath = file.get_current_folder();
	    mEditor->closeAllTab();
	    utils::Package::package().select("");
	    mPackage = "";
            m_modeling->parseXML(file.get_filename());
	    mMenuAndToolbar->onGlobalMode();
	    mMenuAndToolbar->onViewMode();
	    mFileTreeView->clear();
        }
    }
}

void GVLE::saveFile()
{
    int page = mEditor->get_current_page();
    if (page != -1) {
	DocumentText* doc = dynamic_cast < DocumentText* >(
	    mEditor->get_nth_page(page));
	if (not doc->isNew()) {
	    doc->save();
	} else {
	    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	    if (file.run() == Gtk::RESPONSE_OK) {
		std::string filename(file.get_filename());
		doc->saveAs(filename);
	    }
	}
	buildPackageHierarchy();
    }
}

void GVLE::onMenuSave()
{
    std::vector<std::string> vec;
    m_modeling->vpz_is_correct(vec);

    if (vec.size() != 0) {
        //vpz is correct
        std::string error = _("Vpz incorrect :\n");
        std::vector<std::string>::const_iterator it = vec.begin();
        while (it != vec.end()) {
            error += *it + "\n";

            ++it;
        }
        Error(error);
        return;
    }

    if (m_modeling->isSaved()) {
	m_modeling->saveXML(m_modeling->getFileName());
	Editor::Documents::iterator it =
	    mEditor->getDocumentsList().find(m_modeling->getFileName());
	if (it != mEditor->getDocumentsList().end())
	    it->second->setTitle(m_modeling->getFileName(),
				 m_modeling->getTopModel(), false);
    } else if (not utils::Package::package().name().empty()) {
	mSaveVpzBox->show();
    } else {
	Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	file.set_transient_for(*this);
	file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	Gtk::FileFilter filter;
	filter.set_name(_("Vle Project gZipped"));
	filter.add_pattern("*.vpz");
	file.add_filter(filter);

	if (file.run() == Gtk::RESPONSE_OK) {
	    std::string filename(file.get_filename());
	    vpz::Vpz::fixExtension(filename);
	    m_modeling->saveXML(filename);
	    Editor::Documents::iterator it =
		mEditor->getDocumentsList().find(filename);
	    if (it != mEditor->getDocumentsList().end())
		it->second->setTitle(filename,
				     m_modeling->getTopModel(), false);
	}
    }
}

void GVLE::saveFileAs()
{
    int page = mEditor->get_current_page();
    if (page != -1) {
	DocumentText* doc = dynamic_cast < DocumentText* >(
	    mEditor->get_nth_page(page));
	Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	if (file.run() == Gtk::RESPONSE_OK) {
	    std::string filename(file.get_filename());
	    doc->saveAs(filename);
	}
	buildPackageHierarchy();
    }
}

void GVLE::closeFile()
{
    int page = mEditor->get_current_page();
    if (page != -1) {
	Gtk::Widget* tab = mEditor->get_nth_page(page);
	Editor::Documents::iterator it = mEditor->getDocumentsList().begin();
	while (it != mEditor->getDocumentsList().end()) {
	    if (it->second == tab) {
		mEditor->closeTab(it->first);
		break;
	    }
	    ++it;
	}
    }
}

void GVLE::tabClosed()
{
    if (not utils::Package::package().selected()) {
	mMenuAndToolbar->onGlobalMode();
    } else {
	mMenuAndToolbar->onPackageMode();
    }
}

void GVLE::onMenuQuit()
{
    mQuitBox->show();
}


void GVLE::onPreferences()
{
    mPreferencesBox->show();
}

void GVLE::onSimulationBox()
{
    if (m_modeling->isSaved()) {
        mSimulationBox->show();
    } else {
        gvle::Error(_("Save or load a project before simulation"));
    }
}

void GVLE::onParameterExecutionBox()
{
    ParameterExecutionBox* box = new ParameterExecutionBox(m_modeling);
    box->run();
    delete box;
}

void GVLE::onExperimentsBox()
{
    ExperimentBox box(mRefXML, m_modeling);
    box.run();
}

void GVLE::onConditionsBox()
{
    mConditionsBox->show();
}

void GVLE::onHostsBox()
{
    HostsBox* box = new HostsBox(mRefXML);
    box->run();
    delete box;
}

void GVLE::onHelpBox()
{
    if (m_helpbox == 0)
        m_helpbox = new HelpBox;

    m_helpbox->show_all();
}

void GVLE::onViewOutputBox()
{
    ViewOutputBox box(*m_modeling, mRefXML, m_modeling->views());
    box.run();
}

void GVLE::onShowAbout()
{
    About box(mRefXML);
    box.run();
}

void GVLE::setTitle(const Glib::ustring& name)
{
    Glib::ustring title(WINDOW_TITLE);

    if (utils::Package::package().selected()) {
	title += " - " + utils::Package::package().name();
    }

    if (not name.empty()) {
        title += " - " + Glib::path_get_basename(name);
    }
    set_title(title);
}

std::string valuetype_to_string(value::Value::type type)
{
    switch (type) {
    case(value::Value::BOOLEAN):
        return "boolean";
        break;
    case(value::Value::INTEGER):
        return "integer";
        break;
    case(value::Value::DOUBLE):
        return "double";
        break;
    case(value::Value::STRING):
        return "string";
        break;
    case(value::Value::SET):
        return "set";
        break;
    case(value::Value::MAP):
        return "map";
        break;
    case(value::Value::TUPLE):
        return "tuple";
        break;
    case(value::Value::TABLE):
        return "table";
        break;
    case(value::Value::XMLTYPE):
        return "xml";
        break;
    case(value::Value::NIL):
        return "null";
        break;
    case(value::Value::MATRIX):
        return "matrix";
        break;
    default:
        return "(no value)";
        break;
    }
}

bool GVLE::packageTimer()
{
    std::string o, e;
    utils::Package::package().getOutputAndClear(o);
    utils::Package::package().getErrorAndClear(e);

    if (not o.empty()) {
        mLog->get_buffer()->insert(mLog->get_buffer()->end(), o);
    }

    if (not e.empty()) {
        mLog->get_buffer()->insert(mLog->get_buffer()->end(), e);
    }

    Gtk::TextBuffer::iterator iter = mLog->get_buffer()->end();
    mLog->get_buffer()->place_cursor(iter);
    mLog->scroll_to(iter, 0.0, 0.0, 1.0);

    if (utils::Package::package().isFinish()) {
        getMenu()->showPackageMenu();
        return false;
    } else {
        return true;
    }
}

bool GVLE::packageBuildTimer()
{
    std::string o, e;
    utils::Package::package().getOutputAndClear(o);
    utils::Package::package().getErrorAndClear(e);

    if (not o.empty()) {
        mLog->get_buffer()->insert(mLog->get_buffer()->end(), o);
    }

    if (not e.empty()) {
        mLog->get_buffer()->insert(mLog->get_buffer()->end(), e);
    }

    Gtk::TextBuffer::iterator iter = mLog->get_buffer()->end();
    mLog->get_buffer()->place_cursor(iter);
    mLog->scroll_to(iter, 0.0, 0.0, 1.0);

    if (utils::Package::package().isFinish()) {
        if (utils::Package::package().isSuccess()) {
            installProject();
        }
        return false;
    } else {
        return true;
    }
}

void GVLE::configureProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "configure package\n");
    getMenu()->hidePackageMenu();
    try {
        utils::Package::package().configure();
    } catch (const std::exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    } catch (const Glib::Exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::buildProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "build package\n");
    getMenu()->hidePackageMenu();
    try {
        utils::Package::package().build();
    } catch (const std::exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    } catch (const Glib::Exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageBuildTimer), 250);
}

void GVLE::installProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "build package\n");
    getMenu()->hidePackageMenu();
    try {
        utils::Package::package().install();
    } catch (const std::exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    } catch (const Glib::Exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::cleanProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "clean package\n");
    getMenu()->hidePackageMenu();
    try {
        utils::Package::package().clean();
    } catch (const std::exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    } catch (const Glib::Exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::packageProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "make tarball\n");
    getMenu()->hidePackageMenu();
    try {
        utils::Package::package().pack();
    } catch (const std::exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    } catch (const Glib::Exception& e) {
        getMenu()->showPackageMenu();
        gvle::Error(e.what());
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::onCutModel()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->onCutModel();
}

void GVLE::onCopyModel()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->onCopyModel();
}

void GVLE::onPasteModel()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->onPasteModel();
}

void GVLE::clearCurrentModel()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->clearCurrentModel();
}

void GVLE::importModel()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->importModel();
}

void GVLE::exportCurrentModel()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->exportCurrentModel();
}

void GVLE::exportGraphic()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();
    vpz::Experiment& experiment = m_modeling->vpz().project().experiment();
    if (experiment.name().empty() || experiment.duration() == 0) {
        Error(_("Fix a Value to the name and the duration of the experiment before exportation."));
        return;
    }

    Gtk::FileChooserDialog file(_("Image file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::FileFilter filterAuto;
    Gtk::FileFilter filterPng;
    Gtk::FileFilter filterPdf;
    Gtk::FileFilter filterSvg;
    filterAuto.set_name(_("Guess type from file name"));
    filterAuto.add_pattern("*");
    filterPng.set_name(_("Portable Newtork Graphics (.png)"));
    filterPng.add_pattern("*.png");
    filterPdf.set_name(_("Portable Format Document (.pdf)"));
    filterPdf.add_pattern("*.pdf");
    filterSvg.set_name(_("Scalable Vector Graphics (.svg)"));
    filterSvg.add_pattern("*.svg");
    file.add_filter(filterAuto);
    file.add_filter(filterPng);
    file.add_filter(filterPdf);
    file.add_filter(filterSvg);


    if (file.run() == Gtk::RESPONSE_OK) {
        std::string filename(file.get_filename());
	std::string extension(file.get_filter()->get_name());

	if (extension == _("Guess type from file name")) {
	    size_t ext_pos = filename.find_last_of('.');
	    if (ext_pos != std::string::npos) {
		std::string type(filename, ext_pos+1);
		filename.resize(ext_pos);
		if (type == "png")
		    tab->exportPng(filename);
		else if (type == "pdf")
		    tab->exportPdf(filename);
		else if (type == "svg")
		    tab->exportSvg(filename);
		else
		    Error(_("Unsupported file format"));
	    }
	}
	else if (extension == _("Portable Newtork Graphics (.png)"))
	    tab->exportPng(filename);
	else if (extension == _("Portable Format Document (.pdf)"))
	    tab->exportPdf(filename);
	else if (extension == _("Scalable Vector Graphics (.svg)"))
	    tab->exportSvg(filename);
    }
}

void GVLE::addCoefZoom()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->addCoefZoom();
}

void GVLE::delCoefZoom()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->delCoefZoom();
}

void GVLE::setCoefZoom(double coef)
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->setCoefZoom(coef);
}

void  GVLE::updateAdjustment(double h, double v)
{
    DocumentDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab));
    tab->setHadjustment(h);
    tab->setVadjustment(v);
}

void GVLE::onRandomOrder()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->onRandomOrder();
}

}} // namespace vle gvle
