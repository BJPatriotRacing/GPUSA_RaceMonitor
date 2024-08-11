/*

function to let user change settings like track distance and ping times

*/

void SettingsScreen() {
#ifdef SHOW_DEBUG
  Serial.println(F("DEBUG SettingsScreen()_____________________________________"));
#endif
  bool SettingsKeepIn = true;

  DrawSettingsScreen();

  while (SettingsKeepIn) {
    delay(50);

    // if touch screen pressed handle it
    if (Touch.touched()) {

      ProcessTouch();
      RaceMonitorRefresh.press(BtnX, BtnY);
      if (PressIt(DoneBtn) == true) {
        SettingsKeepIn = false;
      }

      if (PressIt(TrackLengthBtn) == true) {
        GetTrackLength.value = TrackLength;
        GetTrackLength.getInput();
        TrackLength = GetTrackLength.value;
        MaxLapTime = (TrackLength * 3600.0f) / (15.0f);

        DrawSettingsScreen();
      }
    }
  }

  RefreshRateID = (uint8_t)RaceMonitorRefresh.option;

  GetRefreshRate();
#ifdef SHOW_DEBUG
  Serial.print(" RefreshRateID: ");
  Serial.println(RefreshRateID);
  Serial.print(" TrackLength: ");
  Serial.println(TrackLength);
#endif
  EEPROM.put(0, RefreshRateID);
  EEPROM.put(1, TrackLength);
  delay(20);
  EEPROM.commit();

#ifdef SHOW_DEBUG
  Serial.println(F("END DEBUG SettingsScreen()_____________________________________"));
  Serial.println();
#endif
}

/*

function to find ping time

*/
void GetRefreshRate() {
#ifdef SHOW_DEBUG
  Serial.println(F("DEBUG GetRefreshRate()_____________________________________"));
#endif
  if (RefreshRateID == 0) {
    RefreshRate = 15;
  } else if (RefreshRateID == 1) {
    RefreshRate = 30;
  } else if (RefreshRateID == 2) {
    RefreshRate = 60;
  }
#ifdef SHOW_DEBUG
  Serial.println(F("END DEBUG GetRefreshRate()_____________________________________"));
  Serial.println();
#endif
}

/*

function to draw settings screen

*/

void DrawSettingsScreen() {
#ifdef SHOW_DEBUG
  Serial.println(F("DEBUG DrawSettingsScreen()_____________________________________"));
#endif
  //nothing fancy, just a header and some buttons
  Display.fillScreen(C_BLACK);
  Display.setFont(&FONT_HEADING);
  Display.setCursor(10, 30);
  Display.fillRect(0, 0, 320, 40, C_LTGREY);
  Display.setTextColor(C_BLACK, C_LTGREY);
  Display.print("Settings");

  Display.fillRect(200, 40, 110, 200, C_BLACK);

  Display.setFont(&FONT_ITEM);
  Display.setCursor(10, 75);
  Display.setTextColor(C_WHITE, C_BLACK);
  Display.print("Server ping time [s]");

  Display.setCursor(10, 178);
  Display.setTextColor(C_WHITE, C_BLACK);
  Display.print("Track length [mi]: ");

  RaceMonitorRefresh.draw(RefreshRateID);
  sprintf(buffer, "%0.3f", TrackLength);
  TrackLengthBtn.setText(buffer);
  TrackLengthBtn.draw();
  DoneBtn.draw();

#ifdef SHOW_DEBUG
  Serial.println(F("END DEBUG DrawSettingsScreen()_____________________________________"));
  Serial.println();
#endif
}

/*

function to get parameters from EEPROM

new chips you will need to force putting data initially

*/

