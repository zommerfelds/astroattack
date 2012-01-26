///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov  5 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EditorGuiBase__
#define __EditorGuiBase__

#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/frame.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorFrameBase
///////////////////////////////////////////////////////////////////////////////
class EditorFrameBase : public wxFrame 
{
	private:
	
	protected:
		wxStatusBar* m_statusBar1;
		wxMenuBar* m_menubar1;
		wxMenu* m_menuFile;
		wxMenu* m_menuHelp;
		wxToolBar* m_toolBar1;
		wxPanel* m_glpanel;
		wxPanel* m_panel1;
		wxPanel* m_panel2;
		wxListCtrl* m_listCtrl1;
		wxPanel* m_panel3;
		wxButton* m_button1;
		wxButton* m_button4;
		wxButton* m_button3;
		wxStaticText* m_staticText3;
	
	public:
		
		EditorFrameBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("AstroAttack Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 700,526 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~EditorFrameBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialog
///////////////////////////////////////////////////////////////////////////////
class AboutDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxButton* m_butOk;
	
	public:
		
		AboutDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("About AAEditor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~AboutDialog();
	
};

#endif //__EditorGuiBase__
