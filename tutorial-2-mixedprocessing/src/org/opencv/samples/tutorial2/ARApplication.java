package org.opencv.samples.tutorial2;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.Log;
import android.content.Context;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.Mat;

import android.widget.RelativeLayout;
import android.view.SurfaceView;

public class ARApplication extends Activity implements CvCameraViewListener2 {

    private GL3CV4View mView = null;
    private Context context;

    private boolean gl3_loaded = false;
    private CameraBridgeViewBase mOpenCvCameraView;
    private RelativeLayout l_layout;

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.i("Loading", "OpenCV loaded successfully");

                    System.loadLibrary("gl3cv4jni");
                    gl3_loaded = true;

                    mView = new GL3CV4View(getApplication());
                    l_layout.addView(mView);
                    setContentView(l_layout);

                    mOpenCvCameraView.enableView();
                }
                break;
                default: {
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    public void onCameraViewStarted(int width, int height) {
    }

    public void onCameraViewStopped() {

    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
        Mat input = inputFrame.rgba();
        if (gl3_loaded) {
            GL3CV4Lib.setImage(input.nativeObj);
        }
        return null;
    }

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        setContentView(R.layout.ar);

        l_layout = (RelativeLayout) findViewById(R.id.linearLayoutRest);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.opencv_camera_surface_view);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setMaxFrameSize(1920, 1080);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.disableView();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mView != null) {
            mView.onPause();
        }
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
        gl3_loaded = false;
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!OpenCVLoader.initDebug()) {
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }

        if (mView != null) {
            mView.onResume();
        }
    }
}
