/*****************************************
 *
 *             include file :
 *
 *****************************************/
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <libARSAL/ARSAL.h>
#include <libARController/ARController.h>
#include <libARDiscovery/ARDiscovery.h>

#include "AuraProject.h"
#include "AuraProjectIHM.h"

/*****************************************
 *
 *             define :
 *
 *****************************************/
#define TAG "AuraBebopDrone"

#define ERROR_STR_LENGTH 2048

#define BEBOP_IP_ADDRESS "192.168.42.1"
#define BEBOP_DISCOVERY_PORT 44444

#define DISPLAY_WITH_MPLAYER 1

#define IHM

#define DEVICE_PORT     21
#define MEDIA_FOLDER    "internal_000"

/*****************************************
*
*	VARIABLE JORDI & LUCAS
*
*****************************************/
ARSAL_Thread_t threadRetreiveAllMedias;   // the thread that will do the media retrieving
ARSAL_Thread_t threadGetThumbnails;       // the thread that will download the thumbnails
ARSAL_Thread_t threadMediasDownloader;    // the thread that will download medias
ARDATATRANSFER_Manager_t *manager;        // the data transfer manager
ARUTILS_Manager_t *ftpListManager;        // an ftp that will do the list
ARUTILS_Manager_t *ftpQueueManager;       // an ftp that will do the download

int gIHMRun = 1;
char gErrorStr[ERROR_STR_LENGTH];
IHM_t *ihm = NULL;

FILE *videoOut = NULL;
int frameNb = 0;
ARSAL_Sem_t stateSem;
// int isBebop2 = 0;

// Find CSV File
FILE *PhotosInfosFile = NULL;
char carac;


/*****************************************
 *
 *      Declaration variable globale :
 *
 *****************************************/

uint8_t percent = 0;

double latitude = 0;
double longitude = 0;
double altitude = 0;

float speedX = 0;
float speedY = 0;
float speedZ = 0;

float roll = 0;
float pitch = 0;
float yaw = 0;

int AU = 0;

time_t t1, t2;

/*****************************************
 *
 *             		main:
 *
 ****************************************/
int main (int argc, char *argv[])
{
	// local declarations
    int failed = 0;
    ARDISCOVERY_Device_t *device = NULL;
    ARCONTROLLER_Device_t *deviceController = NULL;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    eARCONTROLLER_DEVICE_STATE deviceState = ARCONTROLLER_DEVICE_STATE_MAX;
    pid_t child = 0;
    ARSAL_Sem_Init (&(stateSem), 0, 0);

	// Enumeration for format of the photo
	typedef enum typePhotoFormat typePhotoFormat;
	enum typePhotoFormat
	{
	raw, jpeg, snapshot, jpeg_fisheye
	};

	// Enable/Disable video autorecord
	uint8_t videoAutoRecord = 0; 

	// Format of the photo
	typePhotoFormat photosFormat = jpeg;

	// PhotoInfosFile initialisation   
	if ( fopen("./PhotosInfosFile.csv", "r") != NULL)
   	{
   		PhotosInfosFile = fopen("./PhotosInfosFile.csv", "a");
	}
	else
	{
		PhotosInfosFile = fopen("./PhotosInfosFile.csv", "w+");
		fprintf(PhotosInfosFile,"#F=N Y X Z K W P\n");
	   	fprintf(PhotosInfosFile,"#image,latitude,longitude,altitude,yaw,pitch,roll\n");
	}

	/*
    if (!failed)
    {
        if (DISPLAY_WITH_MPLAYER)
        {
            // fork the process to launch mplayer
            if ((child = fork()) == 0)
            {
                execlp("xterm", "xterm", "-e", "mplayer", "-demuxer",  "h264es", "video_fifo_AuraProject.h264", "-benchmark", "-really-quiet", NULL);
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Missing mplayer, you will not see the video. Please install mplayer and xterm.");
                return -1;
            }
        }
        
        if (DISPLAY_WITH_MPLAYER)
        {
            videoOut = fopen("./video_fifo_AuraProject.h264", "w");
        }
    }
    */
    
#ifdef IHM
    ihm = IHM_New (&onInputEvent);
    if (ihm != NULL)
    {
        gErrorStr[0] = '\0';
        ARSAL_Print_SetCallback (customPrintCallback); //use a custom callback to print, for not disturb ncurses IHM
        IHM_PrintHeader (ihm, "-- Bebop Aura Piloting --");
    }
    else
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, "Creation of IHM failed.");
        failed = 1;
    }