void GetParameters() {

  uint8_t temp = 0;

  EEPROM.get(0, RefreshRateID);
  EEPROM.get(1, TrackLength);
  EEPROM.get(5, SelectedRaceID);

  MaxLapTime = (TrackLength * 3600.0f) / (15.0f);

  GetRefreshRate();

  SSID = "";

  for (i = SSID_START; i < (SSID_START + SSID_LENGTH); i++) {
    EEPROM.get(i, temp);
    if (temp == 255) {
      break;
    }
    SSID = SSID + (char)temp;
  }

  PW = "";

  for (i = PW_START; i < (PW_START + PW_LENGTH); i++) {
    EEPROM.get(i, temp);
    if (temp == 255) {
      break;
    }
    PW = PW + (char)temp;
  }

#ifdef SHOW_DEBUG

  Serial.println(F("DEBUG GetParameters()_____________________________________"));

  Serial.print("RefreshRateID ");
  Serial.println(RefreshRateID);

  Serial.print("RefreshRate ");
  Serial.println(RefreshRate);

  Serial.print("Track Length ");
  Serial.println(TrackLength);

  Serial.print("Selected RaceID ");
  Serial.println(SelectedRaceID);

  Serial.print("getting SSID from EEPROM ");
  Serial.println(SSID);

  Serial.print("getting password from EEPROM ");
  Serial.println(PW);

  Serial.println(F("END DEBUG GetParameters()_____________________________________"));
  Serial.println();

#endif
}

/*

function to draw and let users interact with a main menu

*/
void MainMenu() {
#ifdef SHOW_DEBUG

  Serial.println(F("DEBUG MainMenu()_____________________________________"));
#endif
  // UI to draw screen and capture input
  bool MainMenuKeepIn = true;

  DrawMainMenu();

  while (MainMenuKeepIn) {
    delay(50);
    // if touch screen pressed handle it
    if (Touch.touched()) {

      ProcessTouch();

      if (PressIt(MonitorBtn) == true) {
        MainMenuKeepIn = false;
      }
      if (PressIt(DownloadBtn) == true) {
        DownloadLapTimes();
        DrawMainMenu();
      }
      if (PressIt(InternetConnectBtn) == true) {
        ConnectToInternet();
        DrawMainMenu();
      }
      if (PressIt(SettingsBtn) == true) {
        SettingsScreen();
        DrawMainMenu();
      }

      if (PressIt(SetRaceBtn) == true) {
        SelectRace();
        DrawMainMenu();
      }
    }
  }
#ifdef SHOW_DEBUG
  Serial.println(F("END DEBUG MainMenu()_____________________________________"));
  Serial.println();
#endif
}

/*

function to draw fixed text in main menu

*/
void DrawMainMenu() {
#ifdef SHOW_DEBUG
  Serial.println(F("DrawMainMenu MainMenu()_____________________________________"));
#endif
  //nothing fancy, just a header and some buttons
  Display.fillScreen(C_BLACK);

  Display.setFont(&FONT_HEADING);
  Display.setCursor(10, 30);
  Display.fillRect(0, 0, 320, 40, C_LTGREY);
  Display.setTextColor(C_BLACK, C_LTGREY);
  Display.print("Main Menu");

  MonitorBtn.disable();
  SetRaceBtn.disable();
  DownloadBtn.disable();

  if (WiFi.status() == WL_CONNECTED) {
    SetRaceBtn.enable();
  }
  if ((FoundInternet) && (FoundRace)) {
    MonitorBtn.enable();
  }

  if (RaceMaxLaps > 0) {
    DownloadBtn.enable();
  }
  if (FoundInternet) {
    InternetConnectBtn.setColors(DISABLE_COLOR, C_GREEN, C_BLACK, C_BLACK, C_LTGREY, C_DKGREY);
  } else {
    InternetConnectBtn.setColors(DISABLE_COLOR, C_MDRED, C_BLACK, C_BLACK, C_LTGREY, C_DKGREY);
  }
  if (FoundRace) {
    SetRaceBtn.setColors(DISABLE_COLOR, C_GREEN, C_BLACK, C_BLACK, C_LTGREY, C_DKGREY);
  } else {
    SetRaceBtn.setColors(DISABLE_COLOR, C_MDRED, C_BLACK, C_BLACK, C_LTGREY, C_DKGREY);
  }

  MonitorBtn.draw();
  DownloadBtn.draw();
  InternetConnectBtn.draw();
  SettingsBtn.draw();
  SetRaceBtn.draw();

#ifdef SHOW_DEBUG
  Serial.println(F("END DrawMainMenu MainMenu()_____________________________________"));
  Serial.println();
#endif
}

