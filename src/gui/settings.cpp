/*  BOSS

    A plugin load order optimiser for games that use the esp/esm plugin system.

    Copyright (C) 2013    WrinklyNinja

    This file is part of BOSS.

    BOSS is free software: you can redistribute
    it and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    BOSS is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BOSS.  If not, see
    <http://www.gnu.org/licenses/>.
*/

#include "settings.h"
#include <fstream>

BEGIN_EVENT_TABLE ( SettingsFrame, wxFrame )
	EVT_BUTTON ( OPTION_OKExitSettings, SettingsFrame::OnQuit)
	EVT_BUTTON ( OPTION_CancelExitSettings, SettingsFrame::OnQuit)
END_EVENT_TABLE()

using namespace std;

SettingsFrame::SettingsFrame(const wxString title, wxFrame *parent, YAML::Node& settings) : wxFrame(parent, wxID_ANY, title), _settings(settings) {

	wxString DebugVerbosity[] = {
        translate("None"),
        translate("Low"),
		translate("Medium"),
		translate("High")
    };

	wxString Game[] = {
		translate("Autodetect"),
		wxT("Oblivion"),
		wxT("Nehrim"),
		wxT("Skyrim"),
		wxT("Fallout 3"),
		wxT("Fallout: New Vegas"),
	};

	wxString Language[] = {
		wxT("English"),
	/*	wxString("Español", wxConvUTF8),
		wxT("Deutsch"),
		wxString("Русский", wxConvUTF8),
		wxString("简体中文", wxConvUTF8)*/
	};

	//Set up stuff in the frame.
	SetBackgroundColour(wxColour(255,255,255));

	wxSizerFlags leftItem(1);
	leftItem.Border(wxALL, 10).Expand().Left();

	wxSizerFlags rightItem(0);
	rightItem.Border(wxALL, 10).Expand().Right();

	wxSizerFlags wholeItem(1);
	wholeItem.Border(wxALL, 10).Expand();

    wxBoxSizer *bigBox = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer *GridSizer = new wxFlexGridSizer(2, 5, 5);

	GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Default Game:")), leftItem);
	GridSizer->Add(GameChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 6, Game), rightItem);
    
	GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Language:")), leftItem);
	GridSizer->Add(LanguageChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, Language), rightItem);
	
    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Debug Verbosity:")), leftItem);
	GridSizer->Add(DebugVerbosityChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, DebugVerbosity), rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Oblivion Masterlist URL:")), leftItem);
	GridSizer->Add(OblivionURL = new wxTextCtrl(this, wxID_ANY), rightItem);
	
    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Nehrim Masterlist URL:")), leftItem);
	GridSizer->Add(NehrimURL = new wxTextCtrl(this, wxID_ANY), rightItem);
	
    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Skyrim Masterlist URL:")), leftItem);
	GridSizer->Add(SkyrimURL = new wxTextCtrl(this, wxID_ANY), rightItem);
	
    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Fallout 3 Masterlist URL:")), leftItem);
	GridSizer->Add(FO3URL = new wxTextCtrl(this, wxID_ANY), rightItem);
	
    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Fallout: New Vegas Masterlist URL:")), leftItem);
	GridSizer->Add(FONVURL = new wxTextCtrl(this, wxID_ANY), rightItem);
    
	bigBox->Add(GridSizer);
   
    bigBox->Add(UpdateMasterlistBox = new wxCheckBox(this, wxID_ANY, translate("Update masterlist before sorting.")), wholeItem);
	
	wxString text = translate("Language settings will be applied after the BOSS GUI is restarted.");
	bigBox->Add(new wxStaticText(this, wxID_ANY, text), wholeItem);
	
	//Need to add 'OK' and 'Cancel' buttons.
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	hbox->Add(new wxButton(this, OPTION_OKExitSettings, translate("OK"), wxDefaultPosition, wxSize(70, 30)));
	hbox->Add(new wxButton(this, OPTION_CancelExitSettings, translate("Cancel"), wxDefaultPosition, wxSize(70, 30)), 0, wxLEFT, 20);

	//Now add TabHolder and OK button to window sizer.
	bigBox->Add(hbox, 0, wxCENTER|wxALL, 10);

	//Initialise options with values. For checkboxes, they are off by default.
	SetDefaultValues();

	//Tooltips.
	DebugVerbosityChoice->SetToolTip(translate("The output is logged to the BOSSDebugLog.txt file"));

	//Now set the layout and sizes.
	SetSizerAndFit(bigBox);
}

