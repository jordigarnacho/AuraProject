#ifndef _JUMPINGSUMO_PILOTING_H_
#define _JUMPINGSUMO_PILOTING_H_

#include <AuraProjectIHM.h>

// called when the state of the device controller has changed
void stateChanged (eARCONTROLLER_DEVICE_STATE newState, eARCONTROLLER_ERROR error, void *customData);

// called when a command has been received from the drone
void commandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData);

// IHM updates from commands
void batteryStateChanged (uint8_t percent);
void positionChanged (double latitude, double longitude, double altitude);
void speedChanged (float speedX, float speedY, float speedZ);
void attitudeChanged (int roll, int pitch, int yaw);
void maxAltitudeChanged (float current, float min, float max);

// called when a streaming frame has been received
//eARCONTROLLER_ERROR didReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData);
//eARCONTROLLER_ERROR decoderConfigCallback (ARCONTROLLER_Stream_Codec_t codec, void *customData);

/* IHM callbacks: */
void onInputEvent (eIHM_INPUT_EVENT event, void *customData);

int customPrintCallback (eARSAL_PRINT_LEVEL level, const char *tag, const char *format, va_list va);

/* CSV Pictures */
void newPicture(void); 

/* FTP TRANSFER */
void createDataTransferManager(void);
void startMediaListThread(void);
static void* ARMediaStorage_retreiveAllMediasAsync(void* arg);
void getAllMediaAsync(void);
void medias_downloader_completion_callback(void* arg, ARDATATRANSFER_Media_t *media, eARDATATRANSFER_ERROR error);
void medias_downloader_progress_callback(void* arg, ARDATATRANSFER_Media_t *media, float percent);
void downloadMedias(ARDATATRANSFER_Media_t *medias, int count);
void clean(void);
#endif /* _JUMPINGSUMO_PILOTING_H_ */