#endif
    
    // create a discovery device
    if (!failed)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- init discovey device ... ");
        eARDISCOVERY_ERROR errorDiscovery = ARDISCOVERY_OK;
        
        device = ARDISCOVERY_Device_New (&errorDiscovery);
        
        if (errorDiscovery == ARDISCOVERY_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_InitWifi ...");
            errorDiscovery = ARDISCOVERY_Device_InitWifi (device, ARDISCOVERY_PRODUCT_ARDRONE, "bebop", BEBOP_IP_ADDRESS, BEBOP_DISCOVERY_PORT);
            
            if (errorDiscovery != ARDISCOVERY_OK)
            {
                failed = 1;
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :%s", ARDISCOVERY_Error_ToString(errorDiscovery));
            }
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :%s", ARDISCOVERY_Error_ToString(errorDiscovery));
            failed = 1;
        }
    }
    
    // create a device controller
    if (!failed)
    {
        deviceController = ARCONTROLLER_Device_New (device, &error);
        
        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, "Creation of deviceController failed.");
            failed = 1;
        }
        else
        {
            IHM_setCustomData(ihm, deviceController);
        }
    }
    
    if (!failed)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- delete discovey device ... ");
        ARDISCOVERY_Device_Delete (&device);
    }
    
    // add the state change callback to be informed when the device controller starts, stops...
    if (!failed)
    {
        error = ARCONTROLLER_Device_AddStateChangedCallback (deviceController, stateChanged, deviceController);
        
        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, "add State callback failed.");
            failed = 1;
        }
    }
    
    // add the command received callback to be informed when a command has been received from the device
    if (!failed)
    {
        error = ARCONTROLLER_Device_AddCommandReceivedCallback (deviceController, commandReceived, deviceController);
        
        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, "add callback failed.");
            failed = 1;
        }
    }
    
    /*
    // add the frame received callback to be informed when a streaming frame has been received from the device
    if (!failed)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- set Video callback ... ");
        error = ARCONTROLLER_Device_SetVideoStreamCallbacks (deviceController, decoderConfigCallback, didReceiveFrameCallback, NULL , NULL);
        
        if (error != ARCONTROLLER_OK)
        {
            failed = 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }
    */
    
    if (!failed)
    {
        IHM_PrintInfo(ihm, "Connecting ...");
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "Connecting ...");
        error = ARCONTROLLER_Device_Start (deviceController);
        
        if (error != ARCONTROLLER_OK)
        {
            failed = 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }
    
    if (!failed)
    {
        // wait state update update 
        ARSAL_Sem_Wait (&(stateSem));
        
        deviceState = ARCONTROLLER_Device_GetState (deviceController, &error);
        
        if ((error != ARCONTROLLER_OK) || (deviceState != ARCONTROLLER_DEVICE_STATE_RUNNING))
        {
            failed = 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- deviceState :%d", deviceState);
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }

    /*
    // send the command that tells to the Bebop to begin its streaming
    if (!failed)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- send StreamingVideoEnable ... ");
        error = deviceController->aRDrone3->sendMediaStreamingVideoEnable (deviceController->aRDrone3, 1);
        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
            failed = 1;
        }
    }
    */
    
    if (!failed)
    {
        IHM_PrintInfo(ihm, "Running ...\n\n'a' to takeoff\nSpacebar to land\nArrow keys and ('z','q','s','d') to move\n'm' for emergency\n'y' to reset the drone\n'u' to make setting\n'i' to make calibration\n'o' to debug the transfert\n'p' to take a picture\n'w' for cercle\n'x' for cadrillage\n'c' for test auto\n'n' to stop the task\n'esc' to quit");
	

        #ifdef IHM
        while (gIHMRun)
        {
            usleep(50);
        }
        #else
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- sleep 20 ... ");
        sleep(20);
        #endif
    }

    
	if (!failed)	
    {
	deviceController->aRDrone3->sendPictureSettingsPictureFormatSelection(deviceController->aRDrone3, (eARCOMMANDS_ARDRONE3_PICTURESETTINGS_PICTUREFORMATSELECTION_TYPE)photosFormat);
    }

    if (!failed)	
    {
	deviceController->aRDrone3->sendPictureSettingsVideoAutorecordSelection(deviceController->aRDrone3, videoAutoRecord, 0);
    }
    

    
#ifdef IHM
    IHM_Delete (&ihm);
#endif
    
    // we are here because of a disconnection or user has quit IHM, so safely delete everything
    if (deviceController != NULL)
    {
        deviceState = ARCONTROLLER_Device_GetState (deviceController, &error);

        if ((error == ARCONTROLLER_OK) && (deviceState != ARCONTROLLER_DEVICE_STATE_STOPPED))
        {
            IHM_PrintInfo(ihm, "Disconnecting ...");
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "Disconnecting ...");
            
            error = ARCONTROLLER_Device_Stop (deviceController);
            
            if (error == ARCONTROLLER_OK)
            {
                // wait state update update 
                ARSAL_Sem_Wait (&(stateSem));
            }
        }
        
        IHM_PrintInfo(ihm, "");
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "ARCONTROLLER_Device_Delete ...");
        ARCONTROLLER_Device_Delete (&deviceController);
        
        if (DISPLAY_WITH_MPLAYER)
        {
            fflush (videoOut);
            fclose (videoOut);
            
            if (child > 0)
            {
                kill(child, SIGKILL);
            }
        }
    }
    
    ARSAL_Sem_Destroy (&(stateSem));
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "-- END --");
    
    return EXIT_SUCCESS;
}

