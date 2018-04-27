/*
* UNIMPORTANT
* UNIMPORTANT
* UNIMPORTANT
* UNIMPORTANT
* UNIMPORTANT
 */


package com.intugine.nimbledetector;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

/**
 * Created by Admin on 6/15/2015.
 */

public class ServiceTest extends Service {

    private  final String TAG_TEST = "NimbleDetector:GestureDetectionService";
    private InputDevi mInputDevi;

    private class InputDevi{
        private int mnId;
        private String mszPath, mszName;
        private boolean mbOpen;

        InputDevi(int id, String path,String name) {
            mnId = id; mszPath = path; mszName = name;
        }

        public void Close() {
            mbOpen  = false;
//            RemoveDev(m_nId);
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mInputDevi = new InputDevi(0,"/dev/input/event5","PixArt USB Optical Mouse");
        testThread.start();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    public boolean DevOpen(int id){
        int res = OpenDev(id);
        boolean result = (res==0);
        Log.e("ServiceTestCalled ", "Device Opened::" + result);
        return result;
    }

    Thread testThread = new Thread(){
        @Override
        public void run() {
            int n= ScanFiles();
            boolean open = DevOpen(0);
            Log.e("ServiceTestThreadCalled","Results::"+n);
        }
    };

    private native static int intEnableDebug(int enable);

    private native static int ScanFiles(); // return number of devs
    private native static int OpenDev(int devid);
    private native static int RemoveDev(int devid);
    private native static String getDevPath(int devid);
    private native static String getDevName(int devid);
    private native static int PollDev(int devid);
    private native static int getType();
    private native static int getCode();
    private native static int getValue();
    // injector:
    private native static int intSendEvent(int devid, int type, int code, long value);

    static {
        System.loadLibrary("nimbledetector");
    }
}
