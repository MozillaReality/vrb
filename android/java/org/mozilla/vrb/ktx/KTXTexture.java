package org.mozilla.vrb.ktx;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class KTXTexture {

    /**
     * https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/#1
     * Byte[12] identifier
     * UInt32 endianness
     * UInt32 glType
     * UInt32 glTypeSize
     * UInt32 glFormat
     * Uint32 glInternalFormat
     * Uint32 glBaseInternalFormat
     * UInt32 pixelWidth
     * UInt32 pixelHeight
     * UInt32 pixelDepth
     * UInt32 numberOfArrayElements
     * UInt32 numberOfFaces
     * UInt32 numberOfMipmapLevels
     * UInt32 bytesOfKeyValueData
     *
     * for each keyValuePair that fits in bytesOfKeyValueData
     *     UInt32   keyAndValueByteSize
     *     Byte     keyAndValue[keyAndValueByteSize]
     *     Byte     valuePadding[3 - ((keyAndValueByteSize + 3) % 4)]
     * end
     *
     * for each mipmap_level in numberOfMipmapLevels1
     *     UInt32 imageSize;
     *     for each array_element in numberOfArrayElements2
     *        for each face in numberOfFaces3
     *            for each z_slice in pixelDepth2
     *                for each row or row_of_blocks in pixelHeight2
     *                    for each pixel or block_of_pixels in pixelWidth
     *                        Byte data[format-specific-number-of-bytes]4
     *                    end
     *                end
     *            end
     *            Byte cubePadding[0-3]
     *        end
     *     end
     *     Byte mipPadding[0-3]
     * end
     **/

    private final static int[] KTX_SIGNATURE = {
        0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
    };

    public final ByteOrder endianness;
    public final int glType;
    public final int glTypeSize;
    public final int glFormat;
    public final int glInternalFormat;
    public final int glBaseInternalFormat;
    public final int pixelWidth;
    public final int pixelHeight;
    public final int pixelDepth;
    public final int numberOfArrayElements;
    public final int numberOfFaces;
    public final int numberOfMipmapLevels;
    public final int bytesOfKeyValueData;
    public ByteBuffer level0Texture;

    private KTXTexture(Reader aReader) throws IOException {
        int value = aReader.readIntLE();
        if (value == 0x04030201) {
            endianness = ByteOrder.LITTLE_ENDIAN;
        } else if (value == 0x01020304) {
            endianness = ByteOrder.BIG_ENDIAN;
        } else {
            throw new IOException("Unexpected endianess value: " + value);
        }

        glType = aReader.readInt(endianness);
        glTypeSize = aReader.readInt(endianness);
        glFormat = aReader.readInt(endianness);
        glInternalFormat = aReader.readInt(endianness);
        glBaseInternalFormat = aReader.readInt(endianness);
        pixelWidth = aReader.readInt(endianness);
        pixelHeight = aReader.readInt(endianness);
        pixelDepth = aReader.readInt(endianness);
        numberOfArrayElements = aReader.readInt(endianness);
        numberOfFaces = aReader.readInt(endianness);
        numberOfMipmapLevels = aReader.readInt(endianness);
        bytesOfKeyValueData = aReader.readInt(endianness);

        aReader.skip(bytesOfKeyValueData);

        int imageSize = aReader.readInt(endianness);
        if (imageSize <=0) {
            throw new IOException("Unexpected imageSize: " + imageSize);
        }
        level0Texture = ByteBuffer.allocateDirect(imageSize).order(endianness);
        byte[] buffer = new byte[4096];
        for (int i = 0; i < imageSize; ) {
            int chunkSize = Math.min(buffer.length, imageSize - i);
            if (aReader.read(buffer, 0, chunkSize) != chunkSize) {
                throw new IOException("Error reading KTX image data");
            }
            level0Texture.put(buffer, 0, chunkSize);
            i += chunkSize;
        }
        level0Texture.position(0);
    }


    private static boolean isValidSignature(Reader reader) {
        try {
            for (int chunk : KTX_SIGNATURE) {
                if (chunk != reader.readByte()) {
                    return false;
                }
            }
        }
        catch (Exception ex) {
            return false;
        }

        return true;
    }

    public static KTXTexture parse(InputStream aStream) throws IOException {
        Reader reader = new Reader(aStream);
        if (!isValidSignature(reader)) {
            return null;
        }

        return new KTXTexture(reader);
    }

    private static class Reader {
        private InputStream mInputStream;

        Reader(InputStream aInputStream) {
            mInputStream = aInputStream;
        }

        int read(byte[] cbuf, int offset, int len) throws IOException {
            return mInputStream.read(cbuf, offset, len);
        }

        int readByte() throws IOException {
            return mInputStream.read();
        }

        int readInt(ByteOrder aByteOrder) throws IOException {
            if (aByteOrder == ByteOrder.LITTLE_ENDIAN) {
                return readIntLE();
            } else {
                return readIntBE();
            }
        }

        int readIntLE() throws IOException {
            int res = 0;
            for (int i = 0; i < Integer.SIZE / Byte.SIZE; ++i) {
                res |= (mInputStream.read() & 0xff) << (i * Byte.SIZE);
            }
            return res;
        }

        int readIntBE() throws IOException {
            int res = 0;
            for (int i = 0; i < Integer.SIZE / Byte.SIZE; ++i) {
                res = (res << Byte.SIZE) | (mInputStream.read() & 0xff);
            }
            return res;
        }

        void skip(long aBytes) throws IOException {
            mInputStream.skip(aBytes);
        }
    }
}
