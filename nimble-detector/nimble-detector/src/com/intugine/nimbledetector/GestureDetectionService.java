package com.intugine.nimbledetector;

import android.app.Service;
import android.content.Intent;
import android.hardware.Camera;
import android.os.IBinder;
import android.util.Log;
import android.util.Log;
import java.util.ArrayList;

/**
 * Created by siddharthsivaraman on 09/01/15.
 */
public class GestureDetectionService extends Service {


    private class InputDevi{
        private int mnId;
        private String mszPath, mszName;
        private boolean mbOpen;

        InputDevi(int id, String path,String name) {
            mnId = id; mszPath = path; mszName = name;
        }

//        public void Close() {
//            mbOpen  = false;
////            RemoveDev(m_nId);
//        }
    }



    public IBinder onBind(Intent intent) {
        return null;
    }

    private CameraPreviewBackground mPreview;
    private Camera mCamera;
    private GestureDetector mDetector;


    private static final String TAG = "NimbleDetector:GestureDetectionService";
    private static final String mouseDev="PixArt USB Optical Mouse";
    private static int mouseId=0;

    private ArrayList<InputDevi> mDevs = new ArrayList<InputDevi>();

    private Camera getHardwareCamera() {

        Camera cam = null;
        try {
            cam = Camera.open();
        } catch (Exception e) {
//            Log.e(TAG, "Camera could not be opened!");
        }

        return cam;
    }

    @Override
    public  void onCreate() {

        mDetector = new GestureDetector(this);
        cameraThread.start();
        int n = ScanFiles();
        Log.e("No of files","=="+n);
        for (int i=0;i<n;i++){
            DevOpen(i);
            String devname=getDevName(i);
            Log.e("ServiceCalled",devname);
            if(devname.equalsIgnoreCase(mouseDev)){
                mouseId=i;
                break;
            }
        }
        setMouseId(mouseId);
        Log.e("ServiceThreadCalled","should appear frequently"+mouseId);
//          Log.e("Gesture Service","Service started");
//          int n = ScanFiles();
//        Log.e("No of files","counting");
//        Log.e("No of files","=="+n);
//        boolean result = DevOpen(0);
//        Log.e("No of files","=="+result);
    }

    public void onDestroy() {
        if (mCamera != null) {
            mCamera.release();
        }
//        int  number = ScanDevs();

        super.onDestroy();
    }

    Thread cameraThread = new Thread() {
        @Override
        public void run() {
            // NOTE: Not using Camera 2 APIS which are recommended for Lollipop

            mCamera = getHardwareCamera();

            if (mCamera != null) {
                mPreview = new CameraPreviewBackground(GestureDetectionService.this,mCamera,mDetector);
            }
        }
    };


    public boolean DevOpen(int id){
        int res = OpenDev(id);
        boolean result = (res==0);
        //    Log.e("ServiceTestCalled ", "Device Opened::" + result);
        return result;
    }


    private native static int ScanFiles(); // return number of devs
    private native static String getDevPath(int devid);
    private native static int OpenDev(int devid);
    private native static String getDevName(int devid);
    private native static void setMouseId(int id);


}