/*****************************************
 *
 *             private implementation:
 *
 ****************************************/

// called when the state of the device controller has changed
void stateChanged (eARCONTROLLER_DEVICE_STATE newState, eARCONTROLLER_ERROR error, void *customData)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - stateChanged newState: %d .....", newState);
    
    switch (newState)
    {
        case ARCONTROLLER_DEVICE_STATE_STOPPED:
            ARSAL_Sem_Post (&(stateSem));
            //stop
            gIHMRun = 0;
            
            break;
        
        case ARCONTROLLER_DEVICE_STATE_RUNNING:
            ARSAL_Sem_Post (&(stateSem));
            break;
            
        default:
            break;
    }
}

// called when a command has been received from the drone
void commandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)
{
    if ((commandKey == ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED) && (elementDictionary != NULL))
    {
        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
        
        if (element != NULL)
        {
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED_PERCENT, arg);
            if (arg != NULL)
            {
                percent = arg->value.U8;
            }
        }
        batteryStateChanged (percent);
    }

    if ((commandKey == ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED) && (elementDictionary != NULL))
    {
        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
        
        if (element != NULL)
        {
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED_LATITUDE, arg);
            if (arg != NULL)
            {
                latitude = arg->value.Double;
            }
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED_LONGITUDE, arg);
            if (arg != NULL)
            {
                longitude = arg->value.Double;
            }
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED_ALTITUDE, arg);
            if (arg != NULL)
            {
                altitude = arg->value.Double;
            }
        }
        positionChanged (latitude, longitude, altitude);
    }

    if ((commandKey == ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_SPEEDCHANGED) && (elementDictionary != NULL))
    {
        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);
        
        if (element != NULL)
        {
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_SPEEDCHANGED_SPEEDX, arg);
            if (arg != NULL)
            {
                speedX = arg->value.Float;
            }
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_SPEEDCHANGED_SPEEDY, arg);
            if (arg != NULL)
            {
                speedY = arg->value.Float;
            }
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_SPEEDCHANGED_SPEEDZ, arg);
            if (arg != NULL)
            {
                speedZ = arg->value.Float;
            }
        }
        speedChanged(speedX, speedY, speedZ);
    }

    if ((commandKey == ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ATTITUDECHANGED) && (elementDictionary != NULL))
    {
        ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
        ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
        HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);

        if (element != NULL)
        {
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ATTITUDECHANGED_ROLL, arg);
            if (arg != NULL)
            {
                roll = arg->value.Float * 180.0 / 3.141592653589793;
            }
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ATTITUDECHANGED_PITCH, arg);
            if (arg != NULL)
            {
                pitch = arg->value.Float * 180.0 / 3.141592653589793;
            }
            HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ATTITUDECHANGED_YAW, arg);
            if (arg != NULL)
            {
                yaw = arg->value.Float * 180.0/ 3.141592653589793;
            }
        }
        attitudeChanged(roll, pitch, yaw);
    }

	if ((commandKey == ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIARECORDEVENT_PICTUREEVENTCHANGED) && (elementDictionary != NULL))
	{
		ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
		ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
		HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);

		if (element != NULL)
		{
			HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIARECORDEVENT_PICTUREEVENTCHANGED_EVENT, arg);
			
			if (arg != NULL)
			{
				eARCOMMANDS_ARDRONE3_MEDIARECORDEVENT_PICTUREEVENTCHANGED_EVENT event = arg->value.I32;
				if (event == ARCOMMANDS_ARDRONE3_MEDIARECORDEVENT_PICTUREEVENTCHANGED_EVENT_TAKEN)
				{
					newPicture();
				}
			}
			
			HASH_FIND_STR (element->arguments, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIARECORDEVENT_PICTUREEVENTCHANGED_ERROR, arg);
			
			if (arg != NULL)
			{
			eARCOMMANDS_ARDRONE3_MEDIARECORDEVENT_PICTUREEVENTCHANGED_ERROR error = arg->value.I32;
			}
		}
	}
}

