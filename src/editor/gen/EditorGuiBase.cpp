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
	m_sidePanel = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxSize( 300,-1 ), wxTAB_TRAVERSAL );
	m_sidePanel->SetMinSize( wxSize( 200,-1 ) );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter2 = new wxSplitterWindow( m_sidePanel, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxSP_3D|wxSP_LIVE_UPDATE );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( EditorFrameBase::m_splitter2OnIdle ), NULL, this );
	m_splitter2->SetMinimumPaneSize( 100 );
	
	m_entityPanel = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_entityLabel = new wxStaticText( m_entityPanel, wxID_ANY, wxT("Entity"), wxDefaultPosition, wxDefaultSize, 0 );
	m_entityLabel->Wrap( -1 );
	bSizer8->Add( m_entityLabel, 0, wxALL, 5 );
	
	m_entityIdField = new wxTextCtrl( m_entityPanel, ID_ENTITYIDFIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	m_entityIdField->Enable( false );
	
	bSizer8->Add( m_entityIdField, 1, wxALL, 0 );
	
	bSizer2->Add( bSizer8, 0, wxEXPAND, 5 );
	
	m_compList = new wxListCtrl( m_entityPanel, ID_COMPLIST, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer2->Add( m_compList, 1, wxALL|wxEXPAND, 1 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_butNewComp = new wxButton( m_entityPanel, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butNewComp->Enable( false );
	
	bSizer4->Add( m_butNewComp, 0, wxALL|wxEXPAND, 5 );
	
	m_butRemoveComp = new wxButton( m_entityPanel, wxID_ANY, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butRemoveComp->Enable( false );
	
	bSizer4->Add( m_butRemoveComp, 0, wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( bSizer4, 0, wxEXPAND, 5 );
	
	m_entityPanel->SetSizer( bSizer2 );
	m_entityPanel->Layout();
	bSizer2->Fit( m_entityPanel );
	m_compPanel = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxHORIZONTAL );
	
	m_compLabel = new wxStaticText( m_compPanel, wxID_ANY, wxT("Component"), wxDefaultPosition, wxDefaultSize, 0 );
	m_compLabel->Wrap( -1 );
	bSizer81->Add( m_compLabel, 0, wxALL, 5 );
	
	m_compIdField = new wxTextCtrl( m_compPanel, ID_COMPIDFIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	m_compIdField->Enable( false );
	
	bSizer81->Add( m_compIdField, 1, wxALL, 0 );
	
	bSizer6->Add( bSizer81, 0, wxEXPAND, 5 );
	
	m_propList = new wxListCtrl( m_compPanel, ID_PROPLIST, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer6->Add( m_propList, 1, wxALL|wxEXPAND, 1 );
	
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
	m_butNewProp = new wxButton( m_compPanel, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butNewProp->Enable( false );
	
	bSizer41->Add( m_butNewProp, 0, wxALL|wxEXPAND, 5 );
	
	m_butRemoveProp = new wxButton( m_compPanel, wxID_ANY, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butRemoveProp->Enable( false );
	
	bSizer41->Add( m_butRemoveProp, 0, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( bSizer41, 0, wxEXPAND, 5 );
	
	m_compPanel->SetSizer( bSizer6 );
	m_compPanel->Layout();
	bSizer6->Fit( m_compPanel );
	m_splitter2->SplitHorizontally( m_entityPanel, m_compPanel, 0 );
	bSizer7->Add( m_splitter2, 1, wxEXPAND, 0 );
	
	m_sidePanel->SetSizer( bSizer7 );
	m_sidePanel->Layout();
	m_splitter1->SplitVertically( m_glpanel, m_sidePanel, -1 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 1 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

EditorFrameBase::~EditorFrameBase()
{
}

NewCompDialog::NewCompDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 300,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_compChoiceChoices;
	m_compChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_compChoiceChoices, 0 );
	m_compChoice->SetSelection( 0 );
	bSizer11->Add( m_compChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer12->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button6 = new wxButton( this, wxID_OK, wxT("Create"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_button6, 0, wxALL, 5 );
	
	m_button7 = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_button7, 0, wxALL, 5 );
	
	
	bSizer12->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer11->Add( bSizer12, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer11 );
	this->Layout();
	bSizer11->Fit( this );
	
	this->Centre( wxBOTH );
}

NewCompDialog::~NewCompDialog()
{
}

NewPropDialog::NewPropDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 300,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Key"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL, 5 );
	
	m_textCtrl3 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl3, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL, 5 );
	
	m_textCtrl4 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl4, 0, wxALL|wxEXPAND, 5 );
	
	bSizer11->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer12->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button6 = new wxButton( this, wxID_OK, wxT("Create"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_button6, 0, wxALL, 5 );
	
	m_button7 = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_button7, 0, wxALL, 5 );
	
	
	bSizer12->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer11->Add( bSizer12, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer11 );
	this->Layout();
	bSizer11->Fit( this );
	
	this->Centre( wxBOTH );
}

NewPropDialog::~NewPropDialog()
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