/*

function to dump lap data from memory to and SD card
oped to do this once after a race as opposed to writing at each lap
mainly due to complexity of knowing when a race is still on (could use filename as race name)
but keeping it simple

*/
void DownloadLapTimes() {

#ifdef SHOW_DEBUG
  Serial.println(F("DownloadLapTimes MainMenu()_____________________________________"));
#endif

  int Cars = 0, Laps = 0;
  int next = 0;
  bool SDCardStatus = false;
  float CarSpeed = 0.0f;

  SdFat SDCARD;
  SdFile SDDataFile;

  SDCardStatus = SDCARD.begin(PIN_SDCS, SD_SCK_MHZ(SD_SPI_SPEED));  //SD

  if (!SDCardStatus) {
    ShowSDStatus(0);
    return;
  }

  FileName[6] = (int)((next / 100) % 10) + '0';
  FileName[7] = (int)((next / 10) % 10) + '0';
  FileName[8] = (int)(next % 10) + '0';

  while (SDCARD.exists(FileName)) {
    next++;

    FileName[6] = (int)((next / 100) % 10) + '0';
    FileName[7] = (int)((next / 10) % 10) + '0';
    FileName[8] = (int)(next % 10) + '0';

    if (next > 999) {
      ShowSDStatus(1);
      break;
    }
  }

  SDCardStatus = SDDataFile.open(FileName, O_WRITE | O_CREAT);

  if (!SDCardStatus) {
    return;
  }
  if (NumberOfCars == 0) {
    ShowSDStatus(3);  // no race data
  }

  if (RaceMaxLaps == 0) {
    ShowSDStatus(4);  // no race data
  }

  // print information
  SDDataFile.print(F("Series Name, "));
  SDDataFile.println(CRDSeriesName);
  SDDataFile.print(F("Race Name, "));
  SDDataFile.println(CRDName);
  SDDataFile.print(F("Track, "));
  SDDataFile.println(CRDTrack);
  SDDataFile.print(F("Race ID, "));
  SDDataFile.println(SelectedRaceID);
  SDDataFile.print(F("Session Name, "));
  SDDataFile.println(RDSessionName);
  SDDataFile.print(F("Track Name, "));
  SDDataFile.println(RDTrackName);
  SDDataFile.print(F("Start Time, "));
  SDDataFile.println(RDStartTime);
  SDDataFile.print(F("Track length [mi], "));
  SDDataFile.println(TrackLength);
  SDDataFile.print(F("Total cars, "));
  SDDataFile.println(NumberOfCars);
  SDDataFile.print(F("Server Pings, "));
  SDDataFile.println(TotalServerPings);

  // print header
  SDDataFile.print("Lap vs. Car Speed [MPH]");
  for (Cars = 0; Cars < NumberOfCars; Cars++) {
    SDDataFile.print(", Car: ");
    SDDataFile.print(CarNumber[Cars]);
  }
  SDDataFile.println();

  // print car number and lap car speed for each car. lap speeds are in MPH
  for (Laps = 0; Laps < RaceMaxLaps; Laps++) {
    SDDataFile.print(Laps);
    for (Cars = 0; Cars < NumberOfCars; Cars++) {
      SDDataFile.print(",");

      CarSpeed = ComputeMPH(CarLapTime[Cars][Laps]);

      // print nothing if speed = 0, that way Excel can ignore the val and connect adjacent points
      // 0 due to missing data from the server
      if (CarSpeed > 0) {
        SDDataFile.print(CarSpeed, 2);
      }
    }

    SDDataFile.println();
  }


  SDDataFile.close();

  ShowSDStatus(2);

#ifdef SHOW_DEBUG
  Serial.println(F("END DownloadLapTimes MainMenu()_____________________________________"));
  Serial.println();
#endif
}

