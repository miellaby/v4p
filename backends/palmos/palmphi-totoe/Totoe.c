#include <PalmOS.h>
#include <C:\cygwin\usr\src\v4p\src\palmphi-totoe\Totoe.h>
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
#define TotoF_top 0
#define TotoF_left 0
#define TotoF_width 160
#define TotoF_height 160
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

EVENT TOTO_OnStart(EventPtr event);
EVENT TOTO_OnTerminate(EventPtr event);
FormPtr TotoF;
EVENT TotoF_OnOpen(EventPtr event);
EVENT TotoF_OnPenDownXY(UInt16 X, UInt16 Y);
EVENT TotoF_OnPenUpXY(UInt16 X, UInt16 Y);
EVENT TotoF_OnPenMoveXY(UInt16 X, UInt16 Y);
Err StartApplication(void) {
    TOTO_OnStart(NULL);
    FrmGotoForm(_TotoF);
    return 0;
}

void StopApplication(void) {
    if (EditAutoDraw == EditAutoDraw) { }
    TOTO_OnTerminate(NULL);
    FrmSaveAllForms();
    FrmCloseAllForms();
}

Boolean TotoF_EventHandler(EventPtr event) {
    Boolean handled = false;
    CALLBACK_PROLOGUE
    __evt = event;
    TotoF = FrmGetFormPtr(_TotoF);
    switch (event->eType) {
        case ctlSelectEvent:
            break;
        case ctlRepeatEvent:
            break;
        case penDownEvent:
            {
                TotoF_OnPenDownXY(event->screenX, event->screenY);
                break;
            }
        case penUpEvent:
            {
                TotoF_OnPenUpXY(event->screenX, event->screenY);
                break;
            }
        case frmUpdateEvent:
            {
                FrmHandleEvent(TotoF, event);
                handled = true;
                break;
            }
        case penMoveEvent:
            {
                RectangleType __bounds;
                TotoF_OnPenMoveXY(event->screenX, event->screenY);
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
            {
            }
            FrmDrawForm(TotoF);
            TotoF_OnOpen(event);
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
                    case _TotoF:
                        FrmSetEventHandler(frm, TotoF_EventHandler);
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

Boolean pen = false;
#line 2 "TotoF.c"
V4pCoord xpen, ypen;
#line 3 "TotoF.c"
extern Boolean initApp();
#line 4 "TotoF.c"
extern Boolean iterateApp();
#line 5 "TotoF.c"

#line 6 "TotoF.c"

#line 7 "TotoF.c"
static void applicativeLoop(UInt16 almProcCmd, SysAlarmTriggeredParamType* paramP)
#line 8 "TotoF.c"
{
#line 9 "TotoF.c"
    initApp();
#line 10 "TotoF.c"
    while (! StopEvent)
#line 11 "TotoF.c"
    {
#line 12 "TotoF.c"
        iterateApp();
#line 13 "TotoF.c"
        Application_ProcessMessages(5);
#line 14 "TotoF.c"
    }
#line 15 "TotoF.c"
}
#line 16 "TotoF.c"

#line 17 "TotoF.c"
Boolean startApplicativeLoop() {
#line 18 "TotoF.c"
    AlmSetProcAlarm(applicativeLoop, 0, TimGetSeconds() + 1);
#line 19 "TotoF.c"

#line 20 "TotoF.c"
    return success;
#line 21 "TotoF.c"
}
#line 22 "TotoF.c"

#line 23 "TotoF.c"
EVENT TotoF_OnOpen(EventPtr event)
#line 24 "TotoF.c"
{
#line 25 "TotoF.c"
    startApplicativeLoop();
#line 26 "TotoF.c"
    return true;
#line 27 "TotoF.c"
}
#line 28 "TotoF.c"

#line 29 "TotoF.c"
EVENT TOTO_OnStart(EventPtr event)
#line 30 "TotoF.c"
{
#line 31 "TotoF.c"
    SetScreenMode(8);
#line 32 "TotoF.c"
    return false;
#line 33 "TotoF.c"
}
#line 34 "TotoF.c"

#line 35 "TotoF.c"
EVENT TOTO_OnTerminate(EventPtr event)
#line 36 "TotoF.c"
{
#line 37 "TotoF.c"
    WinScreenMode(winScreenModeSetToDefaults, NULL, NULL, NULL, NULL);
#line 38 "TotoF.c"
    return false;
#line 39 "TotoF.c"
}
#line 40 "TotoF.c"

#line 41 "TotoF.c"
EVENT TotoF_OnPenDownXY(UInt16 X, UInt16 Y)
#line 42 "TotoF.c"
{
#line 43 "TotoF.c"

#line 44 "TotoF.c"
    pen = true;
#line 45 "TotoF.c"
    xpen = X - 10;
#line 46 "TotoF.c"
    ypen = Y - 10;
#line 47 "TotoF.c"
    return false;
#line 48 "TotoF.c"
}
#line 49 "TotoF.c"

#line 50 "TotoF.c"
EVENT TotoF_OnPenUpXY(UInt16 X, UInt16 Y)
#line 51 "TotoF.c"
{
#line 52 "TotoF.c"

#line 53 "TotoF.c"
    pen = false;
#line 54 "TotoF.c"
    return false;
#line 55 "TotoF.c"
}
#line 56 "TotoF.c"

#line 57 "TotoF.c"
EVENT TotoF_OnPenMoveXY(UInt16 X, UInt16 Y)
#line 58 "TotoF.c"
{
#line 59 "TotoF.c"

#line 60 "TotoF.c"
    xpen = X - 10;
#line 61 "TotoF.c"
    ypen = Y - 10;
#line 62 "TotoF.c"
    return false;
#line 63 "TotoF.c"
}
#include "v4pi.h"
#line 2 "editor.c"

#line 3 "editor.c"

#line 4 "editor.c"
extern Boolean pen;
#line 5 "editor.c"
extern V4pCoord xpen, ypen;
#line 6 "editor.c"

#line 7 "editor.c"
int i, j, k, dist, mindist;
#line 8 "editor.c"

#line 9 "editor.c"
V4pLayer z;
#line 10 "editor.c"

#line 11 "editor.c"
V4pPolygonP scene;
#line 12 "editor.c"

#line 13 "editor.c"
V4pCoord x, y, xvu, yvu, lvu,
#line 14 "editor.c"
    xpen1, ypen1, yButton = 1,
#line 15 "editor.c"
                  x0, y0, x1, y1, stepGrid;
#line 16 "editor.c"

#line 17 "editor.c"
V4pPolygonP spots[64];
#line 18 "editor.c"

#line 19 "editor.c"
Boolean pen1;
#line 20 "editor.c"

#line 21 "editor.c"
int iButton = 0, sel;
#line 22 "editor.c"
int bAddition, bDel, bLayer,
#line 23 "editor.c"
    bScroll, bSave, bCol, bGrid;
#line 24 "editor.c"

#line 25 "editor.c"
V4pPolygonP
#line 26 "editor.c"
    pSel,
#line 27 "editor.c"
    pCol, pSelCol,
#line 28 "editor.c"
    pGrid, pSelGrid,
#line 29 "editor.c"
    pLayer, pSelLayer,
#line 30 "editor.c"
    currentPolygon, brush, focus;
#line 31 "editor.c"

#line 32 "editor.c"
V4pPointP currentPoint, s;
#line 33 "editor.c"

#line 34 "editor.c"
V4pLayer currentZ;
#line 35 "editor.c"

#line 36 "editor.c"
V4pColor currentColor, nextColor;
#line 37 "editor.c"

#line 38 "editor.c"
V4pPolygonP buttons[16];
#line 39 "editor.c"

#line 40 "editor.c"
int addButton(V4pColor col) {
#line 41 "editor.c"
    V4pPolygonP button = v4pListAddPolygon(&scene, V4P_RELATIVE, col, 14);
#line 42 "editor.c"
    buttons[iButton] = button;
#line 43 "editor.c"
    v4p_rect(button, lineWidth - 10, yButton, lineWidth - 1, 9 + yButton);
#line 44 "editor.c"
    yButton += 10;
#line 45 "editor.c"
    return iButton++;
#line 46 "editor.c"
}
#line 47 "editor.c"

#line 48 "editor.c"
void ajusteSel(int s) {
#line 49 "editor.c"
    if (sel != s) {
#line 50 "editor.c"
        v4p_transform(pSel, 0, (s - sel) * 10, 0, 0);
#line 51 "editor.c"
        sel = s;
#line 52 "editor.c"
    }
#line 53 "editor.c"
}
#line 54 "editor.c"

#line 55 "editor.c"
typedef enum { idle, push, edit } GuiStatus;
#line 56 "editor.c"

#line 57 "editor.c"
int spotNb;
#line 58 "editor.c"
GuiStatus guiStatus;
#line 59 "editor.c"

#line 60 "editor.c"

#line 61 "editor.c"

#line 62 "editor.c"
Boolean initApp() {
#line 63 "editor.c"
    v4pFlavorInit(green);
#line 64 "editor.c"
    scene = NULL;
#line 65 "editor.c"
    v4p_setScene(&scene);
#line 66 "editor.c"
    xvu = -lineWidth / 2;
#line 67 "editor.c"
    yvu = -lineNb / 2;
#line 68 "editor.c"
    lvu = lineWidth;
#line 69 "editor.c"
    v4p_setView(xvu, yvu, xvu + lvu, yvu + lvu);
#line 70 "editor.c"
    spotNb = 0;
#line 71 "editor.c"
    guiStatus = idle;
#line 72 "editor.c"
    brush = NULL;
#line 73 "editor.c"
    focus = NULL;
#line 74 "editor.c"
    stepGrid = 8;
#line 75 "editor.c"
    bAddition = addButton(V4P_RED);
#line 76 "editor.c"
    bDel = addButton(gray);
#line 77 "editor.c"
    bLayer = addButton(V4P_BLUE);
#line 78 "editor.c"
    bScroll = addButton(yellow);
#line 79 "editor.c"
    bCol = addButton(V4P_BLACK);
#line 80 "editor.c"
    bGrid = addButton(cyan);
#line 81 "editor.c"

#line 82 "editor.c"
    sel = 0;
#line 83 "editor.c"
    currentColor = V4P_BLACK;
#line 84 "editor.c"
    currentZ = 7;
#line 85 "editor.c"
    currentPolygon = NULL;
#line 86 "editor.c"

#line 87 "editor.c"
    pSel = v4pListAddPolygon(&scene, V4P_RELATIVE, V4P_BLACK, 13);
#line 88 "editor.c"
    v4p_rect(pSel, lineWidth - 11, 0, lineWidth, 11);
#line 89 "editor.c"

#line 90 "editor.c"
    pCol = v4pListAddPolygon(&scene, V4P_RELATIVE, V4P_BLACK, 14);
#line 91 "editor.c"
    v4p_rect(pCol, -xvu - 20, -yvu - 20, -xvu + 20, -yvu + 20);
#line 92 "editor.c"
    pSelCol = v4p_addNewSub(pCol, V4P_RELATIVE, V4P_BLACK, 15);
#line 93 "editor.c"
    v4p_rect(pSelCol, -xvu - 18, -yvu - 18, -xvu + 18, -yvu + 18);
#line 94 "editor.c"
    v4p_disable(pCol);
#line 95 "editor.c"

#line 96 "editor.c"
    pLayer = v4pListAddPolygon(&scene, V4P_RELATIVE, V4P_BLACK, 14);
#line 97 "editor.c"
    v4p_rect(pLayer, -xvu - 3, -yvu - 17, -xvu + 3, -yvu + 17);
#line 98 "editor.c"
    pSelLayer = v4p_addNewSub(pLayer, V4P_RELATIVE, V4P_RED, 15);
#line 99 "editor.c"
    v4p_rect(pSelLayer, -xvu - 2, -yvu - 1, -xvu + 2, -yvu + 1);
#line 100 "editor.c"
    v4p_disable(pLayer);
#line 101 "editor.c"

#line 102 "editor.c"
    pGrid = v4pListAddPolygon(&scene, V4P_RELATIVE, V4P_BLACK, 14);
#line 103 "editor.c"
    v4p_rect(pGrid, -xvu - 9, -yvu - 9, -xvu + 9, -yvu + 9);
#line 104 "editor.c"
    pSelGrid = v4p_addNewSub(pGrid, V4P_RELATIVE, V4P_RED, 15);
#line 105 "editor.c"
    v4p_rect(pSelGrid, -xvu - 2, -yvu - 2, -xvu + 2, -yvu + 2);
#line 106 "editor.c"
    v4p_disable(pGrid);
#line 107 "editor.c"

#line 108 "editor.c"
    return success;
#line 109 "editor.c"
}
#line 110 "editor.c"

#line 111 "editor.c"
V4pCoord align(V4pCoord x) {
#line 112 "editor.c"
    if (stepGrid <= 1)
#line 113 "editor.c"
        return x;
#line 114 "editor.c"
    else if (x > 0)
#line 115 "editor.c"
        return ((x + stepGrid / 2) / stepGrid) * stepGrid;
#line 116 "editor.c"
    else
#line 117 "editor.c"
        return ((x - stepGrid / 2) / stepGrid) * stepGrid;
#line 118 "editor.c"
}
#line 119 "editor.c"

#line 120 "editor.c"
Boolean iterateApp() {
#line 121 "editor.c"
    V4pCoord stepGrid0, stepGridPrec,
#line 122 "editor.c"
        xs, ys,
#line 123 "editor.c"
        xi, yi,
#line 124 "editor.c"
        t1, t2;
#line 125 "editor.c"
    V4pLayer z0, precZ;
#line 126 "editor.c"
    int selPrec;
#line 127 "editor.c"

#line 128 "editor.c"
    if (true) {
#line 129 "editor.c"

#line 130 "editor.c"
        v4p_render();
#line 131 "editor.c"
        if (pen) {
#line 132 "editor.c"
            if (pen1) {
#line 133 "editor.c"
                xpen = (2 * xpen + xpen1) / 3;
#line 134 "editor.c"
                ypen = (2 * ypen + ypen1) / 3;
#line 135 "editor.c"
            }
#line 136 "editor.c"
            xpen1 = xpen;
#line 137 "editor.c"
            ypen1 = ypen;
#line 138 "editor.c"

#line 139 "editor.c"
            v4p_viewToAbsolute(xpen, ypen, &x, &y);
#line 140 "editor.c"
            xs = align(x);
#line 141 "editor.c"
            ys = align(y);
#line 142 "editor.c"

#line 143 "editor.c"
            if (guiStatus == push) {
#line 144 "editor.c"
                if (sel == bGrid) {
#line 145 "editor.c"
                    stepGridPrec = stepGrid;
#line 146 "editor.c"
                    stepGrid = 1 << ((iabs(ypen - y0) / 4) % 4);
#line 147 "editor.c"
                    if (stepGrid != stepGridPrec)
#line 148 "editor.c"
                        v4p_transform(pSelGrid,
                                      stepGrid - stepGridPrec,
                                      stepGrid - stepGridPrec,
                                      0,
                                      0);
#line 149 "editor.c"
                } else if (sel == bCol) {
#line 150 "editor.c"
                    nextColor = (((iabs(ypen - y0) + iabs(xpen - x0))) + currentColor) % 255;
#line 151 "editor.c"
                    v4p_setColor(pSelCol, nextColor);
#line 152 "editor.c"
                } else if (sel == bLayer) {
#line 153 "editor.c"
                    precZ = currentZ;
#line 154 "editor.c"
                    currentZ = (z0 + (iabs(ypen - y0) / 4)) % 15;
#line 155 "editor.c"
                    if (precZ != currentZ)
                        v4p_transform(pSelLayer, 0, (precZ - currentZ) * 2, 0, 0);
#line 156 "editor.c"
                }
#line 157 "editor.c"
            } else if (guiStatus == edit) {
#line 158 "editor.c"
                if (brush) {
#line 159 "editor.c"
                    if (sel == bScroll) {
#line 160 "editor.c"
                        v4pListDelPolygon(&scene, brush);
#line 161 "editor.c"
                        brush = NULL;
#line 162 "editor.c"
                    } else {
#line 163 "editor.c"
                        v4p_transform(brush, xpen - x0, ypen - y0, 0, 0);
#line 164 "editor.c"
                        x0 = xpen;
#line 165 "editor.c"
                        y0 = ypen;
#line 166 "editor.c"
                    }
#line 167 "editor.c"
                }
#line 168 "editor.c"
                if (sel == bAddition) {
#line 169 "editor.c"
                    if (currentPoint) {
#line 170 "editor.c"
                        if (spotNb < 64)
#line 171 "editor.c"

#line 172 "editor.c"

#line 173 "editor.c"

#line 174 "editor.c"
                            v4p_transform(spots[spotNb],
                                          xs - currentPoint->x,
                                          ys - currentPoint->y,
                                          0,
                                          0);
#line 175 "editor.c"
                        v4p_movePoint(currentPolygon, currentPoint, xs, ys);
#line 176 "editor.c"
                    }
#line 177 "editor.c"
                } else if (sel == bGrid && currentPoint) {
#line 178 "editor.c"
                    v4p_movePoint(focus, currentPoint, xs, ys);
#line 179 "editor.c"

#line 180 "editor.c"
                } else if (g4p_collides[2].q > 0) {
#line 181 "editor.c"
                    if (sel == bScroll) {
#line 182 "editor.c"
                        focus = g4p_collides[2].poly;
#line 183 "editor.c"
                        x0 = xs;
#line 184 "editor.c"
                        y0 = ys;
#line 185 "editor.c"
                    } else if (sel == bGrid) {
#line 186 "editor.c"
                        if (! focus) {
#line 187 "editor.c"
                            focus = g4p_collides[2].poly;
#line 188 "editor.c"
                            s = v4p_getPoints(focus);
#line 189 "editor.c"
                            mindist = gaugeDist(s->x - x, s->y - y);
#line 190 "editor.c"
                            currentPoint = s;
#line 191 "editor.c"
                            s = s->next;
#line 192 "editor.c"
                            while (s) {
#line 193 "editor.c"
                                dist = gaugeDist(s->x - x, s->y - y);
#line 194 "editor.c"
                                if (dist < mindist) {
#line 195 "editor.c"
                                    mindist = dist;
#line 196 "editor.c"
                                    currentPoint = s;
#line 197 "editor.c"
                                }
#line 198 "editor.c"
                                s = s->next;
#line 199 "editor.c"
                            }
#line 200 "editor.c"
                        }
#line 201 "editor.c"
                    } else
#line 202 "editor.c"
                        focus = g4p_collides[2].poly;
#line 203 "editor.c"
                } else if (sel == bScroll) {
#line 204 "editor.c"
                    if (focus) {
#line 205 "editor.c"
                        v4p_transform(focus, xs - x0, ys - y0, 0, 0);
#line 206 "editor.c"
                        x0 = xs;
                        y0 = ys;
#line 207 "editor.c"
                    } else {
#line 208 "editor.c"

#line 209 "editor.c"

#line 210 "editor.c"

#line 211 "editor.c"

#line 212 "editor.c"
                        v4p_setView(align(xvu + xpen - x0),
                                    align(yvu + ypen - y0),
                                    align(xvu + xpen - x0) + lvu,
                                    align(yvu + ypen - y0) + lvu);
#line 213 "editor.c"
                    }
#line 214 "editor.c"
                }
#line 215 "editor.c"
            } else {
#line 216 "editor.c"
                if (xpen > lineWidth - 10 && ypen < yButton) {
#line 217 "editor.c"
                    selPrec = sel;
#line 218 "editor.c"
                    ajusteSel(ypen / 10);
#line 219 "editor.c"
                    if (selPrec == bAddition) {
#line 220 "editor.c"
                        if (currentPolygon && spotNb <= 2)
#line 221 "editor.c"
                            v4pListDelPolygon(&scene, currentPolygon);
#line 222 "editor.c"

#line 223 "editor.c"
                        while (spotNb) {
#line 224 "editor.c"
                            spotNb--;
#line 225 "editor.c"
                            if (spotNb < 64)
                                v4pListDelPolygon(&scene, spots[spotNb]);
#line 226 "editor.c"
                        }
#line 227 "editor.c"
                    }
#line 228 "editor.c"
                    spotNb = 0;
#line 229 "editor.c"
                    focus = NULL;
#line 230 "editor.c"
                    currentPolygon = NULL;
#line 231 "editor.c"
                    currentPoint = NULL;
#line 232 "editor.c"
                    x0 = xpen;
#line 233 "editor.c"
                    y0 = ypen;
#line 234 "editor.c"
                    if (sel == bCol) {
#line 235 "editor.c"
                        v4p_enable(pCol);
#line 236 "editor.c"
                    } else if (sel == bScroll) {
#line 237 "editor.c"
                    } else if (sel == bLayer) {
#line 238 "editor.c"
                        v4p_enable(pLayer);
#line 239 "editor.c"
                        z0 = currentZ;
#line 240 "editor.c"
                    } else if (sel == bGrid) {
#line 241 "editor.c"
                        v4p_enable(pGrid);
#line 242 "editor.c"
                        stepGrid0 = stepGrid;
#line 243 "editor.c"
                        y0 = 4 * (ypen - floorLog2(stepGrid));
#line 244 "editor.c"
                    }
#line 245 "editor.c"
                    guiStatus = push;
#line 246 "editor.c"
                } else {
#line 247 "editor.c"
                    if (sel == bAddition) {
#line 248 "editor.c"
                        if (spotNb == 0) {
#line 249 "editor.c"
                            currentPolygon
                                = v4pListAddPolygon(&scene, V4P_STANDARD, currentColor, currentZ);
#line 250 "editor.c"
                            v4p_concrete(currentPolygon, 0);
#line 251 "editor.c"
                        }
#line 252 "editor.c"
                        currentPoint = v4p_addPoint(currentPolygon, xs, ys);
#line 253 "editor.c"
                        if (spotNb < 64) {
#line 254 "editor.c"
                            spots[spotNb] = v4pListAddPolygon(&scene, V4P_STANDARD, currentColor, 14);
#line 255 "editor.c"
                            v4p_rect(spots[spotNb], xs - 1, ys - 1, xs + 1, ys + 1);
#line 256 "editor.c"
                        }
#line 257 "editor.c"
                    }
#line 258 "editor.c"
                    brush = v4pListAddPolygon(&scene, V4P_RELATIVE, V4P_BLACK, 15);
#line 259 "editor.c"
                    v4p_rect(brush, xpen - 1, ypen - 1, xpen + 1, ypen + 1);
#line 260 "editor.c"
                    v4p_concrete(brush, 2);
#line 261 "editor.c"
                    x0 = xpen;
#line 262 "editor.c"
                    y0 = ypen;
#line 263 "editor.c"
                    guiStatus = edit;
#line 264 "editor.c"
                }
#line 265 "editor.c"
            }
#line 266 "editor.c"
        } else {
#line 267 "editor.c"
            if (guiStatus == push) {
#line 268 "editor.c"
                v4p_disable(pCol);
#line 269 "editor.c"
                if (sel == bCol)
                    v4p_setColor(buttons[bCol], (currentColor = nextColor));
#line 270 "editor.c"
                if (sel == bLayer)
                    v4p_disable(pLayer);
#line 271 "editor.c"
                if (sel == bGrid)
                    v4p_disable(pGrid);
#line 272 "editor.c"
            } else if (guiStatus == edit) {
#line 273 "editor.c"
                if (sel == bAddition) {
#line 274 "editor.c"
                    spotNb++;
#line 275 "editor.c"
                } else if (focus) {
#line 276 "editor.c"
                    if (sel == bCol)
                        v4p_setColor(focus, currentColor);
#line 277 "editor.c"
                    if (sel == bDel)
                        v4pListDelPolygon(&scene, focus);
#line 278 "editor.c"
                    if (sel == bLayer)
                        v4p_transform(focus, 0, 0, 0, currentZ - v4p_getZ(focus));
#line 279 "editor.c"
                    focus = NULL;
#line 280 "editor.c"
                    currentPoint = NULL;
#line 281 "editor.c"
                } else if (sel == bScroll && ! focus) {
#line 282 "editor.c"
                    xvu = align(xvu + (xpen - x0));
#line 283 "editor.c"
                    yvu = align(yvu + (ypen - y0));
#line 284 "editor.c"
                    v4p_setView(xvu, yvu, xvu + lvu, yvu + lvu);
#line 285 "editor.c"
                }
#line 286 "editor.c"
                if (brush) {
#line 287 "editor.c"
                    v4pListDelPolygon(&scene, brush);
#line 288 "editor.c"
                    brush = NULL;
#line 289 "editor.c"
                }
#line 290 "editor.c"
            }
#line 291 "editor.c"
            guiStatus = idle;
#line 292 "editor.c"
        }
#line 293 "editor.c"
        pen1 = pen;
#line 294 "editor.c"
    }
#line 295 "editor.c"
    return success;
#line 296 "editor.c"
}
#line 297 "editor.c"

#line 298 "editor.c"
