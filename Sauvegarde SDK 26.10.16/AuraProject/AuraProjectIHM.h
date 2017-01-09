#ifndef _BEBOP_PILOTING_IHM_H_
#define _BEBOP_PILOTING_IHM_H_

#include <curses.h>
#include <libARSAL/ARSAL.h>
#include <libARDataTransfer/ARDataTransfer.h>

typedef enum
{
    IHM_INPUT_EVENT_NONE,
    IHM_INPUT_EVENT_EXIT,
    IHM_INPUT_EVENT_EMERGENCY,
    IHM_INPUT_EVENT_TAKEOFF,
    IHM_INPUT_EVENT_LAND,
    IHM_INPUT_EVENT_UP,
    IHM_INPUT_EVENT_DOWN,
    IHM_INPUT_EVENT_RIGHT,
    IHM_INPUT_EVENT_LEFT,
    IHM_INPUT_EVENT_FORWARD,
    IHM_INPUT_EVENT_BACK,
    IHM_INPUT_EVENT_ROLL_LEFT,
    IHM_INPUT_EVENT_ROLL_RIGHT,
    IHM_INPUT_EVENT_TAKE_PICTURE,
    IHM_INPUT_EVENT_DEBUGTRANSFERT,
    IHM_INPUT_EVENT_SETTING,
    IHM_INPUT_EVENT_CALIBRATION,
    IHM_INPUT_EVENT_TESTAUTO,
    IHM_INPUT_EVENT_CERCLE,
    IHM_INPUT_EVENT_CADRILLAGE,
    IHM_INPUT_EVENT_RESET,
}eIHM_INPUT_EVENT;

typedef void (*IHM_onInputEvent_t) (eIHM_INPUT_EVENT event, void *customData);

typedef struct
{
    WINDOW *mainWindow;
    ARSAL_Thread_t inputThread;
    int run;
    IHM_onInputEvent_t onInputEventCallback;
    void *customData;
}IHM_t;

IHM_t *IHM_New (IHM_onInputEvent_t onInputEventCallback);
void IHM_Delete (IHM_t **ihm);

void IHM_setCustomData(IHM_t *ihm, void *customData);

void IHM_PrintHeader(IHM_t *ihm, char *headerStr);
void IHM_PrintInfo(IHM_t *ihm, char *infoStr);
void IHM_PrintTest(IHM_t *ihm, char *infoTest);
void IHM_PrintBattery(IHM_t *ihm, uint8_t percent);
void IHM_PrintPosition (IHM_t *ihm, double latitude, double longitude, double altitude);
void IHM_PrintVitesse(IHM_t *ihm, float speedX, float speedY, float speedZ);
void IHM_PrintAttitude(IHM_t *ihm, int roll, int pitch, int yaw);
void IHM_PrintDownload(IHM_t *ihm, float percent, ARDATATRANSFER_Media_t *media);

#endif /* _BEBOP_PILOTING_IHM_H_ */