/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009

	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@gmail.com>
	Lars H <lars_hn@hotmail.com>
	Rob Dunning <rob@raspberryridgesheepfarm.com>
	Russell King <rmk@arm.linux.org.uk>
	Paolo Ventafridda <coolwind@email.it>
	Tobias Lohner <tobias@lohner-net.de>
	Mirek Jezek <mjezek@ipplc.cz>
	Max Kellermann <max@duempel.org>
	Tobias Bieniek <tobias.bieniek@gmx.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Dialogs/Internal.hpp"
#include "Units.hpp"
#include "InputEvents.h"
#include "Screen/Layout.hpp"
#include "WayPointParser.h"
#include "Math/FastMath.h"
#include "DataField/Enum.hpp"
#include "MainWindow.hpp"
#include "Compatibility/string.h"
#include "Components.hpp"
#include "WayPoint.hpp"
#if defined(__BORLANDC__)  // due to compiler bug
  #include "RasterTerrain.h"
#endif

static WndForm *wf=NULL;
static WAYPOINT *global_wpt=NULL;

static WndButton *buttonName = NULL;
static WndButton *buttonComment = NULL;

static void UpdateButtons(void) {
  TCHAR text[MAX_PATH];
  if (buttonName) {
    if (_tcslen(global_wpt->Name)<=0) {
      _stprintf(text,_T("%s: %s"), gettext(_T("Name")),
                gettext(_T("(blank)")));
    } else {
      _stprintf(text,_T("%s: %s"), gettext(_T("Name")),
                global_wpt->Name);
    }
    buttonName->SetCaption(text);
  }
  if (buttonComment) {
    if (_tcslen(global_wpt->Comment)<=0) {
      _stprintf(text,_T("%s: %s"), gettext(_T("Comment")),
                gettext(_T("(blank)")));
    } else {
      _stprintf(text,_T("%s: %s"), gettext(_T("Comment")),
                global_wpt->Comment);
    }
    buttonComment->SetCaption(text);
  }
}


static void OnNameClicked(WindowControl *Sender) {
	(void)Sender;
  if (buttonName) {
    dlgTextEntryShowModal(global_wpt->Name, NAME_SIZE);
  }
  UpdateButtons();
}


static void OnCommentClicked(WindowControl *Sender) {
	(void)Sender;
  if (buttonComment) {
    dlgTextEntryShowModal(global_wpt->Comment, COMMENT_SIZE);
  }
  UpdateButtons();
}

//
//

static void SetUnits(void) {
  WndProperty* wp;
  switch (Units::CoordinateFormat) {
  case 0: // ("DDMMSS");
  case 1: // ("DDMMSS.ss");
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeDDDD"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeDDDD"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudemmm"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudemmm"));
    if (wp) {
      wp->SetVisible(false);
    }
    break;
  case 2: // ("DDMM.mmm");
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeDDDD"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeDDDD"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeS"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeS"));
    if (wp) {
      wp->SetVisible(false);
    }
    break;
  case 3: // ("DD.dddd");
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeM"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeM"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeS"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeS"));
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudemmm"));
    // hide this field for DD.dddd format
    if (wp) {
      wp->SetVisible(false);
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudemmm"));
    if (wp) {
      wp->SetVisible(false);
    }
    break;
  }
}

