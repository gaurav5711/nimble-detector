#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <jni.h>
#include <android/log.h>
#include <time.h>


#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/limits.h>
#include <sys/poll.h>
//#include <C:/android-ndk-r10d/platforms/android-12/arch-x86/usr/include/linux/bitmap.h>

#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/input.h>




// No of frames to record per gesture detection
#define GESTURE_DETECTION_FRAME_COUNT 3

#define mId 3

// Disable/Enable getting timing information
#define DEBUG_TIMING 1
#define DEBUG_DECODE 0

using namespace cv;
using namespace std;

enum SwipeDirection {
    SWIPE_LEFT,
    SWIPE_RIGHT,
    SWIPE_UP,
    SWIPE_DOWN
};

#define LOG_TAG "Nimble"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


#define TAG "EventInjector::JNI"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , TAG, __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
//#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)


int previewHeight;
int previewWidth;
char* cameraFrame;
pthread_t decode_thread;
pthread_t process_thread;
Mat rgbMat;
static int decode =1;
static int process=1;
int processframe;

extern "C" {

#if DEBUG_TIMING

timespec diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}


#endif


//Event Injector

struct uinput_event {
	struct timeval time;
	uint16_t type;
	uint16_t code;
	int32_t value;
};



/* Debug tools
 */
 int g_debug = 0;
 int mouseID;


void debug(char *szFormat, ...)
{
	if (g_debug == 0) return;
	//if (strlen(szDbgfile) == 0) return;

	char szBuffer[4096]; //in this buffer we form the message
	const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	const int LASTCHAR = NUMCHARS - 1;
	//format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);
	// use a bounded buffer size to prevent buffer overruns.  Limit count to
	// character size minus one to allow for a NULL terminating character.
	vsnprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);
	//ensure that the formatted string is NULL-terminated
	szBuffer[LASTCHAR] = '\0';

//	LOGD(szBuffer);
	//TextCallback(szBuffer);
 }



jint Java_com_intugine_nimbledetector_Events_intEnableDebug( JNIEnv* env,jobject thiz, jint enable ) {

	g_debug = enable;
	return g_debug;
}

//jint JNI_OnLoad(JavaVM *vm, void *reserved)
//{
//	debug("eventinterceptor native lib loaded.");
//	return JNI_VERSION_1_2; //1_2 1_4
//}
//
//void JNI_OnUnload(JavaVM *vm, void *reserved)
//{
//	debug("eventinterceptor native lib unloaded.");
//}


static struct typedev {
	struct pollfd ufds;
	char *device_path;
	char *device_name;
} *pDevs = NULL;
struct pollfd *ufds;
static int nDevsCount;

const char *device_path = "/dev/input";

int g_Polling = 0;
//struct input_event event;
int c;
int i;
int pollres;
int get_time = 0;
char *newline = "\n";
uint16_t get_switch = 0;
struct input_event event;
int version;

int dont_block = -1;
int event_count = 0;
int sync_rate = 0;
int64_t last_sync_time = 0;
const char *device = NULL;


 static int open_device(int index)
 {
 //	if (index >= nDevsCount || pDevs == NULL) return -1;
    if (pDevs == NULL) return -1;
 	debug("open_device prep to open");
 	char *device = pDevs[index].device_path;

 	debug("open_device call %s", device);
     int version;
     int fd;

     char name[80];
     char location[80];
     char idstr[80];
     struct input_id id;

     fd = open(device, O_RDWR);
     if(fd < 0) {
 		pDevs[index].ufds.fd = -1;

 		pDevs[index].device_name = NULL;
 		debug("could not open %s, %s", device, strerror(errno));
         return -1;
     }

     pDevs[index].ufds.fd = fd;
     	ufds[index].fd = fd;

         name[sizeof(name) - 1] = '\0';
         if(ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
             debug("could not get device name for %s, %s", device, strerror(errno));
             name[0] = '\0';
         }
     	debug("Device %d: %s: %s", nDevsCount, device, name);

     	pDevs[index].device_name = strdup(name);


         return 0;
     }


     int remove_device(int index)
     {
     	if (index >= nDevsCount || pDevs == NULL ) return -1;

     	int count = nDevsCount - index - 1;
     	debug("remove device %d", index);
     	free(pDevs[index].device_path);
     	free(pDevs[index].device_name);

     	memmove(&pDevs[index], &pDevs[index+1], sizeof(pDevs[0]) * count);
     	nDevsCount--;
     	return 0;
     }