/*

function to show status of SD card

*/
void ShowSDStatus(uint8_t Status) {
  bool KeepIN = true;
  Display.setFont(&FONT_DATA);
  Display.setTextColor(C_BLACK);
  Display.fillRect(52 - 2, 53 - 2, 208 + 4, 119 + 4, C_BLACK);

  if (Status == 0) {
    Display.fillRect(52, 53, 208, 119, C_MDRED);
    Display.setCursor(129, 71);
    Display.print(F("ERROR"));
    Display.setCursor(105, 95);
    Display.print(F("Missing or bad"));
    Display.setCursor(126, 115);
    Display.print(F("SD Card"));
  } else if (Status == 1) {
    Display.fillRect(52, 53, 208, 119, C_MDRED);
    Display.setCursor(129, 71);
    Display.print(F("ERROR"));
    Display.setCursor(115, 95);
    Display.print(F("Full or bad"));
    Display.setCursor(126, 115);
    Display.print(F("SD Card"));
  } else if (Status == 2) {
    Display.fillRect(52, 53, 208, 119, C_DKGREY);
    Display.setTextColor(C_WHITE);
    Display.setCursor(123, 71);
    Display.print(F("SUCCESS"));
    Display.setCursor(96, 95);
    Display.print(F("Data written to:"));
    Display.setCursor(90, 115);
    Display.print(FileName);
  } else if (Status == 3) {
    Display.fillRect(52, 53, 208, 119, C_MDRED);
    Display.setCursor(129, 71);
    Display.print(F("ERROR"));
    Display.setCursor(105, 95);
    Display.print(F("No race data"));
  } else if (Status == 4) {
    Display.fillRect(52, 53, 208, 119, C_MDRED);
    Display.setCursor(129, 71);
    Display.print(F("ERROR"));
    Display.setCursor(112, 95);
    Display.print(F("No lap data"));
  }

  SDStatusBTN.draw();

  while (KeepIN) {
    ProcessTouch();

    if (PressIt(SDStatusBTN) == true) {
      KeepIN = false;
    }
  }
}

/*

service function to handle button presses

*/
bool PressIt(Button TheButton) {

  if (TheButton.press(BtnX, BtnY)) {
    TheButton.draw(B_PRESSED);

    Click();

    while (Touch.touched()) {
      delay(50);
      if (TheButton.press(BtnX, BtnY)) {
        TheButton.draw(B_PRESSED);
      } else {
        TheButton.draw(B_RELEASED);
        return false;
      }
      ProcessTouch();
    }

    TheButton.draw(B_RELEASED);
    return true;
  }
  return false;
}

