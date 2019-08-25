package org.opencv.samples.tutorial2;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class GL3CV4View extends GLSurfaceView {

	private Renderer renderer;

	public GL3CV4View(Context context) {
		super(context);
		setZOrderOnTop(true);
		setEGLConfigChooser(8, 8, 8, 8, 16, 0);
		setEGLContextClientVersion(3);
		getHolder().setFormat(PixelFormat.TRANSLUCENT);
		renderer = new Renderer();
		setRenderer(renderer);

	}

	private class Renderer implements GLSurfaceView.Renderer {
		public void onDrawFrame(GL10 gl) {
			GL3CV4Lib.step();
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {
			GL3CV4Lib.init(width, height);
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) { }
	}

}