static int scan_dir(const char *dirname)
{
	nDevsCount = 0;
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        strcpy(filename, de->d_name);
		debug("scan_dir:prepare to open:%s", devname);
		// add new filename to our structure: devname
		struct typedev *new_pDevs = (typedev *)realloc(pDevs, sizeof(pDevs[0]) * (nDevsCount + 1));
		if(new_pDevs == NULL) {
			debug("out of memory");
			return -1;
		}
		pDevs = new_pDevs;

		struct pollfd *new_ufds = (pollfd *)realloc(ufds, sizeof(ufds[0]) * (nDevsCount + 1));
		if(new_ufds == NULL) {
			debug("out of memory");
			return -1;
		}
		ufds = new_ufds;
		ufds[nDevsCount].events = POLLIN;

		pDevs[nDevsCount].ufds.events = POLLIN;
		pDevs[nDevsCount].device_path = strdup(devname);

        nDevsCount++;
    }
    closedir(dir);
    return 0;
}


jint Java_com_intugine_nimbledetector_CameraPreviewBackground_intSendEvent(JNIEnv* env,jobject thiz, jint index, uint16_t type, uint16_t code, jlong value) {
	if (index >= nDevsCount || pDevs[index].ufds.fd == -1) return -1;
	int fd = pDevs[index].ufds.fd;
	debug("SendEvent call (%d,%d,%d,%d)", fd, type, code, value);
	struct uinput_event event;
	int len;

	if (fd <= fileno(stderr)) return -1;

	memset(&event, 0, sizeof(event));
	event.type = type;
	event.code = code;
	event.value = value;

	len = write(fd, &event, sizeof(event));
	debug("SendEvent done:%d",len);
}



jint Java_com_intugine_nimbledetector_GestureDetectionService_ScanFiles( JNIEnv* env,jobject thiz ) {
	int res = scan_dir(device_path);
	if(res < 0) {
		debug("scan dir failed for %s:", device_path);
		return -1;
	}

	return nDevsCount;
}


void Java_com_intugine_nimbledetector_GestureDetectionService_setMouseId(JNIEnv* env,jobject thiz,jint id){
mouseID=id;
}


jstring Java_com_intugine_nimbledetector_GestureDetectionService_getDevPath( JNIEnv* env,jobject thiz, jint index) {
//	return (*env)->NewStringUTF(env, pDevs[index].device_path);
return (env)->NewStringUTF( pDevs[index].device_path);
}
jstring Java_com_intugine_nimbledetector_GestureDetectionService_getDevName( JNIEnv* env,jobject thiz, jint index) {
	if (pDevs[index].device_name == NULL) return NULL;
//	else return (*env)->NewStringUTF(env, pDevs[index].device_name);
else return (env)->NewStringUTF( pDevs[index].device_name);
}

jint Java_com_intugine_nimbledetector_GestureDetectionService_OpenDev( JNIEnv* env,jobject thiz, jint index ) {
	return open_device(index);
}

jint Java_com_intugine_nimbledetector_Events_RemoveDev( JNIEnv* env,jobject thiz, jint index ) {
	return remove_device(index);
}



jint Java_com_intugine_nimbledetector_Events_PollDev( JNIEnv* env,jobject thiz, jint index ) {
	if (index >= nDevsCount || pDevs[index].ufds.fd == -1) return -1;
	int pollres = poll(ufds, nDevsCount, -1);
	if(ufds[index].revents) {
		if(ufds[index].revents & POLLIN) {
			int res = read(ufds[index].fd, &event, sizeof(event));
			if(res < (int)sizeof(event)) {
				return 1;
			}
			else return 0;
		}
	}
	return -1;
}


jint Java_com_intugine_nimbledetector_Events_getType( JNIEnv* env,jobject thiz ) {
	return event.type;
}

jint Java_com_intugine_nimbledetector_Events_getCode( JNIEnv* env,jobject thiz ) {
	return event.code;
}

jint Java_com_intugine_nimbledetector_Events_getValue( JNIEnv* env,jobject thiz ) {
	return event.value;
}

    int intSendEvent(int index, int type, int code, long value) {
	if (index >= nDevsCount || pDevs[index].ufds.fd == -1) return -1;
	int fd = pDevs[index].ufds.fd;
	debug("SendEvent call (%d,%d,%d,%d)", fd, type, code, value);
	struct uinput_event event;
	int len;

	if (fd <= fileno(stderr)) return -1;

	memset(&event, 0, sizeof(event));
	event.type = type;
	event.code = code;
	event.value = value;

	len = write(fd, &event, sizeof(event));
	debug("SendEvent done:%d",len);
}











