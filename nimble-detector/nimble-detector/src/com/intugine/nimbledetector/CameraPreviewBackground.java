package com.intugine.nimbledetector;

import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.Log;

import java.util.List;

/**
 * Created by siddharthsivaraman on 07/01/15.
 */
public class CameraPreviewBackground implements Camera.PreviewCallback {


//    private class InputDevi{
//        private int mnId;
//        private String mszPath, mszName;
//        private boolean mbOpen;
//
//        InputDevi(int id, String path,String name) {
//            mnId = id; mszPath = path; mszName = name;
//        }
//
//        public void Close() {
//            mbOpen  = false;
////            RemoveDev(m_nId);
//        }
//    }



    private Camera mCamera;
    private byte[] mCallbackBuffer;
    private int mWidth;
    private int mHeight;
    private long[] timeDebug;

    private GestureDetector mDetector;

    private static final String TAG = "CameraPreviewBackground";

    public CameraPreviewBackground(Context ctx, Camera c, GestureDetector detector) {
        mCamera = c;
        mDetector = detector;
        initialize();
        timeDebug = new long[2];
        timeDebug[0]=0L;
    }

    private void initialize() {

        // Stop preview if its already running
        try {
            mCamera.stopPreview();
        } catch (Exception e) {
            // Ignore
        }

        try {
            /* NOTE: This will work predictably only if the default format for preview NV21 is used.
                    It casts it to float when dividing by 8,
                    and still receives the right result. Might not work with other formats
            */

            // Set camera preview width, height and disable autoexposure and white-balance
            Camera.Parameters cParams = mCamera.getParameters();

            cParams.setAutoExposureLock(true);
            cParams.setExposureCompensation(-7);
            setCameraParameters(cParams);

            float bytesPerPixel = ImageFormat.getBitsPerPixel(cParams.getPreviewFormat())/8.0f;
            int bufferSize = (int)(cParams.getPreviewSize().width*cParams.getPreviewSize().height*bytesPerPixel);
            mCallbackBuffer = new byte[bufferSize];
            mCamera.addCallbackBuffer(mCallbackBuffer);
            mCamera.setPreviewCallbackWithBuffer(this);

            mWidth = cParams.getPreviewSize().width;
            mHeight = cParams.getPreviewSize().height;

            mCamera.setPreviewTexture(new SurfaceTexture(0));
            mCamera.startPreview();
            Log.d(TAG,"Started preview");

        } catch (Exception e){
            // TODO: Handle errors
        }

    }


    public boolean DevOpen(int id){
        int res = OpenDev(id);
        boolean result = (res==0);
    //    Log.e("ServiceTestCalled ", "Device Opened::" + result);
        return result;
    }



    @Override
    public void onPreviewFrame(byte[] bytes, Camera camera) {
//        int n= ScanFiles();
//        boolean open = DevOpen(1);
//        intSendEvent(1, 4, 4, 589825);
//        intSendEvent(1, 1, 272,1);
//        intSendEvent(1, 0, 0,0);
//        for(int i = 0;i<=5;i++) {
//
//            intSendEvent(1, 2, 1, 4294967295L);
//            intSendEvent(1, 0, 0, 0);
//            intSendEvent(1,2,0,1);
//            intSendEvent(1,0,0,0);
//        }
//        intSendEvent(1, 4, 4, 589825);
//        intSendEvent(1, 1, 272,0);
//        intSendEvent(1, 0, 0,0);
        timeDebug[1]=System.currentTimeMillis();
        Log.e("CameraPreviewBackgroundCalled",""+(timeDebug[1]-timeDebug[0]));
 //       Log.e("CameraPreviewBackgroundCalled",getDevName(3));
        HandleFrame(mDetector, mWidth, mHeight, bytes);
        mCamera.addCallbackBuffer(mCallbackBuffer);
        timeDebug[0]=timeDebug[1];
    }

    private static native void HandleFrame(GestureDetector detector,int width, int height, byte[] frame);


    private void setCameraParameters(Camera.Parameters params) {

        params.setAutoExposureLock(true);
        params.setExposureCompensation(-10);
        //if (params.isAutoExposureLockSupported()) {
          //  params.setAutoExposureLock(true);

        //}

       // if (params.isAutoWhiteBalanceLockSupported()) {
         //   params.setAutoWhiteBalanceLock(true);
        //}

        List<Camera.Size> previewSizes = params.getSupportedPreviewSizes();
        int idealWidth = 640;
        int idealHeight = 480;

        int bestWidth = 0;
        int bestHeight = 0;

        int minDiff = Integer.MAX_VALUE;
        for (Camera.Size pSize:previewSizes) {
            if (Math.abs(pSize.width - idealWidth) < minDiff) {
                bestWidth = pSize.width;
                bestHeight = pSize.height;
                minDiff = pSize.width - idealWidth;
            }
        }
        params.setPreviewSize(640,480);
        mCamera.setParameters(params);
    }

    private native static int ScanFiles(); // return number of devs
    private native static int OpenDev(int devid);
    private native static int intSendEvent(int devid, int type, int code, long value);
    private native static String getDevName(int devid);

}
