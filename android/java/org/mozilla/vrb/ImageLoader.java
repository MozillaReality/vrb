/* -*- Mode: Java; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.vrb;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.ETC1;

import org.mozilla.vrb.etc2.UtilETC2;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import androidx.annotation.Keep;

import static android.opengl.GLES30.GL_RGBA;

class ImageLoader {

    private static void loadBitmapFromInputStream(InputStream aInputStream, final String aFileName, final long aFileReader, final int aTrackingHandle) {
        try {
            if (aInputStream == null) {
                ImageLoadFailed(aFileReader, aTrackingHandle, "Unable to find image: " + aFileName);
                return;
            }
            Bitmap image = BitmapFactory.decodeStream(aInputStream);
            if (image == null) {
                ImageLoadFailed(aFileReader, aTrackingHandle, "Unable to find image: " + aFileName);
                return;
            }
            final int width  = image.getWidth();
            final int height = image.getHeight();
            ByteBuffer pixels = ByteBuffer.allocateDirect(image.getAllocationByteCount()).order(ByteOrder.BIG_ENDIAN);
            image.copyPixelsToBuffer(pixels);
            ProcessImage(aFileReader, aTrackingHandle, pixels, width, height, GL_RGBA); 
        } catch(Exception e) {
            String reason = "Error loading image file: " + aFileName + " (" + e.toString() + ")";
            ImageLoadFailed(aFileReader, aTrackingHandle, reason);
        }
    }

    private static void loadETC2FromInputStream(InputStream aInputStream, final String aFileName, final long aFileReader, final int aTrackingHandle) {
        try {
            if (aInputStream == null) {
                ImageLoadFailed(aFileReader, aTrackingHandle, "Unable to find image: " + aFileName);
                return;
            }
            byte[] buffer = new byte[4096];
            if (aInputStream.read(buffer, 0, ETC1.ETC_PKM_HEADER_SIZE) != ETC1.ETC_PKM_HEADER_SIZE) {
                ImageLoadFailed(aFileReader, aTrackingHandle, "Couldn't read PKM header: " + aFileName);
                return;
            }
            ByteBuffer header = ByteBuffer.allocateDirect(ETC1.ETC_PKM_HEADER_SIZE).order(ByteOrder.BIG_ENDIAN);
            header.put(buffer, 0, ETC1.ETC_PKM_HEADER_SIZE).position(0);
            if (!UtilETC2.isValid(header)) {
                ImageLoadFailed(aFileReader, aTrackingHandle, "Invalid PKM header: " + aFileName);
                return;
            }
            int width = UtilETC2.getWidth(header);
            int height = UtilETC2.getHeight(header);
            int format = UtilETC2.getETC2CompressionType(header);
            final int encodedSize = UtilETC2.getEncodedDataSize(width, height);
            final ByteBuffer dataBuffer = ByteBuffer.allocateDirect(encodedSize).order(ByteOrder.BIG_ENDIAN);
            for (int i = 0; i < encodedSize; ) {
                int chunkSize = Math.min(buffer.length, encodedSize - i);
                if (aInputStream.read(buffer, 0, chunkSize) != chunkSize) {
                    ImageLoadFailed(aFileReader, aTrackingHandle, "Error reading PKM data: " + aFileName);
                    return;
                }
                dataBuffer.put(buffer, 0, chunkSize);
                i += chunkSize;
            }
            dataBuffer.position(0);
            ProcessImage(aFileReader, aTrackingHandle, dataBuffer, width, height, format);
        } catch(Exception e) {
            String reason = "Error loading image file: " + aFileName + " (" + e.toString() + ")";
            ImageLoadFailed(aFileReader, aTrackingHandle, reason);
        }
    }

    @Keep
    private static void loadFromInputStream(InputStream aInputStream, final String aFileName, final long aFileReader, final int aTrackingHandle) {
        if (aFileName.toLowerCase().endsWith(".pkm")) {
            loadETC2FromInputStream(aInputStream, aFileName, aFileReader, aTrackingHandle);
        } else {
            loadBitmapFromInputStream(aInputStream, aFileName, aFileReader, aTrackingHandle);
        }
    }

    @Keep
    private static void loadFromAssets(final AssetManager aAssets, final String aFileName, final long aFileReader, final int aTrackingHandle) {
        try (InputStream in = aAssets.open(aFileName, AssetManager.ACCESS_STREAMING)) {
            loadFromInputStream(in, aFileName, aFileReader, aTrackingHandle);
        } catch(IOException e) {
            String reason = "Error loading image file: " + aFileName + " (" + e.toString() + ")";
            ImageLoadFailed(aFileReader, aTrackingHandle, reason);
        }
    }

    @Keep
    private static void loadFromRawFile(final String aFileName, final long aFileReader, final int aTrackingHandle) {
        try (InputStream in = new FileInputStream(new File(aFileName))) {
            loadFromInputStream(in, aFileName, aFileReader, aTrackingHandle);
        } catch(IOException e) {
            String reason = "Error loading image file: " + aFileName + " (" + e.toString() + ")";
            ImageLoadFailed(aFileReader, aTrackingHandle, reason);
        }
    }

    private native static void ProcessImage(
            final long aFileReader,
            final int aTrackingHandle,
            final ByteBuffer aBuffer,
            final int aWidth,
            final int aHeight,
            final int aFormat);

    private native static void ImageLoadFailed(final long aFileReader, final int aTrackingHandle, final String aReason);
}
