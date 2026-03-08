#include <PalmOS.h>
#include "g4p.h"
#include "v4pi.h"

#define EVENT int
EventPtr __evt;
#define StrToInt(x) StrAToI(x)
#define FIELDTYPE_STRING 0
#define FIELDTYPE_INTEGER 1
#define StrToInt(x) StrAToI(x)
enum _BorderStyles { bsNone, bsSingle, bsBold };
typedef struct {
    Char Visible, Border;
    UInt16 Left, Top, Width, Height;
    UInt16* ids;
    UInt32 color;
} PanelType;
typedef struct {
    DmOpenRef bd;
    UInt16 cat;
} PalmDBType;
char StopEvent = 0, EditAutoDraw = 1;
#define G4pF_top 0
#define G4pF_left 0
#define G4pF_width 160
#define G4pF_height 160
register void* reg_a4 asm("%a4");
#define CALLBACK_PROLOGUE \
    void* save_a4 = reg_a4; \
    asm("move.l %%a5,%%a4; sub.l #edata,%%a4" : :);
#define CALLBACK_EPILOGUE reg_a4 = save_a4;

// Definition of Function : SetScreenMode
void SetScreenMode(int bpp) {
    UInt32 requestedDepth = bpp;
    WinScreenMode(winScreenModeSet, NULL, NULL, &requestedDepth, NULL);
}

// Definition of Function : Application_ProcessMessages
Boolean Application_EventHandler(EventPtr event);
void Application_ProcessMessages(int ticks) {
    EventType event;
    UInt32 until = TimGetTicks() + ticks;
    UInt16 error;
    do {
        EvtGetEvent(&event, 3);
        if (event.eType == appStopEvent) {
            StopEvent = 1;
            break;
        }
        if (! SysHandleEvent(&event))
            if (! MenuHandleEvent(0, &event, &error))
                if (! Application_EventHandler(&event))
                    FrmDispatchEvent(&event);
    } while (TimGetTicks() < until);
}

EVENT G4p_OnStart(EventPtr event);
EVENT G4p_OnTerminate(EventPtr event);
FormPtr G4pF;
EVENT G4pF_OnOpen(EventPtr event);
EVENT G4pF_OnPenDownXY(UInt16 X, UInt16 Y);
EVENT G4pF_OnPenUpXY(UInt16 X, UInt16 Y);
EVENT G4pF_OnPenMoveXY(UInt16 X, UInt16 Y);
Err StartApplication(void) {
    G4p_OnStart(NULL);
    FrmGotoForm(_G4pF);
    return 0;
}

void StopApplication(void) {
    if (EditAutoDraw == EditAutoDraw) { }
    G4p_OnTerminate(NULL);
    FrmSaveAllForms();
    FrmCloseAllForms();
}

Boolean G4pF_EventHandler(EventPtr event) {
    Boolean handled = false;
    CALLBACK_PROLOGUE
    __evt = event;
    G4pF = FrmGetFormPtr(_G4pF);
    switch (event->eType) {
        case ctlSelectEvent:
            break;
        case ctlRepeatEvent:
            break;
        case penDownEvent:
            G4pF_OnPenDownXY(event->screenX, event->screenY);
            break;
        case penUpEvent:
            G4pF_OnPenUpXY(event->screenX, event->screenY);
            break;
        case frmUpdateEvent:
            FrmHandleEvent(G4pF, event);
            handled = true;
            break;
        case penMoveEvent:
            {
                RectangleType __bounds;
                G4pF_OnPenMoveXY(event->screenX, event->screenY);
                break;
            }
        case tblSelectEvent:
            break;
        case lstSelectEvent:
            break;
        case popSelectEvent:
            break;
        case fldEnterEvent:
            break;
        case sclRepeatEvent:
            break;
        case sclExitEvent:
            break;
        case frmOpenEvent:
            FrmDrawForm(G4pF);
            G4pF_OnOpen(event);
            handled = true;
            break;
        case frmSaveEvent:
            break;
        case frmCloseEvent:
            break;
        default:
            break;
    }
    CALLBACK_EPILOGUE
    return handled;
}
Boolean Application_EventHandler(EventPtr event) {
    FormPtr frm;
    int formId;
    Boolean handled = false;

    switch (event->eType) {
        case frmLoadEvent:
            {
                formId = event->data.frmLoad.formID;
                frm = FrmInitForm(formId);
                FrmSetActiveForm(frm);

                switch (formId) {
                    case _G4pF:
                        FrmSetEventHandler(frm, G4pF_EventHandler);
                        break;
                }
                handled = true;
            }
        case menuEvent:
            {
                switch (event->data.menu.itemID) { }
                handled = true;
            }
        default:
            break;
    }
    return handled;
}
void EventLoop(void) {
    EventType event;
    UInt16 error;

    do {
        if (! StopEvent)
            EvtGetEvent(&event, evtWaitForever);
        if (! SysHandleEvent(&event))
            if (! MenuHandleEvent(0, &event, &error))
                if (! Application_EventHandler(&event))
                    FrmDispatchEvent(&event);
    } while (event.eType != appStopEvent && ! StopEvent);
}

UInt32 PilotMain(UInt16 launchCode, MemPtr cmdPBP, UInt16 launchFlags) {
    Err err;
    if (launchCode == sysAppLaunchCmdNormalLaunch) {
        if ((err = StartApplication()) == 0) {
            EventLoop();
            StopApplication();
        }
    }
    return 0;
}

void applicativeLoop(UInt16 almProcCmd, SysAlarmTriggeredParamType* paramP) {
    g4p_onInit();
    while (! (StopEvent || g4pOnIterate())) {
        Application_ProcessMessages(5);
    }
    g4p_onQuit();
}
Boolean startApplicativeLoop() {
    AlmSetProcAlarm(applicativeLoop, 0, TimGetSeconds() + 1);
    return success;
}
EVENT G4pF_OnOpen(EventPtr event) {
    startApplicativeLoop();
    return true;
}

EVENT G4p_OnStart(EventPtr event) {
    SetScreenMode(8);
    return false;
}

EVENT G4p_OnTerminate(EventPtr event) {
    WinScreenMode(winScreenModeSetToDefaults, NULL, NULL, NULL, NULL);
    return false;
}
EVENT G4pF_OnPenDownXY(UInt16 X, UInt16 Y) {
    g4p_state->buttons[G4P_PEN] = true;
    g4p_state->xpen = X - 10;
    g4p_state->ypen = Y - 10;
    return false;
}

EVENT G4pF_OnPenUpXY(UInt16 X, UInt16 Y) {
    g4p_state->buttons[G4P_PEN] = false;
    return false;
}

EVENT G4pF_OnPenMoveXY(UInt16 X, UInt16 Y) {
    g4p_state->xpen = X - 10;
    g4p_state->ypen = Y - 10;
    return false;
}
