package com.pen_input;

import static android.content.ContentValues.TAG;

import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.ParcelFileDescriptor;
import android.util.Log;
import android.view.View;

import java.io.FileDescriptor;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class UsbStreamService {
    private static ParcelFileDescriptor fileDescriptor;
    private static FileOutputStream fileOutputStream;
    private static boolean isStreamOpen = false;


    public static void streamTouchInputToUsb(UsbManager usbManager, UsbAccessory usbAccessory, View view) {
        try {
            if(!isStreamOpen){
                fileOutputStream = getUsbFileOutputStream(usbManager, usbAccessory);
                fileOutputStream.flush();
                TouchListener touchListener = new TouchListener(fileOutputStream);
                view.setOnTouchListener(touchListener.handleTouch);
                isStreamOpen = true;
            }
        } catch (IOException e) {
            String message = "Unexpected IO exception while attempting to open filestream on usb accessory.";
            Log.d(TAG, message, e);
        }
    }

    public static void closeStream(){
        try {
            if (fileDescriptor != null) {
                fileDescriptor.close();
            }
            if(fileOutputStream != null){
                fileOutputStream.close();
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private static FileOutputStream getUsbFileOutputStream(UsbManager usbManager, UsbAccessory usbAccessory) throws IOException {
        Log.d(TAG, "openAccessory: " + usbAccessory);
        fileDescriptor = usbManager.openAccessory(usbAccessory);
        if (fileDescriptor != null) {
            FileDescriptor fd = fileDescriptor.getFileDescriptor();
            return new FileOutputStream(fd);
        }
        throw new FileNotFoundException("File descriptor not found");
    }
}