/*

service function to handle screen presses and convert press location to screen coordinate

*/
void ProcessTouch() {

  BtnX = -1;
  BtnY = -1;

  if (Touch.touched()) {

    TP = Touch.getPoint();
    BtnX = TP.x;
    BtnY = TP.y;
    BtnZ = TP.z;
    /*
#ifdef SHOW_DEBUG
    Serial.print("Touched coordinates : ");
    Serial.print(BtnX);
    Serial.print(", ");
    Serial.println(BtnY);
    Serial.print(", ");
    Serial.println(BtnZ);
#endif
*/
    //yellow headers
    BtnX = map(BtnX, ScreenLeft, ScreenRight, 320, 0);
    BtnY = map(BtnY, ScreenTop, ScreenBottom, 240, 0);

    // black headers
    // BtnX  = map(BtnX, 0, 3905, 320, 0);
    // BtnY  = map(BtnY, 0, 3970, 240, 0);
    // Display.fillCircle(BtnX, BtnY, 2, C_GREEN);
    /*
#ifdef SHOW_DEBUG
    //Serial.print("Mapped coordinates : ");
    //Serial.print(BtnX);
    //Serial.print(", ");
    //Serial.print(BtnY);
    //Serial.print(", ");
    //Serial.println(BtnZ);
#endif
*/
  }
}
/*

service function to init objects like buttons and graphs

*/
void CreateInterface() {

#ifdef SHOW_DEBUG
  Serial.println(F("DEBUG CreateInterface MainMenu()_____________________________________"));
#endif

  // keeps button text from wrapping
  Display.setTextWrap(false);

  MonitorBtn.init(83, 92, 150, 91, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Begin", 0, 0, &FONT_BUTTON);
  DownloadBtn.init(83, 189, 148, 91, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Download", 0, 0, &FONT_BUTTON);
  InternetConnectBtn.init(241, 77, 146, 59, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Internet", 0, 0, &FONT_BUTTON);
  SetRaceBtn.init(241, 141, 146, 59, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Set Race", 0, 0, &FONT_BUTTON);
  SettingsBtn.init(241, 205, 146, 59, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Settings", 0, 0, &FONT_BUTTON);
  DoneBtn.init(260, 20, 100, 30, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Done", 0, 0, &FONT_BUTTON);
  TrackLengthBtn.init(260, 170, 100, 30, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Set", 0, 0, &FONT_BUTTON);
  RefreshBtn.init(70, 220, 120, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Refresh", 0, 0, &FONT_BUTTON);
  UpBtn.init(30, 120, 40, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "<", 0, 0, &FONT_BUTTON);
  DownBtn.init(280, 120, 40, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, ">", 0, 0, &FONT_BUTTON);
  RemoveBtn.init(75, 220, 100, 30, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Remove", 0, 0, &FONT_BUTTON);
  SilenceBtn.init(245, 220, 100, 30, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Silence", 0, 0, &FONT_BUTTON);
  SDStatusBTN.init(157, 145, 67, 30, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "OK", 0, 0, &FONT_BUTTON);
  MenuBtn.init(255, 20, 120, 30, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Menu", 0, 0, &FONT_BUTTON);
  RaceIDBtn.init(70, 170, 120, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Race ID", 0, 0, &FONT_BUTTON);
  PasswordBtn.init(70, 170, 120, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Password", 0, 0, &FONT_BUTTON);
  ConnectBtn.init(220, 170, 160, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Connect", 0, 0, &FONT_BUTTON);

  // option buttons
  RaceMonitorRefresh.init(C_GREY, C_GREEN, C_DKGREY, C_WHITE, C_BLACK, 0, 0, &FONT_BUTTON);

  // and text
  RefreshID0 = RaceMonitorRefresh.add(140, 95, "15", 15);
  RefreshID1 = RaceMonitorRefresh.add(200, 95, "30", 30);
  RefreshID2 = RaceMonitorRefresh.add(260, 95, "60", 60);

  RaceMonitorRefresh.setClickPin(PIN_TONE);

  // keypad copntrols
  KeyboardInput.init(C_DKGREY, C_BLACK, C_LTGREY, C_LTGREY, DISABLE_COLOR, &FONT_BUTTON);
  KeyboardInput.setTouchLimits(ScreenLeft, ScreenRight, ScreenTop, ScreenBottom);
  KeyboardInput.setClickPin(PIN_TONE);
  GetTrackLength.init(C_DKGREY, C_BLACK, C_LTGREY, C_LTGREY, DISABLE_COLOR, &FONT_BUTTON);
  GetTrackLength.setTouchLimits(ScreenLeft, ScreenRight, ScreenTop, ScreenBottom);
  GetTrackLength.enableDecimal(true);
  GetTrackLength.enableNegative(false);
  GetTrackLength.setDecimals(3);
  GetTrackLength.setClickPin(PIN_TONE);

  // initialize the graph object
  Violator.init("Speed issue", "Lap", "Speed", C_WHITE, C_DKGREY, C_WHITE, C_BLACK, C_BLACK, FONT_DATA, FONT_DATA);

  // use the add method to create a plot for each data
  // PlotID = MyGraph.Add(data title, data color);
  //
  SpeedID = Violator.add("Speed", C_YELLOW);

  // optional
  // Violator.setMarkerSize(SpeedID, 4);
  Violator.setLineThickness(SpeedID, 2);
  Violator.showAxisLabels(false);
  Violator.showTitle(false);
  Violator.showLegend(false);

  // main screen

  MonitorBtn.setCornerRadius(C_RADIUS);
  DownloadBtn.setCornerRadius(C_RADIUS);
  InternetConnectBtn.setCornerRadius(C_RADIUS);
  SetRaceBtn.setCornerRadius(C_RADIUS);
  SettingsBtn.setCornerRadius(C_RADIUS);
  DoneBtn.setCornerRadius(C_RADIUS);
  TrackLengthBtn.setCornerRadius(C_RADIUS);
  RefreshBtn.setCornerRadius(C_RADIUS);
  UpBtn.setCornerRadius(C_RADIUS);
  DownBtn.setCornerRadius(C_RADIUS);
  RemoveBtn.setCornerRadius(C_RADIUS);
  SilenceBtn.setCornerRadius(C_RADIUS);
  SDStatusBTN.setCornerRadius(C_RADIUS);
  MenuBtn.setCornerRadius(C_RADIUS);
  RaceIDBtn.setCornerRadius(C_RADIUS);
  ConnectBtn.setCornerRadius(C_RADIUS);
  PasswordBtn.setCornerRadius(C_RADIUS);

  MonitorBtn.setBorderThickness(BORDER_THICKNESS);
  DownloadBtn.setBorderThickness(BORDER_THICKNESS);
  InternetConnectBtn.setBorderThickness(BORDER_THICKNESS);
  SetRaceBtn.setBorderThickness(BORDER_THICKNESS);
  SettingsBtn.setBorderThickness(BORDER_THICKNESS);
  DoneBtn.setBorderThickness(BORDER_THICKNESS);
  TrackLengthBtn.setBorderThickness(BORDER_THICKNESS);
  RefreshBtn.setBorderThickness(BORDER_THICKNESS);
  UpBtn.setBorderThickness(BORDER_THICKNESS);
  DownBtn.setBorderThickness(BORDER_THICKNESS);
  RemoveBtn.setBorderThickness(BORDER_THICKNESS);
  SilenceBtn.setBorderThickness(BORDER_THICKNESS);
  SDStatusBTN.setBorderThickness(BORDER_THICKNESS);
  MenuBtn.setBorderThickness(BORDER_THICKNESS);
  RaceIDBtn.setBorderThickness(BORDER_THICKNESS);
  ConnectBtn.setBorderThickness(BORDER_THICKNESS);
  PasswordBtn.setBorderThickness(BORDER_THICKNESS);

#ifdef SHOW_DEBUG
  Serial.println(F("CreateInterface MainMenu()_____________________________________"));
#endif
}

/*

service function mono icons
used to draw SSID status icons

*/
void drawBitmap(int16_t x, int16_t y, const unsigned char *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t BackColor) {

  int16_t ByteWidth = (w + 7) / 8, i, j;
  uint8_t sByte = 0;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i & 7) {
        sByte <<= 1;
      } else {
        sByte = pgm_read_byte(bitmap + j * ByteWidth + i / 8);
      }
      if (sByte & 0x80) {
        Display.drawPixel(x + i, y + j, color);
      } else {
        Display.drawPixel(x + i, y + j, BackColor);
      }
    }
  }
}

/*

service function to write STRING data to EEPROM
not used

*/
void SaveToEEPROM(uint16_t StartingByte, char *data) {
  EEPROM.writeString(StartingByte, data);
  EEPROM.commit();
}
/*

service function to read STRING data to EEPROM
not used

*/
void ReadEEPROM(uint16_t StartingByte, char *data) {
  String readData = EEPROM.readString(StartingByte);
  readData.toCharArray(data, readData.length() + 1);
}
/*

service function to draw GPUSA logo

*/
void draw565Bitmap(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t w, uint16_t h) {

  uint16_t offset = 0;

  int j, i;

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      Display.drawPixel(j + x, i + y, bitmap[offset]);
      offset++;
    }
  }
}

/*

service function to make click sound when button is pressed

*/

void Click() {

  analogWrite(PIN_TONE, 60);
  delay(5);
  analogWrite(PIN_TONE, 0);
}


/*

debug purposes only, a simple way to run throuhg an array of preset times and cars

*/

void SimulateRace() {
#ifdef SHOW_DEBUG
  Serial.println(F("DEBUG SimulateRace()_____________________________________"));
#endif
  uint8_t i = 0;
  uint8_t TempLap = 0;
  int32_t TotalTime = 0;
  bool bump = false;

  for (i = 0; i < NumberOfCars; i++) {

    Serial.print("OldLap: ");
    Serial.print(OldLap[i]);
    Serial.print(", NewLap: ");
    Serial.print(NewLap[i]);

    TempLap = OldLap[i];
    bump = false;
    OldLap[i] = NewLap[i];

    TotalTime = CarLapTime[i][NewLap[i]];
    Serial.print(", TotalTime: ");
    Serial.print(TotalTime);
    Serial.print(", target: ");
    Serial.print((SimulationIteration - OldLap[i]) * RefreshRate);

    while (TotalTime <= ((SimulationIteration - OldLap[i]) * RefreshRate)) {

      //Serial.print("614 TotalTime: ");
      //Serial.print(TotalTime);
      //Serial.print(", TempLap: ");
      //Serial.println(TempLap);

      TotalTime += CarLapTime[i][TempLap];
      TempLap++;
      bump = true;
    }

    if (bump) {
      NewLap[i] = TempLap + 1;
      bump = false;
    }


    Serial.print(", Car: ");
    Serial.print(CarNumber[i]);
    Serial.print(", MaxLapTime: ");
    Serial.print(MaxLapTime);
    Serial.print(", refresh rate: ");
    Serial.print(RefreshRate);
    Serial.print(", TempLap: ");
    Serial.print(TempLap);
    Serial.print(", TotalTime: ");
    Serial.print(TotalTime);
    Serial.print(", OldLap: ");
    Serial.print(OldLap[i]);
    Serial.print(", NewLap: ");
    Serial.println(NewLap[i]);
  }
  Serial.println();
#ifdef SHOW_DEBUG
  Serial.println(F("END DEBUG SimulateRace()_____________________________________"));
  Serial.println();
#endif
}

/*

DEBUG function to create a race for testing

*/
void CreateTestData() {
#ifdef SHOW_DEBUG
  Serial.println(F("DEBUG CreateTestData MainMenu()_____________________________________"));
#endif
  // car 0
  strcpy(CarNumber[0], "000");
  CarLapTime[0][0] = 200;
  CarLapTime[0][1] = 205;
  CarLapTime[0][2] = 218;
  CarLapTime[0][3] = 215;
  CarLapTime[0][4] = 215;
  CarLapTime[0][5] = 218;
  CarLapTime[0][6] = 220;
  CarLapTime[0][7] = 225;
  CarLapTime[0][8] = 239;
  CarLapTime[0][9] = 240;
  CarLapTime[0][10] = 240;
  CarLapTime[0][11] = 270;
  CarLapTime[0][12] = 280;
  CarLapTime[0][13] = 300;

  // car 1
  strcpy(CarNumber[1], "001");
  CarLapTime[1][0] = 60;
  CarLapTime[1][1] = 60;
  CarLapTime[1][2] = 60;
  CarLapTime[1][3] = 120;
  CarLapTime[1][4] = 120;
  CarLapTime[1][5] = 150;
  CarLapTime[1][6] = 200;
  CarLapTime[1][7] = 250;
  CarLapTime[1][8] = 290;
  CarLapTime[1][9] = 300;
  CarLapTime[1][10] = 300;
  CarLapTime[1][11] = 300;
  CarLapTime[1][12] = 300;

  //CarLap[1] = 13;

  // car 2
  strcpy(CarNumber[2], "002");
  CarLapTime[2][0] = 195;
  CarLapTime[2][1] = 195;
  CarLapTime[2][2] = 230;
  CarLapTime[2][3] = 170;
  CarLapTime[2][4] = 165;
  CarLapTime[2][5] = 205;
  CarLapTime[2][6] = 210;
  CarLapTime[2][7] = 215;
  CarLapTime[2][8] = 220;
  CarLapTime[2][9] = 230;
  CarLapTime[2][10] = 240;
  CarLapTime[2][11] = 280;
  CarLapTime[2][12] = 320;

  //CarLap[2] = 13;

  strcpy(CarNumber[3], "003");
  CarLapTime[3][0] = 20;
  CarLapTime[3][1] = 40;
  CarLapTime[3][2] = 30;
  CarLapTime[3][3] = 40;
  CarLapTime[3][4] = 60;
  CarLapTime[3][5] = 120;
  CarLapTime[3][6] = 211;
  CarLapTime[3][7] = 215;
  CarLapTime[3][8] = 221;
  CarLapTime[3][9] = 233;
  CarLapTime[3][10] = 245;
  CarLapTime[3][11] = 299;
  CarLapTime[3][12] = 332;

  strcpy(CarNumber[4], "004");
  CarLapTime[4][0] = 197;
  CarLapTime[4][1] = 197;
  CarLapTime[4][2] = 234;
  CarLapTime[4][3] = 198;
  CarLapTime[4][4] = 197;
  CarLapTime[4][5] = 207;
  CarLapTime[4][6] = 212;
  CarLapTime[4][7] = 215;
  CarLapTime[4][8] = 215;
  CarLapTime[4][9] = 230;
  CarLapTime[4][10] = 231;
  CarLapTime[4][11] = 232;
  CarLapTime[4][12] = 234;

  RaceMaxLaps = 14;

  NumberOfCars = 5;

#ifdef SHOW_DEBUG
  Serial.println(F("END DEBUG CreateTestData MainMenu()_____________________________________"));
  Serial.println();
#endif
}

/*

End of code

*/