void SettingsFrame::SetDefaultValues() {

    if (_settings["Language"]) {
        string lang = _settings["Language"].as<string>();
        if (lang == "eng")
            LanguageChoice->SetSelection(0);
    }

    if (_settings["Game"]) {
        string game = _settings["Game"].as<string>();
        if (game == "auto")
            GameChoice->SetSelection(0);
        else if (game == "oblivion")
            GameChoice->SetSelection(1);
        else if (game == "nehrim")
            GameChoice->SetSelection(2);
        else if (game == "skyrim")
            GameChoice->SetSelection(3);
        else if (game == "fallout3")
            GameChoice->SetSelection(4);
        else if (game == "falloutnv")
            GameChoice->SetSelection(5);
    }

    if (_settings["Debug Verbosity"]) {
        unsigned int verbosity = _settings["Debug Verbosity"].as<unsigned int>();
        DebugVerbosityChoice->SetSelection(verbosity);
    }

    if (_settings["Update Masterlist"]) {
        bool update = _settings["Update Masterlist"].as<bool>();
        UpdateMasterlistBox->SetValue(update);
    }

    if (_settings["Masterlist URLs"]) {
        YAML::Node urls = _settings["Masterlist URLs"];

        if (urls["Oblivion"])
            OblivionURL->SetValue(urls["Oblivion"].as<string>());

        if (urls["Nehrim"])
            NehrimURL->SetValue(urls["Nehrim"].as<string>());

        if (urls["Skyrim"])
            SkyrimURL->SetValue(urls["Skyrim"].as<string>());

        if (urls["Fallout 3"])
            FO3URL->SetValue(urls["Fallout 3"].as<string>());

        if (urls["Fallout New Vegas"])
            FONVURL->SetValue(urls["Fallout New Vegas"].as<string>());
    }
}

void SettingsFrame::OnQuit(wxCommandEvent& event) {
    if (event.GetId() == OPTION_OKExitSettings) {

        switch (GameChoice->GetSelection()) {
        case 0:
            _settings["Game"] = "auto";
            break;
        case 1:
            _settings["Game"] = "oblivion";
            break;
        case 2:
            _settings["Game"] = "nehrim";
            break;
        case 3:
            _settings["Game"] = "skyrim";
            break;
        case 4:
            _settings["Game"] = "fallout3";
            break;
        case 5:
            _settings["Game"] = "falloutnv";
            break;
        }
        
        switch (LanguageChoice->GetSelection()) {
        case 0:
            _settings["Language"] = "eng";
            break;
        }

        _settings["Debug Verbosity"] = DebugVerbosityChoice->GetSelection();

        _settings["Update Masterlist"] = UpdateMasterlistBox->IsChecked();

        _settings["Masterlist URLs"]["Oblivion"] = string(OblivionURL->GetValue().ToUTF8());

        _settings["Masterlist URLs"]["Nehrim"] = string(NehrimURL->GetValue().ToUTF8());

        _settings["Masterlist URLs"]["Skyrim"] = string(SkyrimURL->GetValue().ToUTF8());

        _settings["Masterlist URLs"]["Fallout 3"] = string(FO3URL->GetValue().ToUTF8());

        _settings["Masterlist URLs"]["Fallout New Vegas"] = string(FONVURL->GetValue().ToUTF8());

        //Also set the logger settings now.
    /*	g_logger.setOriginTracking(gl_debug_with_source);
        g_logger.setVerbosity(static_cast<LogVerbosity>(LV_WARN + gl_debug_verbosity));
        if (gl_log_debug_output)
            g_logger.setStream(debug_log_path.string().c_str());*/

        YAML::Emitter yout;
        yout.SetIndent(2);
        yout << _settings;
        
        ofstream out(settings_path);
        out << yout.c_str();
        out.close();
    }

	this->Close();
}