static void SetValues(void) {
  WndProperty* wp;
  bool sign;
  int dd,mm,ss;

  Units::LongitudeToDMS(global_wpt->Location.Longitude,
			&dd, &mm, &ss, &sign);

  wp = (WndProperty*)wf->FindByName(_T("prpLongitudeSign"));
  if (wp) {
    DataFieldEnum* dfe;
    dfe = (DataFieldEnum*)wp->GetDataField();
    dfe->addEnumText((_T("W")));
    dfe->addEnumText((_T("E")));
    dfe->Set(sign);
    wp->RefreshDisplay();
  }
  wp = (WndProperty*)wf->FindByName(_T("prpLongitudeD"));
  if (wp) {
    wp->GetDataField()->SetAsFloat(dd);
    wp->RefreshDisplay();
  }

  switch (Units::CoordinateFormat) {
  case 0: // ("DDMMSS");
  case 1: // ("DDMMSS.ss");
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeM"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(mm);
      wp->RefreshDisplay();
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeS"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(ss);
      wp->RefreshDisplay();
    }
    break;
  case 2: // ("DDMM.mmm");
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeM"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(mm);
      wp->RefreshDisplay();
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudemmm"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(1000.0*ss/60.0);
      wp->RefreshDisplay();
    }
    break;
  case 3: // ("DD.dddd");
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeDDDD"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(10000.0*(mm+ss/60.0)/60.0);
      wp->RefreshDisplay();
    }
    break;
  }

  Units::LatitudeToDMS(global_wpt->Location.Latitude,
		       &dd, &mm, &ss, &sign);

  wp = (WndProperty*)wf->FindByName(_T("prpLatitudeSign"));
  if (wp) {
    DataFieldEnum* dfe;
    dfe = (DataFieldEnum*)wp->GetDataField();
    dfe->addEnumText((_T("S")));
    dfe->addEnumText((_T("N")));
    dfe->Set(sign);
    wp->RefreshDisplay();
  }
  wp = (WndProperty*)wf->FindByName(_T("prpLatitudeD"));
  if (wp) {
    wp->GetDataField()->SetAsFloat(dd);
    wp->RefreshDisplay();
  }

  switch (Units::CoordinateFormat) {
  case 0: // ("DDMMSS");
  case 1: // ("DDMMSS.ss");
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeM"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(mm);
      wp->RefreshDisplay();
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeS"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(ss);
      wp->RefreshDisplay();
    }
    break;
  case 2: // ("DDMM.mmm");
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeM"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(mm);
      wp->RefreshDisplay();
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudemmm"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(1000.0*ss/60.0);
      wp->RefreshDisplay();
    }
    break;
  case 3: // ("DD.dddd");
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeDDDD"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(10000.0*(mm+ss/60.0)/60.0);
      wp->RefreshDisplay();
    }
    break;
  }

  wp = (WndProperty*)wf->FindByName(_T("prpAltitude"));
  if (wp) {
    wp->GetDataField()->SetAsFloat(
				   iround(global_wpt->Altitude
					  *ALTITUDEMODIFY));
    wp->GetDataField()->SetUnits(Units::GetAltitudeName());
    wp->RefreshDisplay();
  }

  wp = (WndProperty*)wf->FindByName(_T("prpFlags"));
  if (wp) {
    DataFieldEnum* dfe;
    dfe = (DataFieldEnum*)wp->GetDataField();
    dfe->addEnumText(_T("Turnpoint"));
    dfe->addEnumText(_T("Airport"));
    dfe->addEnumText(_T("Landpoint"));
    dfe->Set(0);
    if ((global_wpt->Flags & LANDPOINT)==LANDPOINT) {
      dfe->Set(2);
    }
    if ((global_wpt->Flags & AIRPORT)==AIRPORT) {
      dfe->Set(1);
    }

    wp->RefreshDisplay();
  }
}


