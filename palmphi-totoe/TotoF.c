Boolean pen = false;
Coord xpen, ypen;
extern Boolean initApp();
extern Boolean iterateApp();

static void applicativeLoop(UInt16 almProcCmd, SysAlarmTriggeredParamType* paramP) {
    initApp();
    while (! System.StopEvent()) {
        iterateApp();
        TApplication.NoBlockDelay(5);
    }
}

Boolean startApplicativeLoop() {
    AlmSetProcAlarm(applicativeLoop, 0, TimGetSeconds() + 1);

    return success;
}

EVENT TotoF_OnOpen(EventPtr event) {
    startApplicativeLoop();
    return true;
}

EVENT TOTO_OnStart(EventPtr event) {
    Screen.bpp = 8;
    return false;
}

EVENT TOTO_OnTerminate(EventPtr event) {
    Screen.RestoreScreenMode();
    return false;
}

EVENT TotoF_OnPenDownXY(UInt16 X, UInt16 Y) {
    // Return "true" to override default message handling
    pen = true;
    xpen = X - 10;
    ypen = Y - 10;
    return false;
}

EVENT TotoF_OnPenUpXY(UInt16 X, UInt16 Y) {
    // Return "true" to override default message handling
    pen = false;
    return false;
}

EVENT TotoF_OnPenMoveXY(UInt16 X, UInt16 Y) {
    // Return "true" to override default message handling
    xpen = X - 10;
    ypen = Y - 10;
    return false;
}