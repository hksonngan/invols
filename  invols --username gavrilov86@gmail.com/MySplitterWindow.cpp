
#include "MySplitterWindow.h"

BEGIN_EVENT_TABLE(MySplitterWindow, wxSplitterWindow)
    EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, MySplitterWindow::OnPositionChanged)
    EVT_SPLITTER_SASH_POS_CHANGING(wxID_ANY, MySplitterWindow::OnPositionChanging)

    EVT_SPLITTER_DCLICK(wxID_ANY, MySplitterWindow::OnDClick)

    EVT_SPLITTER_UNSPLIT(wxID_ANY, MySplitterWindow::OnUnsplitEvent)
END_EVENT_TABLE()

MySplitterWindow::MySplitterWindow(wxFrame *parent)
                : wxSplitterWindow(parent, wxID_ANY,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxSP_3D | wxSP_LIVE_UPDATE |
                                   wxCLIP_CHILDREN /* | wxSP_NO_XP_THEME */ )
{
    m_frame = parent;
}

void MySplitterWindow::OnPositionChanged(wxSplitterEvent& event)
{

    event.Skip();
}

void MySplitterWindow::OnPositionChanging(wxSplitterEvent& event)
{

    event.Skip();
}

void MySplitterWindow::OnDClick(wxSplitterEvent& event)
{
    event.Skip();
}

void MySplitterWindow::OnUnsplitEvent(wxSplitterEvent& event)
{

    event.Skip();
}