void batteryStateChanged (uint8_t percent)
{
    if (ihm != NULL)
    {
        IHM_PrintBattery (ihm, percent);
    }
}

void positionChanged (double latitude, double longitude, double altitude)
{
    if (ihm != NULL)
    {
        IHM_PrintPosition (ihm, latitude, longitude, altitude);
    }   
}

void speedChanged (float speedX, float speedY, float speedZ)
{
    if (ihm != NULL)
    {
        IHM_PrintVitesse (ihm, speedX, speedY, speedX);
    }   
}

void attitudeChanged (int roll, int pitch, int yaw)
{
    if (ihm != NULL)
    {
        IHM_PrintAttitude (ihm, roll, pitch, yaw);
    }   
}

void newPicture()
{
	fprintf(PhotosInfosFile,"                                             , %lf,%lf, %lf, %f, %f, %f\n", latitude, longitude, altitude, yaw, pitch, roll);
}

/*
eARCONTROLLER_ERROR decoderConfigCallback (ARCONTROLLER_Stream_Codec_t codec, void *customData)
{
    if (videoOut != NULL)
    {
        if (codec.type == ARCONTROLLER_STREAM_CODEC_TYPE_H264)
        {
            if (DISPLAY_WITH_MPLAYER)
            {
                fwrite(codec.parameters.h264parameters.spsBuffer, codec.parameters.h264parameters.spsSize, 1, videoOut);
                fwrite(codec.parameters.h264parameters.ppsBuffer, codec.parameters.h264parameters.ppsSize, 1, videoOut);
                
                fflush (videoOut);
            }
        }
            
    }
    else
    {
        ARSAL_PRINT(ARSAL_PRINT_WARNING, TAG, "videoOut is NULL.");
    }
    
    return ARCONTROLLER_OK;
}


eARCONTROLLER_ERROR didReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData)
{
    if (videoOut != NULL)
    {
        if (frame != NULL)
        {
            if (DISPLAY_WITH_MPLAYER)
            {                
                fwrite(frame->data, frame->used, 1, videoOut);
                
                fflush (videoOut);
            }
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_WARNING, TAG, "frame is NULL.");
        }
    }
    else
    {
        ARSAL_PRINT(ARSAL_PRINT_WARNING, TAG, "videoOut is NULL.");
    }
    
    return ARCONTROLLER_OK;
}
*/

