
#ifndef SPLITTER_WINDOW_H
#define SPLITTER_WINDOW_H

#include "wxIncludes.h"

class MySplitterWindow : public wxSplitterWindow
{
public:
    MySplitterWindow(wxFrame *parent);

    // event handlers
    void OnPositionChanged(wxSplitterEvent& event);
    void OnPositionChanging(wxSplitterEvent& event);
    void OnDClick(wxSplitterEvent& event);
    void OnUnsplitEvent(wxSplitterEvent& event);

private:
    wxFrame *m_frame;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(MySplitterWindow)
};

#endif