package org.mozilla.vrb;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.content.res.AssetManager;

import java.io.IOException;
import java.io.InputStream;

class ImageLoader {
    private static void loadFromAssets(final AssetManager aAssets, final String aFileName, final long aFileReader, final int aTrackingHandle) {
        try {
            InputStream in = aAssets.open(aFileName, AssetManager.ACCESS_STREAMING);
            if (in != null) {
                Bitmap texture = BitmapFactory.decodeStream(in);
                if (texture == null) {

                } else {
                    int pixels[] = new int[texture.getWidth() * texture.getHeight()];
                    ProcessTexture(aFileReader, aTrackingHandle, pixels, texture.getWidth(), texture.getHeight());
                }
            }
        } catch(IOException e) {

        }
    }

    private native static void ProcessTexture(
            final long aFileReader,
            final int aTrackingHandle,
            final int[] aPixels,
            final int aWidth,
            final int aHeight);
}