/* UNIMPORTANT
 * UNIMPORTANT
 * UNIMPORTANT
 * UNIMPORTANT
*/






package com.intugine.nimbledetector;

import android.content.Context;
import android.os.Vibrator;
import android.util.Log;

import java.io.DataOutputStream;

/**
 * Created by siddharthsivaraman on 13/01/15.
 */
public class GestureDetector {

    public static final String TAG = "GestureDetector";
    int swipe=0;
    private Context mCtx;
    private Vibrator mVibrator;
    int x00,y00,x01,y01;
    public GestureDetector(Context ctx) {
//        mCtx = ctx;
//        mVibrator = (Vibrator)mCtx.getSystemService(Context.VIBRATOR_SERVICE);
    }

    // NOTE: This function is called directly from native code
    // the integer param is for the direction of swipe
    // 0 - swipe left
    // 1 - swipe right
    // 2 - swipe up
    // 3 - swipe down
    public void swipe(int i) {

       // mVibrator.vibrate(100);
        //Log.d("Vibrate Called","Vibrate Called");
        // Can only run on a rooted device
        swipe = i;
        new Thread(new GestureRunner()).start();
    }

    // NOTE:  For now gestures can be simulated on a
    // rooted device by calling /system/bin/input with appropriate parameters

    private class GestureRunner implements Runnable {
        @Override
        public void run(){
            try {
            //    Process p = Runtime.getRuntime().exec(new String[]{"su", "-c", "adb shell input touchscreen swipe 300 300 500 1000"});
            //    p.waitFor();
                Log.d("Swipe Called",""+swipe);
            } catch (Exception ioe) {
                Log.d(TAG,""+ioe.getMessage());
            }
        }
    };

    public void draw(int x0,int y0, int x1,int y1){
        x00 = x0;
        y00 = y0;
        x01 = x1;
        y01 = y1;
        Log.d("Draw",""+swipe);
        //new Thread(new GestureRunners()).start();
    }

    private class GestureRunners implements Runnable {
        @Override
        public void run(){
            try {
            //    String arg =  "adb shell input touchscreen swipe" + x00 + y00 + x01 + y01;
               // Process p = Runtime.getRuntime().exec(new String[]{"su", "-c", "adb shell input touchscreen swipe 300 300 500 1000"});
            //    Process p = Runtime.getRuntime().exec(new String[]{"su", "-c", arg});
            //    p.waitFor();
                Log.d("Draw",""+swipe);
            } catch (Exception ioe) {
                Log.d(TAG,""+ioe.getMessage());
            }
        }
    };

}