// IHM callbacks: 

void onInputEvent (eIHM_INPUT_EVENT event, void *customData)
{
    // Manage IHM input events
    ARCONTROLLER_Device_t *deviceController = (ARCONTROLLER_Device_t *)customData;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    switch (event)
    {
        case IHM_INPUT_EVENT_EXIT:
            gIHMRun = 0;
            break;
        case IHM_INPUT_EVENT_EMERGENCY:
            if(deviceController != NULL)
            {
                // send a Emergency command to the drone
                error = deviceController->aRDrone3->sendPilotingEmergency(deviceController->aRDrone3);
            }
            break;
        case IHM_INPUT_EVENT_LAND:
            if(deviceController != NULL)
            {
                // send a takeoff command to the drone
                error = deviceController->aRDrone3->sendPilotingLanding(deviceController->aRDrone3);
            }
            break;
        case IHM_INPUT_EVENT_TAKEOFF:
            if(deviceController != NULL)
            {
                // send a landing command to the drone
                error = deviceController->aRDrone3->sendPilotingTakeOff(deviceController->aRDrone3);
            }
            break;
        case IHM_INPUT_EVENT_UP:
            if(deviceController != NULL)
            {
                // set the flag and speed value of the piloting command
                error = deviceController->aRDrone3->setPilotingPCMDGaz(deviceController->aRDrone3, 50);
            }
            break;
        case IHM_INPUT_EVENT_DOWN:
            if(deviceController != NULL)
            {
                error = deviceController->aRDrone3->setPilotingPCMDGaz(deviceController->aRDrone3, -50);
            }
            break;
        case IHM_INPUT_EVENT_RIGHT:
            if(deviceController != NULL)
            {
                error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 50);
            }
            break;
        case IHM_INPUT_EVENT_LEFT:
            if(deviceController != NULL)
            {
                error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -50);
            }
            break;
        case IHM_INPUT_EVENT_FORWARD:
            if(deviceController != NULL)
            {
                error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 50);
                error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
            }
            break;
        case IHM_INPUT_EVENT_BACK:
            if(deviceController != NULL)
            {
                error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, -50);
                error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
            }
            break;
        case IHM_INPUT_EVENT_ROLL_LEFT:
            if(deviceController != NULL)
            {
                error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -50);
                error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
            }
            break;
        case IHM_INPUT_EVENT_ROLL_RIGHT:
            if(deviceController != NULL)
            {
                error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, 50);
                error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
            }
            break;
		case IHM_INPUT_EVENT_TAKE_PICTURE:
            if(deviceController != NULL)
            {
	        error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);
            }
            break;
		case IHM_INPUT_EVENT_DEBUGTRANSFERT:
	    	if(deviceController != NULL)
            {
				IHM_PrintInfo (ihm, "DEBUGTRANSFERT");
                createDataTransferManager();
	    		startMediaListThread();
            }
            break;
        case IHM_INPUT_EVENT_RESET:
            if(deviceController != NULL)
            {
                
            }
            break;
        case IHM_INPUT_EVENT_SETTING:
            if(deviceController != NULL)
            {
                
            }
            break;
        case IHM_INPUT_EVENT_CALIBRATION:
            if(deviceController != NULL)
            {
                
            }
            break;
        case IHM_INPUT_EVENT_TESTAUTO:
            if(deviceController != NULL)
            {
                AU = 0;

                while (((yaw < -5) || (yaw > 5)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, 40);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -40);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 175) || (yaw > -175)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                IHM_PrintTest (ihm, "Test auto done");
            }
            break;
        case IHM_INPUT_EVENT_CERCLE:
            if(deviceController != NULL)
            {

                AU = 0;

                while (((yaw < -5) || (yaw > 5)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 40) || (yaw > 50)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 85) || (yaw > 95)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 130) || (yaw > 140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 175) || (yaw > -175)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw > -130) || (yaw < -140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw > -85) || (yaw < -95)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw > -40) || (yaw < -50)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }
            }
            break;
        case IHM_INPUT_EVENT_CADRILLAGE:
            if(deviceController != NULL)
            {

                AU = 0;

                while (((yaw < 40) || (yaw > 50)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 130) || (yaw > 140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < -140) || (yaw > -130)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 130) || (yaw > 140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 40) || (yaw > 50)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                //Demi tour//

                while (((yaw < -50) || (yaw > -40)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < -140) || (yaw > -130)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 130) || (yaw > 140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < -140) || (yaw > -130)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < -50) || (yaw > -40)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }
            }
            break;
        case IHM_INPUT_EVENT_NONE:
            if(deviceController != NULL)
            {
                error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
            }
            break;
        default:
            break;
    }
    
    // This should be improved, here it just displays that one error occured
    if (error != ARCONTROLLER_OK)
    {
        IHM_PrintInfo(ihm, "Error sending an event");
    }
}

