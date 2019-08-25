package org.opencv.samples.tutorial2;

public class GL3CV4Lib {

    public static native void init(int width, int height);
    public static native void step();
    public static native void setImage(long imageRGBA);
}