void swipe(JNIEnv* jniEnv,jobject callback, SwipeDirection direction) {

    LOGI("Swiping in direction %d",direction);

    jclass gestureDetector = (jniEnv->FindClass("com/intugine/nimbledetector/GestureDetector"));
    jmethodID methodID = (jniEnv->GetMethodID(gestureDetector, "swipe", "(I)V"));
    jniEnv->CallVoidMethod(callback, methodID, direction);

}

void swipeStart(){
 intSendEvent(mouseID, 4, 4, 589825);
 intSendEvent(mouseID, 1, 272,1);
 intSendEvent(mouseID, 0, 0,0);
}

void swipeStop(){
 intSendEvent(mouseID, 4, 4, 589825);
 intSendEvent(mouseID, 1, 272,0);
 intSendEvent(mouseID, 0, 0,0);
}

    void *testProcessMat(void *defaultValue){
        int width=previewWidth;
        int height=previewHeight;
        char* yuv;
        const int frameSize = width * height;
        const int ii = 0;
        const int ij = 0;
        const int di = +1;
        const int dj = +1;
        static int n=1;
        static float xc=0,yc=0;
        while(1){

        int a = 0;
        if((yuv==NULL)) LOGE("Decode FunctionH %ld",420);
      if(processframe==1){
        yuv=cameraFrame;
        uchar* rgb;
        int n=1;
        float xc=0,yc=0;

        for (int i = 0, ci = ii; i < height; i+=1, ci += di)
        {
            a = 0;
            for (int j = 0, cj = ij; j < width; j+=1, cj += dj,a += 3)
            {
                int y = (0xff & ((int) yuv[ci * width + cj]));
                y = y < 16 ? 16 : y;
                if(y>75)
                {
                 xc += (float)j;///(float)bin.cols;
                 yc += (float)i;///(float)bin.rows;
                 n++;
                }
            }
        }

static int counta=0;static int n2=0;
static uchar temp=0;
    xc = xc/(float)n;xc=640-xc;
    yc = yc/(float)n;
    LOGI("Started XC YC N : %f %f new %d             ",xc,yc,n);
    static int dx=0,dy=0;


//    static int previousN=0,currentN=0;
//    currentN = n;
//    if((currentN>0)&&(previousN==0)){
//    swipeStart();
//    }
//    if((currentN==0)&&(previousN>0)){
//    swipeStop();
//    }
//    previousN=currentN;

       static float px[5]={0},py[5]={0},pn[5]={0};
//       static float px=0,py=0;
       dx=(int)(xc-px[3]);
       dy=(int)(yc-py[3]);
       if((dx!=0||dy!=0)&&(pn[4]>5)&&(pn[3]>5)&&(pn[2]>5)&&(pn[1]>5)&&(pn[0]>5)&&(n>5)){

                        if(dx>=0&&dy>=0){
                            //Swipe right
                                intSendEvent(mouseID,2,0,(dx));
                        //        intSendEvent(mouseID,0,0,0);

                                //Swipe down
                                intSendEvent(mouseID, 2, 1,dy);
                        //        intSendEvent(mouseID, 0, 0,0);
                        }

                        else if(dx<=0&&dy>=0) {
                             //Swipe left
                                intSendEvent(mouseID,2,0, 4294967296L+(dx));
                         //       intSendEvent(mouseID, 0, 0, 0);

                                //Swipe down
                                intSendEvent(mouseID, 2, 1,dy);
                         //       intSendEvent(mouseID, 0, 0,0);
                        }

                        else if(dx>=0&&dy<=0){
                            //Swipe right
                                intSendEvent(mouseID,2,0,(dx));
                         //       intSendEvent(mouseID,0,0,0);

                                //Swipe up
                                intSendEvent(mouseID, 2, 1, 4294967296L+(dy));
                         //       intSendEvent(mouseID, 0, 0, 0);
                        }

                        else if(dx<=0&&dy<=0){
                           //Swipe left
                               intSendEvent(mouseID,2,0, 4294967296L+(dx));
                           //    intSendEvent(mouseID, 0, 0, 0);

                               //Swipe up
                               intSendEvent(mouseID, 2, 1, 4294967296L+(dy));
                             //  intSendEvent(mouseID, 0, 0, 0);
                        }
    intSendEvent(mouseID, 0, 0, 0);
    }
    px[4]=px[3];px[3]=px[2];px[2]=px[1];px[1]=px[0];px[0]=xc;
    py[4]=py[3];py[3]=py[2];py[2]=py[1];py[1]=py[0];py[0]=yc;
    pn[4]=pn[3];pn[3]=pn[2];pn[2]=pn[1];pn[1]=pn[0];pn[0]=n;
    processframe=0;
     }
//    px=xc;
//    py=yc;
//    LOGI("Started XC YC N : %d %d new %d             ",dx,dy,n);
 //     LOGI("Mouse ID             %d",mouseID);


}   //while loop closed
}

    void SendXnYn(int x,int y) {


    //Swipe left
    intSendEvent(1,2,0, 4294967296L+x);
    intSendEvent(1, 0, 0, 0);

    //Swipe up
    intSendEvent(1, 2, 1, 4294967296L+y);
    intSendEvent(1, 0, 0, 0);

}

    void SendXpYp(int x,int y) {

    //Swipe right
    intSendEvent(1,2,0,x);
    intSendEvent(1,0,0,0);

    //Swipe down
    intSendEvent(1, 2, 1,y);
    intSendEvent(1, 0, 0,0);

}

    void SendXnYp(int x,int y) {

    //Swipe left
    intSendEvent(1,2,0, 4294967296L+x);
    intSendEvent(1, 0, 0, 0);

    //Swipe down
    intSendEvent(1, 2, 1,y);
    intSendEvent(1, 0, 0,0);
}

    void SendXpYn(int x, int y ) {

    //Swipe right
    intSendEvent(1,2,0,x);
    intSendEvent(1,0,0,0);

    //Swipe up
    intSendEvent(1, 2, 1, 4294967296L+y);
    intSendEvent(1, 0, 0, 0);

}


