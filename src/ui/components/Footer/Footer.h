#pragma once

class Footer {
public:
  Footer(bool show_mode, bool show_help_hint);

  bool show_mode;
  bool show_help_hint;

  void show();
};