int customPrintCallback (eARSAL_PRINT_LEVEL level, const char *tag, const char *format, va_list va)
{
    // Custom callback used when ncurses is runing for not disturb the IHM
    
    if ((level == ARSAL_PRINT_ERROR) && (strcmp(TAG, tag) == 0))
    {
        // Save the last Error
        vsnprintf(gErrorStr, (ERROR_STR_LENGTH - 1), format, va);
        gErrorStr[ERROR_STR_LENGTH - 1] = '\0';
    }
    
    return 1;
}


// TRANSFERT DATA

void createDataTransferManager()
{
    const char *productIP = "192.168.42.1";  // TODO: get this address from libARController

    eARDATATRANSFER_ERROR result = ARDATATRANSFER_OK;
    manager = ARDATATRANSFER_Manager_New(&result);

    if (result == ARDATATRANSFER_OK)
    {
	eARUTILS_ERROR ftpError = ARUTILS_OK;
	ftpListManager = ARUTILS_Manager_New(&ftpError);
	if(ftpError == ARUTILS_OK)
	{
	    IHM_PrintInfo (ihm, "-- ftpQueueManager --");
	    ftpQueueManager = ARUTILS_Manager_New(&ftpError);
	}

	if(ftpError == ARUTILS_OK)
	{
	    IHM_PrintInfo (ihm, "-- InitWifi1 --");
	    ftpError = ARUTILS_Manager_InitWifiFtp(ftpListManager, productIP, DEVICE_PORT, ARUTILS_FTP_ANONYMOUS, "");
	}

	if(ftpError == ARUTILS_OK)
	{
	    IHM_PrintInfo (ihm, "-- InitWifi2 --");
	    ftpError = ARUTILS_Manager_InitWifiFtp(ftpQueueManager, productIP, DEVICE_PORT, ARUTILS_FTP_ANONYMOUS, "");
	}

	if(ftpError != ARUTILS_OK)
	{
	    result = ARDATATRANSFER_ERROR_FTP;
	}
    }
    // NO ELSE

    if (result == ARDATATRANSFER_OK)
    {
	IHM_PrintInfo (ihm, "-- Stockage --");
	ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "stockage");
	const char *path = "./images"; // Change according to your needs, or put as an argument

	result = ARDATATRANSFER_MediasDownloader_New(manager, ftpListManager, ftpQueueManager, MEDIA_FOLDER, path);
    }
}

