/* -*- Mode: Java; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.vrb;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;

class ImageLoader {
    private static void loadFromAssets(final AssetManager aAssets, final String aFileName, final long aFileReader, final int aTrackingHandle) {
        try {
            InputStream in = aAssets.open(aFileName, AssetManager.ACCESS_STREAMING);
            if (in != null) {
                Bitmap image = BitmapFactory.decodeStream(in);
                if (image == null) {
                    ImageLoadFailed(aFileReader, aTrackingHandle, "Unable to find image: " + aFileName);
                    return;
                } else {
                    final int[] pixels = new int[image.getByteCount() / 4];
                    try {
                        final int width  = image.getWidth();
                        final int height = image.getHeight();
                        image.getPixels(pixels, /* offset */ 0, /* stride */ width,
                                 /* x */ 0, /* y */ 0, width, height);
                        ProcessImage(aFileReader, aTrackingHandle, pixels, width, height);

                    } catch (final Exception e) {
                        Log.e("VRB", "Can not load texture image: " + aFileName, e);
                        String reason = "Failed to getPixels from bitmap loaded from file: " + aFileName + " (" + e.toString() + ")";
                        ImageLoadFailed(aFileReader, aTrackingHandle, reason);
                        return;
                    }
                }
            }
        } catch(IOException e) {
            String reason = "Error loading image file: " + aFileName + " (" + e.toString() + ")";
            ImageLoadFailed(aFileReader, aTrackingHandle, reason);
            return;
        }
    }

    private native static void ProcessImage(
            final long aFileReader,
            final int aTrackingHandle,
            final int[] aPixels,
            final int aWidth,
            final int aHeight);

    private native static void ImageLoadFailed(final long aFileReader, final int aTrackingHandle, final String aReason);
}
