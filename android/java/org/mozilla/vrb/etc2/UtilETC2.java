package org.mozilla.vrb.etc2;

import android.opengl.GLES11Ext;

import java.nio.ByteBuffer;

import static android.opengl.GLES30.GL_COMPRESSED_R11_EAC;
import static android.opengl.GLES30.GL_COMPRESSED_RG11_EAC;
import static android.opengl.GLES30.GL_COMPRESSED_RGB8_ETC2;
import static android.opengl.GLES30.GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
import static android.opengl.GLES30.GL_COMPRESSED_RGBA8_ETC2_EAC;

/**
 * Parsing and data utility class for ETC2 textures.
 *
 * @author Jared Woolston (jwoolston@tenkiv.com)
 */
public class UtilETC2 {

    /**
     * The magic sequence for an ETC1 file.
     */
    private static final byte ETC1Magic[] = {
            0x50, //'P'
            0x4B, //'K'
            0x4D, //'M'
            0x20, //' '
            0x31, //'1'
            0x30  //'0'
    };

    /**
     * The magic sequence for an ETC2 file.
     */
    private static final byte ETC2Magic[] = {
            0x50, //'P'
            0x4B, //'K'
            0x4D, //'M'
            0x20, //' '
            0x32, //'2'
            0x30  //'0'
    };

    /**
     * File header offsets.
     */
    private static final int ETC2_PKM_FORMAT_OFFSET = 6;
    private static final int ETC2_PKM_ENCODED_WIDTH_OFFSET = 8;
    private static final int ETC2_PKM_ENCODED_HEIGHT_OFFSET = 10;
    private static final int ETC2_PKM_WIDTH_OFFSET = 12;
    private static final int ETC2_PKM_HEIGHT_OFFSET = 14;

    /**
     * These are the supported PKM format identifiers for the PKM header. The sRGB formats are missing here because I was
     * not able to get header file information for them. This is the only thing preventing them from being supported.
     */
    private static final short ETC1_RGB8_OES = 0x0000;
    private static final short RGB8_ETC2 = 0x0001;
    private static final short RGBA8_ETC2_EAC = 0x0003;
    private static final short RGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x0004;
    private static final short R11_EAC = 0x0005;
    private static final short RG11_EAC = 0x0006;
    private static final short SIGNED_R11_EAC = 0x0007;
    private static final short SIGNED_RG11_EAC = 0x0008;

    /**
     * Checks the provided file header and determines if this is a valid ETC2 file.
     *
     * @param header {@link ByteBuffer} The PKM file header.
     * @return {@code boolean} True if the file header is valid.
     */
    public static boolean isValid(ByteBuffer header) {
        // First check the ETC2 magic sequence
        if ((ETC2Magic[0] != header.get(0)) && (ETC2Magic[1] != header.get(1)) && (ETC2Magic[2] != header.get(2))
                && (ETC2Magic[3] != header.get(3)) && (ETC2Magic[4] != header.get(4)) && (ETC2Magic[5] != header.get(5))) {
            // Check to see if we are ETC1 instead
            if ((ETC1Magic[0] != header.get(0)) && (ETC1Magic[1] != header.get(1)) && (ETC1Magic[2] != header.get(2))
                    && (ETC1Magic[3] != header.get(3)) && (ETC1Magic[4] != header.get(4)) && (ETC1Magic[5] != header.get(5))) {
                return false;
            }
        }

        // Second check the type
        final short ETC2_FORMAT = header.getShort(ETC2_PKM_FORMAT_OFFSET);
        switch (ETC2_FORMAT) {
            case ETC1_RGB8_OES:
            case RGB8_ETC2:
            case RGBA8_ETC2_EAC:
            case RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            case R11_EAC:
            case RG11_EAC:
            case SIGNED_R11_EAC:
            case SIGNED_RG11_EAC:
                break;
            default:
                return false;
        }

        final int encodedWidth = getEncodedWidth(header);
        final int encodedHeight = getEncodedHeight(header);
        final int width = getWidth(header);
        final int height = getHeight(header);

        // Check the width
        if (encodedWidth < width || (encodedWidth - width) > 4) {
            return false;
        }
        // Check the height
        if (encodedHeight < height || (encodedHeight - height) > 4) {
            return false;
        }

        // We passed all the checks, return true
        return true;
    }

    /**
     * Retrieves the particular compression format for the ETC2 Texture.
     *
     * @param header {@link ByteBuffer} The PKM file header.
     * @return {@code int} One of the GL_COMPRESSED_* types for ETC2, or -1 if unrecognized.
     */
    public static int getETC2CompressionType(ByteBuffer header) {
        switch (header.getShort(ETC2_PKM_FORMAT_OFFSET)) {
            case ETC1_RGB8_OES:
                return GLES11Ext.GL_ETC1_RGB8_OES;
            case RGB8_ETC2:
                return GL_COMPRESSED_RGB8_ETC2;
            case RGBA8_ETC2_EAC:
                return GL_COMPRESSED_RGBA8_ETC2_EAC;
            case RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
                return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            case R11_EAC:
                return GL_COMPRESSED_R11_EAC;
            case RG11_EAC:
                return GL_COMPRESSED_RG11_EAC;
            case SIGNED_R11_EAC:
                return SIGNED_R11_EAC;
            case SIGNED_RG11_EAC:
                return SIGNED_RG11_EAC;
            default:
                return -1;
        }
    }

    /**
     * Retrieve the actual texture width in pixels.
     *
     * @param header {@link ByteBuffer} The PKM file header.
     * @return {@code int} The actual texture width.
     */
    public static int getWidth(ByteBuffer header) {
        return (0xFFFF & header.getShort(ETC2_PKM_WIDTH_OFFSET));
    }

    /**
     * Retrieve the actual texture height in pixels.
     *
     * @param header {@link ByteBuffer} The PKM file header.
     * @return {@code int} The actual texture height.
     */
    public static int getHeight(ByteBuffer header) {
        return (0xFFFF & header.getShort(ETC2_PKM_HEIGHT_OFFSET));
    }

    /**
     * Retrieve the encoded texture width in pixels.
     *
     * @param header {@link ByteBuffer} The PKM file header.
     * @return {@code int} The encoded texture width.
     */
    public static int getEncodedWidth(ByteBuffer header) {
        return (0xFFFF & header.getShort(ETC2_PKM_ENCODED_WIDTH_OFFSET));
    }

    /**
     * Retrieve the encoded texture height in pixels.
     *
     * @param header {@link ByteBuffer} The PKM file header.
     * @return {@code int} The encoded texture height.
     */
    public static int getEncodedHeight(ByteBuffer header) {
        return (0xFFFF & header.getShort(ETC2_PKM_ENCODED_HEIGHT_OFFSET));
    }

    /**
     * Return the size of the encoded image data (does not include the size of the PKM header).
     *
     * @param width {@code int} The actual texture width in pixels.
     * @param height {@code int} The actual texture height in pixels.
     * @return {@code int} The number of bytes required to encode this data.
     */
    public static int getEncodedDataSize(int width, int height) {
        return ((((width + 3) & ~3) * ((height + 3) & ~3)) >> 1);
    }
}