void startMediaListThread()
{
    // first retrieve Medias without their thumbnails
    ARSAL_Thread_Create(&threadRetreiveAllMedias, ARMediaStorage_retreiveAllMediasAsync, NULL);
}

static void* ARMediaStorage_retreiveAllMediasAsync(void* arg)
{
    getAllMediaAsync();
    return NULL;
}

void getAllMediaAsync()
{
    eARDATATRANSFER_ERROR result = ARDATATRANSFER_OK;
    int mediaListCount = 0;

    if (result == ARDATATRANSFER_OK)
    {
        mediaListCount = ARDATATRANSFER_MediasDownloader_GetAvailableMediasSync(manager,0,&result);
        if (result == ARDATATRANSFER_OK)
        {
	    int i = 0;
	    
	    //place curseur au debut du fichier
	    rewind(PhotosInfosFile);    
	    int nbIter = 0;
	    for (nbIter = 0; nbIter < 1; nbIter = nbIter +1)
	    {
	   	   do
		    {
			 carac = fgetc(PhotosInfosFile);	 
		    }
		    while(carac != '\n');
	    }
            for (i = 0 ; i < mediaListCount && result == ARDATATRANSFER_OK; i++)
            {
                ARDATATRANSFER_Media_t * mediaObject = ARDATATRANSFER_MediasDownloader_GetAvailableMediaAtIndex(manager, i, &result);
                //printf("Media %i: %s\n", i, mediaObject->name);

                // Do what you want with this mediaObject
	        downloadMedias(mediaObject, mediaListCount);
		do
   		{
			carac = fgetc(PhotosInfosFile);	 
   		}
		while(carac != '\n');
   		fputs(mediaObject->name,PhotosInfosFile);
            }
	    fclose(PhotosInfosFile);
        }
    }
}


void downloadMedias(ARDATATRANSFER_Media_t *medias, int count)
{
    eARDATATRANSFER_ERROR result = ARDATATRANSFER_OK;
    
    int i = 0;
    for (i = 0 ; i < count && result == ARDATATRANSFER_OK; i++)
    {
        ARDATATRANSFER_Media_t *media = &medias[i];
        result = ARDATATRANSFER_MediasDownloader_AddMediaToQueue(manager, media, medias_downloader_progress_callback, NULL, medias_downloader_completion_callback, NULL);
    }

    if (result == ARDATATRANSFER_OK)
    {
        if (threadMediasDownloader == NULL)
        {
            // if not already started, start download thread in background
            ARSAL_Thread_Create(&threadMediasDownloader, ARDATATRANSFER_MediasDownloader_QueueThreadRun, manager);
        }
    }
}

void medias_downloader_progress_callback(void* arg, ARDATATRANSFER_Media_t *media, float percent)
{
    IHM_PrintDownload(ihm, percent, media);
}

void medias_downloader_completion_callback(void* arg, ARDATATRANSFER_Media_t *media, eARDATATRANSFER_ERROR error)
{
    
}