static void GetValues(void) {
  WndProperty* wp;
  bool sign = false;
  int dd = 0;
  double num=0, mm = 0, ss = 0; // mm,ss are numerators (division) so don't want to lose decimals

  wp = (WndProperty*)wf->FindByName(_T("prpLongitudeSign"));
  if (wp) {
    sign = (wp->GetDataField()->GetAsInteger()==1);
  }
  wp = (WndProperty*)wf->FindByName(_T("prpLongitudeD"));
  if (wp) {
    dd = wp->GetDataField()->GetAsInteger();
  }

  switch (Units::CoordinateFormat) {
  case 0: // ("DDMMSS");
  case 1: // ("DDMMSS.ss");
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeM"));
    if (wp) {
      mm = wp->GetDataField()->GetAsInteger();
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeS"));
    if (wp) {
      ss = wp->GetDataField()->GetAsInteger();
    }
    num = dd+mm/60.0+ss/3600.0;
    break;
  case 2: // ("DDMM.mmm");
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeM"));
    if (wp) {
      mm = wp->GetDataField()->GetAsInteger();
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudemmm"));
    if (wp) {
      ss = wp->GetDataField()->GetAsInteger();
    }
    num = dd+(mm+ss/1000.0)/60.0;
    break;
  case 3: // ("DD.dddd");
    wp = (WndProperty*)wf->FindByName(_T("prpLongitudeDDDD"));
    if (wp) {
      mm = wp->GetDataField()->GetAsInteger();
    }
    num = dd+mm/10000;
    break;
  }
  if (!sign) {
    num = -num;
  }

  global_wpt->Location.Longitude = num;

  wp = (WndProperty*)wf->FindByName(_T("prpLatitudeSign"));
  if (wp) {
    sign = (wp->GetDataField()->GetAsInteger()==1);
  }
  wp = (WndProperty*)wf->FindByName(_T("prpLatitudeD"));
  if (wp) {
    dd = wp->GetDataField()->GetAsInteger();
  }

  switch (Units::CoordinateFormat) {
  case 0: // ("DDMMSS");
  case 1: // ("DDMMSS.ss");
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeM"));
    if (wp) {
      mm = wp->GetDataField()->GetAsInteger();
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeS"));
    if (wp) {
      ss = wp->GetDataField()->GetAsInteger();
    }
    num = dd+mm/60.0+ss/3600.0;
    break;
  case 2: // ("DDMM.mmm");
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeM"));
    if (wp) {
      mm = wp->GetDataField()->GetAsInteger();
    }
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudemmm"));
    if (wp) {
      ss = wp->GetDataField()->GetAsInteger();
    }
    num = dd+(mm+ss/1000.0)/60.0;
    break;
  case 3: // ("DD.dddd");
    wp = (WndProperty*)wf->FindByName(_T("prpLatitudeDDDD"));
    if (wp) {
      mm = wp->GetDataField()->GetAsInteger();
    }
    num = dd+mm/10000;
    break;
  }
  if (!sign) {
    num = -num;
  }

  global_wpt->Location.Latitude = num;

  wp = (WndProperty*)wf->FindByName(_T("prpAltitude"));
  if (wp) {
    ss = wp->GetDataField()->GetAsInteger();
    if (ss==0) {
      WaypointAltitudeFromTerrain(*global_wpt, terrain);
    } else {
      global_wpt->Altitude = ss/ALTITUDEMODIFY;
    }
  }

  wp = (WndProperty*)wf->FindByName(_T("prpFlags"));
  if (wp) {
    int myflag = wp->GetDataField()->GetAsInteger();
    switch(myflag) {
    case 0:
      global_wpt->Flags = TURNPOINT;
      break;
    case 1:
      global_wpt->Flags = AIRPORT | TURNPOINT;
      break;
    case 2:
      global_wpt->Flags = LANDPOINT;
      break;
    default:
      global_wpt->Flags = 0;
    };
  }
}


static void OnCloseClicked(WindowControl * Sender){
	(void)Sender;
  wf->SetModalResult(mrOK);
}


static CallBackTableEntry_t CallBackTable[]={
  DeclareCallBackEntry(OnCloseClicked),
  DeclareCallBackEntry(NULL)
};

void
dlgWaypointEditShowModal(WAYPOINT &way_point)
{
  global_wpt = &way_point;

  if (!Layout::landscape) {
    wf = dlgLoadFromXML(CallBackTable,
                        _T("dlgWaypointEdit_L.xml"),
                        XCSoarInterface::main_window,
                        _T("IDR_XML_WAYPOINTEDIT_L"));
  } else {
    wf = dlgLoadFromXML(CallBackTable,
                        _T("dlgWaypointEdit.xml"),
                        XCSoarInterface::main_window,
                        _T("IDR_XML_WAYPOINTEDIT"));
  }

  if (wf == NULL)
    return;

  buttonName = ((WndButton *)wf->FindByName(_T("cmdName")));
  if (buttonName) {
    buttonName->SetOnClickNotify(OnNameClicked);
  }

  buttonComment = ((WndButton *)wf->FindByName(_T("cmdComment")));
  if (buttonComment) {
    buttonComment->SetOnClickNotify(OnCommentClicked);
  }

  UpdateButtons();

  SetUnits();

  SetValues();

  wf->SetModalResult(mrCancel);

  if (wf->ShowModal()==mrOK) {
    GetValues();
  }

  delete wf;
}
