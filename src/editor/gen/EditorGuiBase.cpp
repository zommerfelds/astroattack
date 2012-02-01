///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov  5 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "EditorGuiBase.h"

///////////////////////////////////////////////////////////////////////////

EditorFrameBase::EditorFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_toolBar = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY ); 
	m_toolBar->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_toolBar->Realize();
	
	m_statusBar1 = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	m_menubar1 = new wxMenuBar( 0 );
	m_menuFile = new wxMenu();
	wxMenuItem* m_menuItemNew;
	m_menuItemNew = new wxMenuItem( m_menuFile, wxID_NEW, wxString( wxT("New Level") ) , wxT("Create empty level"), wxITEM_NORMAL );
	m_menuFile->Append( m_menuItemNew );
	
	wxMenuItem* m_menuItemOpen;
	m_menuItemOpen = new wxMenuItem( m_menuFile, wxID_OPEN, wxString( wxT("Open") ) , wxT("Open existing level"), wxITEM_NORMAL );
	m_menuFile->Append( m_menuItemOpen );
	
	wxMenuItem* m_menuItemSave;
	m_menuItemSave = new wxMenuItem( m_menuFile, wxID_SAVE, wxString( wxT("Save") ) , wxT("Save current level"), wxITEM_NORMAL );
	m_menuFile->Append( m_menuItemSave );
	
	wxMenuItem* m_menuItemSaveAs;
	m_menuItemSaveAs = new wxMenuItem( m_menuFile, wxID_SAVEAS, wxString( wxT("Save As") ) , wxT("Save current level with a different file name"), wxITEM_NORMAL );
	m_menuFile->Append( m_menuItemSaveAs );
	
	wxMenuItem* m_separator1;
	m_separator1 = m_menuFile->AppendSeparator();
	
	wxMenuItem* m_menuItemExit;
	m_menuItemExit = new wxMenuItem( m_menuFile, wxID_EXIT, wxString( wxT("Quit") ) , wxT("Quit AAEditor"), wxITEM_NORMAL );
	m_menuFile->Append( m_menuItemExit );
	
	m_menubar1->Append( m_menuFile, wxT("File") ); 
	
	m_menuHelp = new wxMenu();
	wxMenuItem* m_menuItemAbout;
	m_menuItemAbout = new wxMenuItem( m_menuHelp, wxID_ABOUT, wxString( wxT("About") ) , wxT("Show about dialog"), wxITEM_NORMAL );
	m_menuHelp->Append( m_menuItemAbout );
	
	m_menubar1->Append( m_menuHelp, wxT("Help") ); 
	
	this->SetMenuBar( m_menubar1 );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE );
	m_splitter1->SetSashGravity( 1 );
	m_splitter1->SetMinimumPaneSize( 50 );
	
	m_glpanel = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxSize( 300,-1 ), wxTAB_TRAVERSAL );
	m_panel1->SetMinSize( wxSize( 200,-1 ) );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_entityLabel = new wxStaticText( m_panel1, wxID_ANY, wxT("Entity"), wxDefaultPosition, wxDefaultSize, 0 );
	m_entityLabel->Wrap( -1 );
	bSizer2->Add( m_entityLabel, 0, wxALL, 5 );
	
	m_compList = new wxListCtrl( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer2->Add( m_compList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button1 = new wxButton( m_panel1, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button1, 0, wxALL|wxEXPAND, 5 );
	
	m_button4 = new wxButton( m_panel1, wxID_ANY, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button4, 0, wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( bSizer4, 0, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	m_splitter1->SplitVertically( m_glpanel, m_panel1, -1 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 1 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

EditorFrameBase::~EditorFrameBase()
{
}

AboutDialog::AboutDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 300,160 ), wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_panel3 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( m_panel3, wxID_ANY, wxT("AAEditor v###\n\nBy Christian Zommerfelds\nCopyright (c) 2012"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer7->Add( m_staticText2, 1, wxALL, 5 );
	
	m_butOk = new wxButton( m_panel3, wxID_OK, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_butOk, 0, wxALL, 5 );
	
	m_panel3->SetSizer( bSizer7 );
	m_panel3->Layout();
	bSizer7->Fit( m_panel3 );
	bSizer5->Add( m_panel3, 1, wxEXPAND | wxALL, 8 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	bSizer5->Fit( this );
	
	this->Centre( wxBOTH );
}

AboutDialog::~AboutDialog()
{
}