void clean()
{
    if (threadRetreiveAllMedias != NULL)
    {
        ARDATATRANSFER_MediasDownloader_CancelGetAvailableMedias(manager);

        ARSAL_Thread_Join(threadRetreiveAllMedias, NULL);
        ARSAL_Thread_Destroy(&threadRetreiveAllMedias);
        threadRetreiveAllMedias = NULL;
    }

    if (threadGetThumbnails != NULL)
    {
        ARDATATRANSFER_MediasDownloader_CancelGetAvailableMedias(manager);

        ARSAL_Thread_Join(threadGetThumbnails, NULL);
        ARSAL_Thread_Destroy(&threadGetThumbnails);
        threadGetThumbnails = NULL;
    }

    if (threadMediasDownloader != NULL)
    {
        ARDATATRANSFER_MediasDownloader_CancelQueueThread(manager);

        ARSAL_Thread_Join(threadMediasDownloader, NULL);
        ARSAL_Thread_Destroy(&threadMediasDownloader);
        threadMediasDownloader = NULL;
    }

    ARDATATRANSFER_MediasDownloader_Delete(manager);

    ARUTILS_Manager_CloseWifiFtp(ftpListManager);
    ARUTILS_Manager_CloseWifiFtp(ftpQueueManager);

    ARUTILS_Manager_Delete(&ftpListManager);
    ARUTILS_Manager_Delete(&ftpQueueManager);
    ARDATATRANSFER_Manager_Delete(&manager);
}

/*
        case IHM_INPUT_EVENT_TAKE_PICTURE:
            if(deviceController != NULL)
            {
            error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);
            }
            break;
        case IHM_INPUT_EVENT_DEBUGTRANSFERT:
            if(deviceController != NULL)
            {
                IHM_PrintInfo (ihm, "DEBUGTRANSFERT");
                createDataTransferManager();
                startMediaListThread();
            }
            break;
        case IHM_INPUT_EVENT_RESET:
            if(deviceController != NULL)
            {
                
            }
            break;
        case IHM_INPUT_EVENT_SETTING:
            if(deviceController != NULL)
            {
                
            }
            break;
        case IHM_INPUT_EVENT_CALIBRATION:
            if(deviceController != NULL)
            {
                
            }
            break;
        case IHM_INPUT_EVENT_TESTAUTO:
            if(deviceController != NULL)
            {
                AU = 0;

                while (((yaw < -5) || (yaw > 5)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, 40);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -40);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 175) || (yaw > -175)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                IHM_PrintTest (ihm, "Test auto done");
            }
            break;
        case IHM_INPUT_EVENT_CERCLE:
            if(deviceController != NULL)
            {

                AU = 0;

                while (((yaw < -5) || (yaw > 5)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 40) || (yaw > 50)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 85) || (yaw > 95)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 130) || (yaw > 140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 175) || (yaw > -175)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw > -130) || (yaw < -140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw > -85) || (yaw < -95)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw > -40) || (yaw < -50)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 20);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDRoll(deviceController->aRDrone3, -30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }
            }
            break;
        case IHM_INPUT_EVENT_CADRILLAGE:
            if(deviceController != NULL)
            {

                AU = 0;

                while (((yaw < 40) || (yaw > 50)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 130) || (yaw > 140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < -140) || (yaw > -130)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 130) || (yaw > 140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 40) || (yaw > 50)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                //Demi tour//

                while (((yaw < -50) || (yaw > -40)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < -140) || (yaw > -130)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < 130) || (yaw > 140)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, -30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < -140) || (yaw > -130)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                while (((yaw < -50) || (yaw > -40)) ^ (AU == 'n'))
                {
                    error = deviceController->aRDrone3->setPilotingPCMDYaw(deviceController->aRDrone3, 30);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 1) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }

                error = deviceController->aRDrone3->sendMediaRecordPictureV2(deviceController->aRDrone3);

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 3) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMDPitch(deviceController->aRDrone3, 30);
                    error = deviceController->aRDrone3->setPilotingPCMDFlag(deviceController->aRDrone3, 1);
                    AU = getch();
                }

                t1 = time(NULL);
                t2 = t1;
                while (((t2 - t1) < 2) ^ (AU == 'n'))
                {
                    t2 = time(NULL);
                    error = deviceController->aRDrone3->setPilotingPCMD(deviceController->aRDrone3, 0, 0, 0, 0, 0, 0);
                    AU = getch();
                }
            }
            break;
*/
