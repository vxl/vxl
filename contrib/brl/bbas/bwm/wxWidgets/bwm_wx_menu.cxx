
  wxMenu* file_menu = new wxMenu;
  file_menu->Append(wxID_EXIT, wxT("E&xit\tAlt-X"), wxT("Quit this program."));

  wxMenu* help_menu = new wxMenu;
  help_menu->Append(wxID_ABOUT, wxT("&About...\tF1"), wxT("Show about dialog."));

  wxMenuBar* menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, wxT("&File"));
  menu_bar->Append(help_menu, wxT("&Help"));
  SetMenuBar(menu_bar);