//void decodeYUV420SPToRGB(Mat& rgbmat, char* yuv, int width, int height) {
void *decodeYUV420SPToRGB(void *defaultValue){

    rgbMat = Mat(previewHeight,previewWidth,CV_8UC3);
    int width=previewWidth;
    int height=previewHeight;
    char* yuv;
    const int frameSize = width * height;
    const int ii = 0;
    const int ij = 0;
    const int di = +1;
    const int dj = +1;
    static int count3=0;
    while(1){
//    #if DEBUG_TIMING
//        timespec time1,time2;
//        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
//    #endif
//    #if DEBUG_DECODE
//                timespec time1;
//                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
//         //       timespec timeDiff = diff(time1,time2);
//        LOGI("total time decode onlyYY start %ld %d",time1.tv_nsec,count3);
//        #endif
    int a = 0;
    if((yuv==NULL)){
        LOGI("Decode FunctionH %ld",420);
         }
    yuv=cameraFrame;
    uchar* rgb;
//     #if DEBUG_TIMING
//                                    timespec time1;
//                                    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
//                             //       timespec timeDiff = diff(time1,time2);
//                            LOGI("total decode process onlyYY start %ld",time1.tv_nsec);
//                            #endif
    for (int i = 0, ci = ii; i < height; i+=1, ci += di) {
        rgb = (uchar *)(rgbMat.data + i*rgbMat.step);
        a = 0;
        for (int j = 0, cj = ij; j < width; j+=1, cj += dj,a += 3) {
            int y = (0xff & ((int) yuv[ci * width + cj]));
//            int v = (0xff & ((int) yuv[frameSize + (ci >> 1) * width + (cj & ~1) + 0]));
//            int u = (0xff & ((int) yuv[frameSize + (ci >> 1) * width + (cj & ~1) + 1]));
            y = y < 16 ? 16 : y;
//            if(y>16){
//            LOGE("Y from YUV420 %d",y);
//            }

//            int r = (int) (1.164f * (y - 16) + 1.596f * (v - 128));
//            int g = (int) (1.164f * (y - 16) - 0.813f * (v - 128) - 0.391f * (u - 128));
//            int b = (int) (1.164f * (y - 16) + 2.018f * (u - 128));
//
//            r = r < 0 ? 0 : (r > 255 ? 255 : r);
//            g = g < 0 ? 0 : (g > 255 ? 255 : g);
//            b = b < 0 ? 0 : (b > 255 ? 255 : b);
//
//            rgb[a] = b;
//            rgb[a + 1] = g;
//            rgb[a + 2] = r;
        }
    }


//    #if DEBUG_TIMING
//                                        timespec time2;
//                                        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
//                                 //       timespec timeDiff = diff(time1,time2);
//                                LOGI("total decode process onlyYY end %ld",time2.tv_nsec);
//                                #endif

   // testProcessMat(rgbMat);
   // count3++;
//     #if DEBUG_DECODE
//            timespec time2;
//            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
//     //       timespec timeDiff = diff(time1,time2);
//    LOGI("total time decode stop %ld %d",time2.tv_nsec,count3);
//    #endif
    }            //while close
}

 void  Java_com_intugine_nimbledetector_CameraPreviewBackground_HandleFrame(JNIEnv* env, jobject thiz, jobject callback,
                                                              jint width, jint height, jbyteArray yuvData) {


    processframe=1;
    static int count = 0;
    static int count2 = 0;
    static Vec2f sum = Vec2f(0,0);
    static Vec2f delta = Vec2f(0,0);

    jboolean isCopy;
    jbyte* buffer = env->GetByteArrayElements(yuvData, &isCopy);
    jsize length = env->GetArrayLength(yuvData);
 //   LOGI("JPreviewSize %d",height);
        previewHeight=height;
        previewWidth=width;

    // Decode byte array to RGB opencv Mat
    // NOTE: Assumption is that the preview frame format is NV21
    //Mat rgbMat = Mat(height,width,CV_8UC3);
//    decodeYUV420SPToRGB(rgbMat, reinterpret_cast<char*> (buffer), width, height);
    cameraFrame = reinterpret_cast<char*> (buffer);
        if(decode==1){
        int defaultValue = -1;
        pthread_create(&decode_thread,NULL,decodeYUV420SPToRGB,(void *)defaultValue);
        int rc=pthread_create(&process_thread,NULL,testProcessMat,(void *)defaultValue);
        if(rc){
        LOGE("Error Creating thread %d",0);
        }
        decode=0;
        }

    // Process Mat for detecting gestures

    // Detect position of marker in image


    //Edited...........
//    Vec2f ret = testProcessMat(rgbMat);
//            float x=ret.x;
//            float y=ret.y;


//Getting values

//float *coordinate;
//      coordinate= testProcessMat(rgbMat);

//      LOGI("XC YC  : %f %f     ",*coordinate,*(coordinate+1));

//       Vec2f ret(*coordinate,*(coordinate+1));

       // Sending float array to java

//        jfloatArray point_coordinate;
//        point_coordinate = env->NewFloatArray(2);

//        jfloat fill[2];
//        fill[0]=*coordinate;
//        fill[1]=*(coordinate+1);
//        env->SetFloatArrayRegion(point_coordinate, 0,2, fill);
 //       return point_coordinate;


  //Edited............

    // Detect marker
//    if (count < GESTURE_DETECTION_FRAME_COUNT) {
//        sum = Vec2f(sum[0] + ret[0],sum[1] + ret[1]);
//    } else {
//        Vec2f newDelta = Vec2f(sum[0]/(float)count,sum[1]/(float)count);

//       if (fabs(newDelta[0] - delta[0]) > .1f) {
//            if (newDelta[0] > delta[0]) {
//                swipe(env,callback,SWIPE_DOWN);
                // To give swipe  view
                // jclass gestureDetector = (env->FindClass("com/intugine/nimbledetector/GestureDetector"));
                 //jmethodID methodID = (env->GetMethodID(gestureDetector, "draw", "(I)V"));
                //env->CallVoidMethod(callback, methodID, 0 , 100, 500, 500);
//            } else {
//                swipe(env,callback,SWIPE_UP);
                // To give swipe  view
                //  jclass gestureDetector = (env->FindClass("com/intugine/nimbledetector/GestureDetector"));
                  //                jmethodID methodID = (env->GetMethodID(gestureDetector, "draw", "(I)V"));
                    //             env->CallVoidMethod(callback, methodID, 100 , 100, 300, 400);
//            }
//        } else if (fabs(newDelta[1] - delta[1]) > .1f) {
//            if (newDelta[1] > delta[1]) {
//                swipe(env,callback,SWIPE_LEFT);
                // To give swipe  view
          // jclass gestureDetector = (env->FindClass("com/intugine/nimbledetector/GestureDetector"));
            //               jmethodID methodID = (env->GetMethodID(gestureDetector, "draw", "(I)V"));
              //            env->CallVoidMethod(callback, methodID, 500 , 500, 0, 0);
//            } else {
//                swipe(env,callback,SWIPE_RIGHT);
                // To give swipe  view
                 // jclass gestureDetector = (env->FindClass("com/intugine/nimbledetector/GestureDetector"));
                   //               jmethodID methodID = (env->GetMethodID(gestureDetector, "draw", "(I)V"));
                     //            env->CallVoidMethod(callback, methodID, 100 , 200, 300, 400);
//            }
//        }

//        delta = newDelta;
//        sum = Vec2f(0,0);
//        count = -1;
//    }

    ++count;

    // here it is important to use 0 so that JNI takes care of copying
    // the data back to the Java side in case GetByteArrayElements returned a copy
    env->ReleaseByteArrayElements(yuvData,buffer, 0);


//int milliSecondsElapsed = getMilliSpan(start);

//  return point_coordinate;
//  return env->NewStringUTF("Gaurav");
}

}
